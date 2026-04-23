#pragma once
#include <vector>
#include <mutex>
#include <serial/serial.h>


//Serial에서 나오는 Byte담아주는 Class
class ByteBuffer
{
public:
    static ByteBuffer* P_Buffers;

    ByteBuffer();
    ~ByteBuffer();

    ByteBuffer(const ByteBuffer&) = delete;
    ByteBuffer(ByteBuffer&&) = delete;
    ByteBuffer& operator=(const ByteBuffer&) = delete;
    ByteBuffer& operator=(ByteBuffer&&) = delete;


    std::vector<uint8_t>& GetBuffer()
    {
        return Buffer;
    }

    size_t GetBufferSize()
    {
        std::lock_guard<std::mutex> lock(Buffer_Mutex);
        return Buffer.size();
    }

    std::mutex& GetBufferMutex()
    {
        return Buffer_Mutex;
    }
    //얘 호출하면 1바이트씩 버퍼에 알아서 담김
    void StartBuffer();
protected:

private:
    void AppendBuffer(std::string _PortName);

    std::vector<uint8_t> Buffer;
    std::mutex Buffer_Mutex;
    serial::Serial BufferSerial;
};
