
// Andrew Nelson
// CS405 Secure Coding: SQL Injection

#include <algorithm>
#include <iostream>
#include <locale>
#include <tuple>
#include <vector>
#include <regex>

#include "sqlite3.h"

// DO NOT CHANGE
typedef std::tuple<std::string, std::string, std::string> user_record;
const std::string str_where = " where ";

// DO NOT CHANGE
static int callback(void* possible_vector, int argc, char** argv, char** azColName)
{
    if (possible_vector == NULL)
    { // no vector passed in, so just display the results
        for (int i = 0; i < argc; i++)
        {
            std::cout << azColName[i] << " = " << (argv[i] ? argv[i] : "NULL") << std::endl;
        }
        std::cout << std::endl;
    }
    else
    {
        std::vector< user_record >* rows =
            static_cast<std::vector< user_record > *>(possible_vector);

        rows->push_back(std::make_tuple(argv[0], argv[1], argv[2]));
    }
    return 0;
}

// DO NOT CHANGE
bool initialize_database(sqlite3* db)
{
    char* error_message = NULL;
    std::string sql = "CREATE TABLE USERS(" \
        "ID INT PRIMARY KEY     NOT NULL," \
        "NAME           TEXT    NOT NULL," \
        "PASSWORD       TEXT    NOT NULL);";

    int result = sqlite3_exec(db, sql.c_str(), callback, NULL, &error_message);
    if (result != SQLITE_OK)
    {
        std::cout << "Failed to create USERS table. ERROR = " << error_message << std::endl;
        sqlite3_free(error_message);
        return false;
    }
    std::cout << "USERS table created." << std::endl;

    // insert some dummy data
    sql = "INSERT INTO USERS (ID, NAME, PASSWORD)" \
        "VALUES (1, 'Fred', 'Flinstone');" \
        "INSERT INTO USERS (ID, NAME, PASSWORD)" \
        "VALUES (2, 'Barney', 'Rubble');" \
        "INSERT INTO USERS (ID, NAME, PASSWORD)" \
        "VALUES (3, 'Wilma', 'Flinstone');" \
        "INSERT INTO USERS (ID, NAME, PASSWORD)" \
        "VALUES (4, 'Betty', 'Rubble');";

    result = sqlite3_exec(db, sql.c_str(), callback, NULL, &error_message);
    if (result != SQLITE_OK)
    {
        std::cout << "Data failed to insert to USERS table. ERROR = " << error_message << std::endl;
        sqlite3_free(error_message);
        return false;
    }

    return true;
}

bool run_query(sqlite3* db, const std::string& sql, std::vector< user_record >& records)
{
    // Clear previous query results
    records.clear();

    // Define a regular expression pattern to detect SQL injection attempts.
    // 
    // - `\bor\s+`          Matches "OR" as a separate word (`\b` ensures it's not part of another word).
    // - `['"]?\w+['"]?`    Matches a word or string, optionally enclosed in quotes (e.g., 'admin').
    // - `\s*=\s*`          Matches the equal sign (`=`) with optional spaces around it.
    // - `['"]?\w+['"]?`    Matches another word or string, completing the comparison.
    // 
    // - `std::regex_constants::icase` Enables case-insensitive matching (detects "or", "OR", "Or", etc.).
    std::regex sql_injection_pattern(R"(\bor\s+['"]?\w+['"]?\s*=\s*['"]?\w+['"]?)", std::regex_constants::icase);

    // Check if the given SQL query contains an SQL injection pattern.
    if (std::regex_search(sql, sql_injection_pattern))
    {
        // If a pattern match is found, print a warning message and reject the query.
        std::cout << "\nWARNING: \nSQL Injection attempt detected! \nQuery rejected: " << sql << std::endl;
        return false; // Exit the function to prevent execution of the query.
    }

    // Declare a prepared statement to execute the SQL query.
    sqlite3_stmt* stmt;

    // Prepare the SQL statement to prevent SQL injection by using sqlite3_prepare_v2.
    // This function compiles the SQL query into a bytecode that SQLite can safely execute.
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
    {
        // If the preparation fails, print the SQLite error message.
        std::cout << "SQL statement preparation failed. ERROR = " << sqlite3_errmsg(db) << std::endl;
        return false; // Exit the function, the query could not be prepared.
    }

    // Variable to store the execution result of the prepared statement.
    int results;

    // Execute the query and retrieve each row from the results
    while ((results = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        // Extract the first name
        std::string firstName = (const char*)sqlite3_column_text(stmt, 1);

        // Extract the last name
        std::string lastName = (const char*)sqlite3_column_text(stmt, 2);

        // Print the first and last name
        std::cout << "Username: " << firstName << " " << lastName << std::endl;
    }

    // Finalize the prepared statement to release resources.
    sqlite3_finalize(stmt);

    // Check if the query execution completed successfully.
    if (results != SQLITE_DONE)
    {
        // If execution was unsuccessful, print an error message.
        std::cout << "Error executing query. ERROR = " << sqlite3_errmsg(db) << std::endl;
        return false; // Return false indicating failure.
    }

    // If everything executes successfully, return true.
    return true;
}

