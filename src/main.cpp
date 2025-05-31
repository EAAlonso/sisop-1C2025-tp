#include "../headers/cocinero.hpp"
#include "../headers/cocina.hpp"
#include "../headers/menu.hpp"
#include "../headers/pedido.hpp"

#include <limits>


#define MAX_COCINEROS 3

int main()
{

    //Pedido pedidos;
    Cocina cocina = Cocina();
    Menu menu;

    //pedidos.InitColaPedidos();

    cocina.LlamarCocineros(MAX_COCINEROS);
    
    menu.EsperarAccion();
    menu.Mostrar();

    //pedidos.DestroySemaforosColaPedidos();

    return 0;
}