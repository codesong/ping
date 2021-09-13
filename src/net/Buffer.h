/*************************************************************************
* File Name: Buffer.h
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年09月10日 星期五 10时24分04秒
*************************************************************************/

#ifndef __BUFFER_H__
#define __BUFFER_H__

#include <vector>
#include <string.h>
#include <algorithm>
#include "Socket.h"

namespace ping
{
/*
 * +-------------------+-----------------+-----------------+
 * | prependable bytes |  readable bytes |  writable bytes |
 * |                   |     (CONTENT)   |                 |
 * +-------------------+-----------------+-----------------+
 * |                   |                 |                 |
 * 0     <=      readerIndex   <=   writerIndex    <=    size
 *
 */

// 网络IO应用层缓冲区，处理网络IO收发数据
class Buffer
{
public:
    static const size_t KCheapPrepend = 8;
    static const size_t KInitialSize = 1024;

    explicit Buffer(size_t initialSize = KInitialSize)
        : m_buffer(KCheapPrepend + initialSize),
          m_readerIndex(KCheapPrepend),
          m_writerIndex(KCheapPrepend) {}
    
    void swap(Buffer &rhs)
    {
        m_buffer.swap(rhs.m_buffer);
        std::swap(m_readerIndex, rhs.m_readerIndex);
        std::swap(m_writerIndex, rhs.m_writerIndex);
    }

    size_t readableBytes() const { return m_writerIndex - m_readerIndex; }
    size_t writableBytes() const { return m_buffer.size() - m_writerIndex; }
    size_t prependableBytes() const { return m_readerIndex; }
    const char *peek() const { return begin() + m_readerIndex; }

    // 整数考虑字节序，浮点、英文字符不考虑字节序，汉字:UTF-8、GBK编码不考虑字节序，其他编码考虑
    // 程序中没处理汉字的字节序，故使用除UTF-8和GBK以外编码，且服务端和客户端字节序不同时，存在潜在乱码问题
    void write(const char *data, size_t len)
    {
        ensureWritableBytes(len);
        std::copy(data, data+len, writeBegin());
        hasWritten(len);
    }
    
    void write(const void *data, size_t len) { write(static_cast<const char*>(data), len); }
    void writeInt8(int8_t data) { write(&data, sizeof(data)); }
    void writeInt16(int16_t data)
    {
        int16_t be16 = Socket::hton16(data);
        write(&be16, sizeof(be16));
    }

    void writeInt32(int32_t data)
    {
        int32_t be32 = Socket::hton32(data);
        write(&be32, sizeof(be32));
    }

    void writeInt64(int64_t data)
    {
        int64_t be64 = Socket::hton64(data);
        write(&be64, sizeof(be64));
    }

    ssize_t readFd(int fd, int &errorNo);
    
    int8_t readInt8()
    {
        int8_t result = *readBegin();
        hasRead(sizeof(result));
        return result;
    }

    int16_t readInt16()
    {
        int16_t be16 = 0;
        memcpy(&be16, readBegin(), sizeof(be16));
        hasRead(sizeof(be16));
        return Socket::ntoh16(be16);
    }

    int32_t readInt32()
    {
        int32_t be32 = 0; 
        memcpy(&be32, readBegin(), sizeof(be32));
        hasRead(sizeof(be32));
        return Socket::ntoh32(be32);
    }

    int64_t readInt64()
    {
        int64_t be64 = 0;
        memcpy(&be64, readBegin(), sizeof(be64));
        hasRead(sizeof(be64));
        return Socket::ntoh64(be64);
    }

    char *writeBegin() { return begin() + m_writerIndex; }
    void hasWritten(size_t len) { m_writerIndex += len; } // 直接写入缓存后调用
    const char *readBegin() const { return begin() + m_readerIndex; }
    void hasReadAll()
    {
        m_readerIndex = KCheapPrepend;
        m_writerIndex = KCheapPrepend;
    }

    void hasRead(size_t len)
    {
        if(len < readableBytes())
        {
            m_readerIndex += len;
        }else
        {
            hasReadAll();
        }
    }

    void hasReadUntil(const char *pos)
    {
        hasRead(pos - readBegin());
    }

    void ensureWritableBytes(size_t len)
    {
        if(writableBytes() < len)
        {
            if(writableBytes() + prependableBytes() < len + KCheapPrepend)
            {
                m_buffer.resize(m_writerIndex + len);
            }else
            {
                size_t readable = readableBytes();
                std::copy(begin() + m_readerIndex, begin() + m_writerIndex, begin() + KCheapPrepend);
                m_readerIndex = KCheapPrepend;
                m_writerIndex = m_readerIndex + readable;
            }
        }
    }

private:
    char *begin() { return &m_buffer[0]; }
    const char *begin() const { return &m_buffer[0]; }

private:
    std::vector<char> m_buffer;
    size_t m_readerIndex;
    size_t m_writerIndex;
};

}
#endif

