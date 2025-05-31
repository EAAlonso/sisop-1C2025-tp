#pragma once
#include <string>
#include <atomic>

class Cliente {
private:
    int socketCliente;
    std::string combo;

    std::atomic<bool> conectado;

    void escucharServidor();  // Recibe y muestra mensajes del servidor

public:
    Cliente(const std::string& combo);
    ~Cliente();

    bool conectar(const std::string& host, int puerto);
    void enviarPedido();
    void iniciar();
    void cerrar();
};
