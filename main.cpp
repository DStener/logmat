#include <drogon/drogon.h>
#include <locale>

int main()
{
    //Load config file
    drogon::app().loadConfigFile("config.json");

    drogon::app().setCustom404Page(drogon::HttpResponse::newHttpViewResponse("notfound"));
    //Run HTTP framework,the method will block in the internal event loop
    drogon::app().run();

    
    

    return 0;
}
