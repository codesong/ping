/*************************************************************************
* File Name: Log.h
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年08月11日 星期三 08时11分53秒
*************************************************************************/

#ifndef __LOG_H__
#define __LOG_H__

#include "Ostream.h"
#include "Singleton.h"
#include "Noncopyable.h"

namespace ping
{

#define LOG(loglevel) \
    if(Singleton<Logger>::instance().logleve() <= logleve) \
    { \
        if(TRACE == logleve || DEBUG == logleve) \
        { \
            LogMessage(__FILE__, __LINE__, logleve, __FUNC__); \
        }else \
        { \
            LogMessage(__FILE__, __LINE__, logleve); \
        } \
    }
    
#define LOG_TRACE   LOG(TRACE)
#define LOG_DEBUG   LOG(DEBUG)
#define LOG_INFO    LOG(INFO)
#define LOG_WARN    LOG(WARN)
#define LOG_ERROR   LOG(ERROR)
#define LOG_FATAL   LOG(FATAL)

enum LogLevel
{
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL
};

class LogMessage: Noncopyable
{
public:
    LogMessage(const char *file, int line, LogLevel level);
    LogMessage(const char *file, int line, LogLevel level, char *func);
    ~LogMessage();

    Ostream &stream() { return m_ostream; }

private:
    void finish();

private:
    LogLevel m_leve;
    Ostream m_ostream;
};

class ConsoleAppender: Noncopyable
{
public:
    bool append(const char *data, size_t len)
    {
        size_t n = fwrite(data, 1, len, stdout);
        return n == len;
    }
};

class FileAppender: Noncopyable
{
public:

};

class Logger: Noncopyable
{
public:
    void setLoglevel(LogLevel loglevel) {}
    LogLevel loglevel() { return m_loglevel; }
    
    bool append(const char *data, size_t len);

private:
    LogLevel m_loglevel;
};



}

#endif
