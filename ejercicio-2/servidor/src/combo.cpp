#include "../headers/combo.hpp"

Combo::Combo(const string& tipoCombo)
    : tipo(tipoCombo) {}

string Combo::getTipo() const {
    return tipo;
}

string Combo::getDetalle() const {
    if (tipo == "S") {
        return "1 carne, 1 queso, 2 panes";
    } else if (tipo == "D") {
        return "2 carnes, 2 quesos, 3 panes";
    } else if (tipo == "C") {
        return "2 carnes, 2 quesos, lechuga, tomate, 3 panes";
    } else {
        return "Combo desconocido";
    }
}
