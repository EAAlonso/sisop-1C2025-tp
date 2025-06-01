#include "../headers/combo.hpp"

Combo::Combo(const string& tipoCombo)
    : tipo(tipoCombo) {}

string Combo::getTipo() const {
    return tipo;
}

string Combo::getDetalle() const 
{
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

string Combo::getMensajeArmado() const {
    string color;
    if (tipo == "S") color = "\033[38;5;217m";
    else if (tipo == "D") color = "\033[38;5;210m";
    else if (tipo == "C") color = "\033[38;5;203m";
    else color = "\033[38;5;244m";

    return color + "Armando COMBO " + tipo + ": \033[3m" + getDetalle() + "\033[0m";
}

int Combo::getTiempoArmado() const {
    if (tipo == "S") return 3;
    else if (tipo == "D") return 5;
    else if (tipo == "C") return 7;
    else return 2;
}
