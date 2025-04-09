/**
 *
 *  Filer_Login.cc
 *
 */

#include "Filter_Login.h"
#include "System/database.hpp"

using namespace drogon;
using namespace Filter;

void Login::doFilter(const HttpRequestPtr &req,
                         FilterCallback &&fcb,
                         FilterChainCallback &&fccb)
{
    auto finded = DB::get()->Select<::Token>(std::format("token == \"{}\"", req->session()->sessionId())).size();

    if (finded)
    {
        //Passed
        fccb();
        return;
    }

    //Check failed
    fcb(drogon::HttpResponse::newNotFoundResponse());
}
