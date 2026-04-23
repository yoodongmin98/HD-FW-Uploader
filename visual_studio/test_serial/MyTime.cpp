#include "pch.h"
#include "MyTime.h"





MyTime* MyTime::Time = nullptr;

MyTime::MyTime()
{
    Time = this;
}

MyTime::~MyTime()
{

}

std::string MyTime::GetLocalTime()
{
    using namespace std::chrono;

    auto now = system_clock::now();
    auto now_time_t = system_clock::to_time_t(now);
    auto now_ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

    // 현재 시간 변환
    struct tm tm_2;
    localtime_s(&tm_2, &now_time_t);

    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << tm_2.tm_hour << ":"
        << std::setfill('0') << std::setw(2) << tm_2.tm_min << ":"
        << std::setfill('0') << std::setw(2) << tm_2.tm_sec;
    // << std::setfill('0') << std::setw(3) << now_ms.count();  // 밀리초 추가

    return oss.str();
}


std::string MyTime::GetLocalDay()
{
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);

    struct tm tm_2;
    localtime_s(&tm_2, &now_time_t);

    std::ostringstream oss;
    oss << (tm_2.tm_year + 1900) << "-"
        << std::setfill('0') << std::setw(2) << (tm_2.tm_mon + 1) << "-"
        << std::setfill('0') << std::setw(2) << tm_2.tm_mday;

    return oss.str();
}


int MyTime::GetInterval()
{
    auto now = std::chrono::steady_clock::now();
    int Interval = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTriggerTime).count();
    lastTriggerTime = now;

    return Interval;
}


std::string MyTime::Unix_To_KST_Time(std::string _UnixTime)
{
    return timestampToString(std::stoi(_UnixTime));
}


std::string MyTime::timestampToString(std::time_t timestamp)
{
    std::tm* local_time = std::localtime(&timestamp);

    std::ostringstream oss;
    oss << std::put_time(local_time, "%Y-%m-%d");

    return oss.str();
}