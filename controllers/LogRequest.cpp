#include "LogRequest.h"
//#include "System/database.hpp"
#include "System/DTO.hpp"
#include "Request/login.hpp"

using namespace API::Ref;

void Log::Request::logger(const HttpRequestPtr& req, const HttpResponsePtr& resp)
{
	std::vector<std::string> response_head;
	std::vector<std::string> request_head;

	// Fill response_head
	for (auto const& record : resp->getHeaders()) {
		response_head.push_back(std::format("{}: {}", record.first, record.second));
	}
	// Fill request_head
	for (auto const& record : req->getHeaders()) {
		request_head.push_back(std::format("{}: {}", record.first, record.second));
	}

	// Find the record of controller;
	std::string controller;
	const std::string_view path = req->getMatchedPathPattern();

	for (auto const& handle : drogon::app().getHandlersInfo()) {

		if (path != std::get<0>(handle)) {
			continue;
		}

		// Remove "HttpController:"
		controller = std::get<2>(handle).substr(
			std::get<2>(handle).find_last_of(' ') + 1);
		break;
	}

	std::string controller_path = "none";
	std::string controller_method = controller;

	// Find 
	size_t sep = controller.find_last_of("::");
	if (sep != std::string::npos) {
		controller_path = controller.substr(0, sep - 1);
		controller_method = controller.substr(sep + 1);
	}


	// Get id_user
	id_t id_user = 0;
	auto tokens = DB::get()->Select<::Token>(std::format("token == \"{}\"", req->session()->sessionId()));

	if (!tokens.empty()) {
		id_user = tokens[0].second.user;
	}

	std::string request_body{ req->getBody() };
	std::string response_body{ resp->getBody() };

	boost::algorithm::replace_all(request_body, "\"", "'");
	boost::algorithm::replace_all(response_body, "\"", "'");


	::LogRequest log;
	log.address				=	req->getPath();								
	log.method				=	req->methodString();						
	log.controller_path		=	controller_path;							
	log.controller_method	=	controller_method;							
	log.request_body		=	request_body;								
	log.request_head		=	boost::algorithm::join(request_head, ", ");	
	log.id_user				=	id_user;									
	log.ip					=	req->getPeerAddr().toIp();					
	log.user_agent			=	req->getHeader("user-agent");				
	log.response_code		=	static_cast<size_t>(resp->getStatusCode());	
	log.response_body		=	response_body;								
	log.response_head		=	boost::algorithm::join(response_head, ", ");
	log.time				=	std::chrono::system_clock::now();

	app().getDbClient()->execSqlSync(DTO::SQL::Insert(log));
}


void Log::Request::GetList(const HttpRequestPtr& req, callback_func&& callback)
{
	// Login and check that it is admin
	//auto login = ::Request::Login(req, callback);
	//if (!login.id || !login.isAdmin()) { return; }

	/*auto logs = DB::get()->Select<::LogRequest>(std::format("id == {}", id));
	if (logs.empty()) {
		auto response = HttpResponse::newHttpJsonResponse("Incorrect id");
		response->setStatusCode(drogon::k406NotAcceptable);
		callback(response);
		return;
	}

	req->getJsonObject();

	auto response = HttpResponse::newHttpJsonResponse(DTO::JSON::From(logs[0]));
	callback(response);*/
}

void Log::Request::Get(const HttpRequestPtr& req, callback_func&& callback, id_t id)
{
	// Login and check that it is admin
	auto login = ::Request::Login(req, callback);
	if (!login.id || !login.isAdmin()) { return; }

	auto logs = DB::get()->Select<::LogRequest>(std::format("id == {}", id));
	if (logs.empty()) {
		auto response = HttpResponse::newHttpJsonResponse("Incorrect id");
		response->setStatusCode(drogon::k406NotAcceptable);
		callback(response);
		return;
	}

	req->getJsonObject();

	auto response = HttpResponse::newHttpJsonResponse(DTO::JSON::From(logs[0]));
	callback(response);	
}
