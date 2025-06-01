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

    std::string linea;
    int contador = 1;
    while (std::getline(file, linea)) {
        if (linea.empty()) continue;

        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            std::cerr << "[-] Error al crear socket para pedido " << contador << std::endl;
            continue;
        }

        sockaddr_in servidor{};
        servidor.sin_family = AF_INET;
        servidor.sin_port = htons(puerto);
        inet_pton(AF_INET, host.c_str(), &servidor.sin_addr);

        if (connect(sock, (struct sockaddr*)&servidor, sizeof(servidor)) < 0) {
            std::cerr << "[-] No se pudo conectar para pedido " << contador << std::endl;
            close(sock);
            continue;
        }

        json pedido;
        pedido["tipo"] = "pedido";
        pedido["combo"] = linea;

        std::string mensaje = pedido.dump() + "\n";
        send(sock, mensaje.c_str(), mensaje.size(), 0);

        std::cout << "[✓] Pedido " << contador << " enviado: " << mensaje;

        close(sock);
        contador++;
    }

    file.close();
}