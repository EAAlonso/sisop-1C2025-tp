#ifndef COLAPEDIDOS_H
#define COLAPEDIDOS_H

#include "Pedido.h"

#define MAX_PEDIDOS 50

struct ColaPedidos {
    Pedido pedidos[MAX_PEDIDOS];
    int pri;
    int ult;
    int cantidad;
};

#endif
