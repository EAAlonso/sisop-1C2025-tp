// cocina.h
#ifndef COCINA_H
#define COCINA_H

#include "ColaPedidos.h"

class Cocina {
    private:
        int shmid;               // ID de memoria compartida
        ColaPedidos* cola;       // Puntero a la memoria compartida
        int semid;               // ID de semáforos

    public:
        Cocina();
        ~Cocina();

        void inicializar();
        void encolarPedido(Pedido pedido);
        Pedido desencolarPedido();
};

#endif
