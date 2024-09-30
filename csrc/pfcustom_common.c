/* @(#) pfcustom_common.c 2021/12/30 1.0 */

/***************************************************************
** Call Custom Functions for pForth
**
** See Step 1-3 comments below to use this
**
** Using this, you could, for example, call X11 from Forth.
** See "pf_cglue.c" for more information.
** Based on original pfcustom.c to provide much easier way
** to setup custom words in C.
**
** Author: Phil Burk    - pforth
** Author: Ivan Priesol - pforth_mt
** Copyright 1994 3DO, Phil Burk, Larry Polansky, David Rosenboom
** Copyright 2022 Ivan Priesol
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
**
***************************************************************/

/****************************************************************
**
** OBSOLETE (also original pfcustom.c) - use pfcustom.h instead
** 
****************************************************************/


/****************************************************************
** Step 1: Create a header file with custom function prototypes
**         and descriptors using CFW macro
****************************************************************/

/* Example:

#ifdef INCLUDE_CUSTOM_PROTOTYPES
#include "something_required_for_constants_below"
extern cell_t myvar;
#endif

CFW1(ID_CTEST0, CTest0, "CTEST0", C_RETURNS_VALUE, Val)
CFW2(ID_CTEST1, CTest1, "CTEST1", C_RETURNS_VOID, Val1, Val2)
CFWV(ID_CTESTC1, CTestC1, "CTESTCONSTANT1", SOME_CONSTANT)
CFWV(ID_CTESTV1, CTestV1, "CTESTVARIABLE1", myvar)
*/

/****************************************************************
** Step 2: Create a custom user function file e.g.pfcustom.c
**         With this file included at the and of it
****************************************************************/

/* Example:

#include "pf_all.h"

cell_t CTest0( cell_t Val )
{
    MSG_NUM_D("CTest0: Val = ", Val);
    return Val+1;
}

void CTest1( cell_t Val1, cell_t Val2 )
{

    MSG("CTest1: Val1 = "); ffDot(L_TASK Val1);
    MSG_NUM_D(", Val2 = ", Val2);
}

*/


/****************************************************************
** Step 3: Recompile using compiler option with file from Step 1
**         e.g. -DPF_USER_CUSTOM='"pfcustom.h"'
**         and link file from Step 2 with your code.
**         Then rebuild the Forth using "pforth -i system.fth"
**         Test:   10 Ctest0 ( should print message then '11' )
****************************************************************/

#ifndef PFCUSTOM_COMMON_C
#define PFCUSTOM_COMMON_C

#ifdef PF_USER_CUSTOM

#define _pf_c_glue_h
#include "pf_all.h"

#define C_RETURNS_VALUE cell_t
#define C_RETURNS_VOID  void

#define INCLUDE_CUSTOM_PROTOTYPES
#define CFWV(idx,f,w,v)           cell_t f( DL_TASK_VOID );
#define CFWV0(idx,f,w,v)          void   f( DL_TASK_VOID );
#define CFWV2(idx,f,w,v1,v2)      cell_t f( DL_TASK_VOID );
#define CFW0(idx,f,w,r)                r f( DL_TASK_VOID );
#define CFW1(idx,f,w,r,p1)             r f( DL_TASK cell_t p1);
#define CFW2(idx,f,w,r,p1,p2)          r f( DL_TASK cell_t p1, cell_t p2 );
#define CFW3(idx,f,w,r,p1,p2,p3)       r f( DL_TASK cell_t p1, cell_t p2, cell_t p3 );
#define CFW4(idx,f,w,r,p1,p2,p3,p4)    r f( DL_TASK cell_t p1, cell_t p2, cell_t p3, cell_t p4 );
#define CFW5(idx,f,w,r,p1,p2,p3,p4,p5) r f( DL_TASK cell_t p1, cell_t p2, cell_t p3, cell_t p4, cell_t p5 );

#include PF_USER_CUSTOM
#undef INCLUDE_CUSTOM_PROTOTYPES

#undef C_RETURNS_VALUE
#undef C_RETURNS_VOID
#undef _pf_c_glue_h
#include "pf_cglue.h"

/* #include "pf_all.h" */

#undef CFWV
#undef CFWV0
#undef CFWV2
#undef CFW0
#undef CFW1
#undef CFW2
#undef CFW3
#undef CFW4
#undef CFW5

