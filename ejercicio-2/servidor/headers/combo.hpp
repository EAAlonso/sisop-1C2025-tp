#pragma once
#include <string>

using namespace std;

class Combo {
private:
    string tipo;
    string getDetalle() const;

public:
    Combo(const string& tipoCombo);

    string getTipo() const;
    string getMensajeArmado() const;
    int getTiempoArmado() const;
};
