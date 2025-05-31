#include "../headers/combo.hpp"

Combo::Combo() {
}

Combo::Combo(TipoCombo tipo) {
    this->tipo = tipo;
    switch (tipo) {
        case TipoCombo::SIMPLE:
            this->tiempoPreparacion = 4; // 10 seconds for Combo 1
            break;
        case TipoCombo::DOBLE:
            this->tiempoPreparacion = 6; // 15 seconds for Combo 2
            break;
        case TipoCombo::COMPLETO:
            this->tiempoPreparacion = 8; // 20 seconds for Combo 3
            break;
    }
}