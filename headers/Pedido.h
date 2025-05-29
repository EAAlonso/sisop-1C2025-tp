#ifndef PEDIDO_H
#define PEDIDO_H

#include "../enums/TipoCombo.h"
#include "../enums/EstadoPedido.h"

struct Pedido {
    int id;
    TipoCombo tipoCombo;
    EstadoPedido estado;
};

#endif
