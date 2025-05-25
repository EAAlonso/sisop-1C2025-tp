#include "../headers/Combo.h"

int Combo::cantidadCombos = 0;

Combo::Combo(TipoCombo tipo) {
    this->id = ++cantidadCombos;
    this->tipo = tipo;
    this->estado = EstadoCombo::PENDIENTE;
}

void Combo::mostrarId() {
    cout << this->id << endl;
}