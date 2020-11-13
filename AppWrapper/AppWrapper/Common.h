#pragma once

#include <time.h>
#include <iostream>
#include <sstream>
#include <string>
#include <locale>
#include <codecvt>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/rotating_file_sink.h"

class Utils {
public:
    inline static std::string getISO8601() {
        time_t now;
        time(&now);
        char buf[sizeof "2020-10-14T07:07:09Z"];
        struct tm tm_buf;
        gmtime_s(&tm_buf, &now);
        strftime(buf, sizeof buf, "%FT%T", &tm_buf);
        // this will work too, if your compiler doesn't support %F or %T:
        //strftime(buf, sizeof buf, "%Y-%m-%dT%H:%M:%SZ", gmtime(&now));
        std::string ret(buf);
        return ret;
    }
    inline static std::wstring stringToWstring(const std::string& from) {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        std::wstring wide = converter.from_bytes(from);
        return wide;
    }

    inline static std::string wstringToString(const std::wstring & from) {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        std::string narrow = converter.to_bytes(from);
        return narrow;
    }
    
    inline static std::shared_ptr<spdlog::logger> initLogger(const std::string& loggerName)
    {
        std::shared_ptr<spdlog::logger> pLogger = spdlog::rotating_logger_mt(loggerName, "logs/Aetrex.Voice.Service.log", 200000, 3);
        //https://github.com/gabime/spdlog/wiki/3.-Custom-formatting
        //DateTime, Process id, Thread id, log level, logger name, log message
        pLogger->set_pattern("[%Y-%m-%d %H:%M:%S.%e][PID=%P][Thd=%t] <%l> %n - %v");

        return pLogger;
    }
    
};

#define BUFSIZE 512
