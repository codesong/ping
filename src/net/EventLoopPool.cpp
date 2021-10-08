/*************************************************************************
* File Name: EventLoopPool.cpp
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年09月13日 星期一 15时07分54秒
*************************************************************************/

#include "EventLoopPool.h"

namespace ping
{

EventLoopPool::EventLoopPool(const string &name, int threadNum)
    : m_current(0), m_running(false)
{
    if(threadNum > 0)
    {
        EventLoopPtr eventLoop = std::make_shared<EventLoop>(true, name);
        m_vecIoLoop.push_back(eventLoop);
    }
    m_baseLoop = std::make_shared<EventLoop>(false, "BaseLoop");
}

EventLoopPool::~EventLoopPool()
{
    if(m_running) stop();
}

void EventLoopPool::start()
{
    m_running = true;
    for(EventLoopPtr eventLoop: m_vecIoLoop)
    {
        eventLoop->start();
    }
    m_baseLoop->start();
}

void EventLoopPool::stop()
{
    m_running = false;
    m_baseLoop->stop();
    for(EventLoopPtr eventLoop: m_vecIoLoop)
    {
        eventLoop->stop();
    }
}

EventLoopPool::EventLoopPtr EventLoopPool::nextLoop()
{
    EventLoopPtr eventLoop = m_baseLoop;
    if(!m_vecIoLoop.empty())
    {
        eventLoop = m_vecIoLoop[m_current++];
        m_current = m_current % m_vecIoLoop.size();
    }
    return eventLoop;
}

}

