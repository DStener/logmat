#include "File.h"
#include "System/utils.h"
#include "Request/login.hpp"
#include "Request/log.hpp"

using namespace API;

void Ref::File::GetList(const HttpRequestPtr& req, callback_func&& callback)
{
	Json::Value json;

	for (auto& row : DB::get()->Select<::File>())
	{
		json.append(DTO::JSON::From(row));
	}

    if(!json.size()) { json = "Нет записей"; }

	auto response = HttpResponse::newHttpJsonResponse(json);
	callback(response);
}

void Ref::File::Get(const HttpRequestPtr& req, callback_func&& callback, id_t id_file)
{
	auto result = DB::get()->Select<::File>(std::format("id == {}", id_file));

	if (!result.size())
	{
		auto response = HttpResponse::newHttpJsonResponse(Json::Value("Bad index"));
		response->setStatusCode(k404NotFound);
		callback(response);
	}

	auto response = HttpResponse::newFileResponse(result[0].second._path);
	callback(response);
}
void Ref::File::GetAvatar(const HttpRequestPtr& req, callback_func&& callback, id_t id_file)
{
	auto result = DB::get()->Select<::File>(std::format("id == {}", id_file));

	if (!result.size())
	{
		auto response = HttpResponse::newHttpJsonResponse(Json::Value("Bad index"));
		response->setStatusCode(k404NotFound);
		callback(response);
	}

	auto response = HttpResponse::newFileResponse(result[0].second._avatar_path);
	callback(response);
}

void Ref::File::GetInfo(const HttpRequestPtr& req, callback_func&& callback, id_t id_file)
{
	Json::Value json;

	for (auto& row : DB::get()->Select<::File>(std::format("id == {}", id_file)))
	{
		json.append(DTO::JSON::From(row));
	}
	auto response = HttpResponse::newHttpJsonResponse(json);
	callback(response);
}

void Ref::File::Upload(const HttpRequestPtr& req, callback_func&& callback)
{
	// Login and check Permission
	auto login = Request::Login(req, callback);
	if (!login.id || !login.hasPermission<::File>("create")) { return; }

	MultiPartParser parser;

	// Check coorect file 
    if (parser.parse(req) != 0 || parser.getFiles().size() != 1)
	{
        std::cout << parser.getFiles().size() << std::endl;

		auto response = HttpResponse::newHttpJsonResponse(Json::Value("Must only be one file"));
		response->setStatusCode(k403Forbidden);
		callback(response);

		return;
	}

	auto& file = parser.getFiles()[0];
	auto md5 = file.getMd5();
	std::string name = std::format("{}.{}", md5, file.getFileExtension());

	file.saveAs(name);

	::File FileDB{};
	FileDB.name = file.getFileName();
	FileDB.description = parser.getParameter<std::string>("description");
	FileDB.tags = std::format("{}|{}", file.getFileExtension(), parser.getParameter<std::string>("tags"));
	FileDB.size = file.fileLength();
	FileDB._path = std::format("{}/{}", app().getUploadPath(), name);
	//FileDB._avatar_path = "__TEMP__";

    std::cout << "ZDES" << std::endl;

	// Upload file info to DB and get id row
	id_t id = DB::get()->Insert(FileDB);

    std::cout << "TUT" << std::endl;

	auto response = HttpResponse::newHttpJsonResponse(Json::Value(id));
	callback(response);
}

void Ref::File::Update(const HttpRequestPtr& req, callback_func&& callback, id_t id_file)
{
	// Login and check Permission
	auto login = Request::Login(req, callback);
	if (!login.id || !login.hasPermission<::File>("update")) { return; }

	auto info = DTO::RequestBody::To<::File>(req->getBody());

	DB::get()->Update(id_file, info);

	auto response = HttpResponse::newHttpResponse();
	response->setStatusCode(drogon::k200OK);
	callback(response);
}

void Ref::File::Delete(const HttpRequestPtr& req, callback_func&& callback, id_t id_file)
{
	// Login and check Permission
	auto login = Request::Login(req, callback);
	if (!login.id || !login.hasPermission<::File>("delete")) { return; }

	DB::get()->Delete<::File>(id_file);

	auto response = HttpResponse::newHttpResponse();
	response->setStatusCode(drogon::k200OK);
	callback(response);
}

void Ref::File::SoftDelete(const HttpRequestPtr& req, callback_func&& callback, id_t id_file)
{
	// Login and check Permission
	auto login = Request::Login(req, callback);
	if (!login.id || !login.hasPermission<::File>("delete")) { return; }

	auto file = DB::get()->Select<::File>(std::format("id == {}", login.id))[0].second;
	file.deleted_at = std::chrono::system_clock::now();
	file.deleted_by = login.id;

	DB::get()->Update(id_file, file);

	auto response = HttpResponse::newHttpResponse();
	response->setStatusCode(drogon::k200OK);
	callback(response);
}

void Ref::File::Restore(const HttpRequestPtr& req, callback_func&& callback, id_t id_file)
{
	// Login and check Permission
	auto login = Request::Login(req, callback);
	if (!login.id || !login.hasPermission<::File>("restore")) { return; }

	auto permission = DB::get()->Select<::File>(std::format("id == {}", id_file))[0].second;
	permission.deleted_at = time_p();
	permission.deleted_by = 0;

	DB::get()->Update(id_file, permission);

	auto response = HttpResponse::newHttpResponse();
	response->setStatusCode(drogon::k200OK);
	callback(response);
}

void Ref::File::Form(const HttpRequestPtr& req, callback_func &&callback)
{
    Json::Value form;

    Json::Value file;
    Json::Value description;
    Json::Value tags;

    file["type"] = "file";
    file["required"] = "required";

    tags["type"] = "text";
    tags["required"] = "";

    description["type"] = "text" ;
    description["required"] = "required";

    form["file"] = file;
    form["description"] = description;
    tags["tags"] = tags;



    auto response = HttpResponse::newHttpJsonResponse(form);
    response->setStatusCode(drogon::k200OK);
    callback(response);
}
