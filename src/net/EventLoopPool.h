/*************************************************************************
* File Name: EventLoopPool.h
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年09月13日 星期一 15时07分35秒
*************************************************************************/

#ifndef __EVENTLOOPPOOL_H__
#define __EVENTLOOPPOOL_H__

#include "EventLoop.h"

namespace ping
{

class EventLoopPool: Noncopyable
{
public:
    using EventLoopThreadPtr = std::unique_ptr<EventLoopThread>;

    EventLoopPool(const string &name = "", int threadNum = 0);
    ~EventLoopPool();

    void start();
    void stop();

    EventLoopPtr nextLoop();

private:
    int m_current;
    vector<EventLoopThreadPtr> m_vecEventLoop;

};

}

#endif

