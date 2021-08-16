/*************************************************************************
* File Name: BoundedBlockingQueue.h
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年08月13日 星期五 08时20分59秒
*************************************************************************/

#ifndef __BOUNDEDBLOCKINGQUEUE_H__
#define __BOUNDEDBLOCKINGQUEUE_H__

#include <deque>
#include "Mutex.h"
#include "Condtion.h"

namespace ping
{
using std::deque;

template<typename T>
class BoundedBlockingQueue: Noncopyable
{
public:
    BoundedBlockingQueue(int queueSize)
        : m_mutex(), m_notFull(m_mutex), m_notEmpty(m_mutex), 
          m_queueSize(queueSize), m_queue() {}
    ~BoundedBlockingQueue() { m_queue.clear(); }

    void push(const T &t)
    {
        MutexGuard _(m_mutex);
        while(m_queue.size() == m_queueSize)
        {
            m_notFull.wait();
        }
        m_queue.push_back(t);
        m_notEmpty.notify();
    }
    
    void push(T &&t)
    {
        MutexGuard _(m_mutex);
        while(m_queue.size() == m_queueSize)
        {
            m_notFull.wait();
        }
        m_queue.push_back(std::move(t));
        m_notEmpty.notify();
    }

    T pop()
    {
        MutexGuard _(m_mutex);
        while(m_queue.empty())
        {
            m_notEmpty.wait();
        }

        T t(std::move(m_queue.front()));
        m_queue.pop_front();
        m_notFull.notify();
        return t;
    }

    bool empty() const
    {
        MutexGuard _(m_mutex);
        return m_queue.empty();
    }

    bool full() const
    {
        MutexGuard _(m_mutex);
        return m_queue.size() == m_queueSize;
    }

    size_t size() const
    {
        MutexGuard _(m_mutex);
        return m_queue.size();
    }

    size_t capacity() const
    {
        MutexGuard _(m_mutex);
        return m_queue.capacity();
    }

private:
    mutable Mutex m_mutex;
    int m_queueSize;
    deque<T>    m_queue;        
    Condition   m_notFull;
    Condition   m_notEmpty;
};

}

#endif


