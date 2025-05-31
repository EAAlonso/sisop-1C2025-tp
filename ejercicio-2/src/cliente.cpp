#include "../headers/cliente.hpp"
#include "../headers/json.hpp"

#include <iostream>
#include <thread>
#include <unistd.h>
#include <arpa/inet.h>
#include <csignal>

using nlohmann::json;

Cliente::Cliente(const std::string& combo)
    : combo(combo), socketCliente(-1), conectado(false) {}

Cliente::~Cliente() {
    cerrar();
}

bool Cliente::conectar(const std::string& host, int puerto) {
    socketCliente = socket(AF_INET, SOCK_STREAM, 0);
    if (socketCliente < 0) {
        std::cerr << "[-] Error al crear el socket.\n";
        return false;
    }

    sockaddr_in servidor{};
    servidor.sin_family = AF_INET;
    servidor.sin_port = htons(puerto);
    inet_pton(AF_INET, host.c_str(), &servidor.sin_addr);

    if (connect(socketCliente, (struct sockaddr*)&servidor, sizeof(servidor)) < 0) {
        std::cerr << "[-] No se pudo conectar al servidor.\n";
        return false;
    }

    conectado = true;
    return true;
}

void Cliente::enviarPedido() {
    json pedido;
    pedido["tipo"] = "pedido";
    pedido["combo"] = combo;

    std::string msg = pedido.dump() + "\n";
    send(socketCliente, msg.c_str(), msg.size(), 0);
}

void Cliente::escucharServidor() {
    char buffer[1024];

    while (conectado) {
        ssize_t bytes = recv(socketCliente, buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0) {
            std::cout << "[Cliente] Servidor desconectado.\n";
            break;
        }

        buffer[bytes] = '\0';

        try {
            json respuesta = json::parse(buffer);
            std::cout << "[Servidor] Estado: " << respuesta["estado"];
            if (respuesta.contains("detalle"))
                std::cout << " - " << respuesta["detalle"];
            std::cout << std::endl;
        } catch (...) {
            std::cerr << "[Cliente] Error al interpretar mensaje JSON.\n";
        }
    }

    conectado = false;
}

void Cliente::iniciar() {
    if (!conectado) return;

    enviarPedido();

    std::thread escucha(&Cliente::escucharServidor, this);

    std::cout << "\n[Cliente] Presione ENTER para cerrar la conexión.\n";
    std::cin.ignore();
    std::cin.get();

    cerrar();
    escucha.join();

    std::cout << "[Cliente] Conexión cerrada.\n";
}

void Cliente::cerrar() {
    if (conectado && socketCliente >= 0) {
        close(socketCliente);
        conectado = false;
    }
}
