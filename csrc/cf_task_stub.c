/* @(#) posix/cf_task_pthreads.c 20190728 */

/***************************************************************
** TASK stub functions 
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

#include "pf_all.h"
#include "cf_task.h"

cell_t cfNewTask( DL_TASK cell_t n, cell_t XT ) {
  L_TASK_TOUCH;
  TOUCH(n);
  TOUCH(XT);

  UNIMPLEMENTED("cfNewTask");
  
  return THROW_ABORT;
}

cell_t cfNewTaskOnCpu ( DL_TASK cell_t n, cell_t XT, cell_t cpu );
  L_TASK_TOUCH;
  TOUCH(n);
  TOUCH(XT);
  TOUCH(cpu);

  UNIMPLEMENTED("cfNewTaskOnCpu");
  
  return THROW_ABORT;
}


/* Mark the task to automatically free it's memory resources 
   when finished. An ambiguous condition occures in case of
   any further usage of TT.
 */
void cfDetachTask( DL_TASK cell_t TT ) {
  L_TASK_TOUCH;
  TOUCH(TT);

  UNIMPLEMENTED("cfDetachTask");
}


/* Block until the task is completed then take top n task's stack
   items and release task's memory.
   An ambiguous condition occures in case of multiple simultanous
   join requests.
 */
cell_t cfJoinTask ( DL_TASK cell_t n, cell_t TT ) {
  L_TASK_TOUCH;
  TOUCH(n);
  TOUCH(TT);

  UNIMPLEMENTED("cfJoinTask");
  
  return THROW_ABORT;
}


/* Try to cancel and finish the task as soon as posible and detatch it.
 */
void   cfCancelTask ( DL_TASK cell_t TT ) {
}


cell_t cfTaskIsDone ( DL_TASK cell_t TT ) {
  L_TASK_TOUCH;
  TOUCH(TT);

  UNIMPLEMENTED("cfTaskIsDone");
  
  return FFALSE;
}


cell_t cfTaskCPU    ( DL_TASK_VOID ) {
  L_TASK_TOUCH;

  UNIMPLEMENTED("cfTaskCPU");
  
  return -1;
}


void   cfTaskSetCPU ( DL_TASK cell_t mask, cell_t TT ) {
  L_TASK_TOUCH;
  TOUCH(mask);
  TOUCH(TT);

  UNIMPLEMENTED("cfTaskSetCPU");
}


void   cfTaskSetPri ( DL_TASK cell_t pri,  cell_t TT ) {
  L_TASK_TOUCH;
  TOUCH(pri);
  TOUCH(TT);

  UNIMPLEMENTED("cfTaskSetPri");
}


void   cfYieldTask  ( DL_TASK_VOID ) {
  L_TASK_TOUCH;
  UNIMPLEMENTED("cfYieldTask");
}



#ifdef PF_TASK_SUSPEND

void   cfSuspendTask ( DL_TASK cell_t TT ) {
  L_TASK_TOUCH;
  TOUCH(TT);

  UNIMPLEMENTED("cfSuspendTask");
}


void   cfResumeTask ( DL_TASK cell_t TT ) {
  L_TASK_TOUCH;
  TOUCH(TT);

  UNIMPLEMENTED("cfResumeTask");
}


void   cfResumeTaskISR ( DL_TASK cell_t TT ) {
  L_TASK_TOUCH;
  TOUCH(TT);

  UNIMPLEMENTED("cfResumeTaskISR");
}


void   cfSuspendAll ( DL_TASK_VOID ) {
  L_TASK_TOUCH;
  UNIMPLEMENTED("cfSuspendAll");
}


void   cfResumeAll ( DL_TASK_VOID ) {
  L_TASK_TOUCH;
  UNIMPLEMENTED("cfResumeAll");
}

#endif /* PF_TASK_SUSPEND */


#ifdef PF_TASK_SEMAPHORE


