#include <iostream>
#include <string>

class Cocinero {
    private:
        int id;
        std::string nombre;
        bool ocupado = false;

    public:
        Cocinero();
        Cocinero(int id, std::string nombre);
};  