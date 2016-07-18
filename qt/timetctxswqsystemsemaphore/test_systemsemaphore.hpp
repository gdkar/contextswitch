_Pragma("once")
#include <QThread>
#include <QSystemSemaphore>
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
    QSystemSemaphore    m_parent_sem{"parent",0,QSystemSemaphore::Create};
    QSystemSemaphore    m_child_sem {"parent",1,QSystemSemaphore::Create};
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
            while(!m_child_sem.acquire()) { }
            m_parent_sem.release();
        }
    }
    void run_parent()
    {
         for(auto i = size_t{0}; i < m_iterations; i++) {
            while(!m_parent_sem.acquire()) { }
            m_child_sem.release();
        }       
    }
};

