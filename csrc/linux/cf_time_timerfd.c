/* @(#) cf_time_timerfd.c 20190728 */

/***************************************************************
** Timing related functions - linux timerfd driver
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

#include "../pf_all.h"
#include "../cf_time.h"
#include <sys/timerfd.h>
#include <time.h>
#include <unistd.h>

void cfMilliSeconds ( DL_TASK cell_t ms ) {
  struct itimerspec t;
  int fd;
  uint64_t exp;
  ssize_t s;

  if(ms <= 0) return;

  t.it_value.tv_sec = ms / 1000;
  t.it_value.tv_nsec = (ms % 1000) * 1000000;
  t.it_interval.tv_sec = 0;
  t.it_interval.tv_nsec = 0;

  fd = timerfd_create(CLOCK_REALTIME, 0);

  if(fd == -1) {
    MSG("Error: timerfd_create()");
    return;
  }

  if(timerfd_settime(fd, 0, &t, NULL) == -1) {
    MSG("Error: timerfd_settime()");
    return;
  }

  s = read(fd, &exp, sizeof(exp));

  if(s <= 0 || exp <= 0) {
    MSG("Error: timerfd read");
  }  
}

#endif /* PF_MS_TIME */

