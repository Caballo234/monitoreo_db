#include "config.h"
#include "logger.h"
#include "database.h"
#include "api.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <csignal>
#include <memory>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <vector>

volatile sig_atomic_t shutdown_requested = 0;
volatile sig_atomic_t reload_config_requested = 0;
ApiServer* g_api_server = nullptr;

void signalHandler(int signum) {
    if (signum == SIGINT || signum == SIGTERM) {
        Logger::getInstance().log(LogLevel::info, "Señal de apagado recibida");
        shutdown_requested = 1;
        if (g_api_server) {
            g_api_server->stop();
        }
    } else if (signum == SIGHUP) {
        Logger::getInstance().log(LogLevel::info, "Recargando configuración");
        reload_config_requested = 1;
    }
}

void workerThread(ApiServer* api_server, std::shared_ptr<Database> db) {
    Logger::getInstance().log(LogLevel::info, "Hilo iniciado");

    while (!shutdown_requested) {
        if (reload_config_requested) {
            Logger::getInstance().log(LogLevel::info, "Recargando configuración");
            try {
                std::lock_guard<std::mutex> lock(GlobalConfig::getMutex());
                auto& config = GlobalConfig::getInstance();
                config.loadFromFile("config/config.json");
                
                db = std::make_shared<Database>(config.mysql_host, config.mysql_user, config.mysql_password, config.mysql_db);
                db->connect();
                Logger::getInstance().log(LogLevel::info, "Configuración actualizada");
            } catch (const std::exception& e) {
                Logger::getInstance().log(LogLevel::error, "Error con actualizar configuración: " + std::string(e.what()));
            }
            reload_config_requested = 0;
        }

        try {
            int current_threshold;
            { 
                std::lock_guard<std::mutex> lock(GlobalConfig::getMutex());
                current_threshold = GlobalConfig::getInstance().threshold;
            }

            QueryResult result = db->checkMetrics(current_threshold);
            
            ServiceStatus status;
            status.rows_processed = result.rows_processed;
            status.alerts = result.alerts;

            auto now = std::chrono::system_clock::now();
            auto in_time_t = std::chrono::system_clock::to_time_t(now);
            std::tm tm_buf{};
            std::stringstream ss;
            ss << std::put_time(gmtime_r(&in_time_t, &tm_buf), "%Y-%m-%dT%H:%M:%SZ");
            status.last_check = ss.str();
            
            api_server->updateStatus(status);

            std::stringstream log_msg;
            log_msg << "Filas procesadas: " << result.rows_processed << ", Alertas: " << result.alerts << ".";
            Logger::getInstance().log(LogLevel::info, log_msg.str());

        } catch (const std::exception& e) {
            Logger::getInstance().log(LogLevel::error, "Error durante chequeo de BD: " + std::string(e.what()));
        }

        int interval;
        {
            std::lock_guard<std::mutex> lock(GlobalConfig::getMutex());
            interval = GlobalConfig::getInstance().query_interval;
        }
        for (int i = 0; i < interval && !shutdown_requested; ++i) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    Logger::getInstance().log(LogLevel::info, "Apagando hilo");
}

int main() {
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    signal(SIGHUP, signalHandler);

    try {
        {
            std::lock_guard<std::mutex> lock(GlobalConfig::getMutex());
            GlobalConfig::getInstance().loadFromFile("config/config.json");
        }
        
        Logger::getInstance().log(LogLevel::info, "Inicio de servicio de monitoreo");

        Logger::getInstance().log(LogLevel::info, "Conexión a base de datos");
        std::shared_ptr<Database> db;
        try {
            auto& config = GlobalConfig::getInstance();
            db = std::make_shared<Database>(config.mysql_host, config.mysql_user, config.mysql_password, config.mysql_db);
            db->connect();
        } catch (const std::exception& e) {
            Logger::getInstance().log(LogLevel::error, "FGallo critico en conexión a BD: " + std::string(e.what()));
            return 1;
        }
        Logger::getInstance().log(LogLevel::info, "COnexión exitosa");

        int port;
        {
            std::lock_guard<std::mutex> lock(GlobalConfig::getMutex());
            port = GlobalConfig::getInstance().rest_port;
        }
        ApiServer api_server(port);
        g_api_server = &api_server;

        std::thread worker(workerThread, &api_server, db);

        api_server.run();

        worker.join();
        Logger::getInstance().log(LogLevel::info, "Servicio apahgado correctamente");

    } catch (const std::exception& e) {
        Logger::getInstance().log(LogLevel::error, "Error: " + std::string(e.what()));
        return 1;
    }

    return 0;
}