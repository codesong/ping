/*************************************************************************
* File Name: Buffer.cpp
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年09月13日 星期一 14时03分05秒
*************************************************************************/

#include <errno.h>
#include "Buffer.h"

namespace ping
{
ssize_t Buffer::readFd(int fd, int &errorNo)
{
    char extrabuf[65536];
    struct iovec vec[2];
    const size_t writable = writableBytes();
    vec[0].iov_base = writeBegin();
    vec[0].iov_len = writable;
    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof(extrabuf);

    const ssize_t n = Socket::readv(fd, vec, 2);
    if(n < 0)
    {
        errorNo = errno;
    }else if(n <= writable)
    {
        m_writerIndex += n;
    }else
    {
        m_writerIndex = m_buffer.size();
        write(extrabuf, n - writable);
    }
    return n;
}

}
