#pragma once

#include "../headers/json.hpp"
#include "../headers/cocinero.hpp"
#include "../headers/pedido.hpp"
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

class Cocinero; // forward declaration
extern mutex mutexLog;

class Cocina {
private:
    int puerto;
    int socketServidor;

    vector<unique_ptr<Cocinero>> cocineros;
    thread hiloAceptador;

    void aceptarClientes();
    void cocineroLoop(int id);
    void mostrarBienvenidaConsola();

public:
    Cocina(int puerto);
    ~Cocina();
    
    atomic<bool> servidorActivo;
    queue<Pedido> colaPedidos;
    mutex mutexColaPedidos;
    condition_variable cvPedidos; // receive bloqueante ->  espera que otro hilo lo despierte con una señal 

    void abrirCocina();
    void cerrarCocina();
    int contadorPedidos = 1;
};
