#pragma once
#include <string>
#include <atomic>

using namespace std;

class Cliente {
private:
    string combo;     // Mover esto arriba
    int socketCliente;
    atomic<bool> conectado;

    void escucharServidor();  // Recibe y muestra mensajes del servidor

public:
    Cliente(const string& combo);
    ~Cliente();

    bool conectar(const string& host, int puerto);
    void enviarPedido();
    void iniciar();
    void cerrar();
};

