#pragma once
#include "pch.h"
#include "ByteBuffer.h"
#include <string>
#include <mutex>
#include <serial/serial.h>
#include "SerialPort.h"


ByteBuffer* ByteBuffer::P_Buffers = nullptr;

ByteBuffer::ByteBuffer()
{
    P_Buffers = this;
}

ByteBuffer::~ByteBuffer()
{
    if (BufferSerial.isOpen())
    {
        BufferSerial.close();
    }
}

void ByteBuffer::StartBuffer()
{
    int Baudrates = SerialPort::P_SerialPort->GetSelectBaudrate();
    std::string Port = SerialPort::P_SerialPort->GetSelectPort();

    BufferSerial.setBaudrate(Baudrates);
    BufferSerial.setPort(Port);
    BufferSerial.open();

    std::thread(&ByteBuffer::AppendBuffer, this, Port).detach();
}

void ByteBuffer::AppendBuffer(std::string _PortName)
{
    for (;;)
    {
        if (BufferSerial.available() > 0)
        {
            uint8_t byte;
            BufferSerial.read(&byte, 1);
            std::lock_guard<std::mutex> lock(Buffer_Mutex);
            Buffer.push_back(byte);
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}
