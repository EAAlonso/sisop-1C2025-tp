#include "../headers/simulador_clientes.hpp"

using namespace std;

int main() {
    string host = "190.173.130.240";
    //string host = "0.0.0.0";
    
    int puerto = 8080;
    while (true) {
        cout << "\n--- Menú Cliente ---\n";
        cout << "1. Pedir combo simple\n";
        cout << "2. Pedir combo doble\n";
        cout << "3. Pedir combo completo\n";
        cout << "4. Salir\n";
        cout << "Seleccione una opción: ";

        int opcion;
        cin >> opcion;

        switch(opcion)
        {
            case 1:
                simularClientesDesdeArchivo("./pedidosComboS.txt", puerto, host);
            break;
            case 2:
                simularClientesDesdeArchivo("./pedidosComboD.txt", puerto, host);
            break;
            case 3:
                simularClientesDesdeArchivo("./pedidosComboC.txt", puerto, host);
            break;
            case 4:
                cout << "Saliendo del cliente...\n";
            break;
            default:
                cout << "Opción inválida, intente nuevamente.\n";
            break;
        }
    }

    return 0;
}
