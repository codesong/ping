/*************************************************************************
* File Name: Ostream.h
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年08月11日 星期三 08时26分59秒
*************************************************************************/

#ifndef __OSTREAM_H__
#define __OSTREAM_H__

#include <string>
#include <string.h>
#include "Noncopyable.h"

namespace ping
{

const int KSmallBuffer = 4096;
const int KLargeBuffer = 1000 * 4096;

using std::string;

template<int SIZE>
class StreamBuffer: Noncopyable
{
public:
    StreamBuffer() :m_curr(m_data) { bzero(); }
    ~StreamBuffer() {}

    bool append(const char *data, size_t len)
    {
        if(avail() >= len)
        {
            memcpy(m_curr, data, len);
            m_curr += len;
            return true;
        }
        return false;
    }


    const char *data() const { return m_data; }
    void add(size_t len) { m_curr += len; }
    char *current() { return m_curr; }
    size_t avail() const { return capacity() - size(); } 
    size_t size() const { return m_curr - m_data; }
    size_t capacity() const { return sizeof(m_data); }
    
    void reset() { m_curr = m_data; }
    string toString() const { return string(m_data, size()); }

private:
    void bzero() { ::bzero(m_data, sizeof(m_data)); }

private:
    char *m_curr;
    char m_data[SIZE];
};

class Ostream: Noncopyable
{
public:
    using Buffer = StreamBuffer<KSmallBuffer>;

    Ostream &operator<<(bool v);
    Ostream &operator<<(short v);
    Ostream &operator<<(unsigned short v);
    Ostream &operator<<(int v);
    Ostream &operator<<(unsigned int v);
    Ostream &operator<<(long v);
    Ostream &operator<<(unsigned long v);
    Ostream &operator<<(long long v);
    Ostream &operator<<(unsigned long long v);

    Ostream &operator<<(const void *ptr);
    Ostream &operator<<(float v);
    Ostream &operator<<(double v);
    Ostream &operator<<(char v);

    Ostream &operator<<(const char *str);
    Ostream &operator<<(const unsigned char *str);
    Ostream &operator<<(const string &str);

    bool append(const char *data, size_t len) { return m_buffer.append(data, len); }
    const Buffer &buffer() const { return m_buffer; }
    void resetBuffer() { m_buffer.reset(); }

private:
    // 仅支持10进制转10、16进制字串
    template<typename T>
    size_t itoa(char buf[], T v, int base);

    template<typename T>
    bool appendInteger(T v, int base = 10);

    void staticCheck();

private:
    Buffer m_buffer;
};

class Fmt
{
public:
    template<typename T>
    Fmt(const char *fmt, T val);

    const char *data() const { return m_buf; }
    int length() const { return m_length; }

private:
    char m_buf[32] = {0x00};
    int m_length;
};

}

#endif
