#include "Director.h"
#include "System/utils.h"
#include "Request/login.hpp"
#include "Request/log.hpp"
#include <vector>

#include <drogon/drogon.h>

#include <boost/gil.hpp>
#include <boost/gil/extension/io/jpeg.hpp>
#include <boost/gil/extension/numeric/resample.hpp>
#include <boost/gil/extension/numeric/sampler.hpp>
using namespace boost::gil;

void Director::Test(const HttpRequestPtr& req, callback_func&& callback)
{
	auto user = Request::Login::GetUser(req);
	std::vector<aitem> aitems;

	//aitems.push_back({ "Test", "me?section=","<i class=\"ri-admin-line\"></i>" });

	auto data = HttpViewData();
	data.insert("User", user);
	data.insert("aitems", aitems);

	auto response = HttpResponse::newHttpViewResponse("test", data);
	callback(response);
}

void Director::Account(const HttpRequestPtr& req, callback_func&& callback)
{
	std::string view = "info";
	auto user = Request::Login::GetUser(req);
	std::vector<aitem> aitems;

	auto section = req->getOptionalParameter<std::string>("section");

	if (section.has_value())
	{
		view = section.value();

		aitems.push_back({ utils::fromWidePath(L"Осн. информация"), "/me?section=info","ri-user-line", ((section.value() == "info")? "current" : "")});
		aitems.push_back({ utils::fromWidePath(L"Аутентификация"), "/me?section=authentic", "ri-lock-password-line", ((section.value() == "authentic") ? "current" : "") });
	}
	else
	{
		aitems.push_back({ utils::fromWidePath(L"Осн. информация"), "/me?section=info","ri-user-line", "current"});
		aitems.push_back({ utils::fromWidePath(L"Аутентификация"), "/me?section=authentic", "ri-lock-password-line", ""});
	}

	// If User is admin, then add new section
	if (Request::Login::isAdmin(req))
	{
		aitems.push_back({ utils::fromWidePath(L"Панель управления"), "/admin.html", "ri-tools-line" });
	}

	auto data = HttpViewData();
	data.insert("User", user);
	data.insert("aside-header", utils::fromWidePath(L"Аккаунт"));
	data.insert("aitems", aitems);
	data.insert("javascripts", std::vector<std::string>{"/js/userpanel.js"});


	auto response = HttpResponse::newHttpViewResponse(view, data);
	callback(response);
}
//<meta http - equiv = "Content-Type" content = "text/html;charset=ISO-8859-1">