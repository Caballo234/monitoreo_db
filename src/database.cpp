#include "database.h"
#include "logger.h"
#include <nlohmann/json.hpp>
#include <stdexcept>

// Includes para la versión 1.1.12
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

Database::Database(const std::string& host, const std::string& user, const std::string& pass, const std::string& db)
    : host(host), user(user), pass(pass), db(db) {
    driver = get_driver_instance();
}

Database::~Database() {
    disconnect();
}

void Database::connect() {
    try {
        if (connection && !connection->isClosed()) {
            disconnect();
        }
        connection.reset(driver->connect(host, user, pass));
        connection->setSchema(db);
        Logger::getInstance().log(LogLevel::info, "Conexión exitosa a MySQL.");
    } catch (sql::SQLException &e) {
        std::string error_msg = "Error de conexión: " + std::string(e.what());
        Logger::getInstance().log(LogLevel::error, error_msg);
        throw;
    }
}

void Database::disconnect() {
    if (connection && !connection->isClosed()) {
        connection->close();
        Logger::getInstance().log(LogLevel::info, "Desconectado de MySQL.");
    }
}

QueryResult Database::checkMetrics(int threshold) {
    QueryResult result = {0, 0};
    try {
        std::unique_ptr<sql::Statement> stmt(connection->createStatement());
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT name, value FROM metrics"));

        while (res->next()) {
            result.rows_processed++;
            int value = res->getInt("value");
            std::string name = res->getString("name");
            
            nlohmann::json metric_log;
            metric_log["metric_name"] = name;
            metric_log["metric_value"] = value;
            Logger::getInstance().log(LogLevel::info, "Metric processed: " + metric_log.dump());

            if (value > threshold) {
                result.alerts++;
                Logger::getInstance().log(LogLevel::warning, "Threshold exceeded for metric '" + name + "'");
            }
        }
    } catch (sql::SQLException &e) {
        std::string error_msg = "MySQL Query Error: " + std::string(e.what());
        Logger::getInstance().log(LogLevel::error, error_msg);
    }
    return result;
}