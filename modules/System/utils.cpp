#include "utils.h"

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
