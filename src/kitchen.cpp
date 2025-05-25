#include <stdio.h>
#include <unistd.h> // For sleep()
#include <pthread.h>
#include "../include/combo.h"
#include <iostream>

// External variables from combo.c
extern Combo combos[];
extern int comboCount;
extern pthread_mutex_t comboMutex;

void* ProcessOrders(void* arg) {
    while (1) {
        pthread_mutex_lock(&comboMutex);
        if (!IsQueueEmpty()) {
            Combo combo;
            if (DequeueCombo(&combo) == 0) {
                pthread_mutex_unlock(&comboMutex);

                printf("Procesando orden ID: %d | Tipo: %s\n", combo.id, GetComboTypeString(combo.type));
                sleep(5);
                printf("Orden ID: %d completada.\n", combo.id);

                pthread_mutex_lock(&comboMutex);
                combo.status = TERMINADO;
                LogCombo(&combo);
                pthread_mutex_unlock(&comboMutex);
            }
        } else {
            pthread_mutex_unlock(&comboMutex);
            sleep(1); // Wait before checking again
        }
    }
    return nullptr;
}

void StartKitchenThread() {
    pthread_t kitchenThread;

    if (pthread_create(&kitchenThread, nullptr, ProcessOrders, nullptr) != 0) {
        std::cerr << "Error creando el hilo de cocina" << std::endl;
    } else {
        pthread_detach(kitchenThread); // Detach the thread to run independently
    }

    printf("Hilo de cocina iniciado.\n");

    // Optionally, wait for the thread to finish (if needed)
    // pthread_join(kitchenThread, NULL);
}
