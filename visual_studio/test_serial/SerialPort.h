#pragma once
#include <string>
#include <serial/serial.h>
#include <imgui.h>

class SerialPort
{
public:
    static SerialPort* P_SerialPort;
    SerialPort();
    ~SerialPort();

    SerialPort(const SerialPort&) = delete;
    SerialPort(SerialPort&&) = delete;
    SerialPort& operator=(const SerialPort&) = delete;
    SerialPort& operator=(SerialPort&&) = delete;

    void Instance(const bool _IsChild, ImVec2 _Size);

    std::string GetSelectPort()
    {
        return SerialPorts;
    }

    const unsigned int GetSelectBaudrate()
    {
        return BaudRate;
    }
protected:

private:
    std::string SerialPorts;
    unsigned int BaudRate = 0;
    std::vector<const char*> BaudrateArray =
    {
        "110","300","600","1200","2400","4800","9600","14400","19200","38400","57600",
        "115200","230400","460800","921600","1000000","1250000","1843200"
    };

    //Æ÷Æ®
    int SelectPort = -1;
    int SelectBaudRate = -1;
    std::vector<serial::PortInfo> PortInfos;
    std::vector<const char*> AllPort;
};
