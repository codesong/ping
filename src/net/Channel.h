/*************************************************************************
* File Name: Channel.h
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年09月13日 星期一 15时05分08秒
*************************************************************************/

#ifndef __CHANNEL_H__
#define __CHANNEL_H__

#include <memory>
#include <functional>
#include "../Timestamp.h"
#include "../Noncopyable.h"

namespace ping
{

class EventLoop;
class Channel: Noncopyable
{
public:
    using EventLoopPtr = std::shared_ptr<EventLoop>;
    using EventCallback = std::function<void(const Timestamp&)>;

    Channel(EventLoop *eventLoop, int fd);
    ~Channel();

    void setReadCallback(EventCallback cb) { m_readCallback = std::move(cb); }
    void setWriteCallback(EventCallback cb) { m_writeCallback = std::move(cb); }
    void setCloseCallback(EventCallback cb) { m_closeCallback = std::move(cb); }
    void setErrorCallback(EventCallback cb) { m_errorCallback = std::move(cb); }

    void enableRead();
    void disableRead();
    void enableWrite();
    void disableWrite();
    void enableAll();
    void disableAll();

    void handleEvent(const Timestamp &time);

    int fd() const { return m_fd; }
    int events() const { return m_events; }
    void setRevents(int revents) { m_revents = revents; }
    int index() const { return m_index; }
    void setIndex(int index) { m_index = index; }

    bool isWriting() const;
    bool isReading() const;

private:
    void update();

private:
    int m_events;
    int m_revents;
    const int m_fd;
    int m_index;
    bool m_added;
    EventLoop *m_eventLoop;

    EventCallback m_readCallback;
    EventCallback m_writeCallback;
    EventCallback m_closeCallback;
    EventCallback m_errorCallback;
};

}

#endif