#define CFWV(idx,f,w,v) idx,
#define CFWV0(idx,f,w,v)     CFWV(idx,f,w,v)
#define CFWV2(idx,f,w,v1,v2) CFWV(idx,f,w,v1)

#define CFW0(idx,f,w,r)                CFWV(idx,f,w,0)
#define CFW1(idx,f,w,r,p1)             CFWV(idx,f,w,0)
#define CFW2(idx,f,w,r,p1,p2)          CFWV(idx,f,w,0)
#define CFW3(idx,f,w,r,p1,p2,p3)       CFWV(idx,f,w,0)
#define CFW4(idx,f,w,r,p1,p2,p3,p4)    CFWV(idx,f,w,0)
#define CFW5(idx,f,w,r,p1,p2,p3,p4,p5) CFWV(idx,f,w,0)


enum pforth_custom_func_ids
{
  _ID_CUST_INIT_ = -1,
  #include PF_USER_CUSTOM
  NUM_CUSTOM_FUNCS
};

#undef CFWV

#ifdef PF_NO_GLOBAL_INIT
#define CFWV(i,f,w,v) CustomFunctionTable[i] = (void *) f;

/* CFunc0 CustomFunctionTable[NUM_CUSTOM_FUNCS]; */
void *CustomFunctionTable[NUM_CUSTOM_FUNCS];

Err LoadCustomFunctionTable( ) { }
{
  #include "pfcustom.h"
  return 0;
}

#else /* #ifdef PF_NO_GLOBAL_INIT */
#define CFWV(i,f,w,v) (void *) f,

CFunc0 CustomFunctionTable[] =
{  
  #include PF_USER_CUSTOM
  NULL
};
#endif /* #ifdef PF_NO_GLOBAL_INIT */

#if (!defined(PF_NO_INIT)) && (!defined(PF_NO_SHELL))

#undef CFWV
#undef CFWV0
#undef CFWV2
#undef CFW0
#undef CFW1
#undef CFW2
#undef CFW3
#undef CFW4
#undef CFW5

#define CFW(idx,f,w,r,p)	           \
  err = CreateGlueToC(w, idx, r, p); \
  if( err < 0 ) return err;

#define CFWV(idx,f,w,v)	               CFW(idx,f,w,C_RETURNS_VALUE,0)
#define CFWV0(idx,f,w,v)	             CFW(idx,f,w,C_RETURNS_VOID,0)
#define CFWV2(idx,f,w,v1,v2)	         CFW(idx,f,w,C_RETURNS_VALUE,0)
#define CFW0(idx,f,w,r)	               CFW(idx,f,w,r,0)
#define CFW1(idx,f,w,r,p1)	           CFW(idx,f,w,r,1)
#define CFW2(idx,f,w,r,p1,p2)	         CFW(idx,f,w,r,2)
#define CFW3(idx,f,w,r,p1,p2,p3)	     CFW(idx,f,w,r,3)
#define CFW4(idx,f,w,r,p1,p2,p3,p4)	   CFW(idx,f,w,r,4)
#define CFW5(idx,f,w,r,p1,p2,p3,p4,p5) CFW(idx,f,w,r,5)

Err CompileCustomFunctions( )
{
  Err err;
  #include PF_USER_CUSTOM
  return 0;
}

#else /* #if (!defined(PF_NO_INIT)) && (!defined(PF_NO_SHELL)) */
Err CompileCustomFunctions( ) {
  return 0;
}
#endif /* #if (!defined(PF_NO_INIT)) && (!defined(PF_NO_SHELL)) */

#undef CFWV
#undef CFWV0
#undef CFWV2
#undef CFW

#define CFW(idx,f,w,r,p)
#define CFWV(idx,f,w,v)      cell_t f( DL_TASK_VOID ) { return (cell_t) (v); }
#define CFWV0(idx,f,w,v)     void   f( DL_TASK_VOID ) { v; }
#define CFWV2(idx,f,w,v1,v2) cell_t f( DL_TASK_VOID ) { PUSH_DATA_STACK(v1); return (cell_t) (v2); }

#include PF_USER_CUSTOM
#endif /* PF_USER_CUSTOM */

#endif /* PFCUSTOM_COMMON_C */ 
