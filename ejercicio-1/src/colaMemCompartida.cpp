#include "../headers/colaMemCompartida.hpp"

using namespace std;

ColaMemCompartida::ColaMemCompartida(std::string id_prefix)
    : ColaMemCompartida(id_prefix, false) 
{}

ColaMemCompartida::ColaMemCompartida(std::string id_prefix, bool inicializar) {
    shm_name = "/" + id_prefix + "_cola";

    shm_fd = shm_open(shm_name.c_str(), O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("Error abriendo shm");
        exit(1);
    }

    // Reservamos espacio para la estructura de la cola + semáforos
    ftruncate(shm_fd, sizeof(ColaPedidos));

    cola = (ColaPedidos*) mmap(0, sizeof(ColaPedidos), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (cola == MAP_FAILED) {
        perror("Error en mmap");
        exit(1);
    }

    if (inicializar) {
        cola->pri = 0;
        cola->ult = 0;
        cola->cantidad = 0;

        // Inicializamos los semáforos en memoria compartida (unnamed)
        if (sem_init(&(cola->mutex), 1, 1) == -1) {
            perror("Error inicializando semáforo mutex");
            exit(1);
        }
        if (sem_init(&(cola->espacio), 1, MAX_PEDIDOS) == -1) {
            perror("Error inicializando semáforo espacio");
            exit(1);
        }
        if (sem_init(&(cola->items), 1, 0) == -1) {
            perror("Error inicializando semáforo items");
            exit(1);
        }
    }

    // Asignamos los punteros directamente desde la memoria compartida
    mutex = &(cola->mutex);
    espacio = &(cola->espacio);
    items = &(cola->items);
}

ColaMemCompartida::~ColaMemCompartida() {
    munmap(cola, sizeof(ColaPedidos));
    close(shm_fd);
    sem_close(mutex);
    sem_close(espacio);
    sem_close(items);
}

bool ColaMemCompartida::Push(const s_Pedido& pedido) {
    sem_wait(espacio);
    sem_wait(mutex);

    cola->pedidos[cola->ult] = pedido;
    cola->ult = (cola->ult + 1) % MAX_PEDIDOS;
    cola->cantidad++;

    sem_post(mutex);
    sem_post(items);
    return true;
}

bool ColaMemCompartida::Pop(s_Pedido& pedido) {
    sem_wait(items);
    sem_wait(mutex);

    pedido = cola->pedidos[cola->pri];
    cola->pri = (cola->pri + 1) % MAX_PEDIDOS;
    cola->cantidad--;

    sem_post(mutex);
    sem_post(espacio);
    return true;
}

int ColaMemCompartida::CantidadEnCola() {
    sem_wait(mutex);
    int cantidad_actual = cola->cantidad;
    sem_post(mutex);
    return cantidad_actual;
}

void ColaMemCompartida::Log(const s_Pedido &pedido)
{
    ofstream logFile("logs.txt", ios::app);
    if (logFile.is_open())
    {
        logFile << buildLogString(pedido) << endl;
        logFile.flush();
    }
}

string ColaMemCompartida::buildLogString(const s_Pedido &pedido)
{
    time_t now = time(nullptr);
    char buf[32];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));

    string estadoStr;
    switch (pedido.estado)
    {
    case PENDIENTE: estadoStr = "PENDIENTE"; break;
    case RECIBIDO: estadoStr = "RECIBIDO"; break;
    case COCINADO: estadoStr = "COCINADO"; break;
    case ARMADO: estadoStr = "ARMADO"; break;
    case EMPAQUETADO: estadoStr = "EMPAQUETADO"; break;
    case ENTREGADO: estadoStr = "ENTREGADO"; break;
    default: estadoStr = "DESCONOCIDO"; break;
    }

    ostringstream oss;
    oss << "[" << buf << "] "
        << "ID: " << pedido.id << " "
        << "[" << estadoStr << "]";
    return oss.str();
}
