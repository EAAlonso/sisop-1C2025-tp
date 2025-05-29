#ifndef COCINA_H
#define COCINA_H

#include "ColaPedidos.h"


class Cocina {
    private:
        int shmid;               // ID de memoria compartida
        ColaPedidos* colaPedidos;    // Puntero a la memoria compartida (vector de pedidos)
        int semid;               // ID de semáforos

    public:
        Cocina();
        ~Cocina();

        void inicializar();
        void encolarPedido(Pedido pedido);
        Pedido desencolarPedido();
};

#endif
