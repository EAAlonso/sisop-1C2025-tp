#include <iostream>
#include <string>

using namespace std;

class Cocinero {
    private:
        int id;
        string nombre;
        bool ocupado = false;

    public:
        Cocinero();
        Cocinero(int id, string nombre);
        // TO-DO cocinar o atenderPedido para pasar el ocupado a true
};  