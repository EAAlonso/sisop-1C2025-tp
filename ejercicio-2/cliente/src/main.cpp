#include "../headers/simulador_clientes.hpp"

int main() {
    std::string host = "190.173.130.240";
    simularClientesDesdeArchivo("./pedidos.txt", 8080, host);
    return 0;
}
