#include "../headers/pedido.hpp"
#include "../headers/cocina.hpp"

#include <iostream>
#include <string>

Pedido::Pedido() {
    estado = EstadoPedido::PENDIENTE; // Estado inicial del pedido
 }

void Pedido::CrearPedido(int comboId) {
    if (comboId < 1 || comboId > 3) {
        cout << "ID de combo no válido" << endl;
        return;
    }
    
    combo = Combo(static_cast<TipoCombo>(comboId));
    estado = EstadoPedido::PENDIENTE; // Cambia el estado del pedido a PENDIENTE
    id = 0; //TODO: Chequear generacion ID

    Cocina cocina;
    cocina.EncolarPedido(*this); // Encola el pedido en la cocina
}