#include "../headers/cocina.hpp"
#include "../headers/menu.hpp"
#include "../headers/managerPedidos.hpp"

#include <limits>

Cocina* g_cocina = nullptr;

int main()
{

    ManagerPedidos managerPedidos;
    g_cocina = new Cocina();
    Menu menu;

    g_cocina->LlamarCocineros();
    
    Menu::EsperarAccion();
    menu.Mostrar();

    return 0;
}