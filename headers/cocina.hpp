#pragma once

#include <vector>
#include <cstring>
#include <iostream>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <csignal>
#include <sys/wait.h>

#include "colaPedidos.hpp"
#include "pedido.hpp"

using namespace std;

class Cocina
{
private:
    int shmid;                // ID de memoria compartida
    ColaPedidos *colaPedidos; // Puntero a la memoria compartida (vector de pedidos)
    int semid;                // ID de semáforos
    bool colaVacia();
    vector<pid_t> cocineros; // Guarda los PIDs de los hijos

    void inicializar();
    Pedido desencolarPedido();
    void atenderPedidos();
public:
    Cocina();
    ~Cocina();

    void EncolarPedido(Pedido pedido);
    void LlamarCocineros(int cantidadCocineros); // crear procesos hijos
};