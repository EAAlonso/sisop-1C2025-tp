#include "../headers/simulador_clientes.hpp"

int main() {
    //std::string host = "190.173.130.240";
    std::string host = "0.0.0.0";
    simularClientesDesdeArchivo("./pedidos.txt", 8080, host);
    return 0;
}
