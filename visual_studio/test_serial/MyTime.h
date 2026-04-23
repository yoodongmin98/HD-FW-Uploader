#pragma once
#include "pch.h"



class MyTime
{
public:
	MyTime();
	~MyTime();
	static MyTime* Time;
	std::string GetLocalTime();
	std::string GetLocalDay();
	std::string Unix_To_KST_Time(std::string _UnixTime);

	int GetInterval();
protected:
	std::string timestampToString(std::time_t timestamp);
private:
	__time64_t now2;
	tm tm_2;

	std::chrono::steady_clock::time_point lastTriggerTime;
};
