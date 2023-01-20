/* @(#) cf_time.h 20190728 */

/***************************************************************
** TASK related functions for pforth preemptive mutlitasking
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

void cfMilliSeconds ( DL_TASK cell_t ms );

#endif

