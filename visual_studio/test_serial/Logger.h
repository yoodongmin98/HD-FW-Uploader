#pragma once
#include "pch.h"
#include "EngineDebug.h"
#include "CsvLogger.h"

enum class LogType
{
    //로그 사용시 설정해줘야함
    NONE,
};


class GuiInterface;
class CoordLogger
{
    friend class GuiInterface;
public:
    // 범용 로그 함수
    template<typename... Ts>
    void log(int id, LogType type, const Ts&... cols)
    {
        getLogger(id, type).writeRow(cols...);
    }
protected:
private:
    // (ID, LogType) 키로 CsvLogger 관리
    std::map<std::pair<int, LogType>, std::unique_ptr<CsvLogger>> loggers;

    CsvLogger& getLogger(int id, LogType type)
    {
        auto key = std::make_pair(id, type);
        if (!loggers[key])
        {
            std::string fname = BaseFileName + std::to_string(id) + "_" + std::to_string(int(type)) + FileExtension;
            loggers[key] = std::make_unique<CsvLogger>(fname);
        }
        return *loggers[key];
    }
    std::string BaseFileName = "Log_";
    std::string FileExtension = ".csv";
};