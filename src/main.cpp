#include "../headers/cocinero.hpp"
#include "../headers/cocina.hpp"
#include "../headers/menu.hpp"
#include "../headers/managerPedidos.hpp"

#include <limits>

int main()
{

    ManagerPedidos managerPedidos;
    Cocina cocina;
    Menu menu;

    cocina.LlamarCocineros();
    
    Menu::EsperarAccion();
    menu.Mostrar();

    //pedidos.DestroySemaforosColaPedidos();

    return 0;
}