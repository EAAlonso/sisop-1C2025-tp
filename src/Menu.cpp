#include "../headers/Menu.h"

Menu::Menu() {
    int opcion;

    do {
        this->mostrarMenu();
        std::cin >> opcion;

        switch (opcion) {
            case 1:
                mostrarCombos();
                seleccionarCombo();
                break;
            case 2:
                //showStatusMenu();
                break;
            case 3:
                printf("Saliendo del programa.\n");
                break;
            default:
                printf("Opción no válida.\n");
                break;
        }
    } while(opcion != 3);
    exit(0);
}

void Menu::mostrarTitulo() {
    std::cout << "\033[H\033[J";
    std::cout << "¡¡¡Bienvenidos a FORK & BURGER!!!" << std::endl;
    std::cout << SEPARATOR;
}

void Menu::mostrarMenu() {
    mostrarTitulo();
    printf("1. Crear Combo\n");
    printf("2. Mostrar Combos\n");
    printf(SEPARATOR);
    printf("3. Salir\n");
    printf(SEPARATOR);
    printf("Su opción: ");
}

void Menu::seleccionarCombo() {
    int opcion;
    scanf("%d", &opcion);
    
    if (opcion == 4) {
        printf("Volviendo al menú principal...\n");
        return;
    }

    //CreateComboOrder(opcion);
}

void Menu::mostrarCombos() {
    mostrarTitulo();
    printf("Elegir un combo:\n");
    printf("1. Combo Simple\n");
    printf("2. Combo Doble\n");
    printf("3. Combo Completo\n");
    printf(SEPARATOR);
    printf("4. Volver\n");
    printf(SEPARATOR);
    printf("Su opción: ");
}