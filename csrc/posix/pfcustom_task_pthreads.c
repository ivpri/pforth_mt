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

#ifdef PFCUSTOM_DEFS
#include "cf_task_pthreads.c"
#endif


#include "../cf_task.h"

/* W2(NEWTASK,        "TASK.NEW",       n, XT, */
W21(NEWTASK,        "TASK",       n, XT, 
    cell_t ret;

    SAVE_REGISTERS; 
    ret = cfNewTask(L_TASK n, XT);
    LOAD_REGISTERS;
    TOS = ret;
)

W31(TASK_ON_CPU,  "TASK.ON-CPU",  n, XT, cpu,
    cell_t ret;
    
    SAVE_REGISTERS; 
    ret = cfNewTaskOnCpu(L_TASK n, XT, cpu);
    LOAD_REGISTERS;
    TOS = ret;
)

W1(TASK_DETACH,  "TASK.DETACH",  TT, cfDetachTask(L_TASK TT))
W1(TASK_CANCEL,  "TASK.CANCEL",  TT, cfCancelTask(L_TASK TT))

W2(TASK_JOIN,    "TASK.JOINx",    n, TT,
    cell_t excp;
    
    SAVE_REGISTERS; 
    excp = cfJoinTask(L_TASK n, TT);
    LOAD_REGISTERS;

    if(excp != 0) M_THROW(excp);
)

W11(TASK_ISDONE, "TASK.DONE?", TT, 
  cell_t ret;
  
  SAVE_REGISTERS; 
  ret = cfTaskIsDone(L_TASK TT);
  LOAD_REGISTERS;
  TOS = ret;
)


WC(TASK_CPU,     "TASK.CPU", cfTaskCPU(L_TASK_VOID))

W2(TASK_SETCPU,  "TASK.SETCPU",  mask, TT, cfTaskSetCPU(L_TASK mask, TT))
W2(TASK_SETPRI,  "TASK.SETPRI",  pri, TT, cfTaskSetPri(L_TASK pri, TT))
