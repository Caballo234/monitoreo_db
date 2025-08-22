#include "config.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <stdexcept>

Config GlobalConfig::config_instance;
std::mutex GlobalConfig::config_mutex;

Config& GlobalConfig::getInstance() {
    return config_instance;
}

std::mutex& GlobalConfig::getMutex() {
    return config_mutex;
}

void Config::loadFromFile(const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open()) {
        throw std::runtime_error("Could not open config file: " + path);
    }
    nlohmann::json data = nlohmann::json::parse(f);

    mysql_host = data.at("mysql_host").get<std::string>();
    mysql_user = data.at("mysql_user").get<std::string>();
    mysql_password = data.at("mysql_password").get<std::string>();
    mysql_db = data.at("mysql_db").get<std::string>();
    query_interval = data.at("query_interval").get<int>();
    rest_port = data.at("rest_port").get<int>();
    threshold = data.at("threshold").get<int>();    
}