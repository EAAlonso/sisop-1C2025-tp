#include "../headers/cocinero.hpp"

Cocinero::Cocinero() {
    this->id = 0;
    this->nombre = "";
}

Cocinero::Cocinero(int id, std::string nombre) {
    this->id = id;
    this->nombre = nombre;
}