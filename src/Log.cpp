/*************************************************************************
* File Name: Log.cpp
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年08月12日 星期四 02时21分57秒
*************************************************************************/

#include "Log.h"
#include "Util.h"
#include "LogFile.h"

namespace ping
{

thread_local time_t t_lastSecond;
thread_local string t_timeStr;

LogMessage::LogMessage(const char *file, int line, LogLevel level)
    : m_timestamp(Util::currTime()), m_logLevel(level)
{
    appendTime();
    m_ostream << logLevelName() << " " << Util::currThreadId() << " "; 
    m_ostream << file << ":" << line << " ";
}

LogMessage::~LogMessage()
{
    finish();
    Logger &logger = Singleton<Logger>::instance();
    const Ostream::Buffer &buf(m_ostream.buffer());
    logger.append(m_logLevel, buf.data(), buf.size());
}

void LogMessage::finish()
{
    m_ostream << '\n';
}

void LogMessage::appendTime()
{
    if(t_lastSecond != m_timestamp.seconds())
    {
        t_lastSecond = m_timestamp.seconds();
        t_timeStr = m_timestamp.toString(YYYYMMDDHHMMSS);
    }
    Fmt us(".%.6d ", m_timestamp.microSeconds());
    m_ostream << t_timeStr << us.data();
}

const string LogMessage::logLevelName()
{
    switch(m_logLevel)
    {
#define XX(name) \
    case name: \
        return #name; \
        break;

    XX(TRACE);
    XX(DEBUG);
    XX(INFO);
    XX(WARN);
    XX(ERROR);
    XX(FATAL);
#undef XX
    default:
        return "UNKNOW";
        break;
    }
}

FileAppender::FileAppender(const string &logDir, const string &baseName, off_t rollSize, int flushInterval)
    : m_logDir(logDir), m_baseName(baseName), m_rollSize(rollSize), m_flushInterval(flushInterval),
      m_mutex(), m_cond(m_mutex), m_running(false), m_fatalOccur(false),
      m_thread(std::bind(&FileAppender::logging, this), "Logging")
{
    m_vecFullBuffer.reserve(KInitBufferNum);
    m_vecFreeBuffer.reserve(KInitBufferNum);
    for(int i = 0; i < KInitBufferNum; ++i)
    {
        BufferPtr buffer = std::make_unique<Buffer>();
        m_vecFreeBuffer.push_back(std::move(buffer));
    }
    m_currBuffer = std::make_unique<Buffer>();
    m_running = true;
    m_thread.start();
}

FileAppender::~FileAppender()
{
    if(m_running)
    {
        m_running = false;
        m_thread.stop();
    }
}

bool FileAppender::append(LogLevel logLevel, const char *data, size_t len)
{
    bool appended = false;
    if(m_running && !m_fatalOccur)
    {
        if(FATAL == logLevel)
            m_fatalOccur = true;

        MutexGuard _(m_mutex);
        if(m_currBuffer->avail() >= len)
        {
            appended = m_currBuffer->append(data, len);
        }else
        {
            m_vecFullBuffer.push_back(std::move(m_currBuffer));
            getFreeBuffer();
            appended = m_currBuffer->append(data, len);
            m_cond.notify();
        }

    }
    return appended;
}

// 未加锁，外部需加锁
void FileAppender::getFreeBuffer()
{
    if(!m_vecFreeBuffer.empty())
    {
        m_currBuffer = std::move(m_vecFreeBuffer.back());
        m_vecFreeBuffer.pop_back();
    }else
    {
        m_currBuffer.reset(new Buffer);
    }
}

void FileAppender::logging()
{
    int pos = m_baseName.find_last_of("/\\");
    if(string::npos == pos) pos = 0;
    LogFile logFile(m_logDir, m_baseName.substr(string::npos == pos ? 0 : pos+1), m_rollSize, false);
    vector<BufferPtr> vecBufferToWrite;
    vecBufferToWrite.reserve(KInitBufferNum);
    while(m_running)
    {
        do
        {
            MutexGuard _(m_mutex);
            if(m_vecFullBuffer.empty()) // 非while, 不能永远等在这里
            {
                m_cond.timedWait(m_flushInterval);
            }
            m_vecFullBuffer.push_back(std::move(m_currBuffer));
            vecBufferToWrite.swap(m_vecFullBuffer);
            getFreeBuffer();
        }while(false);

        for(const BufferPtr &buffer: vecBufferToWrite)
        {
            logFile.append(buffer->data(), buffer->size());
            buffer->reset();
        }
        
        do
        {
            MutexGuard _(m_mutex);
            while(!vecBufferToWrite.empty())
            {
                m_vecFreeBuffer.push_back(std::move(vecBufferToWrite.back()));
                vecBufferToWrite.pop_back();
            }
        }while(false);

        logFile.flush();
        if(m_fatalOccur)
        {
            MutexGuard _(m_mutex);
            if(m_vecFullBuffer.empty() 
                && m_currBuffer->size() <= 0)
                abort();
        }
    }
    logFile.flush();
}

void Logger::init(const string &filePath, const string &baseName, LogLevel level, 
        int rollSize, int flushInterval, bool alsoLogConsole)
{

    m_logLevel = level;
    if(alsoLogConsole)
    {
        m_consoleAppender = std::make_unique<ConsoleAppender>();
    }
    m_fileAppender = std::make_unique<FileAppender>(filePath, baseName, rollSize, flushInterval);
}

void Logger::init(const string &filePath, const string &baseName, const string &level, 
        int rollSize, int flushInterval, bool alsoLogConsole)
{
    m_logLevel = logLevel(level);
    if(alsoLogConsole)
    {
        m_consoleAppender = std::make_unique<ConsoleAppender>();
    }
    m_fileAppender = std::make_unique<FileAppender>(filePath, baseName, rollSize, flushInterval);
}

void Logger::append(LogLevel logLevel, const char *data, size_t len)
{
    if(m_consoleAppender)
    {
        m_consoleAppender->append(logLevel, data, len);
    }

    if(m_fileAppender)
    {
        m_fileAppender->append(logLevel, data, len);
    }
}

const LogLevel Logger::logLevel(const string &level)
{
#define XX(LogLevel, v) \
    if(level == #v) return LogLevel;

    XX(TRACE,   trace);
    XX(DEBUG,   debug);
    XX(INFO,    info);
    XX(WARN,    warn);
    XX(ERROR,   error);
    XX(FATAL,   fatal);

    XX(TRACE,   TRACE);
    XX(DEBUG,   DEBUG);
    XX(INFO,    INFO);
    XX(WARN,    WARN);
    XX(ERROR,   ERROR);
    XX(FATAL,   FATAL);

    return TRACE;
#undef XX
}
}
