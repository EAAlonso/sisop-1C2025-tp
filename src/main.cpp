#include "../headers/cocinero.hpp"
#include "../headers/cocina.hpp"
#include "../headers/menu.hpp"
#include <limits>

#define MAX_COCINEROS 3

int main()
{

    Cocina cocina;
    Menu menu;

    cocina.LlamarCocineros(MAX_COCINEROS);
    
    menu.EsperarAccion();
    menu.Mostrar();

    return 0;
}