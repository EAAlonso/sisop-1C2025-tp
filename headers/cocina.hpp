#pragma once

#include <vector>
#include <cstring>
#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <csignal>
#include <sys/wait.h>
#include <map>

#include "managerPedidos.hpp"
#include <functional>

#define CANTIDAD_HIJOS_RECIBO 2
#define CANTIDAD_HIJOS_COCINERO 5
#define CANTIDAD_HIJOS_ARMADO 2
#define CANTIDAD_HIJOS_EMPAQUE 2
#define CANTIDAD_HIJOS_ENTREGA 2

#define TIEMPO_PREPARACION_SIMPLE 4
#define TIEMPO_PREPARACION_DOBLE 6
#define TIEMPO_PREPARACION_COMPLETO 8


using namespace std;

extern ManagerPedidos managerPedidos;
extern std::map<std::string, std::vector<pid_t>> workerPids;

struct MapHijosFunc
{
    string tipo;
    function<void()> func;
};

struct MapHijosData
{
    string tipo;
    pid_t pid;
};

class Cocina
{
public:
    Cocina() = default;
    ~Cocina();

    void LlamarCocineros(); // crear procesos hijos
private:
    vector<MapHijosData> hijosData; // Guarda los PIDs de los hijos
    void inicializar();
    void atenderPedidos();

    void RecibirPedidos();
    void Cocinar();
    void ArmarPedidos();
    void EmpaquetarPedidos();
    void EntregarPedidos();
};