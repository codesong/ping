/*************************************************************************
* File Name: Condtion.h
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年08月11日 星期三 07时33分02秒
*************************************************************************/

#ifndef __CONDTION_H__
#define __CONDTION_H__

#include <pthread.h>
#include "Mutex.h"

namespace ping
{
class Condtion: Noncopyable
{
public:
    explicit Condtion(Mutex &mutex): m_mutex(mutex){ CHECK_RETZERO(pthread_cond_init(&m_cond, nullptr)); }
    ~Condtion() { pthread_cond_destroy(&m_cond); }

    void notify() { CHECK_RETZERO(pthread_cond_signal(&m_cond)); }
    void notifyAll() { CHECK_RETZERO(pthread_cond_broadcast(&m_cond)); }

    void wait()
    {
        CHECK_RETZERO(pthread_cond_wait(&m_cond, &m_mutex.rawMutex()));
    }

    void timedWait(double seconds)
    {
        struct timespec time;
        clock_gettime(CLOCK_REALTIME, &time);
        const int64_t KNanoSecondsPerSecond = 1000000000;
        int64_t nanoseconds = static_cast<int64_t>(seconds * KNanoSecondsPerSecond);
        time.tv_sec += static_cast<time_t>((time.tv_nsec + nanoseconds) / KNanoSecondsPerSecond);
        time.tv_nsec = static_cast<long>((time.tv_nsec + nanoseconds) % KNanoSecondsPerSecond);
        pthread_cond_timedwait(&m_cond, &m_mutex.rawMutex(), &time);
        return;
    }

private:
    Mutex &m_mutex;
    pthread_cond_t m_cond;
};

}

#endif
