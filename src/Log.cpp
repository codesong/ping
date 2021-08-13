/*************************************************************************
* File Name: Log.cpp
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年08月12日 星期四 02时21分57秒
*************************************************************************/

#include "Log.h"

namespace ping
{

LogMessage::LogMessage(const char *file, int line, LogLevel level)
{
    
}

LogMessage::LogMessage(const char *file, int line, LogLevel level, char *func)
{

}

LogMessage::~LogMessage()
{
    finish();
    Logger &logger = Singleton<Logger>::instance();
    const Ostream::Buffer &buf(m_ostream.buffer());
    logger.append(buf.data(), buf.size());
}

void LogMessage::finish()
{
    m_ostream << '\n';
}

}
