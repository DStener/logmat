#include <drogon/drogon.h>
#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>

#ifndef NDEBUG
#ifdef _WIN32
static const std::string ROOTDIR = "../../../root";
#else
static const std::string ROOTDIR = "../../root";
#endif
#else
static const std::string ROOTDIR = "root";
#endif

int main()
{

    // Configure DB connection
    drogon::orm::DbConfig config = drogon::orm::Sqlite3Config {
        1,
        "weblab.db",
        "master",
        0
    };

    std::cout << ROOTDIR  << std::endl;

    // Configure and setup instance
    // URL: http://0.0.0.0:5555/
    drogon::app()
        .addDbClient(config)
        .setDocumentRoot(ROOTDIR)
        .addListener("0.0.0.0", 5555); // Set HTTP listner address and port

    drogon::app().run();

    return 0;
}
