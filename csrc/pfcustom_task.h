#ifdef INCLUDE_CUSTOM_PROTOTYPES
/* #include "cf_task.h" */
#endif

#ifdef PF_LOCALIZE_TASK_STACKS

/* idx, func_name, word, return, num_params */
CFW2(ID_TASK,         cfNewTask,      "TASK",         C_RETURNS_VALUE, n, XT)
CFW3(ID_TASK_ON_CPU,  cfNewTaskOnCpu, "TASK.ON-CPU",  C_RETURNS_VALUE, n, XT, cpu)
CFW1(ID_TASK_DETACH,  cfDetachTask,   "TASK.DETACH",  C_RETURNS_VOID,  TT)
CFW2(ID_TASK_JOIN,    cfJoinTask,     "TASK.JOIN",    C_RETURNS_VALUE, n, TT)
CFW1(ID_TASK_CANCEL,  cfCancelTask,   "TASK.CANCEL",  C_RETURNS_VOID,  TT)
CFW1(ID_TASK_ISDONE,  cfTaskIsDone,   "TASK.DONE?",   C_RETURNS_VALUE, TT)
CFW0(ID_TASK_CPU,     cfTaskCPU,      "TASK.CPU",     C_RETURNS_VALUE)
CFW2(ID_TASK_SETCPU,  cfTaskSetCPU,   "TASK.SETCPU",  C_RETURNS_VOID,  mask, TT)
CFW2(ID_TASK_SETPRI,  cfTaskSetPri,   "TASK.SETPRI",  C_RETURNS_VOID,  pri, TT)

#ifdef PF_TASK_SUSPEND
CFW1(ID_TASK_SUSPEND,     cfSuspendTask,   "TASK.SUSPEND",     C_RETURNS_VOID,  TT)
CFW1(ID_TASK_RESUME,      cfResumeTask,    "TASK.RESUME",      C_RETURNS_VOID,  TT)
CFW1(ID_TASK_RESUME_ISR,  cfResumeTaskISR, "TASK.RESUME-ISR",  C_RETURNS_VALUE, TT)
CFW0(ID_TASK_SUSPEND_ALL, cfSuspendAll,    "TASK.SUSPEND-ALL", C_RETURNS_VOID)
CFW0(ID_TASK_RESUME_ALL,  cfResumeAll,     "TASK.RESUME-ALL",  C_RETURNS_VOID)
#endif

#ifdef PF_TASK_SEMAPHORE
CFW2(ID_SEMP,           cfSemaphore,               "SEMP",          C_RETURNS_VALUE, max_count, init)
CFW0(ID_SEMP_MUTEX,     cfSemaphoreMutex,          "SEMP.MUTEX",    C_RETURNS_VALUE)
CFW0(ID_SEMP_MUTEX_REC, cfSemaphoreMutexRecursive, "SEMP.MUTEX-R",  C_RETURNS_VALUE)
CFW1(ID_SEMP_DELETE,    cfSemaphoreDelete,         "SEMP.DELETE",   C_RETURNS_VOID,  ST)
CFW1(ID_SEMP_GET_COUNT, cfSemaphoreGetCount,       "SEMP.COUNT@",   C_RETURNS_VALUE, ST)
CFW2(ID_SEMP_TAKE,      cfSemaphoreTake,           "SEMP.TAKE",     C_RETURNS_VALUE, timeout_ms, ST)
CFW1(ID_SEMP_GIVE,      cfSemaphoreGive,           "SEMP.GIVE",     C_RETURNS_VOID,  ST)
CFW1(ID_SEMP_TAKE_ISR,  cfSemaphoreTakeISR,        "SEMP.TAKE-ISR", C_RETURNS_VALUE, ST)
CFW1(ID_SEMP_GIVE_ISR,  cfSemaphoreGiveISR,        "SEMP.GIVE-ISR", C_RETURNS_VOID,  ST)
#endif

#ifdef PF_TASK_NOTIFY
/* Also include task_notify.fth for additional words */
CFW2(ID_TASK_NOTIFY_ISR,      cfTaskNotifyISR,   "TASK.NOTIFY-ISR",     C_RETURNS_VOID,   action, TT)
CFW2(ID_TASK_NOTIFY,          cfTaskNotify,      "TASK.NOTIFY",         C_RETURNS_VOID,   action, TT)
CFW1(ID_TASK_NOTIFY_TAKE,     cfTaskNotifyTake,  "TASK.NOTIFY-TAKE",    C_RETURNS_VALUE,  timeout_ms)
CFW1(ID_TASK_NOTIFY_TAKE_DEC, cfTaskNotifyTakeD, "TASK.NOTIFY-TAKEDEC", C_RETURNS_VALUE,  timeout_ms)
CFW3(ID_TASK_NOTIFY_WAIT,     cfTaskNotifyWait,  "TASK.NOTIFY-WAIT",    C_RETURNS_VALUE,  exit_clr_mask, entry_clr_mask, timeout_ms)
CFW1(ID_TASK_NOTIFY_CLEAR,    cfTaskNotifyClear, "TASK.NOTIFY-CLEAR",   C_RETURNS_VALUE,  TT)
#endif

#ifdef PF_TASK_QUEUE

#endif


#endif
