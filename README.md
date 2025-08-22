# monitoreo_db
Servicio de monitorización en C++ con API REST, MySQL y Docker.

Este repositorio contiene el código fuente de un servicio de monitorización desarrollado en C++. El servicio consulta periódicamente una base de datos MySQL, expone un endpoint REST para verificar su estado, gestiona su configuración dinámicamente y está diseñado para ser desplegado fácilmente mediante Docker.

---
## Características Principales

* **Consulta Periódica a MySQL:** El servicio ejecuta consultas a una tabla de métricas en intervalos de tiempo configurables.
* **Logs Estructurados en JSON:** Toda la actividad, incluyendo las métricas procesadas y las alertas, se registra en formato JSON.
* **Alertas por Umbral:** Genera logs de nivel `WARNING` cuando el valor de una métrica supera un umbral configurable.
* **Endpoint REST de Estado:** Expone un endpoint `GET /status` que devuelve el estado más reciente del servicio.
* **Configuración Dinámica:** Permite recargar la configuración en caliente (sin reiniciar) al recibir la señal `SIGHUP`.
* **Gestión de Señales:** Se apaga de forma ordenada al recibir las señales `SIGTERM` o `SIGINT`.
* **Despliegue con Docker:** Incluye un `Dockerfile` multi-etapa para una construcción optimizada y un archivo `docker-compose.yml` para orquestar el servicio junto con una base de datos MySQL.

---
## Stack Tecnológico

* **Lenguaje:** C++17
* **Base de Datos:** MySQL 8.0
* **Librerías Principales:**
    * **Crow C++:** Micro-framework para el servidor HTTP y el endpoint REST.
    * **MySQL Connector/C++:** Librería oficial para la conexión con MySQL (versión 1.1.12 a través de los repositorios de Ubuntu 22.04).
    * **Nlohmann JSON:** Para la manipulación de archivos y logs en formato JSON.
    * **Asio:** Librería de red (dependencia de Crow).
* **Contenerización:** Docker y Docker Compose.
* **Sistema de Construcción:** `make`.

---
## Instrucciones de Uso y Despliegue

La forma recomendada de ejecutar este proyecto es utilizando Docker Compose, ya que gestiona automáticamente la red, la base de datos y el servicio.

### Prerrequisitos

* Docker y Docker Compose instalados.
* Git para clonar el repositorio.
* `make` y `g++` para compilación local (opcional).

### Ejecución con Docker Compose (Recomendado)

1.  **Clona el repositorio:**
    ```bash
    git clone [https://github.com/Caballo234/monitoreo_db.git](https://github.com/Caballo234/monitoreo_db.git)
    cd monitoreo_db
    ```

2.  **Modifica la configuración (Opcional):**
    Puedes ajustar los parámetros en el archivo `config/config.json` antes de iniciar.

3.  **Levanta los servicios:**
    Desde la raíz del proyecto, ejecuta el siguiente comando. La primera vez construirá la imagen de Docker, lo que puede tardar unos minutos.
    ```bash
    docker compose up --build
    ```
    Verás los logs de la base de datos MySQL y de tu servicio de monitoreo en la misma terminal. El servicio esperará a que la base de datos esté completamente lista antes de arrancar.

---
## Cómo Probar el Servicio

Una vez que los contenedores estén corriendo con `docker compose up`, abre una **nueva terminal** para realizar las pruebas.

### 1. Verificar el Endpoint `/status`

Ejecuta el siguiente comando. La primera vez puede mostrar valores nulos o en cero hasta que se complete el primer ciclo de consulta.

```bash
curl http://localhost:8080/status
```
### Salida de ejemplo 

```bash
{"alerts":1,"last_check":"2025-08-22T17:14:29Z","rows_processed":3}
```

2.  **Ver los Logs:**
   Los logs se escriben en el archivo monitor_service.log dentro del contenedor. Para verlos en tiempo real:
    ```bash
    docker exec -it monitor_service tail -f monitor_service.log
    ```
3.  **Detener el servicio:**
    Para detener ambos contenedores de forma ordenada, regresa a la terminal donde ejecutaste docker compose up y presiona Ctrl + C.
