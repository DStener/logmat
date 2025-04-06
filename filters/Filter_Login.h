/**
 *
 *  Filer_Login.h
 *
 */

#pragma once

#include <drogon/HttpFilter.h>
using namespace drogon;
namespace Filter
{

class Login : public HttpFilter<Login>
{
  public:
    Login() {}
    void doFilter(const HttpRequestPtr &req,
                  FilterCallback &&fcb,
                  FilterChainCallback &&fccb) override;
};

}
