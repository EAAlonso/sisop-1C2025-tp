#pragma once
#include <sys/mman.h>
#include <unistd.h>
#include <semaphore.h>
#include <cstdio>
#include <string>
#include <iostream>

#include "enum.hpp"
// #include "managerPedidos.hpp"
using namespace std;

#define MAX_PEDIDOS 10

struct s_Combo
{
    int tiempoPreparacion; // Tiempo de preparación en segundos
    TipoCombo tipo;
};

struct s_Pedido
{
    int id;
    s_Combo combo;
    EstadoPedido estado;
};

struct ColaPedidos
{
    s_Pedido pedidos[MAX_PEDIDOS];
    int pri;
    int ult;
    int cantidad;
};

class ColaMemCompartida
{
public:
    ColaMemCompartida() : mutex(nullptr), espacio(nullptr), items(nullptr), cola(nullptr) {}
    ColaMemCompartida(string nombre, string logFileName);
    ~ColaMemCompartida();

    bool Push(const s_Pedido &order);
    bool Pop(s_Pedido &out, bool nonblocking = false);
    void Log(const s_Pedido& pedido);

    int init(string nombre);
    void Destroy();
    
private:
    sem_t *mutex;
    sem_t *espacio;
    sem_t *items;
    sem_t *logMutex;
    ColaPedidos *cola;
    string logFileName; // Nombre del archivo de log
    const string logDir = "logs/"; // Directorio de logs

    string buildLogString(const s_Pedido& pedido);
};