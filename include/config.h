#pragma once
#include <string>
#include <mutex>

struct Config {
    std::string mysql_host;
    std::string mysql_user;
    std::string mysql_password;
    std::string mysql_db;
    int query_interval;
    int rest_port;
    int threshold;
    
    void loadFromFile(const std::string& path); 
};

class GlobalConfig {
public:
    static Config& getInstance();
    static std::mutex& getMutex();
private:
    GlobalConfig() {}
    static Config config_instance;
    static std::mutex config_mutex;
};
