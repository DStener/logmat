#include "loginrequest.h"
#include "database.hpp"

#include <algorithm>

using namespace API;

std::string characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

std::size_t LoginRequest::getUserByToken(std::string token, ReturnDTO& ret)
{
    ret.code = drogon::k406NotAcceptable;
    ret.message = "Пожалуйста, войдите в систему";

    // If there is no cookie with the token
    if(!token.size()) { return 0; }

    auto tokens = DB::get()->Select<Token>(std::format("token == \"{}\"", token));
    if(!tokens.size()) { return 0;}

    ret.code = drogon::k200OK;
    return tokens[0].second.user.id;
}

ReturnDTO LoginRequest::loginUser(const LoginDTO& info)
{
    ReturnDTO ret {HttpStatusCode::k200OK};

    std::cout << "LoginDTO: " << info.username;

    auto arr = DB::get()->Select<User>(std::format("username == \"{}\"", info.username));

    std::cout << "COUNT: " << arr.size();

    // Check correct data
    if(arr.size() == 0 || arr[0].second.password != info.password) {
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
// LoginRequest::LoginRequest() {}

Token LoginRequest::generateToken()
{
    Token t{};
    t.time = 0;

    std::srand(std::time(0));
    for(int i = 0; i < 32; ++i ) {
        t.token.value.value += characters[std::rand() % characters.size()];
    }

    return t;
}
