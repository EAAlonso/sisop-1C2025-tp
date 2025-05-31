#include "../headers/cocina.hpp"
#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>

using json = nlohmann::json;

Cocina::Cocina(int puerto, int capacidadMaxima)
    : puerto(puerto), capacidadMaxima(capacidadMaxima), servidorActivo(true)
{
    sem_init(&semaforoCapacidad, 0, capacidadMaxima);
}

Cocina::~Cocina()
{
    apagar();
    sem_destroy(&semaforoCapacidad);
}

void Cocina::iniciar()
{
    socketServidor = socket(AF_INET, SOCK_STREAM, 0);
    if (socketServidor < 0)
    {
        perror("Error al crear el socket");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(socketServidor, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in direccionServidor{};
    direccionServidor.sin_family = AF_INET;
    direccionServidor.sin_addr.s_addr = INADDR_ANY;
    direccionServidor.sin_port = htons(puerto);

    if (bind(socketServidor, (struct sockaddr *)&direccionServidor, sizeof(direccionServidor)) < 0)
    {
        perror("Error al bindear");
        exit(EXIT_FAILURE);
    }

    if (listen(socketServidor, 10) < 0)
    {
        perror("Error en listen");
        exit(EXIT_FAILURE);
    }

    std::cout << "[Cocina] Servidor escuchando en puerto " << puerto << std::endl;
    aceptarClientes();
}

void Cocina::aceptarClientes()
{
    while (servidorActivo)
    {
        sockaddr_in direccionCliente{};
        socklen_t longitud = sizeof(direccionCliente);

        int socketCliente = accept(socketServidor, (struct sockaddr *)&direccionCliente, &longitud);
        if (socketCliente < 0)
        {
            if (!servidorActivo) {
                std::cout << "[Cocina] Servidor fue apagado manualmente." << std::endl;
                break;
            }

            perror("[Cocina] Error en accept");
            continue;
        }

        // Esperar turno para usar un cocinero (capacidad limitada)
        sem_wait(&semaforoCapacidad);

        // Lanzar hilo para procesar al cliente
        std::lock_guard<std::mutex> lock(mutexHilos);
        hilosActivos.emplace_back(&Cocina::manejarCliente, this, socketCliente);
    }
}

// Lógica de procesamiento del pedido
#include "../headers/json.hpp"

#include "../headers/combo.hpp"
#include "../headers/json.hpp" // Asegurate de incluir json.hpp
#include <unistd.h>            // para close() y sleep()

void Cocina::manejarCliente(int socket)
{
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));

    ssize_t bytesRecibidos = recv(socket, buffer, sizeof(buffer) - 1, 0);
    if (bytesRecibidos <= 0) {
        std::cout << "[Cocina] Cliente desconectado inesperadamente." << std::endl;
        close(socket);
        sem_post(&semaforoCapacidad);
        return;
    }

    try {
        nlohmann::ordered_json recibido = nlohmann::ordered_json::parse(buffer);
        std::string tipo = recibido.value("tipo", "");
        std::string comboStr = recibido.value("combo", "");

        if (tipo != "pedido" || comboStr.empty()) {
            std::cerr << "[Cocina] Pedido inválido recibido." << std::endl;
            close(socket);
            sem_post(&semaforoCapacidad);
            return;
        }

        // Crear el combo a partir del pedido
        Combo combo(comboStr);
        std::cout << "[Cocina] Pedido recibido - Combo: " << combo.getTipo() << std::endl;

        // Enviar los pasos como JSON
        auto enviarPaso = [&](const std::string& estado, const std::string& extra = "", int delaySeg = 1) {
            nlohmann::ordered_json mensaje;
            mensaje["estado"] = estado;
            if (!extra.empty())
                mensaje["detalle"] = extra;

            std::string str = mensaje.dump() + "\n";
            send(socket, str.c_str(), str.size(), 0);

            std::cout << "[Cocina] " << mensaje.dump() << std::endl;
            sleep(delaySeg);
        };

        enviarPaso("tomando_pedido");
        enviarPaso("cocinando");
        enviarPaso("armando_combo", combo.getDetalle());
        enviarPaso("empaquetando");
        enviarPaso("entregado");

    } catch (const nlohmann::json::parse_error& e) {
        std::cerr << "[Cocina] Error al parsear JSON: " << e.what() << std::endl;
    }

    close(socket);
    sem_post(&semaforoCapacidad);
}



// Cerrar conexión de forma segura
void Cocina::cerrarConexion(int socket)
{
    close(socket);
}

// Apagar el servidor
void Cocina::apagar()
{
    servidorActivo = false;
    close(socketServidor);

    {
        std::lock_guard<std::mutex> lock(mutexHilos);
        for (auto &hilo : hilosActivos)
        {
            if (hilo.joinable())
                hilo.join();
        }
        hilosActivos.clear();
    }

    std::cout << "[Cocina] Servidor apagado." << std::endl;
}
