#include "loginrequest.h"
#include "System/database.hpp"

#include <algorithm>
#include <chrono>

using namespace API;

std::string characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

std::size_t LoginRequest::getUserByToken(std::string token, ReturnDTO& ret)
{
    ret.code = drogon::k406NotAcceptable;
    ret.message = "Пожалуйста, войдите в систему";

    // If there is no cookie with the token
    if(!token.size()) { return 0; }

    // Get vector of struct Token where field token is qual to the sample
    auto tokens = DB::get()->Select<Token>(std::format("token == \"{}\"", token));
    if(!tokens.size()) { return 0;}

    // Checking for expired token lifetime
    if(tokens[0].second.time < std::chrono::utc_clock::now()) { return 0; }

    ret.code = drogon::k200OK;
    return tokens[0].second.user.id;
}

ReturnDTO LoginRequest::loginUser(const LoginDTO& info)
{
    ReturnDTO ret {HttpStatusCode::k200OK};

    std::cout << "LoginDTO: " << info.username;

    auto arr = DB::get()->Select<User>(std::format("username == \"{}\"", info.username));

    std::cout << "COUNT: " << arr.size();

    // info.password)

    // Calculate hash
    std::stringstream hash{};
    hash << std::hash<std::string>{}(info.password);

    // Check correct data
    if(arr.size() == 0 || arr[0].second.password != hash.str()) {
        ret.code = HttpStatusCode::k415UnsupportedMediaType;
        ret.message = "Некорректный логин или пароль";
        return ret;
    }

    // If username and password are correct,
    // AND the token limit has not been exceeded
    // then create token
    auto tokens = DB::get()->Select<Token>(std::format("user == {}", arr[0].first));
    if(tokens.size() >= MAX_TOKEN) {
        ret.code = HttpStatusCode::k415UnsupportedMediaType;
        ret.message = "Достигнут лимит по активным сессиям. ";
        return ret;
    }

    // Generate tokens
    Token t = LoginRequest::generateToken();
    t.user = arr[0].first;

    // Add token to DB
    DB::get()->Insert(t);

    // Convert token to std::string
    std::stringstream stream;
    stream << t.token;

    // Return
    ret.message = stream.str();
    return ret;
}

Token LoginRequest::generateToken()
{
    Token t{};
    t.time = std::chrono::utc_clock::now() + std::chrono::months(18);
    // t.time = std::chrono::utc_clock::from_sys(std::chrono::months(18)) + std::chrono::utc_clock::now();

    std::srand(std::time(0));
    for(int i = 0; i < 32; ++i ) {
        t.token.value.value += characters[std::rand() % characters.size()];
    }

    return t;
}
