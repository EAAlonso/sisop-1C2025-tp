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
ColaMemCompartida ColaPendientes("Pendientes", false);
ColaMemCompartida ColaRecibidos("Recibidos", false);
ColaMemCompartida ColaCoccion("Coccion", false);
ColaMemCompartida ColaArmado("Armado", false);
ColaMemCompartida ColaEmpaquetado("Empaquetado", false);
ColaMemCompartida ColaEntrega("Entrega", false);

void ManagerPedidos::CrearPedido(int comboId, bool interactivo)
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
        break;
    }

    pedido.estado = EstadoPedido::PENDIENTE;

    while(!ColaPendientes.Push(pedido)) {
        cout << "Cola de pedidos llena. No se pudo crear el pedido. Reintentando en 2 segundos..." << endl;
        sleep(2);
    }

    ColaPendientes.Log(pedido); 

    cout << "Pedido creado con ID: " << pedido.id << " y tipo de combo: " << Menu::TipoComboToString(pedido.combo.tipo) << endl;

    if (interactivo)
    {
        sleep(1);
        Menu::EsperarAccion();
    }
}

void ManagerPedidos::CargarPedidosDesdeArchivo(const string& filename)
{
    ifstream archivo(filename);
    if (!archivo.is_open())
    {
        cout << "No se pudo abrir el archivo: " << filename << endl;
        return;
    }

    string linea;
    while (getline(archivo, linea))
    {
        int comboId = stoi(linea);
        if (comboId < 1 || comboId > 3)
        {
            cout << "Combo inválido en el archivo: " << comboId << endl;
            continue;
        }

        CrearPedido(comboId, false);
    }

    archivo.close();
    cout << "Todos los pedidos del archivo fueron cargados." << endl;

    EsperarProcesamientoCompleto();
}

void ManagerPedidos::EsperarProcesamientoCompleto()
{
    while (true)
    {
        int pendientes = ColaPendientes.CantidadEnCola();
        int recibidos = ColaRecibidos.CantidadEnCola();
        int coccion = ColaCoccion.CantidadEnCola();
        int armado = ColaArmado.CantidadEnCola();
        int empaquetado = ColaEmpaquetado.CantidadEnCola();

        // Mostrar el estado actual
        cout << "\033[2J\033[H";
        cout << "Estado actual de procesamiento:" << endl;
        cout << "Pendientes  : " << pendientes << endl;
        cout << "Recibidos   : " << recibidos << endl;
        cout << "Coccion     : " << coccion << endl;
        cout << "Armado      : " << armado << endl;
        cout << "Empaquetado : " << empaquetado << endl;

        // Condición de salida
        if (pendientes == 0 && recibidos == 0 && coccion == 0 &&
            armado == 0 && empaquetado == 0)
        {
            break;
        }

        usleep(200000);
    }

    cout << "Todos los pedidos fueron procesados." << endl;
    cout << "Presionar ENTER para continuar...";
    Menu::EsperarAccion();
}