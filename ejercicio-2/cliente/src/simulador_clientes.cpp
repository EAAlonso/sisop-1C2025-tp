#include "../headers/simulador_clientes.hpp"

using nlohmann::json;

void clienteEscuchador(const string& combo, int puerto, const string& host) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        cerr << "[Cliente] Error al crear socket" << endl;
        return;
    }

    sockaddr_in servidor{};
    servidor.sin_family = AF_INET;
    servidor.sin_port = htons(puerto);
    if (inet_pton(AF_INET, host.c_str(), &servidor.sin_addr) <= 0) {
        cerr << "[Cliente] Dirección IP inválida: " << host << endl;
        close(sock);
        return;
    }

    if (connect(sock, (struct sockaddr*)&servidor, sizeof(servidor)) < 0) {
        cerr << "[Cliente] No se pudo conectar al servidor" << endl;
        close(sock);
        return;
    }

    // Armar y enviar JSON con el combo
    json pedidoJson;
    pedidoJson["combo"] = combo;
    string mensaje = pedidoJson.dump();

    cout << "[Cliente] Enviando pedido: " << combo << endl;
    send(sock, mensaje.c_str(), mensaje.size(), 0);

    // Escuchar hasta que el servidor cierre la conexión
    char buffer[1024];
    while (true) {
        ssize_t bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0) {
            cout << "[Cliente] Conexión cerrada por el servidor.\n";
            break;
        }
        buffer[bytes] = '\0';
        cout << "[Cliente] " << buffer;
    }

    close(sock);
}



void simularClientesDesdeArchivo(const string& archivo, int puerto, const string& host)
{
    ifstream file(archivo);
    if (!file.is_open()) {
        cerr << "No se pudo abrir el archivo: " << archivo << endl;
        return;
    }

    vector<thread> threads;
    string combo;

    while (getline(file, combo)) {
        if (combo.empty()) continue;
        threads.emplace_back(clienteEscuchador, combo, puerto, host);
        this_thread::sleep_for(chrono::milliseconds(200));  // le da respiro al server
    }

    for (auto& t : threads) {
        if (t.joinable()) t.join();
    }

    cout << "[✓] Todos los pedidos fueron enviados y escuchados.\n";
}