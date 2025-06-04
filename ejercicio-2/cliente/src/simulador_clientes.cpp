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

    // Armar y enviar pedido
    json pedido;
    pedido["combo"] = combo;

    string pedidoStr = pedido.dump();
    send(sock, pedidoStr.c_str(), pedidoStr.size(), 0);
    cout << "[Cliente] Pedido enviado: " << pedidoStr << endl;
    cout << "[Cliente] Esperando que el pedido esté listo..." << endl;

    // Esperar respuesta con select y timeout
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(sock, &readfds);

    struct timeval timeout;
    timeout.tv_sec = 10;  // Esperar hasta 10 segundos
    timeout.tv_usec = 0;

    int activity = select(sock + 1, &readfds, nullptr, nullptr, &timeout);

    if (activity == 0) {
        cout << "[Cliente] Timeout esperando respuesta del servidor." << endl;
        close(sock);
        return;
    } else if (activity < 0) {
        cerr << "[Cliente] Error en select()." << endl;
        close(sock);
        return;
    }

    // Si hay datos, recibirlos
    char buffer[1024];
    ssize_t bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);

    if (bytes == 0) {
        cout << "[Cliente] El servidor cerró la conexión." << endl;
    } else if (bytes < 0) {
        cerr << "[Cliente] Error al recibir datos del servidor." << endl;
    } else {
        buffer[bytes] = '\0';
        cout << "[Cliente] Respuesta del servidor: " << buffer << endl;
    }

    close(sock);
}



void simularClientesDesdeArchivo(const string& rutaArchivo, int puerto, const string& host) {
    ifstream archivo(rutaArchivo);
    if (!archivo.is_open()) {
        cerr << "[Cliente] Error al abrir archivo: " << rutaArchivo << endl;
        return;
    }

    string linea;
    while (getline(archivo, linea)) {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            cerr << "[Cliente] Error al crear socket\n";
            continue;
        }

        sockaddr_in servidor{};
        servidor.sin_family = AF_INET;
        servidor.sin_port = htons(puerto);
        if (inet_pton(AF_INET, host.c_str(), &servidor.sin_addr) <= 0) {
            cerr << "[Cliente] Dirección IP inválida: " << host << endl;
            close(sock);
            continue;
        }

        if (connect(sock, (sockaddr*)&servidor, sizeof(servidor)) < 0) {
            cerr << "[Cliente] Error al conectar con el servidor\n";
            close(sock);
            continue;
        }

        json pedidoJson;
        pedidoJson["combo"] = linea;

        string pedidoStr = pedidoJson.dump();
        send(sock, pedidoStr.c_str(), pedidoStr.size(), 0);

        cout << "[Cliente] Pedido enviado: " << pedidoStr << endl;
        cout << "[Cliente] Esperando que el pedido esté listo...\n";

        // Bucle de recepción de mensajes hasta que el servidor cierre la conexión
        char buffer[1024];
        while (true) {
            memset(buffer, 0, sizeof(buffer));
            ssize_t bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
            if (bytes <= 0) {
                cout << "[Cliente] Pedido finalizado. Conexión cerrada por el servidor.\n";
                break;
            }

            cout << "[Cliente] " << buffer;
            this_thread::sleep_for(chrono::milliseconds(300));
        }

        close(sock);
    }
}