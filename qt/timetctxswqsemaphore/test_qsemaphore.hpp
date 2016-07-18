_Pragma("once")
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

class Test : public QThread {
    Q_OBJECT
    QSemaphore    m_parent_sem{0};
    QSemaphore    m_child_sem {1};
    size_t              m_iterations{0};
public:
    Test(size_t iterations, QObject *pParent = nullptr)
    : QThread(pParent)
    , m_iterations(iterations)
    {
        start();
    }
   ~Test() { quit(); wait(); }
    void run() override {
        for(auto i = size_t{0}; i < m_iterations; i++) {
            m_child_sem.acquire();
            m_parent_sem.release();
        }
    }
    void run_parent()
    {
         for(auto i = size_t{0}; i < m_iterations; i++) {
            m_parent_sem.acquire();
            m_child_sem.release();
        }       
    }
};

