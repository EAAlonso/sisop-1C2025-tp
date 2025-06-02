#pragma once

#include <iostream>
#include <string>
#include "managerPedidos.hpp"

#define SEPARATOR "----------------------------------------\n"

using namespace std;

class Menu
{
public:
    Menu();

    void Mostrar();
    static void EsperarAccion();
    static string TipoComboToString(TipoCombo tipo);
private:
    void mostrarTitulo();
    void mostrarMenu();
    void seleccionarCombo();
    void mostrarOpcionesCombos();
    ManagerPedidos managerPedidos;
};