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

#include "managerPedidos.hpp"

using namespace std;

extern ManagerPedidos managerPedidos;

class Cocina
{
public:
    Cocina();
    ~Cocina();

    void LlamarCocineros(); // crear procesos hijos
private:
    vector<pid_t> cocineros; // Guarda los PIDs de los hijos
    void inicializar();
    void atenderPedidos();

    void RecibirPedidos();
    void Cocinar();
    void ArmarPedidos();
    void EmpaquetarPedidos();
    void EntregarPedidos();
};