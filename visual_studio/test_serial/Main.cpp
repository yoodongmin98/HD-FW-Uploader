#include "pch.h"
#include "EngineDebug.h"
#include "EngineCore.h"

#pragma comment(lib, "wininet.lib")


int main()
{
    //콘솔창 띄우기, 숨기기
    //ShowWindow(GetConsoleWindow(), SW_NORMAL);
    ShowWindow(GetConsoleWindow(), SW_HIDE);

    HMODULE hWinInet = LoadLibrary("wininet.dll");
    if (hWinInet == NULL)
    {
        std::cout << "Failed to load wininet.dll. Error: " << GetLastError() << std::endl;
    }
    else
    {
        std::cout << "wininet.dll loaded successfully" << std::endl;
    }


    EngineDebug::LeakCheck(); //LeakCheck
    try
    {
        std::shared_ptr<EngineCore> Cores = std::make_shared<EngineCore>();
        Cores->Instance();
    }
    catch (const std::exception& e) 
    {
        std::ofstream logFile("error_log.txt", std::ios::app);
        logFile << "에러가 발생했습니다. Error : " << e.what() << "\n";
        logFile.close();
        std::cerr << "에러가 발생했습니다. Error : " << e.what() << "\n";
        return EXIT_FAILURE;
    }
    catch (...) 
    {
        std::ofstream logFile("error_log.txt", std::ios::app);
        logFile << "알수없는 오류가 발생했습니다.\n";
        logFile.close();
        std::cerr << "알수없는 오류가 발생했습니다.\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}