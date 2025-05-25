#include "../headers/Cocina.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <cstring>
#include <iostream>
#include <unistd.h>

#define CLAVE_MEMORIA 1234
#define CLAVE_SEM 5678
#define MUTEX 0
#define PEDIDOS 1

Cocina::Cocina() {
    inicializar();
}

Cocina::~Cocina() {
    shmdt((void*) cola);
    // Opcional: shmctl(shmid, IPC_RMID, NULL);
}

void Cocina::inicializar() {
    // Crear memoria compartida
    shmid = shmget(CLAVE_MEMORIA, sizeof(ColaPedidos), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("Error creando memoria compartida");
        exit(1);
    }

    cola = (ColaPedidos*) shmat(shmid, NULL, 0);

    // Crear semáforo (0 = mutex, 1 = pedidos disponibles)
    semid = semget(CLAVE_SEM, 2, IPC_CREAT | 0666);
    if (semid == -1) {
        perror("Error creando semáforo");
        exit(1);
    }

    // Inicializar si es nuevo
    if (cola->cantidad == 0) {
        cola->pri = 0;
        cola->ult = 0;
        cola->cantidad = 0;
        semctl(semid, MUTEX, SETVAL, 1); // mutex
        semctl(semid, PEDIDOS, SETVAL, 0); // pedidos disponibles
    }
}

void Cocina::encolarPedido(Pedido pedido) {
    struct sembuf mutex_down = {0, -1, 0}; // define la estructura sembuf (semId, valorMutex, comportamiento por defecto -> esperar)
    struct sembuf mutex_up   = {0, 1, 0};

    semop(semid, &mutex_down, 1);

    if (cola->cantidad < MAX_PEDIDOS) {
        cola->pedidos[cola->ult] = pedido;
        cola->ult = (cola->ult + 1) % MAX_PEDIDOS;
        cola->cantidad++;
    }

    semop(semid, &mutex_up, 1);

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

    Pedido pedido = cola->pedidos[cola->pri];
    cola->pri = (cola->pri + 1) % MAX_PEDIDOS;
    cola->cantidad--;

    semop(semid, &mutex_up, 1);

    return pedido;
}
