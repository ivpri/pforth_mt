/* @(#) cf_task.c 20190728 */

/***************************************************************
** TASK related common functions for pforth preemptive mutlitasking
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

#include "pf_all.h"
#include "cf_task.h"

#ifdef PF_LOCALIZE_TASK_STACKS


void ffStack2Stack( pfLTaskData_t *src, pfLTaskData_t *dst, cell_t n ) {
  cell_t i;

  for(i = n; i > 0; i--) {
    *(--dst->td_StackPtr) = *(src->td_StackPtr + i - 1);
  }

  src->td_StackPtr += n;
}


/* Create new task data structure and allocate resources.
   If n > 0 also copy n items from the caller stack to the task stack
 */
ThrowCode ffTaskNew ( DL_TASK cell_t n, cfTaskToken_t **ttptr ) {
  cfTaskToken_t *TT;

  TT = malloc(sizeof(cfTaskToken_t));
  
  if(TT) { 
    TT->lTask = pfCreateLocalTask( PF_DEFAULT_TASK_STACK_DEPTH, PF_DEFAULT_TASK_RETURN_DEPTH );
    TT->flags = 0;
    TT->lTask->self = *ttptr = TT;
  }

  if(TT && TT->lTask) {
    if(n > 0) 
      ffStack2Stack( lTask, TT->lTask, n );

    return 0;    
  }
  else {
    if(TT) {
      ffTaskFree( TT );
    }
    
    return -59;
  }
}


/* Cleanup the task memory resources */
void ffTaskFree( cfTaskToken_t *tt ) {
  if(tt->lTask) {
    pfDeleteLocalTask(tt->lTask);
  }

  free(tt);
}


/* Expect the task TOS contanins exception code (0 if no exception).
   If n > 0 also copy the top n items from the task stack to the callers stack.
   Finally free the task data.
 */
ThrowCode ffTaskJoin( DL_TASK cell_t n, cfTaskToken_t *TT ) {
  ThrowCode excp;

  excp = POP_TASK_STACK();

  if(excp == 0 && n > 0) { 
    ffStack2Stack( TT->lTask, lTask, n );
  }
  
  ffTaskFree(TT);
  
  return excp;
}


#endif /* PF_LOCALIZE_TASK_STACKS */
