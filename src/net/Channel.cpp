/*************************************************************************
* File Name: Channel.cpp
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年09月13日 星期一 15时05分15秒
*************************************************************************/

#include <poll.h>
#include "../Log.h"
#include "Channel.h"
#include "EventLoop.h"

namespace ping
{

const int KNoneEvent = 0;
const int KReadEvent = POLLIN | POLLPRI;
const int KWriteEvent = POLLOUT;

Channel::Channel(EventLoop *eventLoop, int fd)
    : m_index(-1), m_fd(fd), m_eventLoop(eventLoop), m_added(false),
      m_events(KNoneEvent), m_revents(KNoneEvent)
{
}

Channel::~Channel()
{
    destory();
}

void Channel::destory()
{
    if(m_added)
    {
        m_added = false;
        LOG_TRACE << __FUNCTION__ << " del fd " << m_fd; 
        m_eventLoop->delChannel(this);
    }
}

void Channel::update()
{
    if(!m_added)
    {
        m_added = true;
        LOG_TRACE << __FUNCTION__ << " add fd " << m_fd; 
        m_eventLoop->addChannel(this);
    }else
    {
        LOG_TRACE << __FUNCTION__ << " mod fd " << m_fd; 
        m_eventLoop->updateChannel(this);
    }
}

void Channel::enableRead()
{
    LOG_TRACE << __FUNCTION__ << " fd " << m_fd; 
    m_events |= KReadEvent;    
    update();
}

void Channel::disableRead()
{
    LOG_TRACE << __FUNCTION__ << " fd " << m_fd; 
    m_events &= ~KReadEvent;    
    update();
}

void Channel::enableWrite()
{
    LOG_TRACE << __FUNCTION__ << " fd " << m_fd; 
    m_events |= KWriteEvent;    
    update();
}

void Channel::disableWrite()
{
    LOG_TRACE << __FUNCTION__ << " fd " << m_fd; 
    m_events &= ~KWriteEvent;    
    update();
}

void Channel::enableAll()
{
    LOG_TRACE << __FUNCTION__ << " fd " << m_fd; 
    m_events |= KReadEvent;
    m_events |= KWriteEvent;
    update();
}

void Channel::disableAll()
{
    LOG_TRACE << __FUNCTION__ << " fd " << m_fd; 
    m_events = KNoneEvent;
    update();
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

