#include "utils.h"

#include <ctime>

size_t API::Utils::char2num(char c)
{
    size_t x = c - 65;
    if(x > 25) x -= 6;
    return x;
}

char API::Utils::num2char(size_t x)
{
    char c = 65 + x;
    if(x > 25) c += 6;
    return c;
}

std::string API::Utils::encode(std::string txt, const std::string& key)
{
    for(size_t i = 0; i < txt.size(); ++i ) {
        // encode
        char code = key[i % key.size()];
        txt[i] -= 65;
        if(txt[i] > 25) txt[i] -= 6;
        txt[i] = (txt[i] + code) % 52;
        // normalaze
        if(txt[i] > 25) txt[i] += 6;
        txt[i] += 65;
    }
    return txt;
}

std::string API::Utils::decode(std::string txt, const std::string& key)
{
    for(size_t i = 0; i < txt.size(); ++i) {
        // de-normalaize
        txt[i] -=65;
        if(txt[i] > 25) txt[i] -= 6;
        // decode
        char code = 52 - key[i % key.size()] % 52;
        txt[i] = (txt[i] + code) % 52;
        if(txt[i] > 25) txt[i] += 6;
        txt[i] += 65;
    }
    return txt;
}

void API::Utils::Parse(const std::string& frm, const std::string& fmt,
                       time_p& tp)
{
    using namespace std::chrono;

    std::tm time;
    std::istringstream stream{frm};

    stream >> std::get_time(&time, fmt.data());

    tp = system_clock::from_time_t(std::mktime(&time));
}

// std::string API::Utils::trim(const std::string& str)







// bool Base32::Encode32(unsigned char* in, int inLen,
//    unsigned char* out)
// {
//    if((in == 0) || (inLen <= 0) || (out == 0)) return false;

//    int d = inLen / 5;
//    int r = inLen % 5;

//    unsigned char outBuff[8];

//    for(int j = 0; j < d; j++)
//    {
//       if(!Encode32Block(&in[j * 5], &outBuff[0])) return false;
//       memmove(&out[j * 8], &outBuff[0], sizeof(unsigned char) * 8);
//    }

//    unsigned char padd[5];
//    memset(padd, 0, sizeof(unsigned char) * 5);
//    for(int i = 0; i < r; i++)
//    {
//       padd[i] = in[inLen - r + i];
//    }
//    if(!Encode32Block(&padd[0], &outBuff[0])) return false;
//    memmove(&out[d * 8], &outBuff[0], sizeof(unsigned char)
//       * GetEncode32Length(r));

//    return true;
// }

