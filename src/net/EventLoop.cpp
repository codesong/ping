/*************************************************************************
* File Name: EventLoop.cpp
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年09月13日 星期一 15时07分43秒
*************************************************************************/

#include "EPoll.h"
#include "Socket.h"
#include "../Log.h"
#include "EventLoop.h"
#include "../Timestamp.h"
#include <unistd.h>
#include <sys/eventfd.h>

namespace ping
{

const int KPollTimeoutMs = 10000;
thread_local bool t_existEventLoop = false;

EventLoop::EventLoop()
    : m_running(false), m_threadId(Util::currThreadId()), 
      m_poller(std::make_unique<EPoll>()), m_wakeupFd(eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC))
{
    if(m_wakeupFd < 0)
    {
        LOG_FATAL << "eventfd failed";
    }

    m_wakeupChannel = std::make_unique<Channel>(this, m_wakeupFd);
    m_wakeupChannel->setReadCallback(std::bind(&EventLoop::handleWakeup, this));
    LOG_TRACE << "wake up fd " << m_wakeupFd << " enableRead.";
    m_wakeupChannel->enableRead();
    if(t_existEventLoop)
    {
        LOG_FATAL << "Another EventLoop exists in this thread.";
    }else
    {
        t_existEventLoop = true;
    }
}

EventLoop::~EventLoop()
{
    if(m_running) stop();
    LOG_DEBUG << "EventLoop " << this << " of thread " << m_threadId << " destruct in thread " << Util::currThreadId();
    m_wakeupChannel->disableAll();
    m_wakeupChannel->destory();
    m_wakeupChannel.reset();
    ::close(m_wakeupFd);
    t_existEventLoop = false;
}

void EventLoop::start()
{
    m_running = true;
    loop();
}

void EventLoop::stop()
{
    m_running = false;
    if(!inLoopThread())
    {
        wakeup();
    }
}

void EventLoop::loop()
{
    checkThread();
    while(m_running)
    {
        m_vecActiveChannel.clear();
        Timestamp pollTime = m_poller->poll(KPollTimeoutMs, m_vecActiveChannel);

        for(Channel *channel: m_vecActiveChannel)
        {
            channel->handleEvent(pollTime);
        }
        runFunctors();
    }
}

void EventLoop::runFunctors()
{
    vector<Functor> vecFunctor;
    do{
        MutexGuard _(m_mutex);
        vecFunctor.swap(m_vecFunctor);
    } while(false);

    for(const Functor &functor: vecFunctor)
    {
        functor();
    }
}

void EventLoop::addChannel(Channel *channel)
{
    checkThread();
    m_poller->addChannel(channel);
}

void EventLoop::delChannel(Channel *channel)
{
    checkThread();
    m_poller->delChannel(channel);
}

void EventLoop::updateChannel(Channel *channel)
{
    checkThread();
    m_poller->updateChannel(channel);
}

bool EventLoop::hasChannel(Channel *channel)
{
    checkThread();
    return m_poller->hasChannel(channel);
}

// 可能由其他线程调用
void EventLoop::runInLoop(Functor cb)
{
    if(inLoopThread())
    {
        cb();
    }else
    {
        do{
            MutexGuard _(m_mutex);
            m_vecFunctor.push_back(std::move(cb));
        }while(false);

        wakeup();
    }
}

bool EventLoop::inLoopThread()
{
    return m_threadId == Util::currThreadId();
}

void EventLoop::checkThread() 
{
    if(!inLoopThread())
    {
        LOG_FATAL << "EventLoop was created in thread " << m_threadId 
            << ", current thread " << Util::currThreadId();
    }
}

// eventloop 阻塞在poll上，wakeupFd也通过poll监视，通过wakeup(write wakeupFd)唤醒阻塞在poll的线程
void EventLoop::wakeup() 
{
    uint64_t one = 1;
    ssize_t n = Socket::write(m_wakeupFd, &one, sizeof(one));
    if(n != sizeof(one))
    {
        LOG_ERROR << "EventLoop wakeup writes failed.";
    }
}

void EventLoop::handleWakeup()
{
    uint64_t one = 1;
    ssize_t n = Socket::read(m_wakeupFd, &one, sizeof(one));
    if(n != sizeof(one))
    {
        LOG_ERROR << "EventLoop handleWakeup read failed.";
    }
}

EventLoopThread::EventLoopThread(const string name)
    : m_thread(std::bind(&EventLoopThread::threadFunc, this), name)
{
    m_thread.start();
}

EventLoopThread::~EventLoopThread()
{
    m_thread.stop();
}

void EventLoopThread::threadFunc()
{
    m_eventLoop = std::make_shared<EventLoop>();
    m_eventLoop->start();
}

}
