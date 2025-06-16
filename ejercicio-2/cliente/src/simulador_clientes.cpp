#include "../headers/simulador_clientes.hpp"

string leerComboDesdeArchivo(const string& rutaArchivo) {
    ifstream archivo(rutaArchivo);
    if (!archivo.is_open()) {
        cerr << "[Cliente] Error al abrir archivo: " << rutaArchivo << endl;
        return "";
    }

    string combo;
    getline(archivo, combo);
    return combo;
}

void enviarPedido(const string& combo, int sock) {
    json pedido;
    pedido["combo"] = combo;

    string pedidoStr = pedido.dump();
    ssize_t enviado = send(sock, pedidoStr.c_str(), pedidoStr.size(), 0);
    if (enviado < 0) {
        cerr << "[Cliente] Error al enviar el pedido.\n";
        return;
    }

    cout << "[Cliente] Pedido enviado: " << pedidoStr << endl;
    cout << "[Cliente] Esperando respuesta..." << endl;

    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    ssize_t bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);

    if (bytes <= 0) {
        cout << "[Cliente] El servidor cerró la conexión o hubo error.\n";
        return;
    }

    buffer[bytes] = '\0';
    cout << "[Cliente] Respuesta del servidor: " << buffer << endl;
}