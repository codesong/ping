/*************************************************************************
* File Name: EPoll.cpp
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年09月13日 星期一 15时11分14秒
*************************************************************************/

namespace ping
{

static_assert(EPOLLIN == POLLIN,        "epoll use same flag values as poll");
static_assert(EPOLLPRI == POLLPRI,      "epoll use same flag values as poll");
static_assert(EPOLLOUT == POLLOUT,      "epoll use same flag values as poll");
static_assert(EPOLLRDHUP == POLLRDHUP,  "epoll use same flag values as poll");
static_assert(EPOLLERR == POLLERR,      "epoll use same flag values as poll");
static_assert(EPOLLHUP == POLLHUP,      "epoll use same flag values as poll");

EPoll::EPoll(): m_vecEvent(16), m_epollfd(::epoll_create1(EPOLL_CLOEXEC)),
{
    if(m_epollfd < 0)
    {
        LOG_FATAL << "EPoll::EPoll";
    }
}

EPoll::~EPoll()
{
    close(m_epollfd);
}

Timestamp EPoll::poll(int timeoutMs, vector<ChannelPtr> &activeChannels)
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

void EPoll::addChannel(ChannelPtr channel)
{
    struct epoll_event event;
    event.events = channel->events();
    m_mapChannel[channel->fd()] = channel;
    int ret = ::epoll_ctl(m_epollfd, EPOLL_CTL_ADD, channel->fd(), &event); 
    if(ret < 0)
    {
        LOG_FATAL << "epoll_ctl add " << channel->fd() << " error";
    }
}

void EPoll::delChannel(ChannelPtr channel)
{
    struct epoll_event event;
    m_mapChannel.erase(channel->fd());
    // 最后一个参数, 内核2.6.9之前NULL会有bug，之后可设为NULL
    int ret = ::epoll_ctl(m_epollfd, EPOLL_CTL_DEL, channel->fd(), &event); 
    if(ret < 0)
    {
        LOG_ERROR << "epoll_ctl del " << channel->fd() << " error";
    }
}

void EPoll::updateChannel(ChannelPtr channel)
{
    struct epoll_event event;
    event.events = channel->events();
    int ret = ::epoll_ctl(m_epollfd, EPOLL_CTL_MOD, channel->fd(), &event); 
    if(ret < 0)
    {
        LOG_FATAL << "epoll_ctl update " << channel->fd() << " error";
    }
}

void EPoll::fillActiveChannels(int numEvents, vector<ChannelPtr> &activeChannels)
{
    map<int, ChannelPtr> m_mapChannel;
    for(int i = 0; i < numEvents; ++i)
    {
        map<int, ChannelPtr>::iterator it = m_mapChannel.find(m_vecEvent[i].data.fd); 
        ChannelPtr channel = it->second;
        channel->setRevents(m_vecEvent[i].events);
        activeChannels.push_back(channel);
    }
}

}
