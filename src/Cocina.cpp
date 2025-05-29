#include "../headers/Cocina.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <cstring>
#include <iostream>
#include <unistd.h>

#define CLAVE_MEMORIA 1234
#define CLAVE_SEM 5678
#define MUTEX_PEDIDOS 0 // Para el semnum
#define PEDIDOS_PENDIENTES 1 // Para el semnum

Cocina::Cocina() {
    inicializar();
}

Cocina::~Cocina() {
    shmdt((void*) colaPedidos);
    // Opcional: shmctl(shmid, IPC_RMID, NULL);
}

void Cocina::inicializar() {
    
    // Crear memoria compartida
    shmid = shmget(CLAVE_MEMORIA, sizeof(ColaPedidos), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("Error creando memoria compartida");
        exit(1);
    }

    colaPedidos = (ColaPedidos*) shmat(shmid, NULL, 0);

    // Crear semáforo (0 = mutex, 1 = pedidos disponibles)
    semid = semget(CLAVE_SEM, 2, IPC_CREAT | 0666);
    if (semid == -1) {
        perror("Error creando semáforo");
        exit(1);
    }

    // Inicializar si es nuevo
    if (colaPedidos->cantidad == 0) {
        colaPedidos->pri = 0;
        colaPedidos->ult = 0;
        colaPedidos->cantidad = 0;
        semctl(semid, MUTEX_PEDIDOS, SETVAL, 1); // mutex
        semctl(semid, PEDIDOS_PENDIENTES, SETVAL, 0); // Hay 0 pedidos disponibles al principio
    }
}

void Cocina::encolarPedido(Pedido pedido) {

    // define la estructura sembuf (semId, valorMutex, comportamiento por defecto -> esperar)
    struct sembuf pedirMutex = {0, -1, 0};
    struct sembuf liberarMutex  = {0, 1, 0};

    semop(semid, &pedirMutex, 1);

    if (colaPedidos->cantidad < MAX_PEDIDOS) {
        colaPedidos->pedidos[colaPedidos->ult] = pedido;
        colaPedidos->ult = (colaPedidos->ult + 1) % MAX_PEDIDOS;
        colaPedidos->cantidad++;
    }

    semop(semid, &liberarMutex, 1);

    // Señalar que hay un nuevo pedido
    struct sembuf pedido_up = {1, 1, 0};
    semop(semid, &pedido_up, 1);
}

Pedido Cocina::desencolarPedido() {
    struct sembuf pedido_down = {1, -1, 0};
    struct sembuf mutex_down  = {0, -1, 0};
    struct sembuf mutex_up    = {0, 1, 0};

    // Espera un pedido disponible
    semop(semid, &pedido_down, 1);
    semop(semid, &mutex_down, 1);

    Pedido pedido = colaPedidos->pedidos[colaPedidos->pri];
    colaPedidos->pri = (colaPedidos->pri + 1) % MAX_PEDIDOS;
    colaPedidos->cantidad--;

    semop(semid, &mutex_up, 1);

    return pedido;
}
