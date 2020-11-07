#pragma once

#include <time.h>
#include <iostream>
#include <sstream>
#include <string>
#include <locale>
#include <codecvt>


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

};

#define BUFSIZE 512
