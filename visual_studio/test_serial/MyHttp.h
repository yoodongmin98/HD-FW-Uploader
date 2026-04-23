#pragma once
#include "pch.h"
#include <wininet.h>



class Myhttp
{
	struct httpValue
	{
		std::string Method = NULL;
		std::string EndPoint = NULL;
		std::string body = NULL;
		std::string header = NULL;
	};
public:
	Myhttp(std::string _URL, unsigned int _Port);
	~Myhttp();

	std::pair<bool, std::string> SendRequest(const httpValue& _Param);
protected:
	void PrintLastError();
private:

	HINTERNET hInternet = 0, hConnect = 0, hRequest = 0;
	BOOL result = 0;
	DWORD statusCode = 0;
	DWORD statusSize = sizeof(statusCode);

	std::string URL;
	unsigned int Port;
};