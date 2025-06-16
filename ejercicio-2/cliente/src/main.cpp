#include "../headers/simulador_clientes.hpp"
#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

int main() {
    string host = "127.0.0.1";    
    int puerto = 8080;

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        cerr << "[Cliente] Error al crear socket\n";
        return 1;
    }

    sockaddr_in servidor{};
    servidor.sin_family = AF_INET;
    servidor.sin_port = htons(puerto);
    if (inet_pton(AF_INET, host.c_str(), &servidor.sin_addr) <= 0) {
        cerr << "[Cliente] Dirección IP inválida: " << host << endl;
        close(sock);
        return 1;
    }

    if (connect(sock, (sockaddr*)&servidor, sizeof(servidor)) < 0) {
        cerr << "[Cliente] Error al conectar con el servidor\n";
        close(sock);
        return 1;
    }

    cout << "[Cliente] Conexión establecida con el servidor." << endl;

    while (true) {
        cout << "\n--- Menú Cliente ---\n";
        cout << "1. Pedir combo simple\n";
        cout << "2. Pedir combo doble\n";
        cout << "3. Pedir combo completo\n";
        cout << "4. Salir\n";
        cout << "Seleccione una opción: ";

        int opcion;
        cin >> opcion;

        string rutaArchivo;
        switch(opcion)
        {
            case 1: rutaArchivo = "./pedidosComboS.txt"; break;
            case 2: rutaArchivo = "./pedidosComboD.txt"; break;
            case 3: rutaArchivo = "./pedidosComboC.txt"; break;
            case 4:
                cout << "Saliendo del cliente...\n";
                close(sock);
                return 0;
            default:
                cout << "Opción inválida, intente nuevamente.\n";
                continue;
        }

        string combo = leerComboDesdeArchivo(rutaArchivo);
        if (combo.empty()) {
            cerr << "[Cliente] No se pudo leer el combo.\n";
            continue;
        }

        enviarPedido(combo, sock);
    }

    close(sock);
    return 0;
}