/*************************************************************************
* File Name: LogFile.cpp
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年08月15日 星期日 03时11分33秒
*************************************************************************/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "Util.h"
#include "Mutex.h"
#include "LogFile.h"
#include "Timestamp.h"

namespace ping
{

LogFile::File::File(const string &fileName)
    : m_size(0), m_file(::fopen(fileName.c_str(), "ae"))
{
    ::setbuffer(m_file, m_buffer, sizeof(m_buffer));
}

LogFile::File::~File()
{
    ::fclose(m_file);
}

void LogFile::File::flush()
{
    ::fflush(m_file);
}

bool LogFile::File::append(const char *data, size_t len)
{
    size_t written = 0;
    while(written != len)
    {
        size_t remain = len - written;
        size_t n = write(data + written, remain);
        if(n > 0)
        {
            written += n;
        }else
        {
            int err = ferror(m_file);
            if(0 != err)
            {
                fprintf(stderr, "File::append failed: %s\n", strerror(err));
                break;
            }
        }
    }
    return written == len;
}

size_t LogFile::File::write(const char *data, size_t len)
{
    return ::fwrite_unlocked(data, 1, len, m_file);
}

LogFile::LogFile(const string &baseName, off_t rollSize, bool threadSafe)
    : m_baseName(baseName), m_rollSize(rollSize), m_mutex(threadSafe ? new Mutex : nullptr)
{
    rollFile();
}

LogFile::~LogFile()
{

}

bool LogFile::append(const char *data, size_t len)
{
    if(m_mutex)
    {
        MutexGuard _(*m_mutex);
        return fileAppend(data, len);
    }else
    {
        return fileAppend(data, len);
    }
}

void LogFile::flush()
{
    if(m_mutex)
    {
        MutexGuard _(*m_mutex);
        m_file->flush();
    }else
    {
        m_file->flush();
    }
}

void LogFile::rollFile()
{
    string fileName = getFileName(m_baseName);
    m_file.reset(new File(fileName));
}

string LogFile::getFileName(const string &baseName)
{
    string fileName = baseName;
    Timestamp tt = Util::currTime();
    fileName += ".";
    fileName += tt.toString(YYYYMMDDHHMMSSMS);
    fileName += ".log";
    return fileName;
}

bool LogFile::fileAppend(const char *data, size_t len)
{
    if(m_file->fileSize() >= m_rollSize)
    {
        rollFile();
    }
    return m_file->append(data, len);
}

}

