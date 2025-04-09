﻿#include "API_WebSocet_WebHook.h"

using namespace API::WebSocet;

static std::optional<trantor::InetAddress> addres;

void WebHook::handleNewMessage(const WebSocketConnectionPtr& wsConnPtr, std::string &&message, const WebSocketMessageType &type)
{
    LOG_INFO << "NEW MESSAGE: " << message;
}

void WebHook::handleNewConnection(const HttpRequestPtr &req, const WebSocketConnectionPtr& wsConnPtr)
{

    if (addres.has_value())
    {
        wsConnPtr->shutdown(CloseCode::kNormalClosure, "Обновление уже выполняется другим пользователем");
        return;
    }
    
    // Set user address
    addres.emplace(req->getLocalAddr());
    wsConnPtr->send(utils::urlEncode("START UPDATE"));


    Request::HookGit::Update(wsConnPtr);

    wsConnPtr->send(utils::urlEncode("END UPDATE"));
}

void WebHook::handleConnectionClosed(const WebSocketConnectionPtr& wsConnPtr)
{
    LOG_INFO << "CLOSE CONNECT";
    addres.reset();
}
