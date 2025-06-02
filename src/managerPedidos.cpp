#include "../headers/cocina.hpp"
#include "../headers/managerPedidos.hpp"
#include "../headers/menu.hpp"

#include <iostream>
#include <string>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>

// Instancias globales para las 5 colas de estados
ColaMemCompartida ColaPendientes = ColaMemCompartida("Pendientes");
ColaMemCompartida ColaRecibidos = ColaMemCompartida("Recibidos");
ColaMemCompartida ColaCoccion = ColaMemCompartida("Coccion");
ColaMemCompartida ColaArmado = ColaMemCompartida("Armado");
ColaMemCompartida ColaEmpaquetado = ColaMemCompartida("Empaquetado");
ColaMemCompartida ColaEntrega = ColaMemCompartida("Entrega");

/*ManagerPedidos::ManagerPedidos()
{
    ColaPendientes = ColaMemCompartida("Pendientes");
    ColaRecibidos = ColaMemCompartida("Recibidos");
    ColaCoccion = ColaMemCompartida("Coccion");
    ColaArmado = ColaMemCompartida("Armado");
    ColaEmpaquetado = ColaMemCompartida("Empaquetado");
    ColaEntrega = ColaMemCompartida("Entrega");

    std::cout << "Colas de pedidos inicializadas." << std::endl;
}*/

void ManagerPedidos::CrearPedido(int comboId)
{
    if (comboId < 1 || comboId > 3)
    {
        cout << "ID de combo no válido" << endl;
        return;
    }
    s_Pedido pedido; // Estructura para el pedido

    pedido.id = countId++;    // TODO: Chequear generacion ID
    pedido.combo = s_Combo(); // Inedido.combo.tipo = static_cast<TipoCombo>(comboId - 1);
    switch (pedido.combo.tipo)
    {
    case TipoCombo::SIMPLE:
        pedido.combo.tiempoPreparacion = 4;
        break;
    case TipoCombo::DOBLE:
        pedido.combo.tiempoPreparacion = 6;
        break;
    case TipoCombo::COMPLETO:
        pedido.combo.tiempoPreparacion = 8;
        break;
    }
    pedido.estado = EstadoPedido::PENDIENTE; // TODO: Esperar si no hay espacio
    ColaPendientes.Push(pedido);             // Encola el pedido en la cocina
    cout << "Pedido creado con ID: " << pedido.id << " y tipo de combo: " << static_cast<int>(pedido.combo.tipo) << endl;

    Menu::EsperarAccion(); // Espera una acción del usuario
}

