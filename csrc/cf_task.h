/* @(#) cf_task.h 20190728 */

/***************************************************************
** TASK related prototypes for pforth preemptive mutlitasking
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

#ifndef _cf_task_h
#define _cf_task_h


#ifdef PF_LOCALIZE_TASK_STACKS

#define TASK_DETACH_FLAG 1
#define TASK_DONE_FLAG 2

#define TASK_LTASK         ((cfTaskToken_t *) TT)->lTask
#define TASK_FLAGS         ((cfTaskToken_t *) TT)->flags

#define TD_TASK_STACK_BASE TASK_LTASK->td_StackBase
#define TD_TASK_STACK_PTR  TASK_LTASK->td_StackPtr
#define TASK_STACK_DEPTH   (TD_TASK_STACK_BASE - TD_TASK_STACK_PTR)

#define PUSH_TASK_STACK(x) *(--TD_TASK_STACK_PTR) = (cell_t) (x)
#define POP_TASK_STACK()   (*(TD_TASK_STACK_PTR++))

typedef struct cfTaskToken_s {
  pfLTaskData_t *lTask;
  void          *data; /* Additional data allocated by driver */
  uint8_t       flags;
} cfTaskToken_t;

void ffStack2Stack( pfLTaskData_t *src, pfLTaskData_t *dst, cell_t n );

void      ffTaskFree  ( cfTaskToken_t *tt );
ThrowCode ffTaskNew   ( DL_TASK cell_t n, cfTaskToken_t **ttptr );
ThrowCode ffTaskJoin  ( DL_TASK cell_t n, cfTaskToken_t *tt );


#ifdef PF_TASK_NOTIFY

#define TASK_NOTIFY_ACTION_MASK  0x7
#define TASK_NOTIFY_ACTION_NA    0x0
#define TASK_NOTIFY_ACTION_INC   0x1
#define TASK_NOTIFY_ACTION_SET   0x2
#define TASK_NOTIFY_ACTION_SETF  0x3
#define TASK_NOTIFY_ACTION_SETB  0x4
#define TASK_NOTIFY_ACTION_QUERY 0x8

#endif


#ifdef PF_TASK_QUEUE

#endif



#endif /* PF_LOCALIZE_TASK_STACKS */

#endif /* #ifndef _cf_task_h */