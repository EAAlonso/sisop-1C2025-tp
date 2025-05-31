#include "../headers/json.hpp"
#include "../headers/combo.hpp"

#pragma once
#include <vector>
#include <thread>
#include <mutex>
#include <semaphore.h>
#include <atomic>

using namespace std;

class Cocina {
private:
    int puerto;
    int capacidadMaxima; // cantidad máxima de pedidos en paralelo
    int socketServidor;

    std::vector<std::thread> hilosActivos;
    std::mutex mutexHilos;

    sem_t semaforoCapacidad; // limita la cantidad de cocineros activos
    std::atomic<bool> servidorActivo;

    void aceptarClientes();           // loop principal que acepta conexiones
    void manejarCliente(int socket);  // función que ejecuta cada cocinero

    void cerrarConexion(int socket);

public:
    Cocina(int puerto, int capacidadMaxima);
    ~Cocina();

    void iniciar();
    void apagar();
};
