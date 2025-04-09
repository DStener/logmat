#include "DataBase_Seeder.h"
#include <ranges>

using namespace drogon;
using namespace DataBase;

void Seeder::initAndStart(const Json::Value &config)
{
    using std::filesystem::directory_iterator;
    using std::filesystem::directory_entry;

    // Create new Transaction
    auto clientPtr = drogon::app().getDbClient();
    clientPtr->execSqlSync("BEGIN TRANSACTION;");

    // Get parametr "seedrs_directory"
    const auto seedrs = config.get("seedrs_directory", "").asString();

    // Check, that config has value and value is exists path
    if (seedrs.empty() || !std::filesystem::exists(seedrs)) { return; }

    // Sorted file in directory
    std::vector<directory_entry> entries(directory_iterator(seedrs), directory_iterator{});
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
            for (auto const& command : utils::splitString(sstr.str(), ";"))
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

void Seeder::shutdown() 
{
    /// Shutdown the plugin
}
