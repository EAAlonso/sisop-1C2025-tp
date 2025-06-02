#include "../headers/colaMemCompartida.hpp"


ColaMemCompartida::ColaMemCompartida(string nombre) {
    init(nombre);
}

int ColaMemCompartida::init(string nombre) {
    void *sem_mem = mmap(
        nullptr,
        sizeof(sem_t) * 3,
        PROT_READ | PROT_WRITE,
        MAP_SHARED | MAP_ANONYMOUS,
        -1,
        0);
    if (sem_mem == MAP_FAILED) {
        perror("mmap semaphores");
        return -1;
    }
    mutex = (sem_t *)sem_mem;
    espacio = ((sem_t *)sem_mem) + 1;
    items = ((sem_t *)sem_mem) + 2;

    sem_init(mutex, 1, 1); // Acceso cola
    sem_init(espacio, 1, MAX_PEDIDOS); // Espacio disponible para productor
    sem_init(items, 1, 0); // Pedidos disponibles para consumidor

    // Allocate shared memory for the queue
    cola = (ColaPedidos *)mmap(
        nullptr,
        sizeof(ColaPedidos),
        PROT_READ | PROT_WRITE,
        MAP_SHARED | MAP_ANONYMOUS,
        -1,
        0);
    if (cola == MAP_FAILED) {
        perror("mmap cola");
        return -1;
    }
    // Initialize queue indices and count
    cola->pri = 0;
    cola->ult = 0;
    cola->cantidad = 0;

    cout << "Cola de pedidos '" << nombre << "' inicializada." << endl;
    return 0;
}

bool ColaMemCompartida::Push(const s_Pedido& order) {
    sem_wait(espacio);
    sem_wait(mutex);
    int next = (cola->ult + 1) % MAX_PEDIDOS;
    cola->pedidos[cola->ult] = order;
    cola->ult = next;
    cola->cantidad++;
    cout << "Total orders in queue: " << cola->cantidad << endl;
    sem_post(mutex);
    sem_post(items);
    return true;
}

bool ColaMemCompartida::Pop(s_Pedido& out) {
    sem_wait(items);
    sem_wait(mutex);
    if (cola->pri == cola->ult && cola->cantidad == 0) {
        sem_post(mutex);
        return false;
    }
    out = cola->pedidos[cola->pri];
    cola->pri = (cola->pri + 1) % MAX_PEDIDOS;
    cola->cantidad--;
    // Parse enum value to string for estado
    std::string estadoStr;
    switch (out.estado) {
        case PENDIENTE: estadoStr = "PENDIENTE"; break;
        case RECIBIDO: estadoStr = "RECIBIDO"; break;
        case COCINADO: estadoStr = "COCINADO"; break;
        case ARMADO: estadoStr = "ARMADO"; break;
        case EMPAQUETADO: estadoStr = "EMPAQUETADO"; break;
        case ENTREGADO: estadoStr = "ENTREGADO"; break;
        default: estadoStr = "DESCONOCIDO"; break;
    }
    cout << estadoStr << " - Order ID: " << out.id << " removed from queue. Total orders: " << cola->cantidad << endl;
    sem_post(mutex);
    sem_post(espacio);
    return true;
}

ColaMemCompartida::~ColaMemCompartida() {
    if (mutex) sem_destroy(mutex);
    if (espacio) sem_destroy(espacio);
    if (items) sem_destroy(items);
    if (cola) munmap(cola, sizeof(ColaPedidos) * MAX_PEDIDOS);
}