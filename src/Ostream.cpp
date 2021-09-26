/*************************************************************************
* File Name: Ostream.cpp
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年08月11日 星期三 09时07分21秒
*************************************************************************/

#include <limits>
#include <algorithm>
#include "Ostream.h"

namespace ping
{

const int KMaxNumericSize = 32;
const char digits[] = "0123456789ABCDEF";

void Ostream::staticCheck()
{
    static_assert(KMaxNumericSize - 10 > std::numeric_limits<double>::digits10, "KMaxNumericSize is large enough");
    static_assert(KMaxNumericSize - 10 > std::numeric_limits<long double>::digits10, "KMaxNumericSize is large enough");
    static_assert(KMaxNumericSize - 10 > std::numeric_limits<long>::digits10, "KMaxNumericSize is large enough");
    static_assert(KMaxNumericSize - 10 > std::numeric_limits<long long>::digits10, "KMaxNumericSize is large enough");
}

Ostream &Ostream::operator<<(short v)
{
    return operator<<(static_cast<int>(v));
}

Ostream &Ostream::operator<<(unsigned short v)
{
    return operator<<(static_cast<unsigned int>(v));
}

Ostream &Ostream::operator<<(int v)
{
    appendInteger(v);
    return *this;
}

Ostream &Ostream::operator<<(unsigned int v)
{
    appendInteger(v);
    return *this;
}

Ostream &Ostream::operator<<(long v)
{
    appendInteger(v);
    return *this;
}

Ostream &Ostream::operator<<(unsigned long v)
{
    appendInteger(v);
    return *this;
}

Ostream &Ostream::operator<<(long long v)
{
    appendInteger(v);
    return *this;
}

Ostream &Ostream::operator<<(unsigned long long v)
{
    appendInteger(v);
    return *this;
}

Ostream &Ostream::operator<<(const void *ptr)
{
    uintptr_t v = reinterpret_cast<uintptr_t>(ptr);
    appendInteger(v, 16);
    return *this;
}

Ostream &Ostream::operator<<(float v)
{
    return operator<<(static_cast<double>(v));
}

Ostream &Ostream::operator<<(double v)
{
    if(m_buffer.avail() >= KMaxNumericSize)
    {
        int len = snprintf(m_buffer.current(), KMaxNumericSize, "%.12g", v);
        m_buffer.add(len);
    }
    return *this;
}

template<typename T>
size_t Ostream::itoa(char buf[], T v, int base)
{
    T i = v;
    char *p = buf;

    do
    {
        int lsd = static_cast<int>(i % base);
        i /= base;
        *p++ = digits[lsd];
    }while(i != 0);

    if(v < 0) { *p++ = '-'; }
    std::reverse(buf, p);
    return p - buf;
}

template<typename T>
bool Ostream::appendInteger(T v, int base)
{
    if(m_buffer.avail() >= KMaxNumericSize )
    {
        if(16 == base)
        {
            char *buf = m_buffer.current();
            buf[0] = '0';
            buf[1] = 'x';
            m_buffer.add(2);
        }
        size_t len = itoa(m_buffer.current(), v, base);
        m_buffer.add(len);
        return true;
    }
    return false;
}

template<typename T>
Fmt::Fmt(const char *fmt, T val)
{
    static_assert(std::is_arithmetic<T>::value == true, "Must be arithmetic type");
    m_length = snprintf(m_buf, sizeof(m_buf), fmt, val);
}

// 显示实例化
template Fmt::Fmt(const char *fmt, char);

template Fmt::Fmt(const char *fmt, short);
template Fmt::Fmt(const char *fmt, unsigned short);
template Fmt::Fmt(const char *fmt, int);
template Fmt::Fmt(const char *fmt, unsigned int);
template Fmt::Fmt(const char *fmt, long);
template Fmt::Fmt(const char *fmt, unsigned long);
template Fmt::Fmt(const char *fmt, long long);
template Fmt::Fmt(const char *fmt, unsigned long long);

template Fmt::Fmt(const char *fmt, float);
template Fmt::Fmt(const char *fmt, double);

}

