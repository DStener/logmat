#pragma once

#include <string>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <cmath>

using time_p = std::chrono::system_clock::time_point;
#ifdef WIN32
using id_t = unsigned int;
#endif
template <typename T>
using DTORow = std::pair<id_t,T>;
template <typename T>
using ResponseVec = std::vector<DTORow<T>>;

namespace API {
class Utils
{
public:

    static size_t char2num(char c);
    static char num2char(size_t x);

    static std::string encode(std::string txt, const std::string& key);
    static std::string decode(std::string txt, const std::string& key);

    static void Parse(const std::string& frm, const std::string& fmt,
                        time_p& tp);

    static std::string base32Encode(std::string& str)
    {
        int d = std::ceil(str.size() / 5.0);

        std::string out;
        for(int j = 0; j < d; j++)
        {
            out += base32EncodeBlock(str.substr(j * 5, 5));
        }

        return out;
    }
    static std::string trim(const std::string& str)
    {
        const auto start_s = str.find_first_not_of(' ');
        if (start_s == std::string::npos)
        {
           return ""; // no content
        }

        const auto end_s = str.find_last_not_of(' ');
        const auto txt = str.substr(start_s, end_s - start_s + 1);

        const auto start_q = txt.find_first_not_of('"');
        if (start_q == std::string::npos)
        {
           return txt;
        }
        const auto end_q = txt.find_last_not_of('"');
        return txt.substr(start_q, end_q - start_q + 1);
    }




private:

    static std::string base32EncodeBlock(std::string str)
    {
        std::string out;
        out.resize(8);
        std::fill_n(out.begin(), 8, '=');

        int size = std::ceil(str.size() * 8 / 5.0);
        if(str.size() != 5)
        {
            size_t size = str.size();
            str.resize(5);
            std::fill_n(str.begin() + size, 5 - size, 0);
        }

        uint64_t buffer = 0;
        for(auto& c : str)
        {
            buffer = (buffer << 8) | c;
        }

        for(int i = 7; i >= 8 - size; --i)
        {
            uint8_t c = (buffer >> 5 * i) & 0b11111;
            out[7 - i] = c +((c < 26)? 'A' : '2' - 0b11010);
        }
        return out;
    }

    Utils() = delete;
    ~Utils() = delete;

};
}

#define ADMIN_PASSWORD "adminsuper"
#define ADMIN_NAME "admin"

#define CHECK_SOFT_DELETE(link_val)                                 \
if(link_val.second.deleted_by != 0 &&                               \
    link_val.second.deleted_at <= std::chrono::system_clock::now())

