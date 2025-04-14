#include "DataBase_Migration.h"

using namespace drogon;
using namespace DataBase;

void Migration::initAndStart(const Json::Value &config)
{
    using std::filesystem::directory_iterator;
    using std::filesystem::directory_entry;

    auto clientPtr = drogon::app().getDbClient();
    
    // Get parametr "migrations_directory"
    const auto migrations = config.get("migrations_directory", "").asString();

    // Check, that config has value and value is exists path
    if (migrations.empty() || !std::filesystem::exists(migrations)) { return; }

    // Begin new transaction
    clientPtr->execSqlSync("BEGIN TRANSACTION;");

    // Sorted file in directory
    std::vector<directory_entry> entries(directory_iterator(migrations), directory_iterator{});
    std::ranges::sort(entries, {}, &directory_entry::path);

    for (auto const& entry : entries)
    {
        // Check that file exists and it has ".sql" extension
        if (!entry.exists() || entry.path().extension() != ".sql") { continue; }

        // Open file as ifstream
        std::ifstream istrm(entry.path());

        if (istrm.is_open())
        {
            std::ostringstream sstr;
            sstr << istrm.rdbuf();

            // Multiple semicolon separated statements are not supported,
            // so we break them down into separate commands.
            for (auto const& command : utils::splitString(sstr.str(), ";;"))
            {
                // Execute sql command from file 
                clientPtr->execSqlAsync(command,
                    [](const drogon::orm::Result& result) {},
                    [](const orm::DrogonDbException& e) { LOG_ERROR << " error:" << e.base().what();});
            }
        }
        // Close file
        istrm.close();
    }
    clientPtr->execSqlSync("COMMIT;");
}

void Migration::shutdown() 
{
    /// Shutdown the plugin
}
