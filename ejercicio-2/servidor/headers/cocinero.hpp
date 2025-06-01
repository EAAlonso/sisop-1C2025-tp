#pragma once

#include "../headers/pedido.hpp"
#include "../headers/cocina.hpp"
#include <thread>
#include <atomic>
#include <unistd.h>
#include <iostream>
#include <chrono>
#include <iomanip>

using namespace std;
using namespace chrono;

class Cocina; // forward declaration

class Cocinero {
    public:
        Cocinero(int id, Cocina& cocina);
        ~Cocinero();

        Cocinero(const Cocinero&) = delete;
        Cocinero& operator=(const Cocinero&) = delete;
        Cocinero(Cocinero&&) noexcept = default;
        Cocinero& operator=(Cocinero&&) noexcept = default;

        void iniciar();
        void unirse();

    private:
        void trabajar();
        void logPedido(string) const;

        int id;
        Cocina& cocina;
        thread hilo;
        atomic<bool> activo;
};
