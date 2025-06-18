#include <iostream>
#include <string>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <fstream>

#include "../headers/cocina.hpp"
#include "../headers/managerPedidos.hpp"
#include "../headers/menu.hpp"


// Instancias globales para las 5 colas de estados
ColaMemCompartida ColaPendientes = ColaMemCompartida("Pendientes", "1-pendientes.log");
ColaMemCompartida ColaRecibidos = ColaMemCompartida("Recibidos", "2-recibidos.log");
ColaMemCompartida ColaCoccion = ColaMemCompartida("Coccion", "3-coccion.log");
ColaMemCompartida ColaArmado = ColaMemCompartida("Armado", "4-armado.log");
ColaMemCompartida ColaEmpaquetado = ColaMemCompartida("Empaquetado", "5-empaquetado.log");
ColaMemCompartida ColaEntrega = ColaMemCompartida("Entrega", "6-entrega.log");

void ManagerPedidos::CrearPedido(int comboId, bool lote)
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
        cout << "Cola de pedidos llena. No se pudo crear el pedido. Reintentando en 1 segundo..." << endl;
        sleep(1);
    }

    ColaPendientes.Log(pedido); 

    cout << "Pedido creado con ID: " << pedido.id << " y tipo de combo: " << Menu::TipoComboToString(pedido.combo.tipo) << endl;
    
    if (lote) return;
    
    Menu::EsperarAccion(1);
}

void ManagerPedidos::CargarPedidosDesdeArchivo(const string& filename)
{
    cout << "Cargando pedidos desde el archivo: " << filename << endl;
    ifstream archivo(filename);
    if (!archivo.is_open())
    {
        cout << "No se pudo abrir el archivo: " << filename << endl;
        sleep(1);
        return;
    }

    string linea;
    while (getline(archivo, linea))
    {
        if (main_terminar) return;
        int comboId = stoi(linea);
        CrearPedido(comboId, true);
    }

    archivo.close();
    cout << "Todos los pedidos del archivo fueron cargados." << endl;
    Menu::EsperarAccion(1);
}

void ManagerPedidos::Terminar()
{
    cout << "Terminando el programa..." << endl;
    terminar = true;
    if (cocina)
    {
        delete cocina;
        cocina = nullptr;
    }
}