// DO NOT CHANGE
bool run_query_injection(sqlite3* db, const std::string& sql, std::vector< user_record >& records)
{
    std::string injectedSQL(sql);
    std::string localCopy(sql);

    // we work on the local copy because of the const
    std::transform(localCopy.begin(), localCopy.end(), localCopy.begin(), ::tolower);
    if (localCopy.find_last_of(str_where) >= 0)
    { // this sql has a where clause
        if (localCopy.back() == ';')
        { // smart SQL developer terminated with a semicolon - we can fix that!
            injectedSQL.pop_back();
        }

        switch (rand() % 4)
        {
        case 1:
            injectedSQL.append(" or 2=2;");
            break;
        case 2:
            injectedSQL.append(" or 'hi'='hi';");
            break;
        case 3:
            injectedSQL.append(" or 'hack'='hack';");
            break;
        case 0:
        default:
            injectedSQL.append(" or 1=1;");
            break;
        }
    }

    return run_query(db, injectedSQL, records);
}


// DO NOT CHANGE
void dump_results(const std::string& sql, const std::vector< user_record >& records)
{
    std::cout << std::endl << "SQL: " << sql << " ==> " << records.size() << " records found." << std::endl;

    for (auto record : records)
    {
        std::cout << "User: " << std::get<1>(record) << " [UID=" << std::get<0>(record) << " PWD=" << std::get<2>(record) << "]" << std::endl;
    }
}

// DO NOT CHANGE
void run_queries(sqlite3* db)
{
    char* error_message = NULL;

    std::vector< user_record > records;

    // query all
    std::string sql = "SELECT * from USERS";
    if (!run_query(db, sql, records)) return;
    dump_results(sql, records);

    //  query 1
    sql = "SELECT ID, NAME, PASSWORD FROM USERS WHERE NAME='Fred'";
    if (!run_query(db, sql, records)) return;
    dump_results(sql, records);

    //  run query 1 with injection 5 times
    for (auto i = 0; i < 5; ++i)
    {
        if (!run_query_injection(db, sql, records)) continue;
        dump_results(sql, records);
    }

}

// You can change main by adding stuff to it, but all of the existing code must remain, and be in the
// in the order called, and with none of this existing code placed into conditional statements
int main()
{
    // initialize random seed:
    srand(time(nullptr));

    int return_code = 0;
    std::cout << "SQL Injection Example" << std::endl;

    // the database handle
    sqlite3* db = NULL;
    char* error_message = NULL;

    // open the database connection
    int result = sqlite3_open(":memory:", &db);

    if (result != SQLITE_OK)
    {
        std::cout << "Failed to connect to the database and terminating. ERROR=" << sqlite3_errmsg(db) << std::endl;
        return -1;
    }

    std::cout << "Connected to the database." << std::endl;

    // initialize our database
    if (!initialize_database(db))
    {
        std::cout << "Database Initialization Failed. Terminating." << std::endl;
        return_code = -1;
    }
    else
    {
        run_queries(db);
    }

    // close the connection if opened
    if (db != NULL)
    {
        sqlite3_close(db);
    }

    return return_code;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu
