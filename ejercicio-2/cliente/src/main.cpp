#include "../headers/simulador_clientes.hpp"

using namespace std;

int main() {
    string host = "190.173.130.240";
    //string host = "0.0.0.0";
    
    int puerto = 8080;
    while (true) {
        cout << "\n--- Menú Cliente ---\n";
        cout << "1. Mandar pedidos del archivo\n";
        cout << "2. Salir\n";
        cout << "Seleccione una opción: ";

        int opcion;
        cin >> opcion;

        if (opcion == 1) {
            simularClientesDesdeArchivo("./pedidos.txt", puerto, host);
        } else if (opcion == 2) {
            cout << "Saliendo del cliente...\n";
            break;
        } else {
            cout << "Opción inválida, intente nuevamente.\n";
        }
    }

    return 0;
    return 0;
}
