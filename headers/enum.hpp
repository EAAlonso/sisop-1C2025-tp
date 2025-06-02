#pragma once

enum EstadoPedido {
    PENDIENTE,
    RECIBIDO,
    COCINADO,
    ARMADO,
    EMPAQUETADO,
    ENTREGADO
};

enum TipoCombo {
    SIMPLE = 0,
    DOBLE = 1,
    COMPLETO = 2
};