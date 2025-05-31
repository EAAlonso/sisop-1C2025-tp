#pragma once
#include <string>

class Combo {
private:
    std::string tipo;

public:
    Combo(const std::string& tipoCombo);

    std::string getTipo() const;
    std::string getDetalle() const;
};
