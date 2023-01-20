/* @(#) pforth.h 98/01/26 1.2 */
#ifndef _pforth_h
#define _pforth_h

/***************************************************************
** Include file for pForth, a portable Forth based on 'C'
**
** This file is included in any application that uses pForth as a library.
**
** Author: Phil Burk
** Copyright 1994 3DO, Phil Burk, Larry Polansky, David Rosenboom
**
** Permission to use, copy, modify, and/or distribute this
** software for any purpose with or without fee is hereby granted.
**
** THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
** WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
** WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL
** THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
** CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING
** FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
** CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
** OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
**
**
***************************************************************/

/* Define stubs for data types so we can pass pointers but not touch inside. */
typedef void *PForthTask;
typedef void *PForthDictionary;

#include <stdint.h>
/* Integer types for Forth cells, signed and unsigned: */
typedef intptr_t cell_t;
typedef uintptr_t ucell_t;

typedef ucell_t ExecToken;              /* Execution Token */
typedef cell_t ThrowCode;

#ifdef __cplusplus
extern "C" {
#endif


#ifdef PF_LOCALIZE_TASK_STACKS

#define L_TASK        lTask,
#define L_TASK_VOID   lTask
#define L_TASK_TOUCH  (void) lTask
#define DL_TASK       pfLTaskData_t *lTask,
#define DL_TASK_VOID  pfLTaskData_t *lTask
#define TD_STACK_BASE lTask->td_StackBase
#define TD_STACK_PTR  lTask->td_StackPtr
#define TD_STACK_LIMIT  lTask->td_StackLimit
#define TD_RETURN_BASE lTask->td_ReturnBase
#define TD_RETURN_PTR  lTask->td_ReturnPtr
#define TD_RETURN_LIMIT  lTask->td_ReturnLimit
#define TD_FLOAT_STACK_BASE lTask->td_FloatStackBase
#define TD_FLOAT_STACK_PTR  lTask->td_FloatStackPtr
#define gScratch       lTask->Scratch
#define gVarBase       lTask->Base
typedef struct pfLTaskData_s pfLTaskData_t;

#define PF_TASK_DATA_T pfLTaskData_t
#define PF_CREATE_TASK pfCreateLocalTask
#define PF_DELETE_TASK pfDeleteLocalTask

#ifndef PF_DEFAULT_TASK_STACK_DEPTH
#define PF_DEFAULT_TASK_STACK_DEPTH (64)
#endif

#ifndef PF_DEFAULT_TASK_RETURN_DEPTH
#ifndef PF_DEFAULT_TASK_STACK_DEPTH
#define PF_DEFAULT_TASK_RETURN_DEPTH 64
#else
#define PF_DEFAULT_TASK_RETURN_DEPTH PF_DEFAULT_TASK_STACK_DEPTH
#endif
#endif
  
#else /* PF_LOCALIZE_TASK_STACKS */

#define L_TASK
#define L_TASK_VOID
#define L_TASK_TOUCH
#define DL_TASK
#define DL_TASK_VOID  void
#define TD_STACK_BASE gCurrentTask->td_StackBase
#define TD_STACK_PTR  gCurrentTask->td_StackPtr
#define TD_STACK_LIMIT  gCurrentTask->td_StackLimit
#define TD_RETURN_BASE gCurrentTask->td_ReturnBase
#define TD_RETURN_PTR  gCurrentTask->td_ReturnPtr
#define TD_RETURN_LIMIT  gCurrentTask->td_ReturnLimit
#define TD_FLOAT_STACK_BASE gCurrentTask->td_FloatStackBase
#define TD_FLOAT_STACK_PTR  gCurrentTask->td_FloatStackPtr

#define PF_TASK_DATA_T pfTaskData_t
#define PF_CREATE_TASK pfCreateTask
#define PF_DELETE_TASK pfDeleteTask

#endif /* PF_LOCALIZE_TASK_STACKS */

  
/* Main entry point to pForth. */
ThrowCode pfDoForth( const char *DicName, const char *SourceName, cell_t IfInit );

/* Turn off messages. */
void  pfSetQuiet( cell_t IfQuiet );

/* Query message status. */
cell_t  pfQueryQuiet( void );

/* Send a message using low level I/O of pForth */
void  pfMessage( const char *CString );

int pfCreateTaskStacks( PForthTask task, cell_t UserStackDepth, cell_t ReturnStackDepth );
void pfDeleteTaskStacks( PForthTask task );

#ifdef PF_LOCALIZE_TASK_STACKS
PForthTask pfCreateLocalTask( cell_t UserStackDepth, cell_t ReturnStackDepth );
void  pfDeleteLocalTask( PForthTask task );
#endif

/* Create a task used to maintain context of execution. */
PForthTask pfCreateTask( cell_t UserStackDepth, cell_t ReturnStackDepth );

/* Establish this task as the current task. */
void  pfSetCurrentTask( PForthTask task );

/* Delete task created by pfCreateTask */
void  pfDeleteTask( PForthTask task );

/* Build a dictionary with all the basic kernel words. */
PForthDictionary pfBuildDictionary( DL_TASK cell_t HeaderSize, cell_t CodeSize );

/* Create an empty dictionary. */
PForthDictionary pfCreateDictionary( cell_t HeaderSize, cell_t CodeSize );

/* Load dictionary from a file. */
PForthDictionary pfLoadDictionary( DL_TASK const char *FileName, ExecToken *EntryPointPtr );

/* Load dictionary from static array in "pfdicdat.h". */
PForthDictionary pfLoadStaticDictionary( DL_TASK_VOID );

/* Delete dictionary data. */
void  pfDeleteDictionary( PForthDictionary dict );

/* Execute the pForth interpreter. Yes, QUIT is an odd name but it has historical meaning. */
ThrowCode pfQuit( DL_TASK_VOID );

/* Execute a single execution token in the current task and return 0 or an error code. */
ThrowCode pfCatch( DL_TASK ExecToken XT );

/* Include the given pForth source code file. */
ThrowCode pfIncludeFile( DL_TASK const char *FileName );

/* Execute a Forth word by name. */
ThrowCode  pfExecIfDefined( DL_TASK const char *CString );

#ifdef __cplusplus
}
#endif

#endif  /* _pforth_h */
