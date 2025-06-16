#include "../headers/cocina.hpp"
#include <fstream>
#include <ctime>
#include <sstream>
#include <csignal>

volatile sig_atomic_t terminar = 0;

void handler_sigterm(int)
{
    terminar = 1;
}

void Cocina::LlamarCocineros() {
    // fork para cada etapa
    vector<pair<string, void (Cocina::*)()>> procesos = {
        {"Recibo", &Cocina::RecibirPedidos},
        {"Cocina", &Cocina::Cocinar},
        {"Armado", &Cocina::ArmarPedidos},
        {"Empaque", &Cocina::EmpaquetarPedidos},
        {"Entrega", &Cocina::EntregarPedidos}
    };

    for (auto& proceso : procesos) {
        pid_t pid = fork();
        if (pid == 0) {
            cout << "[Hijo] Proceso " << proceso.first << " iniciado." << endl;
            (this->*proceso.second)();  // Llama al método correspondiente
            exit(0);
        }
    }

    // El padre espera a que todos los hijos terminen (por ahora nunca terminan)
    for (size_t i = 0; i < procesos.size(); i++) {
        wait(nullptr);
    }
}

void Cocina::RecibirPedidos() {
    ColaMemCompartida pendientes("Pendientes");
    ColaMemCompartida recibidos("Recibidos");

    while (true) {
        s_Pedido pedido;
        pendientes.Pop(pedido);
        sleep(1);
        pedido.estado = RECIBIDO;
        recibidos.Push(pedido);
        recibidos.Log(pedido);
    }
}

void Cocina::Cocinar() {
    ColaMemCompartida recibidos("Recibidos");
    ColaMemCompartida coccion("Coccion");

    while (true) {
        s_Pedido pedido;
        recibidos.Pop(pedido);
        sleep(pedido.combo.tiempoPreparacion);
        pedido.estado = COCINADO;
        coccion.Push(pedido);
        coccion.Log(pedido);
    }
}

void Cocina::ArmarPedidos() {
    ColaMemCompartida coccion("Coccion");
    ColaMemCompartida armado("Armado");

    while (true) {
        s_Pedido pedido;
        coccion.Pop(pedido);
        sleep(1);
        pedido.estado = ARMADO;
        armado.Push(pedido);
        armado.Log(pedido);
    }
}

void Cocina::EmpaquetarPedidos() {
    ColaMemCompartida armado("Armado");
    ColaMemCompartida empaquetado("Empaquetado");

    while (true) {
        s_Pedido pedido;
        armado.Pop(pedido);
        sleep(1);
        pedido.estado = EMPAQUETADO;
        empaquetado.Push(pedido);
        empaquetado.Log(pedido);
    }
}

void Cocina::EntregarPedidos() {
    ColaMemCompartida empaquetado("Empaquetado");
    ColaMemCompartida entrega("Entrega");

    while (true) {
        s_Pedido pedido;
        empaquetado.Pop(pedido);
        sleep(1);
        pedido.estado = ENTREGADO;
        entrega.Push(pedido);
        entrega.Log(pedido);
    }
}

Cocina::~Cocina()
{

    cout << "Cerrando cocina...\n";

    std::vector<std::string> orden = {"recibo", "cocinero", "armado", "empaque", "entrega"};
    for (const auto& tipo : orden) {
        cout << "Terminando cocineros de tipo: " << tipo << "\n";
        for (const auto& hijo : hijosData) {
            if (hijo.tipo == tipo) {
                cout << "Enviando SIGTERM a cocinero " << hijo.tipo << " con PID " << hijo.pid << "\n";
                kill(hijo.pid, SIGTERM);
            }
        }
        for (const auto& hijo : hijosData) {
            if (hijo.tipo == tipo) {
                waitpid(hijo.pid, nullptr, 0);
                cout << "Cocinero " << hijo.tipo << " con PID " << hijo.pid << " terminado.\n";
            }
        }
    }

    cout << "Cocina cerrada correctamente.\n";
}