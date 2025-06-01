#include "../headers/cocinero.hpp"

Cocinero::Cocinero(int id, Cocina& cocina) : id(id), cocina(cocina), activo(true) {}

Cocinero::~Cocinero() {
    unirse();
}

void Cocinero::iniciar() {
    hilo = thread(&Cocinero::ejecutar, this);
}

void Cocinero::unirse() {
    activo = false;
    if (hilo.joinable()) {
        hilo.join();
    }
}

void Cocinero::ejecutar() {
    while (cocina.servidorActivo) {
        Pedido pedido;

        {
            unique_lock<mutex> lock(cocina.mutexColaPedidos);
            cocina.cvPedidos.wait(lock, [&]() {
                return !cocina.colaPedidos.empty() || !cocina.servidorActivo;
            });

            if (!cocina.servidorActivo && cocina.colaPedidos.empty())
                return;

            pedido = cocina.colaPedidos.front();
            cocina.colaPedidos.pop();
        }

        auto logPaso = [&](const string& paso) {
            cout << "[Cocinero " << id << "] " << paso << endl;
        };

        logPaso("Tomando pedido...");
        sleep(2);

        logPaso("Cocinando...");
        sleep(2);

        if (pedido.combo == "S") {
            logPaso("Armando combo: 1 carne, 1 queso, 2 panes");
            sleep(3);
        } else if (pedido.combo == "D") {
            logPaso("Armando combo: 2 carnes, 2 quesos, 3 panes");
            sleep(5);
        } else {
            logPaso("Armando combo: 2 carnes, 2 quesos, lechuga, tomate");
            sleep(7);
        }

        logPaso("Empaquetando...");
        sleep(3);

        logPaso("Entregado.");
        sleep(2);

        close(pedido.clienteSocket);
    }
}
