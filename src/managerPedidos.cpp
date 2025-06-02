#include "../headers/cocina.hpp"
#include "../headers/managerPedidos.hpp"
#include "../headers/menu.hpp"

#include <iostream>
#include <string>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>

// Instancias globales para las 5 colas de estados
ColaMemCompartida ColaPendientes = ColaMemCompartida("Pendientes", "1-pendientes.log");
ColaMemCompartida ColaRecibidos = ColaMemCompartida("Recibidos", "2-recibidos.log");
ColaMemCompartida ColaCoccion = ColaMemCompartida("Coccion", "3-coccion.log");
ColaMemCompartida ColaArmado = ColaMemCompartida("Armado", "4-armado.log");
ColaMemCompartida ColaEmpaquetado = ColaMemCompartida("Empaquetado", "5-empaquetado.log");
ColaMemCompartida ColaEntrega = ColaMemCompartida("Entrega", "6-entrega.log");

void ManagerPedidos::CrearPedido(int comboId)
{
    if (terminar)
    {
        cout << "El programa ha terminado. No se pueden crear más pedidos." << endl;
        return;
    }

    if (comboId < 1 || comboId > 3)
    {
        cout << "ID de combo no válido" << endl;
        return;
    }
    s_Pedido pedido; // Estructura para el pedido

    pedido.id = countId++;    // TODO: Chequear generacion ID
    pedido.combo = s_Combo(); // Inedido.combo.tipo = static_cast<TipoCombo>(comboId - 1);
    switch (comboId-1)
    {
    case TipoCombo::SIMPLE:
        pedido.combo.tipo = TipoCombo::SIMPLE;
        pedido.combo.tiempoPreparacion = 4;
        break;
    case TipoCombo::DOBLE:
        pedido.combo.tipo = TipoCombo::DOBLE;
        pedido.combo.tiempoPreparacion = 6;
        break;
    case TipoCombo::COMPLETO:
        pedido.combo.tipo = TipoCombo::COMPLETO;
        pedido.combo.tiempoPreparacion = 8;
        break;
    }
    pedido.estado = EstadoPedido::PENDIENTE; // TODO: Esperar si no hay espacio

    while(!ColaPendientes.Push(pedido)) {
        cout << "Cola de pedidos llena. No se pudo crear el pedido. Intentando en 2 segundos..." << endl;
        sleep(2); // Espera 2 segundos antes de intentar nuevamente
    }

    ColaPendientes.Log(pedido); 

    cout << "Pedido creado con ID: " << pedido.id << " y tipo de combo: " << Menu::TipoComboToString(pedido.combo.tipo) << endl;

    sleep(0.5);
    Menu::EsperarAccion();
}