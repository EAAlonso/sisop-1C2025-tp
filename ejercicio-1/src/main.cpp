#include "../headers/cocina.hpp"
#include "../headers/menu.hpp"
#include "../headers/managerPedidos.hpp"
#include <cstdio>

#include <limits>

Cocina* g_cocina = nullptr;

volatile sig_atomic_t main_terminar = 0;

void handleSignal(int sig)
{
    //cout << "Recibida señal: " << sig << endl;
    main_terminar = 1;
}

void registrarSeniales()
{
   signal(SIGINT, handleSignal);
   signal(SIGTERM, handleSignal);
}

int main()
{
    registrarSeniales(); // Registrar señales al inicio del menú

    g_cocina = new Cocina();
    ManagerPedidos managerPedidos(g_cocina);
    Menu menu (&managerPedidos);

    g_cocina->LlamarCocineros();
    Menu::EsperarAccion(2);
    menu.Mostrar();
    
    cout << "Terminando el programa...\n";
    managerPedidos.Terminar();

    return 0;
}