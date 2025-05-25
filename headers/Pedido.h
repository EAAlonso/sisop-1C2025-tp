#ifndef PEDIDO_H
#define PEDIDO_H

#include "../enums/TipoCombo.h"
#include "../enums/EstadoCombo.h"

struct Pedido {
    int id;
    TipoCombo tipoCombo;
    EstadoCombo estado;
};

#endif
