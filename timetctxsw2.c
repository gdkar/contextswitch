#include <sched.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <errno.h>

#include "timecore.h"
static const int iterations = 1000000;
static void* thread(void*ctx) {
  (void)ctx;
  for (int i = 0; i < iterations * 8; i++)
      sched_yield();
  return NULL;
}

int main(void) {
  struct sched_param param;
  param.sched_priority = 1;
  if (sched_setscheduler(getpid(), SCHED_RR, &param))
    fprintf(stderr, "sched_setscheduler(): %s\n", strerror(errno));

  struct timespec ts;
  pthread_t thd;
  clock_start(&ts);
  if (pthread_create(&thd, NULL, thread, NULL)) {
    return 1;
  }
  for (int i = 0; i < iterations; i++)
      sched_yield();
  void *retval = NULL;
  unsigned long long delta = clock_end(&ts);
  pthread_join(thd, &retval);

  const int nswitches = iterations << 1;
  printf("%i sched_yield()'s in %lluns (%.1fns/ctxsw)\n",
         nswitches, delta, (delta / (float) nswitches));
  return 0;
}
