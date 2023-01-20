/* @(#) posix/cf_task_pthreads.c 20190728 */

/***************************************************************
** TASK driver functions with pthreads
**
** Author:Ivan Priesol (c) 2019
**
** Part of pForth:
**
** Author: Phil Burk
** Copyright 1994 3DO, Phil Burk, Larry Polansky, David Rosenboom
**
** The pForth software code is dedicated to the public domain,
** and any third party may reproduce, distribute and modify
** the pForth software code or any derivative works thereof
** without any compensation or license.  The pForth software
** code is provided on an "as is" basis without any warranty
** of any kind, including, without limitation, the implied
** warranties of merchantability and fitness for a particular
** purpose and their equivalents under the laws of any jurisdiction.
**
***************************************************************/

#ifdef PF_LOCALIZE_TASK_STACKS

#include "../pf_all.h"
#include "../cf_task.h"

#include <pthread.h>
#include <errno.h>


typedef struct pthreadTT_s {
  pthread_t           pt;
  #ifdef PF_TASK_NOTIFY
  uint32_t            notify_val;
  pthread_cond_t      notify_cond;
  pthread_mutex_t     notify_mux;
  pthread_mutexattr_t notify_mux_attr;
  uint8_t             flags;
  #endif
} pthreadTT_t;


static void *ffTaskCatch( void *data ) {
  cfTaskToken_t *TT;
  ThrowCode exception;

  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

  TT = (cfTaskToken_t *) data;
  exception = pfCatch(TT->lTask, POP_TASK_STACK());

  if(TT->flags & TASK_DETACH_FLAG) {
    pthreadTT_t *pt;

    pt = (pthreadTT_t *) TT->data;
    
#ifdef PF_TASK_NOTIFY
    pthread_mutex_destroy(&pt->notify_mux);
    pthread_mutexattr_destroy(&pt->notify_mux_attr);
    pthread_cond_destroy(&pt->notify_cond);
#endif
    
    free(pt);
    ffTaskFree(TT);
  }

  return (void *) exception;
}

#if defined PF_TASK_SEMAPHORE || defined PF_TASK_NOTIFY

#include <sys/time.h>

static void ms2abs(cell_t ms, struct timespec *abs) {
  struct timeval now;
  int nsec;

  gettimeofday(&now, NULL);
  nsec = now.tv_usec * 1000 + (ms % 1000) * 1000000;
      
  abs->tv_sec = now.tv_sec + ms / 1000 + nsec / 1000000000;
  abs->tv_nsec = nsec % 1000000000;
}

#endif /* defined PF_TASK_SEMAPHORE || defined PF_TASK_NOTIFY */


cell_t cfNewTask( DL_TASK cell_t n, cell_t XT ) {
  int ret;
  ThrowCode excp;
  cfTaskToken_t *TT;
  pthreadTT_t *pt;

  excp = ffTaskNew( lTask, n, &TT );
  
  if(excp) {
    return excp;
  }

  PUSH_TASK_STACK(XT);
  TT->data = pt = malloc(sizeof(pthreadTT_t));
  ret = pthread_create( &pt->pt, NULL, ffTaskCatch, TT );

  if(ret) {
    free(pt);
    ffTaskFree(TT);
    MSG_NUM_D("pthread_create() return code ", ret);
    return THROW_ABORT;
  }

  #ifdef PF_TASK_NOTIFY
  pt->notify_val = 0;
  pt->flags = 0;
  ret = pthread_cond_init(&pt->notify_cond, 0);

  if(ret) {
    MSG_NUM_D("pthread_cond_init() for TASK.NOTIFY return code ", ret);
  }

  pthread_mutexattr_init( &pt->notify_mux_attr );
  pthread_mutexattr_settype( &pt->notify_mux_attr, PTHREAD_MUTEX_FAST_NP);
  ret = pthread_mutex_init( &pt->notify_mux, &pt->notify_mux_attr );
	
  if(ret) {
    MSG_NUM_D("pthread_mutex_init() for TASK.NOTIFY return code ", ret);
  }  
  #endif
  
  PUSH_DATA_STACK(TT);

  return 0;
}


cell_t cfNewTaskOnCpu( DL_TASK cell_t n, cell_t XT, cell_t cpu ) {
  ThrowCode excp;
  cell_t TT;

  excp = cfNewTask( L_TASK n, XT );

  if(excp == 0) {
    TT = POP_DATA_STACK;
    cfTaskSetCPU( L_TASK 1 << cpu, TT );
    PUSH_DATA_STACK(TT);
  }

  return excp;
}


