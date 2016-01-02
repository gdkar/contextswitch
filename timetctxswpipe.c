
#include <signal.h>
#include <string.h>
#include <poll.h>
#include <sched.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include <linux/futex.h>

#include "timecore.h"
struct arg {
    int fd_r;
    int fd_w;
};

  const int iterations = 100000;
void *thrd_func(void *arg_){
  struct arg *arg = arg_;
  int fd_r = arg->fd_r;
  int fd_w = arg->fd_w;
  ssize_t ret = 0;
  for (int i = 0; i < iterations; i++) {
    char ibuf;
    do{
        errno = 0;
        ret = read(fd_r, &ibuf,1);
        if((ret < 0 && (errno != EINTR && errno != EAGAIN))||ret==0)
        {
            fprintf(stderr,"read error, %d ( %s ).\n",errno,strerror(errno));
            abort();
        }
    }while(ret < 0);
    do{
        errno = 0;
        ret = write(fd_w, " ", 1);
        if(ret < 0 && (errno != EINTR && errno != EAGAIN))
        {
            fprintf(stderr,"write error, %d ( %s ).\n",errno,strerror(errno));
            abort();
        }
    }while(ret <= 0);
  }
  return NULL;
}
int main(int argc, char **argv) {
    (void) argv;
    (void) argc;
  struct timespec ts;
  uint64_t tsc;
  int       pipe_to_child[2];
  int       pipe_from_child[2];
  if(pipe2(pipe_to_child,0) < 0 || pipe2(pipe_from_child,0) < 0) {
        fprintf(stderr,"pipe error, %d ( %s ).\n",errno,strerror(errno));
        _exit(EXIT_FAILURE);
  }
//  const int shm_id = shmget(IPC_PRIVATE, sizeof (int), IPC_CREAT | 0666);
  pthread_t      thrd;
  ssize_t ret = 0;
  struct sched_param param = (struct sched_param){.sched_priority = 1 };
  if(sched_setscheduler(getpid(),SCHED_FIFO,&param))
    fprintf(stderr, "sched_setscheduler(): %s\n", strerror(errno));
  struct arg arg_ = (struct arg){ .fd_r = pipe_to_child[0], .fd_w = pipe_from_child[1]};
  ret = pthread_create(&thrd,NULL,thrd_func,&arg_);

  if(ret < -1) {
    fprintf(stderr,"fork error, %d ( %s ).\n",errno,strerror(errno));
    _exit(EXIT_FAILURE);
  }
    int fd_w = 0;
    int fd_r = 0;
    fd_w = pipe_to_child[1];
    fd_r = pipe_from_child[0];
    fprintf(stderr,"Parent process,pid = %d.\n",getpid());
  clock_start(&ts);
  tsc_start(&tsc);
  for (int i = 0; i < iterations; i++) {
    char ibuf;
    do{
        errno = 0;
        ret = write(fd_w, " ", 1);
        if(ret < 0 && (errno != EINTR && errno != EAGAIN))
        {
            fprintf(stderr,"write error, %d ( %s ).\n",errno,strerror(errno));
            abort();
        }
    }while(ret <= 0);
    do{
        errno = 0;
        ret = read(fd_r, &ibuf,1);
        if((ret < 0 && (errno != EINTR && errno != EAGAIN))||ret==0)
        {
            fprintf(stderr,"read error, %d ( %s ).\n",errno,strerror(errno));
            abort();
        }
    }while(ret < 0);
  }
  void *retval;
  ret = pthread_join(thrd,&retval);
  uint64_t delta = clock_end(&ts);
  uint64_t delta_tsc = tsc_end(&tsc);
  const int nswitches = iterations << 1;
  fprintf(stderr, "%i pipe / thread switches in %zu (%.1fns/pipe block | unblock,  %0.1f clocks / pipe block | unblock)\n",
         nswitches, delta, (delta / (double) nswitches), delta_tsc/(double)nswitches);
  return 0;
}
