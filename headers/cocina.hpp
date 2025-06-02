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