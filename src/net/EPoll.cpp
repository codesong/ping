/*************************************************************************
* File Name: EPoll.cpp
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年09月13日 星期一 15时11分14秒
*************************************************************************/

#include <poll.h>
#include <sys/epoll.h>
#include "EPoll.h"
#include "Socket.h"
#include "../Log.h"
#include "../Util.h"

namespace ping
{

static_assert(EPOLLIN == POLLIN,        "epoll use same flag values as poll");
static_assert(EPOLLPRI == POLLPRI,      "epoll use same flag values as poll");
static_assert(EPOLLOUT == POLLOUT,      "epoll use same flag values as poll");
static_assert(EPOLLRDHUP == POLLRDHUP,  "epoll use same flag values as poll");
static_assert(EPOLLERR == POLLERR,      "epoll use same flag values as poll");
static_assert(EPOLLHUP == POLLHUP,      "epoll use same flag values as poll");

EPoll::EPoll(): m_vecEvent(16), m_epollfd(::epoll_create1(EPOLL_CLOEXEC))
{
    if(m_epollfd < 0)
    {
        LOG_FATAL << "EPoll::EPoll";
    }
}

EPoll::~EPoll()
{
    Socket::close(m_epollfd);
}

Timestamp EPoll::poll(int timeoutMs, vector<Channel *> &activeChannels)
{
    int numEvents = ::epoll_wait(m_epollfd, &m_vecEvent[0], m_vecEvent.size(), timeoutMs);
    int errorNo = errno;

    Timestamp now = Util::currTime();
    if(numEvents > 0)
    {
        LOG_TRACE << numEvents << " events happened";
        fillActiveChannels(numEvents, activeChannels);
        if(static_cast<size_t>(numEvents) == m_vecEvent.size())
        {
            m_vecEvent.resize(m_vecEvent.size() * 2); // 活跃fd占满，重新分配较大内存
        }
    }else if(0 == numEvents)
    {
        LOG_TRACE << "nothing happened";
    }else
    {
        if(errorNo != EINTR)
        {
            LOG_ERROR << "Poll::poll()";
        }
    }
    return now;
}

void EPoll::addChannel(Channel *channel)
{
    struct epoll_event event;
    event.events = channel->events();
    event.data.fd = channel->fd();
    m_mapChannel[channel->fd()] = channel;
    LOG_TRACE << "epoll_ctl add fd " << channel->fd();
    int ret = ::epoll_ctl(m_epollfd, EPOLL_CTL_ADD, channel->fd(), &event); 
    if(ret < 0)
    {
        LOG_FATAL << "epoll_ctl add fd " << channel->fd() << " error";
    }
}

void EPoll::delChannel(Channel *channel)
{
    struct epoll_event event;
    m_mapChannel.erase(channel->fd());
    LOG_TRACE << "epoll_ctl del fd " << channel->fd();
    // 最后一个参数, 内核2.6.9之前NULL会有bug，之后可设为NULL
    int ret = ::epoll_ctl(m_epollfd, EPOLL_CTL_DEL, channel->fd(), &event); 
    if(ret < 0)
    {
        int errorNo = Socket::socketError(channel->fd());
        LOG_ERROR << "epoll_ctl del fd " << channel->fd() << " error: " << strerror(errorNo);
    }
}

void EPoll::updateChannel(Channel *channel)
{
    struct epoll_event event;
    event.events = channel->events();
    event.data.fd = channel->fd();
    LOG_TRACE << "epoll_ctl mod fd " << channel->fd();
    int ret = ::epoll_ctl(m_epollfd, EPOLL_CTL_MOD, channel->fd(), &event); 
    if(ret < 0)
    {
        LOG_FATAL << "epoll_ctl update fd " << channel->fd() << " error";
    }
}

void EPoll::fillActiveChannels(int numEvents, vector<Channel *> &activeChannels)
{
    for(int i = 0; i < numEvents; ++i)
    {
        struct epoll_event &event = m_vecEvent[i];
        map<int, Channel *>::iterator it = m_mapChannel.find(event.data.fd); 
        if(it == m_mapChannel.end())
        {
            LOG_FATAL << "EPoll fd[" << event.data.fd << "] not found"; 
        }
        Channel *channel = it->second;
        channel->setRevents(event.events);
        activeChannels.push_back(channel);
    }
}

}
