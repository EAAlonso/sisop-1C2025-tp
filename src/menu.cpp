#include "../headers/menu.hpp"
#include "../headers/cocina.hpp"
#include <limits>

extern Cocina* g_cocina;

Menu::Menu() {
    this->managerPedidos = ManagerPedidos();
}

void Menu::Mostrar() {
    int opcion;

    do {
        this->mostrarMenu();
        cin >> opcion;

        switch (opcion) {
            case 1:
                mostrarOpcionesCombos();
                seleccionarCombo();
                break;
            case 2:
                //showStatusMenu();
                break;
            case 3:
                managerPedidos.Terminar();
                cout << "Saliendo del programa. Cerrando cocina y esperando a los cocineros...\n" << endl;
                if (g_cocina) {
                    delete g_cocina;
                    g_cocina = nullptr;
                }
                break;
            default:
                cout << "Opción no válida." << endl;
                break;
        }
    } while(opcion != 3);
    exit(0);
}

void Menu::mostrarTitulo() {
    system("clear");
    cout << "¡¡¡Bienvenidos a FORK & BURGER!!!" << endl;
    cout << SEPARATOR;
}

void Menu::mostrarMenu() {
    mostrarTitulo();
    cout <<  "1. Crear Combo" << endl;
    //cout << "2. Mostrar Combos" << endl;
    cout << SEPARATOR << endl;
    cout << "3. Salir" << endl;
    cout << SEPARATOR << endl;
    cout << "Su opción: " << endl;
}

void Menu::seleccionarCombo() {
    int opcion;
    cin >> opcion;
    
    if (opcion == 4) {
        cout << "Volviendo al menú principal..." << endl;
        return;
    }

    managerPedidos.CrearPedido(opcion);
}

void Menu::mostrarOpcionesCombos() {
    mostrarTitulo();
    cout << "Elegir un combo:" << endl;
    cout << "1. Combo Simple" << endl;
    cout << "2. Combo Doble" << endl;
    cout << "3. Combo Completo" << endl;
    cout << SEPARATOR << endl;
    cout << "4. Volver" << endl;
    cout << SEPARATOR << endl;
    cout << "Su opción: " << endl;
}

void Menu::EsperarAccion() {  
    std::cout << "Presionar ENTER para continuar...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

string Menu::TipoComboToString(TipoCombo tipo) {
    switch (tipo) {
        case TipoCombo::SIMPLE: return "SIMPLE";
        case TipoCombo::DOBLE: return "DOBLE";
        case TipoCombo::COMPLETO: return "COMPLETO";
        default: return "Desconocido";
    }
}