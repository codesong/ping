/*************************************************************************
* File Name: Log.h
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年08月11日 星期三 08时11分53秒
*************************************************************************/

#ifndef __LOG_H__
#define __LOG_H__

#include <vector>
#include "Thread.h"
#include "Ostream.h"
#include "Condtion.h"
#include "Timestamp.h"
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
    void appendTime();
    void finish();

private:
    LogLevel    m_logLevel;
    Ostream     m_ostream;
    Timestamp   m_timestamp;
};

class ConsoleAppender: Noncopyable
{
public:
    ConsoleAppender() = default;
    ~ConsoleAppender() = default;

    bool append(LogLevel logLevel, const char *data, size_t len)
    {
        size_t n = fwrite(data, 1, len, stdout);
        return n == len;
    }
};

class LogFile;
using std::vector;
class FileAppender: Noncopyable
{
public:
    using Buffer = StreamBuffer<KLargeBuffer>;
    using BufferPtr = std::unique_ptr<Buffer>;
    using LogFilePtr = std::unique_ptr<LogFile>;
    
    FileAppender(const string &filePath, const string &baseName, off_t rollSize, int flushInterval);
    ~FileAppender();

    bool append(LogLevel logLevel, const char *data, size_t len);

private:
    void logging();
    void getFreeBuffer();

private:
    Mutex   m_mutex;
    Condtion m_cond;
    Thread m_thread;
    bool m_fatalOccur;
    atomic<bool> m_running;
    const string m_baseName;
    const off_t m_rollSize;
    const int m_flushInterval;
    BufferPtr m_currBuffer;
    vector<BufferPtr> m_vecFullBuffer;
    vector<BufferPtr> m_vecFreeBuffer;
};

class Logger: Noncopyable
{
public:
    using FileAppenderPtr = std::unique_ptr<FileAppender>;
    using ConsoleAppenderPtr = std::unique_ptr<ConsoleAppender>;

    Logger() = default;
    ~Logger() = default;

    void init(const string &filePath, const string &baseName, LogLevel level, int rollSize, int flushInterval, bool alsoLogConsole);
    void init(const string &filePath, const string &baseName, const string &level, int rollSize, int flushInterval, bool alsoLogConsole);
    void append(LogLevel logLevel, const char *data, size_t len);

private:
    LogLevel            m_logLevel;
    FileAppenderPtr     m_fileAppender;
    ConsoleAppenderPtr  m_consoleAppender;
};



}

#endif
