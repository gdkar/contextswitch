#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/mman.h>
#include <linux/futex.h>
#include <semaphore.h>

#include "timecore.h"

static const int iterations = 500000;
struct sem_pair {
    sem_t parent;
    sem_t child;
};
static void* thread(void* restrict ftx) {
  struct sem_pair *pair = (struct sem_pair*) ftx;
  for (int i = 0; i < iterations; i++) {
    int ret;
    do {
        ret = sem_wait(&pair->child);
        if(ret < 0)
            ret = errno;
    }while(ret == EINTR || ret == EAGAIN);
    sem_post(&pair->parent);
  }
  return NULL;
}

int main(void) {
  struct timespec ts;
  int ret;
  const int shm_id = shmget(IPC_PRIVATE, sizeof(struct sem_pair), IPC_CREAT | 0666);

  struct sem_pair *pair = shmat(shm_id,NULL,0);
  if((ret = sem_init(&pair->parent,1,1)) < 0
  || (ret = sem_init(&pair->child,1,0)) < 0){
    ret = -errno;
    perror("sem_init");
    return ret;
  }
  pid_t other = -1;
  struct sched_param param;
  param.sched_priority = 1;

  if (sched_setscheduler(getpid(), SCHED_FIFO, &param))
    fprintf(stderr, "sched_setscheduler(): %s\n", strerror(errno));

  other = fork();
  if(other < 0) {
    ret = -errno;
    perror("fork");
    return ret;
  }
  if(!other) {
    pair = shmat(shm_id,NULL,0);
    thread(pair);
    return 0;
  }
  clock_start(&ts);
  for (int i = 0; i < iterations ; i++) {
    do {
        ret = sem_wait(&pair->parent);
        if(ret < 0)
            ret = errno;
    }while(ret == EINTR || ret == EAGAIN);
    sem_post(&pair->child);
  }
  const uint64_t delta = clock_end(&ts);
  int status = 0;
  if((ret =waitpid(other,&status,0))<0){
    ret = -errno;
    perror("waitpid");
    return ret;
  }
  sem_destroy(&pair->child);
  sem_destroy(&pair->parent);
  const int nswitches = (iterations << 1) ;
  printf("%i  thread context switches (sem) in %zu(%.1fns/ctxsw)\n",nswitches, delta, (delta / (float) nswitches));
  return 0;
}
