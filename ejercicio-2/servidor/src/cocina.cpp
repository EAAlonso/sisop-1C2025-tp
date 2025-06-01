#include "../headers/cocina.hpp"
#include "../headers/json.hpp"

#include <iostream>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>

using nlohmann::json;

Cocina::Cocina(int puerto) : puerto(puerto), servidorActivo(true) {}

Cocina::~Cocina() {
    apagar();
}

void Cocina::iniciar() {
    socketServidor = socket(AF_INET, SOCK_STREAM, 0);
    if (socketServidor < 0) {
        perror("Error al crear socket");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(socketServidor, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in direccionServidor{};
    direccionServidor.sin_family = AF_INET;
    direccionServidor.sin_addr.s_addr = INADDR_ANY;
    direccionServidor.sin_port = htons(puerto);

    if (bind(socketServidor, (struct sockaddr*)&direccionServidor, sizeof(direccionServidor)) < 0) {
        perror("Error en bind");
        exit(EXIT_FAILURE);
    }

    if (listen(socketServidor, 10) < 0) {
        perror("Error en listen");
        exit(EXIT_FAILURE);
    }

    std::cout << "[Cocina] Servidor escuchando en puerto " << puerto << std::endl;

    // Lanzar cocineros
    for (int i = 0; i < 5; ++i) {
        hilosCocineros.emplace_back(&Cocina::cocineroLoop, this, i + 1);
    }

    // Lanzar hilo aceptador
    hiloAceptador = std::thread(&Cocina::aceptarClientes, this);
}

void Cocina::aceptarClientes() {
    while (servidorActivo) {
        sockaddr_in direccionCliente;
        socklen_t tam = sizeof(direccionCliente);
        int socketCliente = accept(socketServidor, (struct sockaddr*)&direccionCliente, &tam);
        if (socketCliente < 0) {
            if (servidorActivo) perror("[Cocina] Error en accept");
            continue;
        }

        char buffer[1024] = {0};
        ssize_t bytes = recv(socketCliente, buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0) {
            close(socketCliente);
            continue;
        }

        try {
            json pedidoJson = json::parse(buffer);
            Pedido pedido;
            pedido.combo = pedidoJson.value("combo", "");
            pedido.clienteSocket = socketCliente;

            {
                std::lock_guard<std::mutex> lock(mutexCola);
                colaPedidos.push(pedido);
            }
            cvPedidos.notify_one();

            std::cout << "[Cocina] Pedido recibido: " << pedido.combo << std::endl;

        } catch (...) {
            std::cerr << "[Cocina] Error al parsear JSON del cliente." << std::endl;
            close(socketCliente);
        }
    }
}

void Cocina::cocineroLoop(int id) {
    while (servidorActivo) {
        Pedido pedido;
        {
            std::unique_lock<std::mutex> lock(mutexCola);
            cvPedidos.wait(lock, [&]() { return !colaPedidos.empty() || !servidorActivo; });

            if (!servidorActivo && colaPedidos.empty()) return;

            pedido = colaPedidos.front();
            colaPedidos.pop();
        }

        auto logPaso = [&](const std::string& paso) {
            std::cout << "[Cocinero " << id << "] " << paso << std::endl;
        };

        logPaso("Tomando pedido...");
        sleep(2);

        logPaso("Cocinando...");
        sleep(2);

        if (pedido.combo == "S") {
            logPaso("Armando combo: 1 carne, 1 queso, 2 panes");
            sleep(3);
        } else if (pedido.combo == "D") {
            logPaso("Armando combo: 2 carnes, 2 quesos, 3 panes");
            sleep(5);
        } else {
            logPaso("Armando combo: 2 carnes, 2 quesos, lechuga, tomate");
            sleep(7);
        }

        logPaso("Empaquetando...");
        sleep(3);

        logPaso("Entregado.");
        sleep(2);

        close(pedido.clienteSocket);
    }
}


void Cocina::apagar() {
    servidorActivo = false;
    close(socketServidor);
    cvPedidos.notify_all();

    if (hiloAceptador.joinable()) hiloAceptador.join();

    for (auto& hilo : hilosCocineros) {
        if (hilo.joinable()) hilo.join();
    }

    std::cout << "[Cocina] Apagado completo." << std::endl;
}
