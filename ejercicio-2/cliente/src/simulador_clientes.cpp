#include "../headers/simulador_clientes.hpp"

using nlohmann::json;

void simularClientesDesdeArchivo(const string& archivo, int puerto, const string host) {
    ifstream file(archivo);
    if (!file.is_open()) {
        cerr << "No se pudo abrir el archivo: " << archivo << endl;
        return;
    }

    json batch;
    batch["tipo"] = "batch";
    batch["pedidos"] = json::array();

    string linea;
    while (getline(file, linea)) {
        if (linea.empty()) continue;
        batch["pedidos"].push_back({ {"combo", linea} });
    }

    string mensaje = batch.dump() + "\n";
    file.close();

    // Enviar batch al servidor
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        cerr << "[-] Error al crear socket para batch" << endl;
        return;
    }

    sockaddr_in servidor{};
    servidor.sin_family = AF_INET;
    servidor.sin_port = htons(puerto);
    if (inet_pton(AF_INET, host.c_str(), &servidor.sin_addr) <= 0) {
        cerr << "[-] Dirección IP inválida: " << host << endl;
        close(sock);
        return;
    }

    if (connect(sock, (struct sockaddr*)&servidor, sizeof(servidor)) < 0) {
        cerr << "[-] ¡La hamburgueseria Fork & Burger está cerrada! (No se pudo conectar al servidor)" << endl;
        close(sock);
        return;
    }

    send(sock, mensaje.c_str(), mensaje.size(), 0);

    // Leer respuesta del servidor
    char buffer[1024] = {0};
    ssize_t bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (bytes > 0) {
        buffer[bytes] = '\0';
        cout << "Respuesta del servidor: " << buffer << endl;
    }

    cout << "[✓] Pedidos enviados: " << batch["pedidos"].size() << endl;
    close(sock);
}