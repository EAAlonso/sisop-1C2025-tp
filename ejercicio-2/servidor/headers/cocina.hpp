#pragma once

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <atomic>
#include <string>
#include <netinet/in.h>

struct Pedido {
    int clienteSocket;  // Para enviar respuesta si se desea
    std::string combo;
};

class Cocina {
private:
    int puerto;
    int socketServidor;
    std::atomic<bool> servidorActivo;

    std::queue<Pedido> colaPedidos;
    std::mutex mutexCola;
    std::condition_variable cvPedidos;

    std::vector<std::thread> hilosCocineros;
    std::thread hiloAceptador;

    void aceptarClientes();
    void cocineroLoop(int id);

public:
    Cocina(int puerto);
    ~Cocina();

    void iniciar();
    void apagar();
};
