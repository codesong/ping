/*************************************************************************
* File Name: LogFile.h
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年08月14日 星期六 13时01分16秒
*************************************************************************/

#ifndef __LOGFILE_H__
#define __LOGFILE_H__

#include <string>
#include <memory>

namespace ping
{
using std::string;
class LogFile
{
public:
    class File
    {
    public:
        File(const string &fileName);
        ~File();

        void flush();
        bool append(const char *data, size_t len);
        off_t fileSize() const { return m_size; }

    private:
        size_t write(const char *data, size_t len);

    private:
        off_t   m_size;
        FILE    *m_file;
        char    m_buffer[64*1024];
    };

public:
    using FilePtr = std::unique_ptr<File>;
    using MutexPtr = std::unique_ptr<Mutex>;

    LogFile(const string &baseName, off_t rollSize, bool threadSafe = true);
    ~LogFile();

    bool append(const char *data, size_t len);
    void flush();

private:
    void rollFile();
    string getFileName(const string &baseName);
    bool fileAppend(const char *data, size_t len);

private:
    FilePtr m_file;
    MutexPtr m_mutex;
    const int m_rollSize;
    const string m_baseName;
};

}

#endif
