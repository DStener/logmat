#pragma once

#include <string>

namespace API {
class Utils
{
public:

    static size_t char2num(char c);
    static char num2char(size_t x);

    static std::string encode(std::string txt, const std::string& key);
    static std::string decode(std::string txt, const std::string& key);

private:

    Utils() = delete;
    ~Utils() = delete;

};
}

