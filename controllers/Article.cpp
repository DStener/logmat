#include "Article.h"
#include "System/utils.h"
#include "Request/login.hpp"
#include "Request/log.hpp"

#include <boost/gil.hpp>
#include <boost/gil/extension/io/jpeg.hpp>
#include <boost/gil/extension/numeric/resample.hpp>
#include <boost/gil/extension/numeric/sampler.hpp>
using namespace boost::gil;

using namespace API;

void Ref::Article::GetList(const HttpRequestPtr& req, callback_func&& callback)
{
	Json::Value json;
	std::string tags = req->getParameter("tags");

	for (auto& row : DB::get()->Select<::Article>())
	{
		json.append(DTO::JSON::From(row));
	}

    if(!json.size()) { json = "Нет записей"; }

	auto response = HttpResponse::newHttpJsonResponse(json);
	callback(response);
}

void Ref::Article::Get(const HttpRequestPtr& req, callback_func&& callback, id_t id_file)
{
	auto result = DB::get()->Select<::Article>(std::format("id == {}", id_file));

	if (!result.size())
	{
		auto response = HttpResponse::newHttpJsonResponse(Json::Value("Bad index"));
		response->setStatusCode(k404NotFound);
		callback(response);
	}

	auto response = HttpResponse::newFileResponse(result[0].second.content);
	callback(response);
}

void Ref::Article::GetInfo(const HttpRequestPtr& req, callback_func&& callback, id_t id_file)
{
	
}

void Ref::Article::Upload(const HttpRequestPtr& req, callback_func&& callback)
{
	// Login and check Permission
	auto login = Request::Login(req, callback);
	if (!login.id) { return; }

	LOG_INFO << req->getBody();
	
	auto article = DTO::ConvertTo<::Article>(req);

	LOG_INFO << "TEST";

	article.content = utils::base64Encode(article.content);
	article.created_by = login.id;
	article.created_at = std::chrono::system_clock::now();
	article.deleted_by = 0;
	article.deleted_at = time_p();

	// Add Permission to DB
	LOG_INFO << "TR-2";
	DB::get()->Insert(article);

	auto response = HttpResponse::newHttpResponse();
	response->setStatusCode(drogon::k200OK);
	callback(response);
}

void Ref::Article::Update(const HttpRequestPtr& req, callback_func&& callback, id_t id_file)
{
	// Login and check Permission
	auto login = Request::Login(req, callback);
	if (!login.id) { return; }

	auto article = DTO::ConvertTo<::Article>(req);

	DB::get()->Update(id_file, article);

	auto response = HttpResponse::newHttpResponse();
	response->setStatusCode(drogon::k200OK);
	callback(response);
}

void Ref::Article::Delete(const HttpRequestPtr& req, callback_func&& callback, id_t id_file)
{
	// Login and check Permission
	auto login = Request::Login(req, callback);
	if (!login.id) { return; }

	DB::get()->Delete<::Article>(id_file);

	auto response = HttpResponse::newHttpResponse();
	response->setStatusCode(drogon::k200OK);
	callback(response);
}

void Ref::Article::SoftDelete(const HttpRequestPtr& req, callback_func&& callback, id_t id_file)
{
	// Login and check Permission
	auto login = Request::Login(req, callback);
	if (!login.id) { return; }

	auto article = DB::get()->Select<::Article>(std::format("id == {}", id_file))[0].second;
	article.deleted_at = std::chrono::system_clock::now();
	article.deleted_by = login.id;

	DB::get()->Update(id_file, article);

	auto response = HttpResponse::newHttpResponse();
	response->setStatusCode(drogon::k200OK);
	callback(response);
}

void Ref::Article::Restore(const HttpRequestPtr& req, callback_func&& callback, id_t id_file)
{
	// Login and check Permission
	auto login = Request::Login(req, callback);
	if (!login.id) { return; }

	auto article = DB::get()->Select<::Article>(std::format("id == {}", id_file))[0].second;
	article.deleted_at = time_p();
	article.deleted_by = 0;

	DB::get()->Update(id_file, article);

	auto response = HttpResponse::newHttpResponse();
	response->setStatusCode(drogon::k200OK);
	callback(response);
}

void Ref::Article::Form(const HttpRequestPtr& req, callback_func &&callback)
{
}
