#include "../headers/simulador_clientes.hpp"

int main() {
    simularClientesDesdeArchivo("../pedidos.txt", "127.0.0.1", 8080);
    return 0;
}
