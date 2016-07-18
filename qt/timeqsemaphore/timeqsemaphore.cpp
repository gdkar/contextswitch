
#include <QThread>
#include <QSemaphore>
#include <QObject>
#include <QRunnable>
#include <QtGlobal>
#include <QDebug>
#include <memory>
#include <utility>

#include <pthread.h>
#include <sched.h>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <cerrno>
#include <cstring>

#include "timecore.hpp"

static constexpr const size_t iterations = 5000000;
int main(void) {
  struct sched_param param;
  int SCHED = SCHED_FIFO;
  param.sched_priority = sched_get_priority_max(SCHED);
  if (sched_setscheduler(getpid(), SCHED, &param))
    fprintf(stderr, "sched_setscheduler(): %s\n", strerror(errno));
  QSemaphore sem{0};
  {
    auto timer = TimeCore::tsc_timer();

    for(int i = 0; i < iterations; i++) { sem.release(); }
    auto per_iteration = timer.stop(iterations);
    qDebug() << (iterations * 2) << " posts in " << (timer.duration() *1.) << " ticks. " << per_iteration << " ticks per post , " << (1. / per_iteration ) << " posts per tick ";
  }
  {
    auto timer = TimeCore::tsc_timer();
    for(int i = 0; i < iterations; i++) { sem.acquire(); }
    auto per_iteration = timer.stop(iterations);
    qDebug() << (iterations * 2) << " waits in " << (timer.duration() *1.) << " ticks. " << per_iteration << " ticks per wait , " << (1. / per_iteration ) << " wait per tick ";
  }
  {
    auto timer = TimeCore::tsc_timer();
    for(int i = 0; i < iterations; i++) { sem.tryAcquire(); }
    auto per_iteration = timer.stop(iterations);
    qDebug() << (iterations * 2) << " try waits in " << (timer.duration() *1.) << " ticks. " << per_iteration << " ticks per wait , " << (1. / per_iteration ) << " wait per tick ";
  }
  return 0;
}
