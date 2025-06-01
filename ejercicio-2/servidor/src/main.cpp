#include "../headers/cocina.hpp"
#include <csignal>
#include <iostream>

Cocina* cocinaPtr = nullptr;

void signalHandler(int signum) {
    std::cout << "\n[Main] Señal recibida, apagando cocina...\n";
    if (cocinaPtr) {
        cocinaPtr->apagar();
    }
    exit(signum);
}

int main() {
    signal(SIGPIPE, SIG_IGN);  // 👈 ACÁ

    int puerto = 8080;
    cocinaPtr = new Cocina(puerto);

    signal(SIGINT, signalHandler);  // Para CTRL+C

    cocinaPtr->iniciar();

    std::cout << "[Main] Presione CTRL+C para apagar el servidor...\n";
    pause();

    delete cocinaPtr;
    return 0;
}
