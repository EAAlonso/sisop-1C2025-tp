#pragma once
#include <string>

using namespace std;

class Combo {
private:
    string tipo;

public:
    Combo(const string& tipoCombo);

    string getTipo() const;
    string getDetalle() const;
};
