/* @(#) posix/cf_task_pthreads.c 20190728 */

/***************************************************************
** TASK driver functions with pthreads
**
** Author:Ivan Priesol (c) 2024
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

#include "../cf_task.h"

#include <pthread.h>
#include <errno.h>

#ifdef PFCUSTOM_DECLS

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

#endif /* #ifdef PF_TASK_SEMAPHORE */

#ifdef PF_TASK_NOTIFY
#define TASK_NOTIFY_PENDING_FLAG 1
void cfTaskNotify(DL_TASK cell_t action, cell_t TT);
#endif


cell_t cfNewTask(DL_TASK cell_t n, cell_t XT);
cell_t cfJoinTask(DL_TASK cell_t n, cell_t TT);
void   cfCancelTask(DL_TASK cell_t TT);
void   cfTaskSetCPU(ucell_t umask, cfTaskToken_t * TT);

#if defined PF_TASK_SEMAPHORE || defined PF_TASK_NOTIFY
#include <sys/time.h>
void ms2abs(cell_t ms, struct timespec *abs);
#endif

#endif /* #ifdef PFCUSTOM_DECLS */


#ifdef PFCUSTOM_DEFS

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

void ms2abs(cell_t ms, struct timespec *abs) {
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

cell_t cfJoinTask ( DL_TASK cell_t n, cell_t TT ) {
  cfTaskToken_t *tt = (cfTaskToken_t *) TT;
  pthreadTT_t *pt = (pthreadTT_t *) tt->data;
  ThrowCode excp;
  int pterr;

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
  cfTaskToken_t *tt = (cfTaskToken_t *) TT;
  pthreadTT_t *pt = (pthreadTT_t *) tt->data;
  int ret;
  void *join_ret;

  if((ret = pthread_cancel(pt->pt)) == 0) {
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

void   cfTaskSetCPU ( ucell_t umask, cfTaskToken_t * TT ) {
  cpu_set_t cpuset;
  int cpu = 0;

  CPU_ZERO(&cpuset);
  
  for(cpu = 0; umask != 0; cpu++, umask >>= 1)
    if(umask & 1)
      CPU_SET(cpu, &cpuset);
  
  pthread_setaffinity_np(((pthreadTT_t *) TT->data)->pt, sizeof(cpu_set_t), &cpuset);
}

#endif /* #ifdef PFCUSTOM_DEFS */


W2(NEWTASK,        "TASK",       n, XT, 
    cell_t ret;
    
    SAVE_REGISTERS;
    ret = cfNewTask(L_TASK n, XT);
    LOAD_REGISTERS;

    PUSH_TOS;
    TOS = ret;
)

W3(TASK_ON_CPU,  "TASK.ON-CPU",  n, XT, cpu,
    cell_t ret;
    
    SAVE_REGISTERS;
    ret = cfNewTask(L_TASK n, XT);
    LOAD_REGISTERS;

    if(ret == 0)
      cfTaskSetCPU( 1 << cpu, (cfTaskToken_t *) TOS );

    PUSH_TOS;
    TOS = ret;
)

W1(TASK_DETACH,  "TASK.DETACH",  TT,
  cfTaskToken_t *tt = (cfTaskToken_t *) TT;
  pthreadTT_t *pt = (pthreadTT_t *) tt->data;

  tt->flags |= TASK_DETACH_FLAG;
  pthread_detach(pt->pt);
)

W1(TASK_CANCEL,  "TASK.CANCEL",  TT, cfCancelTask(L_TASK TT))

W2(TASK_JOIN,    "TASK.JOIN",    n, TT,
    cell_t ret;
    SAVE_REGISTERS;
    ret = cfJoinTask(L_TASK n, TT);
    LOAD_REGISTERS;
    PUSH_TOS;
    TOS = ret;
)

W11(TASK_ISDONE, "TASK.DONE?", TT, 
  ThrowCode excp;
  cfTaskToken_t *tt = (cfTaskToken_t *) TT;

  if(tt->flags &= TASK_DONE_FLAG)
    TOS = FTRUE;
  else {
    TOS = pthread_tryjoin_np(((pthreadTT_t *) tt->data)->pt, (void **) &excp);

    if(TOS == EBUSY)
      TOS = FFALSE;
    else {
      PUSH_TASK_STACK(excp);
      tt->flags |= TASK_DONE_FLAG;
      TOS = FTRUE;
    }
  }
)



WC(TASK_CPU,    "TASK.CPU",     sched_getcpu())

W2(TASK_SETCPU, "TASK.SETCPU",  mask, TT,
  cfTaskSetCPU((ucell_t) mask, (cfTaskToken_t *) TT); 
)

W2(TASK_SETPRI, "TASK.SETPRI",  pri, TT,
  pthread_setschedprio(
    ((pthreadTT_t *) (((cfTaskToken_t *) TT)->data))->pt, (int) pri);
)

W(TASK_YELD,    "TASK.YIELD", sched_yield())


#ifdef PF_TASK_SEMAPHORE

W21(SEMP_NEW, "SEMP", max_count, init,
  cfSemaphoreToken_t *st;

  TOS = THROW_ABORT; /* TODO: malloc error */

  if((st = malloc(sizeof(cfSemaphoreToken_t))) != 0) {
    st->flags = 0;
    st->count = max_count;

    do {
      if(sem_init(&st->sem, 0, init) != 0) break;   
      if(max_count > 0) {
        if(pthread_mutexattr_init( &st->mux_attr ) != 0) break;
        if(pthread_mutexattr_settype( &st->mux_attr, PTHREAD_MUTEX_FAST_NP) != 0) break;
        if(pthread_mutex_init( &st->mux, &st->mux_attr ) != 0) break;
      }

      M_PUSH(st);
      TOS = 0;
    }
    while(0);

    if(TOS != 0) {
      TOS = -512 - errno;
      free(st);
    }
  }
)

W(SEMP_MUTEX, "SEMP.MUTEX",
  cfSemaphoreToken_t *st;

  PUSH_TOS;
  TOS = THROW_ABORT; /* TODO: malloc error */

  if((st = malloc(sizeof(cfSemaphoreToken_t))) != 0) {
    st->flags = FLAG_SEM_MUTEX;

    do {
      if(pthread_mutexattr_init( &st->mux_attr ) != 0) break;
      if(pthread_mutexattr_settype( &st->mux_attr, PTHREAD_MUTEX_FAST_NP) != 0) break;
      if(pthread_mutex_init( &st->mux, &st->mux_attr ) != 0) break;

      M_PUSH(st);
      TOS = 0;
    }
    while(0);

    if(TOS != 0) {
      TOS = -512 - errno;
      free(st);
    }
  }
)

W(SEMP_MUTEX_REC, "SEMP.MUTEX-R",
  cfSemaphoreToken_t *st;

  PUSH_TOS;
  TOS = THROW_ABORT; /* TODO: malloc error */

  if((st = malloc(sizeof(cfSemaphoreToken_t))) != 0) {
    st->flags = FLAG_SEM_MUTEX | FLAG_SEM_MUTEX_RECURSIVE;
    st->count = 0;

    do {
      if(pthread_mutexattr_init( &st->mux_attr ) != 0) break;
      if(pthread_mutexattr_settype( &st->mux_attr, PTHREAD_MUTEX_RECURSIVE_NP) != 0) break;
      if(pthread_mutex_init( &st->mux, &st->mux_attr ) != 0) break;

      M_PUSH(st);
      TOS = 0;
    }
    while(0);

    if(TOS != 0) {
      TOS = -512 - errno;
      free(st);
    }
  }
)

W1(SEMP_DELETE, "SEMP.DELETE", ST,
  cfSemaphoreToken_t *st = (cfSemaphoreToken_t *) ST;

  if((st->flags & FLAG_SEM_MUTEX) || (st->count > 0)) {
    pthread_mutex_destroy(&st->mux);
    pthread_mutexattr_destroy(&st->mux_attr);
  }
  
  if(!(st->flags & FLAG_SEM_MUTEX)) {
    sem_destroy(&st->sem);
  }

  free(st);
)

W11(SEMP_GET_COUNT, "SEMP.COUNT@", ST,
  cfSemaphoreToken_t *st = (cfSemaphoreToken_t *) ST;

  if(st->flags & FLAG_SEM_MUTEX) {
    if(st->flags & FLAG_SEM_MUTEX_RECURSIVE) {
      TOS = 1 - st->count;
    }
    else {
      TOS = pthread_mutex_trylock(&st->mux) == EBUSY ? 0 : 1;

      if(TOS == 1) {
	      pthread_mutex_unlock(&st->mux);
      }      
    }
  }
  else {
    int val = -1;

    sem_getvalue(&st->sem, &val);
    TOS = val;
  }
)

W21(SEMP_TAKE, "SEMP.TAKE", timeout_ms, ST,
  cfSemaphoreToken_t *st = (cfSemaphoreToken_t *) ST;

  if(timeout_ms < 0) {
    TOS = st->flags & FLAG_SEM_MUTEX ?
      pthread_mutex_lock(&st->mux) :
      sem_wait(&st->sem);
  }
  else if(timeout_ms == 0) {
    TOS = st->flags & FLAG_SEM_MUTEX ?
      pthread_mutex_trylock(&st->mux) :
      sem_trywait(&st->sem);
  }
  else {
    struct timespec t;

    ms2abs(timeout_ms, &t);
    
    TOS = st->flags & FLAG_SEM_MUTEX ?
      pthread_mutex_timedlock(&st->mux, &t) :
      sem_timedwait(&st->sem, &t);
  }

  if(st->flags & FLAG_SEM_MUTEX_RECURSIVE && TOS == 0) {
    st->count++;
  }

  TOS = TOS == 0 ? FTRUE : FFALSE;
)


W1(SEMP_GIVE, "SEMP.GIVE", ST,
  cfSemaphoreToken_t *st = (cfSemaphoreToken_t *) ST;

  if(st->flags & FLAG_SEM_MUTEX) {
    if(st->flags & FLAG_SEM_MUTEX_RECURSIVE) {
      st->count--;
    }

    pthread_mutex_unlock(&st->mux);    
  }
  else {
    if(st->count > 0) {
      int val;

      pthread_mutex_lock(&st->mux);
      sem_getvalue(&st->sem, &val);

      if(st->count > val)
	      sem_post(&st->sem);
      
      pthread_mutex_unlock(&st->mux);
    }
    else sem_post(&st->sem);
  }
)

#endif /* #ifdef PF_TASK_SEMAPHORE */


#ifdef PF_TASK_NOTIFY

/* Also include task_notify.fth for additional words */
/* CFW2(ID_TASK_NOTIFY_ISR,      cfTaskNotifyISR,   "TASK.NOTIFY-ISR",     C_RETURNS_VOID,   action, TT)
CFW2(ID_TASK_NOTIFY,          cfTaskNotify,      "TASK.NOTIFY",         C_RETURNS_VOID,   action, TT)
 */



/* TODO: alias */
W2(TASK_NOTIFY_ISR, "TASK.NOTIFY-ISR", action, TT, 
  (void) action; 
  (void) TT;
  goto _task_notify_start
)

W2(TASK_NOTIFY, "TASK.NOTIFY", action, TT,
  _task_notify_start:
  cfTaskToken_t *tt;
  pthreadTT_t *pt;
  cell_t val;
  cell_t prev_val;
  cell_t ret = FFALSE;

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
      val = TOS;
      M_DROP;
      
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
    PUSH_TOS;
    TOS = prev_val;
  }

  if((action & TASK_NOTIFY_ACTION_MASK) == TASK_NOTIFY_ACTION_SET) {
    PUSH_TOS;
    TOS = ret;
  }
)

W11(TASK_NOTIFY_TAKE, "TASK.NOTIFY-TAKE", timeout_ms,
  cfTaskToken_t *tt;
  pthreadTT_t *pt;

  tt = (cfTaskToken_t *) lTask->self;
  pt = (pthreadTT_t *) tt->data;

  pthread_mutex_lock(&pt->notify_mux);

  if(pt->flags & TASK_NOTIFY_PENDING_FLAG) {
    pthread_cond_wait(&pt->notify_cond, &pt->notify_mux);
    pt->flags &= ~TASK_NOTIFY_PENDING_FLAG;
  }    

  if(pt->notify_val == 0) {
    if(timeout_ms < 0) {
      pthread_cond_wait(&pt->notify_cond, &pt->notify_mux);
    }
    else {
      struct timespec t;

      ms2abs(timeout_ms, &t);        
      pthread_cond_timedwait(&pt->notify_cond, &pt->notify_mux, &t);
    }

    pt->flags &= ~TASK_NOTIFY_PENDING_FLAG;
  }
  
  TOS = pt->notify_val;
  pt->notify_val = 0;
  
  pthread_mutex_unlock(&pt->notify_mux);
)


W11(TASK_NOTIFY_TAKE_DEC, "TASK.NOTIFY-TAKEDEC", timeout_ms,
  cfTaskToken_t *tt;
  pthreadTT_t *pt;

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
    TOS = FTRUE;
  }
  else TOS = FFALSE;

  pthread_mutex_unlock(&pt->notify_mux);
)


W31(TASK_NOTIFY_WAIT, "TASK.NOTIFY-WAIT", exit_clr_mask, entry_clr_mask, timeout_ms,
  cfTaskToken_t *tt;
  pthreadTT_t *pt;

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
    M_PUSH(pt->notify_val);
    pt->notify_val &= ~exit_clr_mask;
    TOS = FTRUE;
  }
  else TOS = FFALSE;

  pthread_mutex_unlock(&pt->notify_mux);
)


W11(TASK_NOTIFY_CLEAR, "TASK.NOTIFY-CLEAR", TT,
  cfTaskToken_t *tt;
  pthreadTT_t *pt;

  tt = (cfTaskToken_t *) TT;
  pt = (pthreadTT_t *) tt->data;

  pthread_mutex_lock(&pt->notify_mux);

  if(pt->flags & TASK_NOTIFY_PENDING_FLAG) {
    pthread_cond_wait(&pt->notify_cond, &pt->notify_mux);
    pt->flags &= ~TASK_NOTIFY_PENDING_FLAG;
    TOS = FTRUE;
  }
  else TOS = FFALSE;

  pthread_mutex_unlock(&pt->notify_mux);
)



#endif /* #ifdef PF_TASK_NOTIFY */
