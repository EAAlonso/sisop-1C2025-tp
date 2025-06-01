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

        logPedido("Tomando pedido...");
        sleep(2);

        logPedido("Cocinando...");
        sleep(2);

        if (pedido.combo == "S") {
            logPedido("Armando combo: 1 carne, 1 queso, 2 panes");
            sleep(3);
        } else if (pedido.combo == "D") {
            logPedido("Armando combo: 2 carnes, 2 quesos, 3 panes");
            sleep(5);
        } else {
            logPedido("Armando combo: 2 carnes, 2 quesos, lechuga, tomate");
            sleep(7);
        }

        logPedido("Empaquetando...");
        sleep(3);

        logPedido("Entregado.");
        sleep(2);

        close(pedido.clienteSocket);
    }
}

void Cocinero::logPedido(string mensaje) const
{
    auto now = system_clock::now();

    time_t now_time = system_clock::to_time_t(now);
    tm* tm_ptr = localtime(&now_time);

    cout << "[" << put_time(tm_ptr, "%H:%M:%S") << "] "
         << "[Cocinero " << this->id << "] " << mensaje << endl;

}
