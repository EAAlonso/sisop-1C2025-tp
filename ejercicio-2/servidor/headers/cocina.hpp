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
#include <csignal>

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
    atomic<bool> cierreSolicitado = false;

    void abrirCocina();
    void cerrarCocina();
    void liberarCliente();
    int contadorPedidos = 1;

    int clientesActivos = 0;
    const int MAX_CLIENTES = 3;
    mutex mutexClientes;
    condition_variable cvClientes;
};
