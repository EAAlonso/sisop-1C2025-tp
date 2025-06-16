#include "../headers/cocina.hpp"
#include <csignal>
#include <iostream>

atomic<bool> interrupcionSolicitada(false);
Cocina* cocinaPtr = nullptr;

void signalCerrarCocina(int) {
    if (cocinaPtr != nullptr) {
        cocinaPtr->cierreSolicitado = true;
        cocinaPtr->cvClientes.notify_one();
    }
}

int main() {
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, signalCerrarCocina);

    int puerto = 8080;
    cocinaPtr = new Cocina(puerto);
    cocinaPtr->abrirCocina();

    cout << "[Main - Servidor] Presione CTRL+C para cerrar la cocina...\n";

    cocinaPtr->esperarCierre();

    cocinaPtr->cerrarCocina();  // cerramos desde el main

    delete cocinaPtr;
    return 0;
}
