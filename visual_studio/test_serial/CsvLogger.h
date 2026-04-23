#pragma once
#include "pch.h"



class CsvLogger
{
    std::ofstream file;
    bool headerWritten = false;
public:
    CsvLogger(const std::string& filename)
        : file(filename, std::ios::app)
    {
        if (!file.is_open())
            throw std::runtime_error("파일 열기 실패: " + filename);
    }
    ~CsvLogger()
    {
        if (file.is_open())
            file.close();
    }
    void writeHeader(std::vector<std::string> cols)
    {
        if (!headerWritten)
        {
            for (size_t i = 0; i < cols.size(); ++i)
                file << cols[i] << (i + 1 < cols.size() ? "," : "\n");
            headerWritten = true;
        }
    }

    template<typename... Ts>
    void writeRow(const Ts&... args)
    {
        // (file << arg << ",", ...) 을 이용해 모두 찍고
        (void)std::initializer_list<int>{ (file << args << ",", 0)... };
        file.seekp(-1, std::ios_base::cur);   // 마지막 콤마 제거
        file << "\n";
        file.flush();
    }
};
