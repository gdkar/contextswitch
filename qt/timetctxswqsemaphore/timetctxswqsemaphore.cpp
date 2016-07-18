
#include <QThread>
#include <QSystemSemaphore>
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
#include "test_qsemaphore.hpp"

static constexpr const size_t iterations = 500000;
int main(void) {
  struct sched_param param;
  int SCHED = SCHED_FIFO;
  param.sched_priority = sched_get_priority_max(SCHED);
  if (sched_setscheduler(getpid(), SCHED, &param))
    fprintf(stderr, "sched_setscheduler(): %s\n", strerror(errno));
    
  auto test = std::make_unique<Test>(iterations);
  {
    auto timer = TimeCore::tsc_timer();
    test->run_parent();
    auto per_iteration = timer.stop(iterations << 2);
    qDebug() << (iterations * 2) << " iterations in " << (timer.duration() *1.) << " ticks. " << per_iteration << " ticks per iteration, " << (1. / per_iteration ) << " iterations per tick ";
  }
  return 0;
}
