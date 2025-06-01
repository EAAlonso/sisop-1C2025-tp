#pragma once

#include "../headers/json.hpp"
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <atomic>
#include <string>
#include <netinet/in.h>
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>

using namespace std;
using nlohmann::json;

struct Pedido {
    int clienteSocket;
    string combo;
};

class Cocina {
private:
    int puerto;
    int socketServidor;
    atomic<bool> servidorActivo;

    queue<Pedido> colaPedidos;
    mutex mutexColaPedidos;
    condition_variable cvPedidos; // receive bloqueante ->  espera que otro hilo lo despierte con una señal 

    vector<thread> hilosCocineros;
    thread hiloAceptador;

    void aceptarClientes();
    void cocineroLoop(int id);
    void mostrarBienvenidaConsola();

public:
    Cocina(int puerto);
    ~Cocina();

    void abrirCocina();
    void cerrarCocina();
};
