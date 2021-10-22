/*************************************************************************
* File Name: Channel.cpp
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年09月13日 星期一 15时05分15秒
*************************************************************************/

#include <poll.h>
#include "Channel.h"
#include "EventLoop.h"

namespace ping
{

const int KNoneEvent = 0;
const int KReadEvent = POLLIN | POLLPRI;
const int KWriteEvent = POLLOUT;

Channel::Channel(EventLoopPtr eventLoop, int fd)
    : m_index(-1), m_fd(fd), m_eventLoop(eventLoop), 
      m_events(KNoneEvent), m_revents(KNoneEvent) 
{
    m_eventLoop->addChannel(shared_from_this());
}

Channel::~Channel()
{
    m_eventLoop->delChannel(shared_from_this());
}

void Channel::enableRead()
{
    m_events |= KReadEvent;    
    m_eventLoop->updateChannel(shared_from_this());
}

void Channel::disableRead()
{
    m_events &= ~KReadEvent;    
    m_eventLoop->updateChannel(shared_from_this());
}

void Channel::enableWrite()
{
    m_events |= KWriteEvent;    
    m_eventLoop->updateChannel(shared_from_this());
}

void Channel::disableWrite()
{
    m_events &= ~KWriteEvent;    
    m_eventLoop->updateChannel(shared_from_this());
}

void Channel::enableAll()
{
    m_events |= KReadEvent;
    m_events |= KWriteEvent;
    m_eventLoop->updateChannel(shared_from_this());
}

void Channel::disableAll()
{
    m_events = KNoneEvent;
    m_eventLoop->updateChannel(shared_from_this());
}

bool Channel::isWriting() const
{
    return m_events & KWriteEvent;
}

bool Channel::isReading() const
{
    return m_events & KReadEvent;
}

void Channel::handleEvent(const Timestamp &time)
{
    // 对端关闭，且从对端发送的数据全部已读取完整
    if((m_revents & POLLHUP) && !(m_revents & POLLIN))
    {
        if(m_closeCallback) m_closeCallback(time);
    }

    if(m_revents & (POLLERR | POLLNVAL))
    {
        if(m_errorCallback) m_errorCallback(time);
    }

    // POLLRDHUP 对端关闭，或者只关闭了写端
    if(m_revents & (POLLIN | POLLPRI | POLLRDHUP))
    {
        if(m_readCallback) m_readCallback(time);
    }

    if(m_revents & POLLOUT)
    {
        if(m_writeCallback) m_writeCallback(time);
    }
}

}

