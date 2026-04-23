#include "pch.h"
#include "Myhttp.h"



//URL, Port 이니셜라이저
Myhttp::Myhttp(std::string _URL, unsigned int _Port)
    :URL(_URL), Port(_Port)
{
    //인터넷 오픈 확인
    hInternet = InternetOpen("WinINet", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet)
    {
        std::cout << "Internet Open failed\n";
        PrintLastError();
    }

    //인터넷 연결 확인
    hConnect = InternetConnect(hInternet, URL.c_str(), Port,
        NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
    if (!hConnect)
    {
        std::cout << "Internet Connect failed\n";
        PrintLastError();
        if (hInternet)
            InternetCloseHandle(hInternet);
        return;
    }
}


Myhttp::~Myhttp()
{

}

void Myhttp::PrintLastError()
{
    DWORD errorCode = GetLastError();
    char* errorMsg = NULL;

    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&errorMsg, 0, NULL);

    std::cout << "오류 코드: " << errorCode << " - " << (errorMsg ? errorMsg : "알 수 없는 오류") << std::endl;

    if (errorMsg)
        LocalFree(errorMsg);
}


std::pair<bool, std::string> Myhttp::SendRequest(const httpValue& _Param)
{
    bool IsSuccess = false;
    hRequest = HttpOpenRequest(hConnect, _Param.Method.c_str(), _Param.EndPoint.c_str(), NULL, NULL, NULL,
        INTERNET_FLAG_SECURE | INTERNET_FLAG_RELOAD, 0);

    //http오픈 확인
    if (!hRequest)
    {
        PrintLastError();
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return { false ,"HttpOpenRequest fail" };
    }

    //정보 전송
    result = HttpSendRequestA
    (
        hRequest,
        _Param.header.c_str(), (DWORD)_Param.header.length(),
        (LPVOID)_Param.body.c_str(), (DWORD)_Param.body.length()
    );


    char buffer[4096];
    DWORD bytesRead;
    std::string responseData;

    if (!result)
    {
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        DWORD dwError = GetLastError();
        return { false , "The internet is not connected." };
    }
    else
    {
        if (HttpQueryInfo(hRequest, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,
            &statusCode, &statusSize, NULL) && statusCode == 200)
        {
            IsSuccess = true;


            while (InternetReadFile(hRequest, buffer, sizeof(buffer) - 1, &bytesRead) && bytesRead > 0)
            {
                buffer[bytesRead] = '\0';
                responseData += buffer;
            }
        }
        else
        {
            InternetCloseHandle(hRequest);
            InternetCloseHandle(hConnect);
            InternetCloseHandle(hInternet);
            //상황에 따라 200 못받았을때 문자 보내기
            return { false , "Respon 200 fail" };
        }
    }

    InternetCloseHandle(hRequest);
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);

    return { IsSuccess ,responseData };
}