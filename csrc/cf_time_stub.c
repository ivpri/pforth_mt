/* @(#) cf_time_timerfd.c 20190728 */

/***************************************************************
** Timing related functions - stub functions
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

#ifdef PF_MS_TIME

#include "pf_all.h"
#include "cf_time.h"

void cfMilliSeconds ( DL_TASK cell_t ms ) {
  L_TASK_TOUCH;
  TOUCH(ms);
  UNIMPLEMENTED("cfMilliSeconds");
}

#endif /* PF_MS_TIME */

