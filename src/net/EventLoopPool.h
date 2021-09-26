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

// base eventloop 专门为TcpServer的Acceptor作为监听使用，存在于主线程，由Acceptor操作
// io eventloop，用来处理io请求，单独开线程处理io请求，避免io请求耗时过长影响连接
// 若io eventloop不存在，io请求和acceptor请求共用base eventloop
class EventLoopPool: Noncopyable
{
public:
    EventLoopPool(const string &name = "", int threadNum = 0);
    ~EventLoopPool();

    void start();
    void stop();

    EventLoopPtr baseLoop() { return m_baseLoop; }
    EventLoopPtr nextLoop();

private:
    int m_current;
    atomic<bool> m_running;
    EventLoopPtr m_baseLoop; // Acceptor start/stop
    vector<EventLoopPtr> m_vecIoLoop;
};

}

#endif

