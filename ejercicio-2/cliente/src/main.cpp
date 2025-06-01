#include "../headers/simulador_clientes.hpp"

using namespace std;

int main() {
    string host = "190.173.130.240";
    //string host = "0.0.0.0";
    simularClientesDesdeArchivo("./pedidos.txt", 8080, host);
    return 0;
}
