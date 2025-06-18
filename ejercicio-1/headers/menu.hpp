#pragma once

#include <iostream>
#include <string>
#include "managerPedidos.hpp"

#define SEPARATOR "----------------------------------------\n"

using namespace std;

extern volatile sig_atomic_t main_terminar;

class Menu
{
public:
    Menu(ManagerPedidos *managerPedidos) : managerPedidos(managerPedidos) {};

    void Mostrar();
    static void EsperarAccion(int segundos = 2);
    static string TipoComboToString(TipoCombo tipo);

private:
    void mostrarTitulo();
    void mostrarMenu();
    void mostrarOpcionesCombos();
    void seleccionarCombo();
    void cargarArchivo();
    void terminarDesdeMenu();
    ManagerPedidos *managerPedidos;
};