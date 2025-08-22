// src/api.cpp
#include "api.h"
#include "crow_all.h"
#include <nlohmann/json.hpp>

// El constructor es una implementación, va en el .cpp
ApiServer::ApiServer(int port) : port(port) {
    app = std::make_unique<crow::SimpleApp>();

    CROW_ROUTE((*app), "/status")
    ([this]() {
        std::lock_guard<std::mutex> lock(status_mutex);
        
        nlohmann::json response_json;
        if (current_status.last_check == "null") {
            response_json["last_check"] = nullptr;
        } else {
            response_json["last_check"] = current_status.last_check;
        }
        response_json["rows_processed"] = current_status.rows_processed;
        response_json["alerts"] = current_status.alerts;
        
        return crow::response(200, response_json.dump());
    });
}

// El destructor también va en el .cpp
ApiServer::~ApiServer() = default;

// Todas las demás funciones deben estar aquí
void ApiServer::run() {
    app->port(port).run();
}

void ApiServer::stop() {
    app->stop();
}

void ApiServer::updateStatus(const ServiceStatus& new_status) {
    std::lock_guard<std::mutex> lock(status_mutex);
    current_status = new_status;
}