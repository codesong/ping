/*************************************************************************
* File Name: EventLoopPool.cpp
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年09月13日 星期一 15时07分54秒
*************************************************************************/

#include "../Log.h"
#include "EventLoopPool.h"

namespace ping
{

EventLoopPool::EventLoopPool(const string &name, int threadNum)
    : m_current(0)
{
    if(threadNum < 0)
    {
        LOG_FATAL << "thread number invalid.";
    }

    for(int i = 0; i < threadNum; ++i)
    {
        m_vecEventLoop.push_back(std::make_unique<EventLoopThread>(name + std::to_string(i)));
    }
}

EventLoopPool::~EventLoopPool()
{
    m_vecEventLoop.clear();
}

EventLoopPtr EventLoopPool::EventLoopPool::nextLoop()
{
    EventLoopPtr eventLoop;
    if(!m_vecEventLoop.empty())
    {
        eventLoop = m_vecEventLoop[m_current++]->eventLoop();
        m_current = m_current % m_vecEventLoop.size();
    }
    return eventLoop;
}

}

