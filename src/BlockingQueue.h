/*************************************************************************
* File Name: BlockingQueue.h
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年08月13日 星期五 07时28分21秒
*************************************************************************/

#ifndef __BLOCKINGQUEUE_H__
#define __BLOCKINGQUEUE_H__

#include <deque>
#include "Mutex.h"
#include "Condtion.h"

namespace ping
{
using std::deque;

template<typename T>
class BlockingQueue: Noncopyable
{
public:
    BlockingQueue(): m_mutex(), m_notEmpty(m_mutex), m_queue() {}
    ~BlockingQueue() { m_queue.clear(); }

    void push(const T &t)
    {
        MutexGuard _(m_mutex);
        m_queue.push_back(t);
        m_notEmpty.notify();
    }
    
    void push(T &&t)
    {
        MutexGuard _(m_mutex);
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
        return t;
    }

    size_t size() const
    {
        MutexGuard _(m_mutex);
        return m_queue.size();
    }

private:
    mutable Mutex m_mutex;
    Condition   m_notEmpty;
    deque<T>    m_queue;        
};

}

#endif

