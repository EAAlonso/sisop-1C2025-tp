#include "../headers/cocina.hpp"
#include <csignal>
#include <iostream>

atomic<bool> interrupcionSolicitada(false);
Cocina* cocinaPtr = nullptr;

void signalCerrarCocina(int) {
    cout << "\n[Main - Servidor] Señal SIGINT recibida. Cerrando cocina pronto...\n";
    interrupcionSolicitada = true;
    exit(0);
}

int main() {
    signal(SIGPIPE, SIG_IGN); // previene que el proceso se termine si intenta escribir en un socket que ya fue cerrado por el cliente
    signal(SIGINT, signalCerrarCocina);

    int puerto = 8080;
    cocinaPtr = new Cocina(puerto);
    cocinaPtr->abrirCocina();

    cout << "[Main - Servidor] Presione CTRL+C para cerrar la cocina...\n";

    while (!interrupcionSolicitada) 
    {
        this_thread::sleep_for(chrono::milliseconds(200));
    }

    cocinaPtr->cerrarCocina();
    
    delete cocinaPtr;
    return 0;
}
