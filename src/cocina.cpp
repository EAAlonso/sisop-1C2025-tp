#include "../headers/cocina.hpp"
#include <functional>
#include <fstream>
#include <ctime>
#include <sstream>

void Cocina::LlamarCocineros() {
    const int _cantidadCocineros = 5;
    cout << "Llamando a " << _cantidadCocineros << " cocineros..." << endl;

    // Definir 5 funciones diferentes para los cocineros
    auto recibo = [this]() { RecibirPedidos(); };
    auto cocinero = [this]() { Cocinar(); };
    auto armado = [this]() { ArmarPedidos(); };
    auto empaque = [this]() { EmpaquetarPedidos(); };
    auto entrega = [this]() { EntregarPedidos(); };
    

    // Vector de punteros a funciones
    std::vector<std::function<void()>> funciones;
    for (int i = 0; i < 2; ++i) {
        funciones.push_back(recibo);
    }

    for (int i = 0; i < _cantidadCocineros; ++i) {
        funciones.push_back(cocinero);
    }
    
    for (int i = 0; i < 2; ++i) {
        funciones.push_back(armado);
    }

    for (int i = 0; i < 2; ++i) {
        funciones.push_back(empaque);
    }

    for (int i = 0; i < 2; ++i) {
        funciones.push_back(entrega);
    }
    
    
    for (int i = 0; i < (int)funciones.size(); ++i) {
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
    // Abrir el archivo en cada iteración para evitar problemas de concurrencia entre procesos
    while(true)
    {
        s_Pedido pedido;
        sleep(5);
        if (!ColaPendientes.Pop(pedido)) {
            sleep(1);
            continue;
        }
        sleep(1);
        pedido.estado = EstadoPedido::RECIBIDO;
        while (!ColaRecibidos.Push(pedido)) {
            sleep(1);
        }
        ColaRecibidos.Log(pedido);
    }
}

void Cocina::Cocinar() {
    while (true) {
        s_Pedido pedido;
        if (!ColaRecibidos.Pop(pedido)) {
            sleep(1);
            continue;
        }
        sleep(pedido.combo.tiempoPreparacion);
        pedido.estado = EstadoPedido::COCINADO;
        while (!ColaCoccion.Push(pedido)) {
            sleep(1);
        }
        ColaCoccion.Log(pedido);
    }
}

void Cocina::ArmarPedidos() {
    while (true)
    {
        s_Pedido pedido;
        if (!ColaCoccion.Pop(pedido)) {
            sleep(1);
            continue;
        }
        sleep(1);
        pedido.estado = EstadoPedido::ARMADO;
        while (!ColaArmado.Push(pedido)) {
            sleep(1);
        }
        ColaArmado.Log(pedido);
    }
}

void Cocina::EmpaquetarPedidos() {
    while (true)
    {
        s_Pedido pedido;
        if (!ColaArmado.Pop(pedido)) {
            sleep(1);
            continue;
        }
        sleep(1);
        pedido.estado = EstadoPedido::EMPAQUETADO;
        while (!ColaEmpaquetado.Push(pedido)) {
            sleep(1);
        }
        ColaEmpaquetado.Log(pedido);
    }
}

void Cocina::EntregarPedidos() {
    while(true)
    {
        s_Pedido pedido;
        if (!ColaEmpaquetado.Pop(pedido)) {
            sleep(1);
            continue;
        }

        sleep(1);
        pedido.estado = EstadoPedido::ENTREGADO;
        while (!ColaEntrega.Push(pedido)) {
            sleep(1);
        }
        ColaEntrega.Log(pedido);
        ColaEntrega.Pop(pedido); // Remover de la cola de entrega
    }
}



Cocina::~Cocina() {

    cout << "Cerrando cocina...\n";
    
    // Matar procesos hijos
    for (pid_t pid : cocineros) {
        kill(pid, SIGTERM);  // Podés usar SIGKILL si no responden
        waitpid(pid, nullptr, 0); // Esperamos que terminen
        cout << "Cocinero con PID " << pid << " terminado.\n";
    }

    cout << "Cocina cerrada correctamente.\n";
}