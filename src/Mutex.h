/*************************************************************************
* File Name: Mutex.h
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年08月10日 星期二 07时05分05秒
*************************************************************************/

#ifndef __MUTEX_H__
#define __MUTEX_H__

#include <pthread.h>

namespace ping
{

#define CHECK(condition) \
    throw std::logic_error("check failed: " #conditon " ");

#define CHECK_RETZERO(FUNC) CHECK(0 == FUNC)

class Mutex: Noncopyable
{
public:
    Mutex() { CHECK(0 == pthread_mutex_init(&m_mutex, nullptr)); }
    ~Mutex() { CHECK(0 == pthread_mutex_destroy(&m_mutex)); }

    void lock() { CHECK(0 == pthread_mutex_lock(&m_mutex)); }
    void unlock() { CHECK(0 == pthread_mutex_unlock(&m_mutex)); }

private:
    pthread_mutex_t m_mutex;
};

class MutexGuard: Noncopyable
{
public:
    explicit MutexGuard(Mutex &mutex): m_mutex(mutex) { m_mutex.lock(); }
    ~MutexGuard() { m_mutex.unlock(); }
private:
    Mutex &m_mutex;
};

class RWMutex: Noncopyable
{
public:
    RWMutex() { CHECK(0 == pthread_rwlock_init(&m_lock, nullptr)); }
    ~RWMutex() { CHECK(0 == pthread_rwlock_destroy(&m_lock)); }

    void rdlock() { CHECK(0 == pthread_rwlock_rdlock(&m_lock)); }
    void wrlock() { CHECK(0 == pthread_rwlock_wrlock(&m_lock)); }
    void unlock() { CHECK(0 == pthread_rwlock_unlock(&m_lock)); }

private:
    pthread_rwlock_t m_lock;
};

class RdMutexGuard: Noncopyable
{
public:
    explicit RdMutexGuard(RWMutex &mutex): m_mutex(mutex) { m_mutex.rdlock(); }
    ~RdMutexGuard() { m_mutex.unlock(); }
private:
    RWMutex &m_mutex;
};

class WrMutexGuard: Noncopyable
{
public:
    explicit WrMutexGuard(RWMutex &mutex): m_mutex(mutex) { m_mutex.wrlock(); }
    ~WrMutexGuard() { m_mutex.unlock(); }
private:
    RWMutex &m_mutex;
};

class Spinlock: Noncopyable
{
public:
    Spinlock() { CHECK(0 == pthread_spin_init(&m_spinlock, 0)); }
    ~Spinlock() { CHECK(0 == pthread_spin_destroy(&m_spinlock)); }
    void lock() { CHECK(0 == pthread_spin_lock(&m_spinlock)); }
    void unlock() { CHECK(0 == pthread_spin_unlock(&m_spinlock)); }

private:
    pthread_spinlock_t m_spinlock;
};

class Semaphore: Noncopyable
{
public:
    Semaphore(unsigined int count = 0) { CHECK(0 == sem_init(&m_semaphore, 0, count)); }
    ~Semaphore() { CHECK(0 == sem_destroy(&m_semaphore)); }

    void wait() { CHECK(0 == sem_wait(&m_semaphore)); }
    void notify() { CHECK_RETZERO(sem_post(&m_semaphore)); };

private:
    sem_t m_semaphore;
};

}

#endif
