#include <iostream>
#include <string>

#include "../enums/EstadoCombo.h"
#include "../enums/TipoCombo.h"

using namespace std;


class Combo {
    private:
        int id;
        EstadoCombo estado;
        TipoCombo tipo;

    public:
        static int cantidadCombos;
        Combo(TipoCombo tipo);
        void mostrarId();
};



