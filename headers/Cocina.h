#ifndef COCINA_H
#define COCINA_H

#include "ColaPedidos.h"
#include "../headers/Pedido.h"

using namespace std;


class Cocina {
    private:
        int shmid;               // ID de memoria compartida
        ColaPedidos* colaPedidos;    // Puntero a la memoria compartida (vector de pedidos)
        int semid;               // ID de semáforos
        bool colaVacia();

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
