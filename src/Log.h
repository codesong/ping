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

#define LOGGER Singleton<Logger>::instance()

#define LOG(level) \
    if(LOGGER.logLevel() <= level) \
        LogMessage(__FILE__, __LINE__, level).stream() 

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

extern thread_local Ostream t_ostream;
class LogMessage: Noncopyable
{
public:
    LogMessage(const char *file, int line, LogLevel level);
    ~LogMessage();

    Ostream &stream() { return m_ostream; }

private:
    void finish();
    void appendTime();
    const string logLevelName();

private:
    Ostream     m_ostream;
    LogLevel    m_logLevel;
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
    
    static const int KInitBufferNum = 32;

    FileAppender(const string &logDir, const string &baseName, off_t rollSize, int flushInterval);
    ~FileAppender();

    bool append(LogLevel logLevel, const char *data, size_t len);

private:
    void logging();
    void getFreeBuffer();

private:
    enum State
    {
        Stoped,
        Running,
        WaitStoped,
    };

private:
    Mutex   m_mutex;
    Condtion m_cond;
    Thread m_thread;

    atomic<bool> m_running;
    atomic<bool> m_fatalOccur;
    const string m_logDir;
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

    static const int KFlushInterval = 3;
    static const off_t KRollSize = 10 * 1024 * 1024;

    Logger() = default;
    ~Logger() = default;

    void init(const string &filePath, const string &baseName, LogLevel level, 
        int rollSize = KRollSize, int flushInterval = KFlushInterval, bool alsoLogConsole = false);
    void init(const string &filePath, const string &baseName, const string &level, 
        int rollSize = KRollSize, int flushInterval = KFlushInterval, bool alsoLogConsole = false);
    void append(LogLevel logLevel, const char *data, size_t len);
    const LogLevel logLevel() const { return m_logLevel; }

private:
    const LogLevel logLevel(const string &level);

private:
    LogLevel            m_logLevel;
    FileAppenderPtr     m_fileAppender;
    ConsoleAppenderPtr  m_consoleAppender;
};

}

#endif
