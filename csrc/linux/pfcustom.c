#include <sys/timerfd.h>
#include <time.h>
#include <unistd.h>


W1(MS,      "MS", ms,
  struct itimerspec t;
  int fd;
  uint64_t exp;
  ssize_t s;

  if(ms <= 0) break;

  t.it_value.tv_sec = ms / 1000;
  t.it_value.tv_nsec = (ms % 1000) * 1000000;
  t.it_interval.tv_sec = 0;
  t.it_interval.tv_nsec = 0;

  fd = timerfd_create(CLOCK_REALTIME, 0);

  if(fd == -1) {
    MSG("Error: timerfd_create()");
    break;
  }

  if(timerfd_settime(fd, 0, &t, NULL) == -1) {
    MSG("Error: timerfd_settime()");
    break;
  }

  s = read(fd, &exp, sizeof(exp));

  if(s <= 0 || exp <= 0) {
    MSG("Error: timerfd read");
  }  
)


#include "../posix/pfcustom_task_pthreads.c"