#ifndef COCINA_H
#define COCINA_H

#include <vector>
#include "ColaPedidos.h"
#include "../headers/Pedido.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <cstring>
#include <iostream>
#include <unistd.h>

using namespace std;


class Cocina {
    private:
        int shmid;               // ID de memoria compartida
        ColaPedidos* colaPedidos;    // Puntero a la memoria compartida (vector de pedidos)
        int semid;               // ID de semáforos
        bool colaVacia();
        vector<pid_t> cocineros; // Guarda los PIDs de los hijos

    public:
        Cocina();
        ~Cocina();

        void inicializar();
        void encolarPedido(Pedido pedido);
        Pedido desencolarPedido();
        void atenderPedidos();
        void llamarCocineros(int cantidadCocineros); // crear procesos hijos
        
};

#endif
