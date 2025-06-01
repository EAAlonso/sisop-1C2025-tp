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

        auto logPaso = [&](const std::string& mensaje) {
            logPedido("Pedido #" + std::to_string(pedido.numeroPedido) + ": " + mensaje);
        };

        logPaso("\033[38;5;33mTomando pedido...\033[0m");
        sleep(2);

        logPaso("\033[38;5;208mCocinando...\033[0m");
        sleep(2);

        if (pedido.combo == "S") {
            logPaso("\033[38;5;210mArmando COMBO S: \033[3m1 carne, 1 queso, 2 panes\033[0m");
            sleep(3);
        } else if (pedido.combo == "D") {
            logPaso("\033[38;5;203mArmando COMBO D: \033[3m2 carnes, 2 quesos, 3 panes\033[0m");
            sleep(5);
        } else {
            logPaso("\033[38;5;217mArmando COMBO C: \033[3m2 carnes, 2 quesos, lechuga, tomate\033[0m");
            sleep(7);
        }

        logPaso("\033[38;5;179mEmpaquetando...\033[0m");
        sleep(3);

        logPaso("\033[38;5;82mEntregado.\033[0m");
        sleep(2);

        if (pedido.clienteSocket != -1) {
            close(pedido.clienteSocket);
        }
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
