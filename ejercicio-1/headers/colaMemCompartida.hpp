#pragma once

#include <string>
#include <semaphore.h>
#include <fcntl.h> 
#include <sys/mman.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>

#define MAX_PEDIDOS 10

enum EstadoPedido {
    PENDIENTE,
    RECIBIDO,
    COCINADO,
    ARMADO,
    EMPAQUETADO,
    ENTREGADO
};

enum TipoCombo {
    SIMPLE = 0,
    DOBLE = 1,
    COMPLETO = 2
};

struct s_Combo {
    TipoCombo tipo;
    int tiempoPreparacion;
};

struct s_Pedido {
    int id;
    s_Combo combo;
    EstadoPedido estado;
};

struct ColaPedidos {
    int pri;
    int ult;
    int cantidad;
    s_Pedido pedidos[MAX_PEDIDOS];

    // Semáforos embedded
    sem_t mutex;
    sem_t espacio;
    sem_t items;
};

using namespace std;

class ColaMemCompartida {
private:
    std::string shm_name;
    std::string sem_mutex_name;
    std::string sem_espacio_name;
    std::string sem_items_name;

    int shm_fd;
    ColaPedidos* cola;

    sem_t* mutex;
    sem_t* espacio;
    sem_t* items;

public:
    ColaMemCompartida(std::string id_prefix);
    ColaMemCompartida(std::string id_prefix, bool inicializar);
    ~ColaMemCompartida();

    bool Push(const s_Pedido& pedido);
    bool Pop(s_Pedido& pedido);
    int CantidadEnCola();

    void Log(const s_Pedido& pedido);
    std::string buildLogString(const s_Pedido& pedido);
};