/*
// Inicializa una cola de pedidos compartida y sus semáforos
int ManagerPedidos::InitCola(ColaMemCompartida &q)
{
    void *sem_mem = mmap(
        nullptr,
        sizeof(sem_t) * 3,
        PROT_READ | PROT_WRITE,
        MAP_SHARED | MAP_ANONYMOUS,
        -1,
        0);
    if (sem_mem == MAP_FAILED)
    {
        perror("mmap semaphores");
        return -1;
    }
    q.mutex = (sem_t *)sem_mem;
    q.espacio = ((sem_t *)sem_mem) + 1;
    q.items = ((sem_t *)sem_mem) + 2;

    sem_init(q.mutex, 1, 1);
    sem_init(q.espacio, 1, MAX_PEDIDOS);
    sem_init(q.items, 1, 0);

    q.cola = (ColaPedidos *)mmap(
        nullptr,
        sizeof(ColaPedidos) * MAX_PEDIDOS,
        PROT_READ | PROT_WRITE,
        MAP_SHARED | MAP_ANONYMOUS,
        -1,
        0);
    if (q.cola == MAP_FAILED)
    {
        perror("mmap cola");
        return -1;
    }
    return 0;
}

void ManagerPedidos::DestroyColaPedidosShared(ColaMemCompartida &q)
{
    sem_destroy(q.mutex);
    sem_destroy(q.espacio);
    sem_destroy(q.items);
    munmap(q.cola, sizeof(ColaPedidos) * MAX_PEDIDOS);
}

// Push y Pop parametrizados
bool ManagerPedidos::Push(ColaMemCompartida &q, s_Pedido order)
{
    sem_wait(q.espacio);
    sem_wait(q.mutex);
    int next = (q.cola->ult + 1) % MAX_PEDIDOS;
    q.cola->pedidos[q.cola->ult] = order;
    q.cola->ult = next;
    q.cola->cantidad++;
    sem_post(q.mutex);
    sem_post(q.items);
    return true;
}

bool ManagerPedidos::Pop(ColaMemCompartida &q, s_Pedido &out)
{
    sem_wait(q.items);
    sem_wait(q.mutex);
    if (q.cola->pri == q.cola->ult && q.cola->cantidad == 0)
    {
        sem_post(q.mutex);
        return false;
    }
    out = q.cola->pedidos[q.cola->pri];
    q.cola->pri = (q.cola->pri + 1) % MAX_PEDIDOS;
    q.cola->cantidad--;
    sem_post(q.mutex);
    sem_post(q.espacio);
    return true;
}

*/
/*
int Pedido::InitColaPedidos()
{
    // Allocate space for 3 semaphores in shared memory
    void *sem_mem = mmap(
        nullptr,
        sizeof(sem_t) * 3,
        PROT_READ | PROT_WRITE,
        MAP_SHARED | MAP_ANONYMOUS,
        -1,
        0);
    if (sem_mem == MAP_FAILED)
    {
        perror("mmap semaphores");
        return -1;
    }
    g_mutex = (sem_t *)sem_mem;
    g_espacio = ((sem_t *)sem_mem) + 1;
    g_items = ((sem_t *)sem_mem) + 2;

    sem_init(g_mutex, 1, 1);             // acceso a la cola
    sem_init(g_espacio, 1, MAX_PEDIDOS); // espacio disponible
    sem_init(g_items, 1, 0);    // pedido disponible para tomarara tomar

    g_colaPedidos = (ColaPedidos *)mmap(
        nullptr,
        sizeof(ColaPedidos) * MAX_PEDIDOS,
        PROT_READ | PROT_WRITE,
        MAP_SHARED | MAP_ANONYMOUS,
        -1,
        0);

    if (g_colaPedidos == MAP_FAILED)
    {
        perror("mmap");
        return -1;
    }

    cout << "Cola de pedidos inicializada." << endl;
    return 0;
}

void Pedido::DestroySemaforosColaPedidos()
{
    sem_destroy(g_mutex);
    sem_destroy(g_espacio);
    sem_destroy(g_items);

    munmap(g_colaPedidos, sizeof(ColaPedidos) * MAX_PEDIDOS);
}

bool Pedido::PushPedido(s_Pedido order)
{
    sem_wait(g_espacio);
    sem_wait(g_mutex);

    int next = (g_colaPedidos->ult + 1) % MAX_PEDIDOS;
    g_colaPedidos->pedidos[g_colaPedidos->ult] = order;
    g_colaPedidos->ult = next;
    g_colaPedidos->cantidad++;
    cout << "Pedido ID: " << order.id << " agregado a la cola. Total pedidos: " << g_colaPedidos->cantidad << endl;
    sem_post(g_mutex);
    sem_post(g_items);
    return true;
}

bool Pedido::PopPedido(s_Pedido &out)
{
    sem_wait(g_items);
    sem_wait(g_mutex);

    if (g_colaPedidos->pri == g_colaPedidos->ult && g_colaPedidos->cantidad == 0)
    {
        sem_post(g_mutex);
        return false;
    }

    out = g_colaPedidos->pedidos[g_colaPedidos->pri];
    g_colaPedidos->pri = (g_colaPedidos->pri + 1) % MAX_PEDIDOS;
    g_colaPedidos->cantidad--;
    cout << "Pedido ID: " << out.id << " retirado de la cola. Total pedidos: " << g_colaPedidos->cantidad << endl;

    sem_post(g_mutex);
    sem_post(g_espacio);
    return true;
}


*/