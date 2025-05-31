#include "../headers/cocina.hpp"
#include <csignal>
#include <iostream>

Cocina* cocinaPtr = nullptr;

// Para manejar Ctrl+C y cerrar el servidor de forma limpia
void signalHandler(int signum) {
    std::cout << "\n[Main] Señal de interrupción recibida. Cerrando cocina..." << std::endl;
    if (cocinaPtr) {
        cocinaPtr->apagar();
    }
    exit(signum);
}

int main() {
    int puerto = 8080;
    int capacidad = 3; // Máximo de cocineros atendiendo al mismo tiempo

    Cocina cocina(puerto, capacidad);
    cocinaPtr = &cocina;

    // Captura SIGINT (Ctrl+C)
    signal(SIGINT, signalHandler);

    cocina.iniciar();  // Este bloquea y maneja todos los clientes

    return 0;
}
