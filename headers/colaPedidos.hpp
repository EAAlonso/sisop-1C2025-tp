#pragma once

#include "pedido.hpp"

#define MAX_PEDIDOS 50

struct ColaPedidos {
    Pedido pedidos[MAX_PEDIDOS];
    int pri;
    int ult;
    int cantidad;
};