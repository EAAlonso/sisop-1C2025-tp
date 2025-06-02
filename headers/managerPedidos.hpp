#pragma once

#include <semaphore.h>
#include "colaMemCompartida.hpp"

using namespace std;


extern ColaMemCompartida ColaPendientes;
extern ColaMemCompartida ColaRecibidos;
extern ColaMemCompartida ColaCoccion;
extern ColaMemCompartida ColaArmado;
extern ColaMemCompartida ColaEmpaquetado;
extern ColaMemCompartida ColaEntrega;

class ManagerPedidos
{
public:
    ManagerPedidos() = default;
    void CrearPedido(int comboId);
private:
    int countId = 0; // Contador para generar IDs únicos de pedidos
};