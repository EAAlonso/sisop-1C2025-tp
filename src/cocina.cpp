#include "../headers/cocina.hpp"
#include <functional>

Cocina::Cocina(ManagerPedidos *managerPedidos){
    cout << "Iniciando cocina..." << endl;
    this->managerPedidos = managerPedidos; // Asignar la instancia de ManagerPedidos
}

void Cocina::LlamarCocineros() {
    const int _cantidadCocineros = 5;
    cout << "Llamando a " << _cantidadCocineros << " cocineros..." << endl;

    // Definir 5 funciones diferentes para los cocineros
    auto funcionCocinero0 = [this]() { RecibirPedidos(); };
    auto funcionCocinero1 = [this]() { Cocinar(); };
    auto funcionCocinero2 = [this]() { ArmarPedidos(); };
    auto funcionCocinero3 = [this]() { EmpaquetarPedidos(); };
    auto funcionCocinero4 = [this]() { EntregarPedidos(); };
    

    // Vector de punteros a funciones
    std::vector<std::function<void()>> funciones = {
        funcionCocinero0, funcionCocinero1, funcionCocinero2, funcionCocinero3, funcionCocinero4
    };

    for (int i = 0; i < _cantidadCocineros; ++i) {
        pid_t pid = fork();

        if (pid == 0) {
            funciones[i]();
            exit(EXIT_SUCCESS);
        } else if (pid > 0) {
            cocineros.push_back(pid);
            cout << "Cocinero con PID " << pid << " (función " << i << ") creado.\n";
        } else {
            cerr << "Error al crear el proceso del cocinero." << endl;
            exit(EXIT_FAILURE);
        }
    }

    cout << "Todos los cocineros están activos.\n";
}

void Cocina::RecibirPedidos() {
    while(true)
    {
        cout << getpid() << " - Recibiendo pedidos..."  << endl;
        s_Pedido pedido;
        if (!ColaPendientes.Pop(pedido)) {
            cout << "No hay pedidos pendientes. Cocinero " << getpid() << " esperando..." << endl;
            sleep(1);
            continue;
        }
        sleep(1);
        pedido.estado = EstadoPedido::RECIBIDO;
        ColaRecibidos.Push(pedido);
        cout << "Cocinero " << getpid() << " recibió el pedido ID: " << pedido.id << endl;
    }
}

void Cocina::Cocinar() {
    while (true) {
        s_Pedido pedido;
        if (!ColaRecibidos.Pop(pedido)) {
            cout << "No hay pedidos recibidos. Cocinero " << getpid() << " esperando..." << endl;
            sleep(1);
            continue;
        }
        cout << getpid() << " - Cocinando pedido " << pedido.id << endl;
        sleep(pedido.combo.tiempoPreparacion);
        pedido.estado = EstadoPedido::COCINADO;
        ColaCoccion.Push(pedido);
        cout << "Cocinero " << getpid() << " cocinó el pedido ID: " << pedido.id << endl;
    }
}

void Cocina::ArmarPedidos() {
    while (true)
    {
        s_Pedido pedido;
        if (!ColaCoccion.Pop(pedido)) {
            cout << "No hay pedidos cocinados. Cocinero " << getpid() << " esperando..." << endl;
            sleep(1);
            continue;
        }
        cout << getpid() << " - Armando pedido..." << pedido.id << endl;
        sleep(1);
        pedido.estado = EstadoPedido::ARMADO;
        ColaArmado.Push(pedido);
        cout << "Cocinero " << getpid() << " armó el pedido ID: " << pedido.id << endl;
    }
}

void Cocina::EmpaquetarPedidos() {
    while (true)
    {
        s_Pedido pedido;
        if (!ColaArmado.Pop(pedido)) {
            cout << "No hay pedidos armados. Cocinero " << getpid() << " esperando..." << endl;
            sleep(1);
            continue;
        }
        cout << getpid() << " - Empaquetando pedido..." << pedido.id << endl;
        sleep(1);
        pedido.estado = EstadoPedido::EMPAQUETADO;
        ColaEmpaquetado.Push(pedido);
        cout << "Cocinero " << getpid() << " empaquetó el pedido ID: " << pedido.id << endl;
    }
}

void Cocina::EntregarPedidos() {
    while(true)
    {
        s_Pedido pedido;
        if (!ColaEmpaquetado.Pop(pedido)) {
            cout << "No hay pedidos empaquetados. Cocinero " << getpid() << " esperando..." << endl;
            sleep(1);
            continue;
        }
        cout << getpid() << " - Entregando pedido..." << pedido.id << endl;
        sleep(1);
        pedido.estado = EstadoPedido::ENTREGADO;
        ColaEntrega.Push(pedido);
        cout << "Cocinero " << getpid() << " entregó el pedido ID: " << pedido.id << endl;
    }
}

Cocina::~Cocina() {

    cout << "Cerrando cocina...\n";
    
    // Matar procesos hijos
    /*for (pid_t pid : cocineros) {
        kill(pid, SIGTERM);  // Podés usar SIGKILL si no responden
        waitpid(pid, nullptr, 0); // Esperamos que terminen
        cout << "Cocinero con PID " << pid << " terminado.\n";
    }*/

    cout << "Cocina cerrada correctamente.\n";
}