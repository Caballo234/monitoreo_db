#pragma once
#include <string>
#include <memory>

// Declaraciones adelantadas para la v1.1.12
namespace sql {
    class Driver;
    class Connection;
    class Statement;
    class ResultSet;
};

struct QueryResult {
    int rows_processed;
    int alerts;
};

class Database {
public:
    Database(const std::string& host, const std::string& user, const std::string& pass, const std::string& db);
    ~Database();
    void connect();
    void disconnect();
    QueryResult checkMetrics(int threshold);
private:
    sql::Driver* driver;
    std::unique_ptr<sql::Connection> connection;
    std::string host, user, pass, db;
};