void cfDetachTask( DL_TASK cell_t TT ) {
  cfTaskToken_t *tt;
  pthreadTT_t *pt;

  tt = (cfTaskToken_t *) TT;
  pt = (pthreadTT_t *) tt->data;
  
  tt->flags |= TASK_DETACH_FLAG;
  pthread_detach(pt->pt);
}


cell_t cfJoinTask ( DL_TASK cell_t n, cell_t TT ) {
  cfTaskToken_t *tt;
  pthreadTT_t *pt;
  ThrowCode excp;
  int pterr;

  tt = (cfTaskToken_t *) TT;
  pt = (pthreadTT_t *) tt->data;

  /* TASK_DONE_FLAG is set by TASK.DONE? if task was 
     joined by it then ret code is already placed 
     in the task stack */
  if(! (tt->flags & TASK_DONE_FLAG)) {
    #ifdef PF_TASK_NOTIFY
    pthread_mutex_destroy(&pt->notify_mux);
    pthread_mutexattr_destroy(&pt->notify_mux_attr);
    pthread_cond_destroy(&pt->notify_cond);
    #endif

    pterr = pthread_join(pt->pt, (void **) &excp);
    
    if(pterr > 0) {
      MSG_NUM_D("pthread_join() return code ", pterr);
      ffTaskFree( tt );
      free(pt);    

      return THROW_ABORT;
    } else {
      PUSH_TASK_STACK(excp);
    }
  }

  free(pt);    

  return (cell_t) ffTaskJoin( lTask, n, tt );
}


void   cfCancelTask ( DL_TASK cell_t TT ) {
  cfTaskToken_t *tt;
  pthreadTT_t *pt;
  int ret;
  void *join_ret;

  tt = (cfTaskToken_t *) TT;
  pt = (pthreadTT_t *) tt->data;
  ret = pthread_cancel(pt->pt);
  
  if(ret == 0) {
    #ifdef PF_TASK_NOTIFY
    pthread_mutex_destroy(&pt->notify_mux);
    pthread_mutexattr_destroy(&pt->notify_mux_attr);
    pthread_cond_destroy(&pt->notify_cond);
    #endif

    ret = pthread_join(pt->pt, (void **) &join_ret);

    if(ret) {
      MSG_NUM_D("pthread_join() return code ", ret);
    }
    else {
      if(join_ret != PTHREAD_CANCELED) {
	MSG_NUM_D("pthread_join() unexpected return status ", ret);
      }
    }

    ffTaskFree( tt );
    free(pt);    
  }
  else {
    MSG_NUM_D("ERROR: pthread_cancel() return code ", ret);
  }
}


cell_t cfTaskIsDone ( DL_TASK cell_t TT ) {
  cfTaskToken_t *tt;
  pthreadTT_t *pt;
  ThrowCode excp;
  int ret;

  tt = (cfTaskToken_t *) TT;

  if(tt->flags &= TASK_DONE_FLAG) {
    return FTRUE;
  }  
  
  pt = (pthreadTT_t *) tt->data;

  ret = pthread_tryjoin_np(pt->pt, (void **) &excp);

  if(ret == EBUSY) {
    return FFALSE;
  }
  else {
    PUSH_TASK_STACK(excp);
    tt->flags |= TASK_DONE_FLAG;

    return FTRUE;
  }
}

cell_t cfTaskCPU    ( DL_TASK_VOID ) {
  return sched_getcpu();
}

void   cfTaskSetCPU ( DL_TASK cell_t mask, cell_t TT ) {
  cfTaskToken_t *tt;
  pthreadTT_t *pt;
  cpu_set_t cpuset;
  ucell_t umask;
  int cpu;

  tt = (cfTaskToken_t *) TT;
  pt = (pthreadTT_t *) tt->data;

  umask = (ucell_t) mask;
  CPU_ZERO(&cpuset);
  cpu = 0;
  
  while(umask != 0) {
    if(umask & 1) {
      CPU_SET(cpu, &cpuset);
    }

    cpu++;
    umask >>= 1;
  }
  
  pthread_setaffinity_np(pt->pt, sizeof(cpu_set_t), &cpuset);
}

void   cfTaskSetPri ( DL_TASK cell_t pri,  cell_t TT ) {
  cfTaskToken_t *tt;
  pthreadTT_t *pt;

  tt = (cfTaskToken_t *) TT;
  pt = (pthreadTT_t *) tt->data;

  pthread_setschedprio(pt->pt, (int) pri);
}


void   cfYieldTask  ( DL_TASK_VOID ) {
  sched_yield();
}



#ifdef PF_TASK_SEMAPHORE

#include <semaphore.h>
  
#define FLAG_SEM_MUTEX           1
#define FLAG_SEM_MUTEX_RECURSIVE 2

typedef struct cfSemaphoreToken_s {
  uint8_t             flags;
  sem_t               sem;
  pthread_mutex_t     mux;
  pthread_mutexattr_t mux_attr;
  int                 count; 	/* used as max count by semaphore or as recursive mutex counter */
} cfSemaphoreToken_t;


cell_t cfSemaphore ( DL_TASK cell_t max_count, cell_t init ) {
  int ret;
  cfSemaphoreToken_t *st;  

  st = malloc(sizeof(cfSemaphoreToken_t));

  if(st) {
    st->flags = 0;
    st->count = max_count;

    ret = sem_init(&st->sem, 0, init);

    if(ret) {
      MSG_NUM_D("sem_init() return code ", ret);
    }

    if(max_count > 0) {
      ret |= pthread_mutexattr_init( &st->mux_attr );
      ret |= pthread_mutexattr_settype( &st->mux_attr, PTHREAD_MUTEX_FAST_NP);
      ret |= pthread_mutex_init( &st->mux, &st->mux_attr );
    }
	
    if(ret) {
      MSG_NUM_D("pthread_mutex_init() return code ", ret);
    }
    else {
      PUSH_DATA_STACK(st);
      
      return 0;
    }

    free(st);
  }

  return THROW_ABORT;
}


cell_t cfSemaphoreMutex ( DL_TASK_VOID ) {
  int ret;
  cfSemaphoreToken_t *st;

  st = malloc(sizeof(cfSemaphoreToken_t));
	      
  if(st) {
    st->flags = FLAG_SEM_MUTEX;
    
    pthread_mutexattr_init( &st->mux_attr );
    pthread_mutexattr_settype( &st->mux_attr, PTHREAD_MUTEX_FAST_NP );
    ret = pthread_mutex_init( &st->mux, &st->mux_attr );

    if(ret) {
      MSG_NUM_D("pthread_mutex_init() return code ", ret);
    }
    else {
      PUSH_DATA_STACK(st);
	
      return 0;
    }

    free(st);
  }

  return THROW_ABORT;
}

/* TODO: ffSemaphoreMutex */
cell_t cfSemaphoreMutexRecursive ( DL_TASK_VOID ) {
  int ret;
  cfSemaphoreToken_t *st;

  st = malloc(sizeof(cfSemaphoreToken_t));
	      
  if(st) {
    st->flags = FLAG_SEM_MUTEX | FLAG_SEM_MUTEX_RECURSIVE;
    st->count = 0;

    pthread_mutexattr_init( &st->mux_attr );
    pthread_mutexattr_settype(&st->mux_attr, PTHREAD_MUTEX_RECURSIVE_NP);
    ret = pthread_mutex_init( &st->mux, &st->mux_attr );

    if(ret) {
      MSG_NUM_D("pthread_mutex_init() return code ", ret);
    }
    else {
      PUSH_DATA_STACK(st);
	
      return 0;
    }

    free(st);
  }

  return THROW_ABORT;
}


void   cfSemaphoreDelete ( DL_TASK cell_t ST ) {
  cfSemaphoreToken_t *st;  

  st = (cfSemaphoreToken_t *) ST;

  if((st->flags & FLAG_SEM_MUTEX) || (st->count > 0)) {
    pthread_mutex_destroy(&st->mux);
    pthread_mutexattr_destroy(&st->mux_attr);
  }
  
  if(!(st->flags & FLAG_SEM_MUTEX)) {
    sem_destroy(&st->sem);
  }

  free(st);
}


cell_t cfSemaphoreGetCount ( DL_TASK cell_t ST ) {
  cfSemaphoreToken_t *st;
  int ret;

  st = (cfSemaphoreToken_t *) ST;

  if(st->flags & FLAG_SEM_MUTEX) {
    if(st->flags & FLAG_SEM_MUTEX_RECURSIVE) {
      ret = 1 - st->count;
    }
    else {
      ret = pthread_mutex_trylock(&st->mux) == EBUSY ? 0 : 1;

      if(ret == 1) {
	pthread_mutex_unlock(&st->mux);
      }      
    }
  }
  else {
    ret = -1;
    sem_getvalue(&st->sem, &ret);
  }

  return (cell_t) ret;
}


