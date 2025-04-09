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

		aitems.push_back({ utils::fromWidePath(L"Осн. информация"), "/me?section=info","ri-user-line", "", ((section.value() == "info") ? "current" : "")});
		aitems.push_back({ utils::fromWidePath(L"Аутентификация"), "/me?section=authentic", "ri-lock-password-line", "", ((section.value() == "authentic") ? "current" : "")});
	}
	else
	{
		aitems.push_back({ utils::fromWidePath(L"Осн. информация"), "/me?section=info","ri-user-line", "", "current"});
		aitems.push_back({ utils::fromWidePath(L"Аутентификация"), "/me?section=authentic", "ri-lock-password-line",});
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

void Director::ArticleEditor(const HttpRequestPtr& req, callback_func&& callback,
							 id_t id)
{
	auto user = Request::Login::GetUser(req);
	std::vector<aitem> aitems{
		{
			utils::fromWidePath(L"Заголовок раздела"), 
			"#", "ri-h-2", "insertTag('h2')"
		},

		{"hr"},

		{
			utils::fromWidePath(L"Жирный"),
			"#", "ri-bold", "insertTag('b')"
		},
		{
			utils::fromWidePath(L"Курсив"),
			"#", "ri-italic", "insertTag('i')"
		},
		{
			utils::fromWidePath(L"Подчёркнутый"),
			"#", "ri-underline", "insertTag('u')"
		},

		{"hr"},

		{
			utils::fromWidePath(L"Изображение"),
			"#", "ri-image-add-line", "InsertFile(true)"
		},
		{
			utils::fromWidePath(L"Файл"),
			"#", "ri-file-add-line", "InsertFile(false)"
		}
	};

	// Get Article DTO by id
	auto articles = DB::get()->Select<::Article>(std::format("id == {}", id));
	// If the Article exists, then use UPDATE action
	std::string action = std::format("/api/ref/article/{}", id);
	// Else, use CREATE action
	if (articles.empty()) { action = "/api/ref/article/"; }


	auto data = HttpViewData();
	data.insert("User", user);
	data.insert("aside-header", utils::fromWidePath(L"Аккаунт"));
	data.insert("aitems", aitems);
	data.insert("javascripts", std::vector<std::string>{"/js/editor.js"});


	auto response = HttpResponse::newHttpViewResponse("article_editor", data);
	callback(response);
}

void Director::AdminPanel(const HttpRequestPtr& req, callback_func&& callback)
{
	auto user = Request::Login::GetUser(req);
	std::vector<aitem> aitems{
		{
			utils::fromWidePath(L"Файлы"),
			"/admin?section=file", "ri-file-line", "", 
			((req->getParameter("section") == "file") ? "current" : "")
		},
		{
			utils::fromWidePath(L"Пользователи"),
			"/admin?section=user", "ri-group-line", "",
			((req->getParameter("section") == "user") ? "current" : "")
		},
		{
			utils::fromWidePath(L"Роли"),
			"/admin?section=role", "ri-id-card-line", "",
			((req->getParameter("section") == "role") ? "current" : "")
		},
		{
			utils::fromWidePath(L"Разрешения"),
			"/admin?section=permission", "ri-door-lock-line", "",
			((req->getParameter("section") == "permission") ? "current" : "")

		},
		{
			utils::fromWidePath(L"Change logs"),
			"/admin?section=changelogs", "ri-file-list-2-line", "",
			((req->getParameter("section") == "changelogs") ? "current" : "")
		},
		{
			utils::fromWidePath(L"Request logs"),
			"/admin?section=requestlogs", "ri-file-list-3-line", "",
			((req->getParameter("section") == "requestlogs") ? "current" : "")
		},
		{
			utils::fromWidePath(L"Webhook"),
			"/admin?section=webhook", "ri-git-pull-request-line", "",
			((req->getParameter("section") == "webhook") ? "current" : "")
		},
	};

	auto data = HttpViewData();
	data.insert("User", user);
	data.insert("aside-header", utils::fromWidePath(L"Панель управления"));
	data.insert("aitems", aitems);
	data.insert("javascripts", std::vector<std::string>{"/js/editor.js"});

	auto response = HttpResponse::newHttpViewResponse("temp", data);
	callback(response);
}
