#pragma once

#include "enum.hpp"
#include "combo.hpp"

using namespace std;

class Pedido {
    public:
            int id;
            Combo combo;
            EstadoPedido estado;

            Pedido();

            void CrearPedido(int comboId);
};