cell_t cfSemaphoreTake ( DL_TASK cell_t timeout_ms, cell_t ST ) {
  cfSemaphoreToken_t *st;
  int ret;

  /* printf(">>>> st: %d, to: %d ms\n", ST, timeout_ms); */

  st = (cfSemaphoreToken_t *) ST;

  if(timeout_ms < 0) {
    ret = st->flags & FLAG_SEM_MUTEX ?
      pthread_mutex_lock(&st->mux) :
      sem_wait(&st->sem);
  }
  else if(timeout_ms == 0) {
    ret = st->flags & FLAG_SEM_MUTEX ?
      pthread_mutex_trylock(&st->mux) :
      sem_trywait(&st->sem);
  }
  else {
    struct timespec t;

    ms2abs(timeout_ms, &t);
    
    ret = st->flags & FLAG_SEM_MUTEX ?
      pthread_mutex_timedlock(&st->mux, &t) :
      sem_timedwait(&st->sem, &t);
  }

  if(st->flags & FLAG_SEM_MUTEX_RECURSIVE && ret == 0) {
    st->count++;
  }

  return ret == 0 ? FTRUE : FFALSE;
}


void   cfSemaphoreGive ( DL_TASK cell_t ST ) {
  cfSemaphoreToken_t *st;
  int ret;

  st = (cfSemaphoreToken_t *) ST;

  if(st->flags & FLAG_SEM_MUTEX) {
    if(st->flags & FLAG_SEM_MUTEX_RECURSIVE) {
      st->count--;
    }

    pthread_mutex_unlock(&st->mux);    
  }
  else {
    if(st->count > 0) {
      pthread_mutex_lock(&st->mux);
      sem_getvalue(&st->sem, &ret);

      if(st->count > ret) {
	sem_post(&st->sem);
      }
      
      pthread_mutex_unlock(&st->mux);
    }
    else sem_post(&st->sem);
  }
}


void   cfSemaphoreGiveISR ( DL_TASK cell_t ST ) {
  cfSemaphoreGive( L_TASK ST );
}


cell_t cfSemaphoreTakeISR ( DL_TASK cell_t ST ) {
  return cfSemaphoreTake( L_TASK 0, ST );
}

#endif /* PF_TASK_SEMAPHORE */



#ifdef PF_TASK_NOTIFY

#define TASK_NOTIFY_PENDING_FLAG 1

void cfTaskNotifyISR   ( DL_TASK cell_t action, cell_t TT ) {
  cfTaskNotify( L_TASK action, TT );
}


void cfTaskNotify      ( DL_TASK cell_t action, cell_t TT ) {
  cfTaskToken_t *tt;
  pthreadTT_t *pt;
  cell_t val, prev_val, ret = FFALSE;

  tt = (cfTaskToken_t *) TT;
  pt = (pthreadTT_t *) tt->data;

  pthread_mutex_lock(&pt->notify_mux);
  prev_val = pt->notify_val;

  switch(action & TASK_NOTIFY_ACTION_MASK) {
  case TASK_NOTIFY_ACTION_INC:
    pt->notify_val++;
    break;
    
  case TASK_NOTIFY_ACTION_SETB:
    pt->notify_val |= POP_DATA_STACK;
    break;

  case TASK_NOTIFY_ACTION_SETF:
    pt->notify_val = POP_DATA_STACK;
    break;

  case TASK_NOTIFY_ACTION_SET:
    val = POP_DATA_STACK;
    
    if(pt->flags & TASK_NOTIFY_PENDING_FLAG) {
      ret = FFALSE;
    }
    else {      
      pt->notify_val = val;
      ret = FTRUE;
    }
    
    break;
  }

  pthread_cond_signal(&pt->notify_cond);
  pthread_mutex_unlock(&pt->notify_mux);

  if(action & TASK_NOTIFY_ACTION_QUERY) {
    PUSH_DATA_STACK(prev_val);
  }

  if((action & TASK_NOTIFY_ACTION_MASK) == TASK_NOTIFY_ACTION_SET) {
    PUSH_DATA_STACK(ret);
  }
}


