#include "API_WebSocet_WebHook.h"


using namespace API;

void WebSocet::WebHook::handleNewMessage(const WebSocketConnectionPtr& wsConnPtr, std::string &&message, const WebSocketMessageType &type)
{
    LOG_INFO << "NEW MESSAGE: " << message;
}

void WebSocet::WebHook::handleNewConnection(const HttpRequestPtr &req, const WebSocketConnectionPtr& wsConnPtr)
{
    std::cout << Hooks::user_addres.value().toIp() << " ?= " << req->getPeerAddr().toIp();


    if (Hooks::user_addres.has_value() && Hooks::user_addres.value().toIp() != req->getPeerAddr().toIp())
    {
        wsConnPtr->shutdown(CloseCode::kNormalClosure, "Обновление уже выполняется другим пользователем");
        return;
    }

    wsConnPtr->send(utils::base64Encode("START UPDATE\n\n"));

    Request::HookGit::Update(wsConnPtr);

    wsConnPtr->send(utils::base64Encode("END UPDATE\n\n"));
}

void WebSocet::WebHook::handleConnectionClosed(const WebSocketConnectionPtr& wsConnPtr)
{
    LOG_INFO << "CLOSE CONNECT";
    Hooks::user_addres.reset();
}
