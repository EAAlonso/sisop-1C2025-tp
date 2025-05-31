#pragma once
#include "enum.hpp"

using namespace std;

class Combo {
    public:    
        Combo();
        Combo(TipoCombo);
    private:
        int tiempoPreparacion; // Tiempo de preparación en segundos
        TipoCombo tipo;
};