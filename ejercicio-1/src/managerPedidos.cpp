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

    s_Pedido pedido; 
    pedido.id = countId++;
    pedido.combo = s_Combo();

    switch (comboId-1)
    {
    case TipoCombo::SIMPLE:
        pedido.combo.tipo = TipoCombo::SIMPLE;
        pedido.combo.tiempoPreparacion = TIEMPO_PREPARACION_SIMPLE;
        break;
    case TipoCombo::DOBLE:
        pedido.combo.tipo = TipoCombo::DOBLE;
        pedido.combo.tiempoPreparacion = TIEMPO_PREPARACION_DOBLE;
        break;
    case TipoCombo::COMPLETO:
        pedido.combo.tipo = TipoCombo::COMPLETO;
        pedido.combo.tiempoPreparacion = TIEMPO_PREPARACION_COMPLETO;
        break;
    }
    pedido.estado = EstadoPedido::PENDIENTE;

    while(!ColaPendientes.Push(pedido)) {
        cout << "Cola de pedidos llena. No se pudo crear el pedido. Reintentando en 2 segundos..." << endl;
        sleep(2);
    }

    ColaPendientes.Log(pedido); 

    cout << "Pedido creado con ID: " << pedido.id << " y tipo de combo: " << Menu::TipoComboToString(pedido.combo.tipo) << endl;

    sleep(1);
    Menu::EsperarAccion();
}