cell_t cfSemaphore ( DL_TASK cell_t max_count, cell_t init ) {
  L_TASK_TOUCH;
  TOUCH(init);
  TOUCH(max_count);
  UNIMPLEMENTED("cfSemaphore");

  return THROW_ABORT;
}


cell_t cfSemaphoreMutex ( DL_TASK_VOID ) {
  L_TASK_TOUCH;
  UNIMPLEMENTED("cfSemaphoreMutex");

  return THROW_ABORT;
}


cell_t cfSemaphoreMutexRecursive ( DL_TASK_VOID ) {
  L_TASK_TOUCH;
  UNIMPLEMENTED("cfSemaphoreMutexRecursive");

  return THROW_ABORT;
}


void   cfSemaphoreDelete ( DL_TASK cell_t ST ) {
  L_TASK_TOUCH;
  TOUCH(ST);
  UNIMPLEMENTED("cfSemaphoreDelete");
}


cell_t cfSemaphoreGetCount ( DL_TASK cell_t ST ) {
  L_TASK_TOUCH;
  TOUCH(ST);
  UNIMPLEMENTED("cfSemaphoreGetCount");

  return -1;
}


cell_t cfSemaphoreTake ( DL_TASK cell_t timeout_ms, cell_t ST ) {
  L_TASK_TOUCH;
  TOUCH(ST);
  TOUCH(timeout_ms);
  UNIMPLEMENTED("cfSemaphoreTake");

  return FFALSE;
}


void   cfSemaphoreGive ( DL_TASK cell_t ST ) {
  L_TASK_TOUCH;
  TOUCH(ST);
  UNIMPLEMENTED("cfSemaphoreGive");
}


void   cfSemaphoreGiveISR ( DL_TASK cell_t ST ) {
  L_TASK_TOUCH;
  TOUCH(ST);
  UNIMPLEMENTED("cfSemaphoreGiveISR");
}


cell_t cfSemaphoreTakeISR ( DL_TASK cell_t ST ) {
  L_TASK_TOUCH;
  TOUCH(ST);
  UNIMPLEMENTED("cfSemaphoreTakeISR");
  
  return FFALSE;
}

#endif /* PF_TASK_SEMAPHORE */

#ifdef PF_TASK_NOTIFY

void cfTaskNotifyISR   ( DL_TASK cell_t action, cell_t TT ) {
  L_TASK_TOUCH;
  TOUCH(TT);
  TOUCH(action);
  UNIMPLEMENTED("cfTaskNotifyISR");
}


void cfTaskNotify      ( DL_TASK cell_t action, cell_t TT ) {
  L_TASK_TOUCH;
  TOUCH(TT);
  TOUCH(action);
  UNIMPLEMENTED("cfTaskNotify");
}


cell_t cfTaskNotifyTake  ( DL_TASK cell_t timeout_ms ) {
  L_TASK_TOUCH;
  TOUCH(timeout_ms);
  UNIMPLEMENTED("cfTaskNotifyTake");

  return FFALSE;
}


cell_t cfTaskNotifyTakeD ( DL_TASK cell_t timeout_ms ) {
  L_TASK_TOUCH;
  TOUCH(timeout_ms);
  UNIMPLEMENTED("cfTaskNotifyTakeD");

  return FFALSE;
}


cell_t cfTaskNotifyWait  ( DL_TASK cell_t exit_clr_mask, cell_t entry_clr_mask, cell_t timeout_ms ) {
  L_TASK_TOUCH;
  TOUCH(timeout_ms);
  TOUCH(entry_clr_mask);
  TOUCH(exit_clr_mask);
  UNIMPLEMENTED("cfTaskNotifyWait");

  return FFALSE;
}


cell_t cfTaskNotifyClear ( DL_TASK cell_t TT ) {
  L_TASK_TOUCH;
  TOUCH(TT);
  UNIMPLEMENTED("cfTaskNotifyClear");

  return FFALSE;
}

#endif /* PF_TASK_NOTIFY */



#endif /* PF_LOCALIZE_TASK_STACKS */
