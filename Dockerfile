# ---- Etapa 1: Compilación (Builder) ----
# (Esta etapa no cambia y está bien)
FROM ubuntu:22.04 AS builder
RUN apt-get update && apt-get install -y \
    build-essential g++ libmysqlcppconn-dev libasio-dev nlohmann-json3-dev
WORKDIR /app
COPY . .
RUN make

# ---- Etapa 2: Ejecución (Runtime) ----
FROM ubuntu:22.04

RUN apt-get update && apt-get install -y libmysqlcppconn7v5 && rm -rf /var/lib/apt/lists/*

# --- ARREGLO: Realiza las operaciones como root PRIMERO ---
# 1. Crear el directorio de logs
RUN mkdir -p /var/log
# 2. Crear el usuario
RUN useradd -ms /bin/bash appuser
# 3. Darle permisos al usuario sobre el directorio de logs
RUN chown appuser:appuser /var/log

WORKDIR /home/appuser/app

# Copiar los archivos compilados
COPY --from=builder /app/build/monitor_service .
COPY --from=builder /app/config/config.json ./config/

# Darle permisos al usuario sobre la carpeta de la aplicación
RUN chown -R appuser:appuser /home/appuser/app

# --- ARREGLO: Cambia al usuario no-root al FINAL ---
USER appuser

EXPOSE 8080
CMD ["./monitor_service"]