static void _taskNotifyWait( pthreadTT_t *pt, cell_t timeout_ms ) {
  if(timeout_ms < 0) {
    pthread_cond_wait(&pt->notify_cond, &pt->notify_mux);
  }
  else {
    struct timespec t;

    ms2abs(timeout_ms, &t);        
    pthread_cond_timedwait(&pt->notify_cond, &pt->notify_mux, &t);
  }
}


cell_t cfTaskNotifyTake  ( DL_TASK cell_t timeout_ms ) {
  cfTaskToken_t *tt;
  pthreadTT_t *pt;
  cell_t val;

  tt = (cfTaskToken_t *) lTask->self;
  pt = (pthreadTT_t *) tt->data;

  pthread_mutex_lock(&pt->notify_mux);

  if(pt->flags & TASK_NOTIFY_PENDING_FLAG) {
    pthread_cond_wait(&pt->notify_cond, &pt->notify_mux);
    pt->flags &= ~TASK_NOTIFY_PENDING_FLAG;
  }    

  if(pt->notify_val == 0) {
    _taskNotifyWait(pt, timeout_ms);
    pt->flags &= ~TASK_NOTIFY_PENDING_FLAG;
  }
  
  val = pt->notify_val;
  pt->notify_val = 0;
  
  pthread_mutex_unlock(&pt->notify_mux);
  
  return val;
}


cell_t cfTaskNotifyTakeD ( DL_TASK cell_t timeout_ms ) {
  cfTaskToken_t *tt;
  pthreadTT_t *pt;
  cell_t ret;

  tt = (cfTaskToken_t *) lTask->self;
  pt = (pthreadTT_t *) tt->data;

  pthread_mutex_lock(&pt->notify_mux);

  if(timeout_ms < 0) {
    pthread_cond_wait(&pt->notify_cond, &pt->notify_mux);
  }
  else {
    struct timespec t;

    ms2abs(timeout_ms, &t);        
    pthread_cond_timedwait(&pt->notify_cond, &pt->notify_mux, &t);
  }

  if(pt->flags & TASK_NOTIFY_PENDING_FLAG) {
    pt->flags &= ~TASK_NOTIFY_PENDING_FLAG;
    pt->notify_val--;
    ret = FTRUE;
  }
  else ret = FFALSE;

  pthread_mutex_unlock(&pt->notify_mux);
  
  return ret;
}


cell_t cfTaskNotifyWait  ( DL_TASK cell_t exit_clr_mask, cell_t entry_clr_mask, cell_t timeout_ms ) {
  cfTaskToken_t *tt;
  pthreadTT_t *pt;
  cell_t ret;

  tt = (cfTaskToken_t *) lTask->self;
  pt = (pthreadTT_t *) tt->data;

  pthread_mutex_lock(&pt->notify_mux);

  pt->notify_val &= ~entry_clr_mask;

  if(timeout_ms < 0) {
    pthread_cond_wait(&pt->notify_cond, &pt->notify_mux);
  }
  else {
    struct timespec t;

    ms2abs(timeout_ms, &t);        
    pthread_cond_timedwait(&pt->notify_cond, &pt->notify_mux, &t);
  }

  if(pt->flags & TASK_NOTIFY_PENDING_FLAG) {
    pt->flags &= ~TASK_NOTIFY_PENDING_FLAG;
    PUSH_DATA_STACK(pt->notify_val);
    pt->notify_val &= ~exit_clr_mask;
    ret = FTRUE;
  }
  else ret = FFALSE;

  pthread_mutex_unlock(&pt->notify_mux);
  
  return ret;
}


cell_t cfTaskNotifyClear ( DL_TASK cell_t TT ) {
  cfTaskToken_t *tt;
  pthreadTT_t *pt;
  cell_t ret;

  tt = (cfTaskToken_t *) TT;
  pt = (pthreadTT_t *) tt->data;

  pthread_mutex_lock(&pt->notify_mux);

  if(pt->flags & TASK_NOTIFY_PENDING_FLAG) {
    pthread_cond_wait(&pt->notify_cond, &pt->notify_mux);
    pt->flags &= ~TASK_NOTIFY_PENDING_FLAG;
    ret = FTRUE;
  }
  else ret = FFALSE;

  pthread_mutex_unlock(&pt->notify_mux);
  
  return ret;
}

#endif /* PF_TASK_NOTIFY */



#endif /* PF_LOCALIZE_TASK_STACKS */

