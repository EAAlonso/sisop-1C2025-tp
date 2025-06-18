#include "../headers/menu.hpp"
#include "../headers/cocina.hpp"
#include <limits>
#include <fcntl.h>

int leerOpcionConSelect() {
    std::string buffer;
    int opcion = -1;
    fd_set set;
    struct timeval timeout;

    while (true) {
        FD_ZERO(&set);
        FD_SET(STDIN_FILENO, &set);
        timeout.tv_sec = 1;  // 1 second timeout
        timeout.tv_usec = 0;

        int rv = select(STDIN_FILENO + 1, &set, NULL, NULL, &timeout);
        if (rv == -1) {
            perror("select");
            return -1;
        } else if (rv == 0) {
            // Timeout: check for signal or other events
            extern volatile sig_atomic_t main_terminar;
            if (main_terminar) return -1;
            continue;
        } else {
            char c;
            ssize_t n = read(STDIN_FILENO, &c, 1);
            if (n > 0) {
                if (c == '\n') break;
                buffer += c;
                std::cout << c << std::flush;
            }
        }
    }
    try {
        opcion = std::stoi(buffer);
    } catch (...) {
        opcion = -1;
    }
    return opcion;
}




void Menu::Mostrar()
{
    int opcion;

    do
    {
        this->mostrarMenu();
        //cin >> opcion;
        opcion = leerOpcionConSelect();
        if (main_terminar) return;
        switch (opcion)
        {
        case 1:
            mostrarOpcionesCombos();
            seleccionarCombo();
            break;
        case 2:
            cargarArchivo();
            break;
        case 3:
            terminarDesdeMenu();
            break;
        default:
            break;
        }
    } while (opcion != 3 && !main_terminar);
    return;
}

void Menu::mostrarTitulo()
{
    system("clear");
    cout << "¡¡¡Bienvenidos a FORK & BURGER!!!" << endl;
    cout << SEPARATOR;
}

void Menu::mostrarMenu()
{
    mostrarTitulo();
    cout << "1. Crear Combo" << endl;
    cout << "2. Cargar Combos desde archivo" << endl;
    cout << SEPARATOR << endl;
    cout << "3. Salir" << endl;
    cout << SEPARATOR << endl;
    cout << "Su opción: " << endl;
}

void Menu::mostrarOpcionesCombos()
{
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

void Menu::seleccionarCombo()
{
    int opcion;
    cin >> opcion;

    if (opcion == 4)
    {
        cout << "Volviendo al menú principal..." << endl;
        return;
    }

    managerPedidos->CrearPedido(opcion);
}

void Menu::cargarArchivo()
{
    string filename;
    cout << "Ingrese el nombre del archivo de pedidos: ";
    cin >> filename;
    managerPedidos->CargarPedidosDesdeArchivo(filename);
    Menu::EsperarAccion(1);
}

void Menu::terminarDesdeMenu()
{
    managerPedidos->Terminar();
}

void Menu::EsperarAccion(int segundos)
{
    cout << "Continuando en " << segundos << " segundos..." << endl;
    sleep(segundos);

    //std::cout << "Presionar ENTER para continuar...";
    //std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

string Menu::TipoComboToString(TipoCombo tipo)
{
    switch (tipo)
    {
    case TipoCombo::SIMPLE:
        return "SIMPLE";
    case TipoCombo::DOBLE:
        return "DOBLE";
    case TipoCombo::COMPLETO:
        return "COMPLETO";
    default:
        return "Desconocido";
    }
}