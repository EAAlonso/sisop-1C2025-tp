#include "../headers/simulador_clientes.hpp"
#include "../headers/json.hpp"
#include <fstream>
#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>

using nlohmann::json;

void simularClientesDesdeArchivo(const std::string& archivo, int puerto, const std::string host) {
    std::ifstream file(archivo);
    if (!file.is_open()) {
        std::cerr << "No se pudo abrir el archivo: " << archivo << std::endl;
        return;
    }

    json batch;
    batch["tipo"] = "batch";
    batch["pedidos"] = json::array();

    std::string linea;
    while (std::getline(file, linea)) {
        if (linea.empty()) continue;
        batch["pedidos"].push_back({ {"combo", linea} });
    }

    std::string mensaje = batch.dump() + "\n";
    file.close();

    // Enviar batch al servidor
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "[-] Error al crear socket para batch" << std::endl;
        return;
    }

    sockaddr_in servidor{};
    servidor.sin_family = AF_INET;
    servidor.sin_port = htons(puerto);
    if (inet_pton(AF_INET, host.c_str(), &servidor.sin_addr) <= 0) {
        std::cerr << "[-] Dirección IP inválida: " << host << std::endl;
        close(sock);
        return;
    }

    if (connect(sock, (struct sockaddr*)&servidor, sizeof(servidor)) < 0) {
        std::cerr << "[-] No se pudo conectar al servidor" << std::endl;
        close(sock);
        return;
    }

    send(sock, mensaje.c_str(), mensaje.size(), 0);

    // Leer respuesta del servidor
    char buffer[1024] = {0};
    ssize_t bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (bytes > 0) {
        buffer[bytes] = '\0';
        std::cout << "Respuesta del servidor: " << buffer << std::endl;
    }

    std::cout << "[✓] Pedidos enviados: " << batch["pedidos"].size() << std::endl;
    close(sock);
}