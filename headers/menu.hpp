#pragma once

#include <iostream>
#include <string>
#include "pedido.hpp"

#define SEPARATOR "----------------------------------------\n"

using namespace std;

class Menu {
    private:
        Pedido pedido;
        void mostrarTitulo(); 
        void mostrarMenu();
        void seleccionarCombo();
        void mostrarOpcionesCombos();
        
    public:
        Menu();
        void Mostrar();
        void EsperarAccion();
}; 