#include "../headers/cocina.hpp"
#include <csignal>
#include <iostream>

Cocina* cocinaPtr = nullptr;

void signalCerrarCocina(int signum) {
    cout << "\n[Main - Servidor] Señal recibida, cerrando cocina...\n";
    if (cocinaPtr) {
        cocinaPtr->cerrarCocina();
    }
    exit(signum);
}

int main() {
    signal(SIGPIPE, SIG_IGN); // previene que el proceso se termine si intenta escribir en un socket que ya fue cerrado por el cliente

    int puerto = 8080;
    cocinaPtr = new Cocina(puerto);

    signal(SIGINT, signalCerrarCocina);

    cocinaPtr->abrirCocina();

    cout << "[Main - Servidor] Presione CTRL+C para cerrar la cocina...\n";
    pause();

    delete cocinaPtr;
    return 0;
}
