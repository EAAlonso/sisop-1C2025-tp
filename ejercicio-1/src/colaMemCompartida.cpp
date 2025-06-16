#include <fstream>
#include <ctime>
#include <sstream>
#include <filesystem>
#include <sys/stat.h>
#include <iomanip>
#include <cerrno>

#include "../headers/menu.hpp"
#include "../headers/colaMemCompartida.hpp"

#include <fcntl.h>

ColaMemCompartida::ColaMemCompartida(string nombre, string logFileName)
{
    this->logFileName = this->logDir + logFileName;
    this->semName = nombre;
    this->colName = nombre;

    init(nombre);
}

int ColaMemCompartida::init(string nombre)
{
    

    sem_shm = shm_open(GetShmSemName().c_str(), O_CREAT | O_RDWR, 0666);
    if (sem_shm == -1) {
        perror("shm_open");
        exit(1);
    }

    void *sem_mem = mmap(
        nullptr,
        sizeof(sem_t) * 4,
        PROT_READ | PROT_WRITE,
        MAP_SHARED | MAP_ANONYMOUS,
        sem_shm,
        0);
    if (sem_mem == MAP_FAILED)
    {
        perror("mmap semaphores");
        return -1;
    }
    mutex = (sem_t *)sem_mem;
    espacio = ((sem_t *)sem_mem) + 1;
    items = ((sem_t *)sem_mem) + 2;
    logMutex = ((sem_t *)sem_mem) + 3;

    sem_init(mutex, 1, 1);             // Acceso cola
    sem_init(espacio, 1, MAX_PEDIDOS); // Espacio disponible para productor
    sem_init(items, 1, 0);             // Pedidos disponibles para consumidor
    sem_init(logMutex, 1, 1);          // Escritura en log

    col_shm = shm_open(GetShmColName().c_str(), O_CREAT | O_RDWR, 0666);
    if (col_shm == -1) {
        perror("shm_open_col");
        exit(1);
    }

    // Memoria para cola compartida
    cola = (ColaPedidos *)mmap(
        nullptr,
        sizeof(ColaPedidos),
        PROT_READ | PROT_WRITE,
        MAP_SHARED | MAP_ANONYMOUS,
        col_shm,
        0);
    if (cola == MAP_FAILED)
    {
        perror("mmap cola");
        return -1;
    }
    // Indices cola
    cola->pri = 0;
    cola->ult = 0;
    cola->cantidad = 0;

    cout << "Cola de pedidos '" << nombre << "' inicializada." << endl;

    InitLog(nombre);

    return 0;
}

void ColaMemCompartida::InitLog(string nombre)
{
    // Crear directorio si no existe
    if (!this->logDir.empty())
    {
        mkdir(this->logDir.c_str(), 0777);
    }

    // Truncate antes de usar
    ofstream truncateFile(this->logFileName, ios::trunc);
    truncateFile.close();

    // Titulo
    std::ofstream logFile(this->logFileName, ios::app);
    if (logFile.is_open())
    {
        logFile << "Cola de pedidos '" << nombre << "' iniciada." << endl;
        logFile.close();
    }
    else
    {
        cerr << "Error al abrir el archivo de log: " << this->logFileName << endl;
    }
}

bool ColaMemCompartida::Push(const s_Pedido &order)
{
     if (sem_trywait(espacio) == -1) {
        if (errno == EAGAIN) {
            // Queue is full
            return false;
        } else {
            perror("sem_trywait(espacio)");
            return false;
        }
    }
    sem_wait(mutex);
    int next = (cola->ult + 1) % MAX_PEDIDOS;
    cola->pedidos[cola->ult] = order;
    cola->ult = next;
    cola->cantidad++;
    sem_post(mutex);
    sem_post(items);
    return true;
}

// Pop: modo bloqueante o no bloqueante
bool ColaMemCompartida::Pop(s_Pedido &out, bool nonblocking)
{
    int sem_result;
    if (nonblocking)
    {
        sem_result = sem_trywait(items);
        if (sem_result == -1)
        {
            if (errno == EAGAIN) { return false; }
            else
            {
                perror("sem_trywait(items)");
                return false;
            }
        }
    }
    else
    {
        sem_wait(items);
    }

    sem_wait(mutex);
    if (cola->pri == cola->ult && cola->cantidad == 0)
    {
        sem_post(mutex);
        if (!nonblocking)
            sem_post(items); // Solo reponer si era bloqueante
        return false;
    }
    out = cola->pedidos[cola->pri];
    cola->pri = (cola->pri + 1) % MAX_PEDIDOS;
    cola->cantidad--;
    sem_post(mutex);
    sem_post(espacio);
    return true;
}

void ColaMemCompartida::Log(const s_Pedido &pedido)
{
    sem_wait(logMutex);
    ofstream logFile(this->logFileName, ios::app);
    if (logFile.is_open())
    {
        logFile << buildLogString(pedido) << endl;
        logFile.flush();
    }
    sem_post(logMutex);
}

string ColaMemCompartida::buildLogString(const s_Pedido &pedido)
{
    // Timestamp
    time_t now = time(nullptr);
    char buf[32];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));

    // Parse enum a string
    string estadoStr;
    switch (pedido.estado)
    {
    case PENDIENTE:
        estadoStr = "PENDIENTE";
        break;
    case RECIBIDO:
        estadoStr = "RECIBIDO";
        break;
    case COCINADO:
        estadoStr = "COCINADO";
        break;
    case ARMADO:
        estadoStr = "ARMADO";
        break;
    case EMPAQUETADO:
        estadoStr = "EMPAQUETADO";
        break;
    case ENTREGADO:
        estadoStr = "ENTREGADO";
        break;
    default:
        estadoStr = "DESCONOCIDO";
        break;
    }

    ostringstream oss;
    oss << "[" << buf << "] "
        << "Cocinero: [" << setw(5) << left << getpid() << "] "
        << "ID: " << setw(2) << left << pedido.id << " "
        << setw(2) << left << Menu::TipoComboToString(pedido.combo.tipo) << " "
        << "[" << left << estadoStr << "]";
    return oss.str();
}

ColaMemCompartida::~ColaMemCompartida()
{
    if (mutex)
        sem_destroy(mutex);
    if (espacio)
        sem_destroy(espacio);
    if (items)
        sem_destroy(items);
    if (logMutex)
        sem_destroy(logMutex);
    if (cola)
        munmap(cola, sizeof(ColaPedidos) * MAX_PEDIDOS);
    
    if (sem_shm != -1) {
        close(sem_shm);
        shm_unlink(GetShmSemName().c_str());
    }

    if (col_shm != -1) {
        close(col_shm);
        shm_unlink(GetShmColName().c_str());
    }
}