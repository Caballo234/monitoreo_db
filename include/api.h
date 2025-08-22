// include/api.h
#pragma once
#include <string>
#include <mutex>
#include <memory>

namespace crow {
    template<typename... Middlewares>
    class Crow;
}

struct ServiceStatus {
    std::string last_check = "null";
    int rows_processed = 0;
    int alerts = 0;
};

class ApiServer {
public:
    ApiServer(int port);
    ~ApiServer();

    // SOLO LAS FIRMAS DE LAS FUNCIONES, terminando en ;
    void run();
    void stop();
    void updateStatus(const ServiceStatus& new_status);

private:
    std::unique_ptr<crow::Crow<>> app;
    ServiceStatus current_status;
    std::mutex status_mutex;
    int port;
};