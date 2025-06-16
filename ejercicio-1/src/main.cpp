#include "../headers/cocina.hpp"
#include "../headers/menu.hpp"
#include "../headers/managerPedidos.hpp"

#include <limits>

Cocina* g_cocina = nullptr;

void inicializarColasCompartidas();
void lanzarCocineros();

void inicializarColasCompartidas() {
    ColaMemCompartida pendientes("Pendientes", true);
    ColaMemCompartida recibidos("Recibidos", true);
    ColaMemCompartida coccion("Coccion", true);
    ColaMemCompartida armado("Armado", true);
    ColaMemCompartida empaquetado("Empaquetado", true);
    ColaMemCompartida entrega("Entrega", true);
}

void lanzarCocineros() {
    pid_t pid;

    // Recibo
    pid = fork();
    if (pid == 0) {
        Cocina cocina;
        cocina.RecibirPedidos();
        exit(0);
    }

    // Cocina
    pid = fork();
    if (pid == 0) {
        Cocina cocina;
        cocina.Cocinar();
        exit(0);
    }

    // Armado
    pid = fork();
    if (pid == 0) {
        Cocina cocina;
        cocina.ArmarPedidos();
        exit(0);
    }

    // Empaque
    pid = fork();
    if (pid == 0) {
        Cocina cocina;
        cocina.EmpaquetarPedidos();
        exit(0);
    }

    // Entrega
    pid = fork();
    if (pid == 0) {
        Cocina cocina;
        cocina.EntregarPedidos();
        exit(0);
    }
}

void handler_sigint(int) {
    cout << "\n[Main] Señal SIGINT recibida. Cerrando cocina limpiamente...\n";
    if (g_cocina) {
        delete g_cocina;
        g_cocina = nullptr;
    }
    exit(0);
}


int main() {
    signal(SIGINT, handler_sigint);
    inicializarColasCompartidas();
    sleep(1);

    // FORKEAMOS HIJOS PRIMERO
    lanzarCocineros();  // Acá hacemos todos los fork().

    // EL PADRE SE QUEDA EN EL MENU
    ManagerPedidos managerPedidos;
    Menu menu;
    menu.Mostrar();

    return 0;
}