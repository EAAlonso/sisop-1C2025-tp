#pragma once
#include "enum.hpp"
#include <semaphore.h>

#define MAX_PEDIDOS 50

using namespace std;

struct s_Combo
{
    int tiempoPreparacion; // Tiempo de preparación en segundos
    TipoCombo tipo;
};

struct s_Pedido
{
    int id;
    s_Combo combo;
    EstadoPedido estado;
};

struct ColaPedidos
{
    s_Pedido pedidos[MAX_PEDIDOS];
    int pri;
    int ult;
    int cantidad;
};

extern ColaPedidos *g_colaPedidos;

extern sem_t *g_mutex;
extern sem_t *g_espacio;
extern sem_t *g_items;

class Pedido
{
public:
    Pedido() = default;
    void CrearPedido(int comboId);
    int InitColaPedidos();
    void DestroySemaforosColaPedidos();

    bool PushPedido(s_Pedido order);
    bool PopPedido(s_Pedido &out);

private:
    int countId = 0; // Contador para generar IDs únicos de pedidos
                     // void CrearPedido(int comboId);
};