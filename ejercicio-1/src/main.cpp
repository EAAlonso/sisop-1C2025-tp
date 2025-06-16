#include "../headers/cocina.hpp"
#include "../headers/menu.hpp"
#include "../headers/managerPedidos.hpp"

#include <limits>

Cocina* g_cocina = nullptr;

void handler_sigint(int) {
    cout << "\n[Main] Señal SIGINT recibida. Cerrando cocina limpiamente...\n";
    if (g_cocina) {
        delete g_cocina;
        g_cocina = nullptr;
    }
    exit(0);
}

int main()
{
    signal(SIGINT, handler_sigint);
    ManagerPedidos managerPedidos;
    g_cocina = new Cocina();
    Menu menu;

    g_cocina->LlamarCocineros();
    
    Menu::EsperarAccion();
    menu.Mostrar();

    return 0;
}