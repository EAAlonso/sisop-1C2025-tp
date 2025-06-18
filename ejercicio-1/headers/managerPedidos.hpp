#pragma once

#include <semaphore.h>
#include "colaMemCompartida.hpp"
#include "cocina.hpp"

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
    ManagerPedidos(Cocina *cocina) : cocina(cocina) {};
    void CrearPedido(int comboId, bool lote = false);
    void Terminar(); // Método para indicar que se debe terminar el programa
    void CargarPedidosDesdeArchivo(const string &filename);
private:
    Cocina* cocina = nullptr;
    int countId = 0; // Contador para generar IDs únicos de pedidos
    int terminar = false; // Variable para controlar el estado del programa
};