#include "../headers/cocina.hpp"
#include "../headers/managerPedidos.hpp"
#include <fstream>
#include <ctime>
#include <sstream>
#include <csignal>

volatile sig_atomic_t c_terminar = 0;

void handler_sigterm_child(int)
{
    c_terminar = 1;
}

void Cocina::LlamarCocineros()
{
    auto recibo = [this]() { recibirPedidos(); };
    auto cocinero = [this]() { cocinar(); };
    auto armado = [this]() { armarPedidos(); };
    auto empaque = [this]() { empaquetarPedidos(); };
    auto entrega = [this]() { entregarPedidos(); };

    // Vector de punteros a funciones
    std::vector<MapHijosFunc> _hijos;
    for (int i = 0; i < CANTIDAD_HIJOS_RECIBO; ++i)
    {
        _hijos.push_back({"recibo", recibo});
    }

    for (int i = 0; i < CANTIDAD_HIJOS_COCINERO; ++i)
    {
        _hijos.push_back({"cocinero", cocinero});
    }

    for (int i = 0; i < CANTIDAD_HIJOS_ARMADO; ++i)
    {
        _hijos.push_back({"armado", armado});
    }

    for (int i = 0; i < CANTIDAD_HIJOS_EMPAQUE; ++i)
    {
        _hijos.push_back({"empaque", empaque});
    }

    for (int i = 0; i < CANTIDAD_HIJOS_ENTREGA; ++i)
    {
        _hijos.push_back({"entrega", entrega});
    }

    for (const auto &hijo : _hijos)
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            hijo.func();
            exit(EXIT_SUCCESS);
        }
        else if (pid > 0)
        {
            hijosData.push_back({hijo.tipo, pid});
            cout << "Cocinero " << hijo.tipo << " con PID " << pid << " creado.\n";
        }
        else
        {
            cerr << "Error al crear el proceso del cocinero." << endl;
            exit(EXIT_FAILURE);
        }
    }

    cout << "Todos los cocineros están activos.\n";
}

void Cocina::recibirPedidos()
{
    signal(SIGTERM, handler_sigterm_child);
    while (true)
    {
        s_Pedido pedido;
        sleep(2);
        if (!ColaPendientes.Pop(pedido, true))
        {
            if (c_terminar)
            {
                cout << "Recibo: Terminado por SIGTERM." << endl;
                break;
            }

            sleep(1);
            continue;
        }
        sleep(1);
        pedido.estado = EstadoPedido::RECIBIDO;
        while (!ColaRecibidos.Push(pedido))
        {
            sleep(1);
        }
        ColaRecibidos.Log(pedido);
    }
}

void Cocina::cocinar()
{
    signal(SIGTERM, handler_sigterm_child);
    while (true)
    {
        s_Pedido pedido;
        if (!ColaRecibidos.Pop(pedido, true))
        {
            if (c_terminar)
            {
                cout << "Cocinero: Terminado por SIGTERM." << endl;
                break;
            }

            sleep(1);
            continue;
        }
        sleep(pedido.combo.tiempoPreparacion);
        pedido.estado = EstadoPedido::COCINADO;
        while (!ColaCoccion.Push(pedido))
        {
            sleep(1);
        }
        ColaCoccion.Log(pedido);
    }
}

void Cocina::armarPedidos()
{
    signal(SIGTERM, handler_sigterm_child);
    while (true)
    {
        s_Pedido pedido;
        if (!ColaCoccion.Pop(pedido,true))
        {
            if (c_terminar)
            {
                cout << "Armado: Terminado por SIGTERM." << endl;
                break;
            }
            sleep(1);
            continue;
        }
        sleep(1);
        pedido.estado = EstadoPedido::ARMADO;
        while (!ColaArmado.Push(pedido))
        {
            sleep(1);
        }
        ColaArmado.Log(pedido);
    }
}

void Cocina::empaquetarPedidos()
{
    signal(SIGTERM, handler_sigterm_child);
    while (true)
    {
        s_Pedido pedido;
        if (!ColaArmado.Pop(pedido, true))
        {
            if (c_terminar)
            {
                cout << "Empaque: Terminado por SIGTERM." << endl;
                break;
            }
            sleep(1);
            continue;
        }
        sleep(1);
        pedido.estado = EstadoPedido::EMPAQUETADO;
        while (!ColaEmpaquetado.Push(pedido))
        {
            sleep(1);
        }
        ColaEmpaquetado.Log(pedido);
    }
    
}

void Cocina::entregarPedidos()
{
    signal(SIGTERM, handler_sigterm_child);
    while (true)
    {
        s_Pedido pedido;
        if (!ColaEmpaquetado.Pop(pedido, true))
        {
            if (c_terminar)
            {
                cout << "Entrega: Terminado por SIGTERM." << endl;
                break;
            }
            sleep(1);
            continue;
        }

        sleep(1);
        pedido.estado = EstadoPedido::ENTREGADO;
        while (!ColaEntrega.Push(pedido))
        {
            sleep(1);
        }
        ColaEntrega.Log(pedido);
        ColaEntrega.Pop(pedido); // Remover de la cola de entrega
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
