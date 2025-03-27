#include <drogon/drogon.h>


static const std::string ROOTDIR = "../../root";


int main()
{

    // Configure DB connection
    drogon::orm::DbConfig config = drogon::orm::Sqlite3Config {
        1,
        "weblab.db",
        "master",
        0
    };

    drogon::app().enableReusePort(true);
    // drogon::app().enableDynamicViewsLoading({"../../views"});
    // drogon::app().enableDynamicViewsLoading()


    // Configure and setup instance
    // URL: http://0.0.0.0:5555/
    drogon::app()
        .addDbClient(config)
        .setDocumentRoot(ROOTDIR)
        .addListener("0.0.0.0", 5555); // Set HTTP listner address and port

    drogon::app().run();

    return 0;
}
