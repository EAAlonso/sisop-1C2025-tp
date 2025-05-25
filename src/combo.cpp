#include <stdio.h>
#include <stdlib.h>

#include "../include/combo.h"
#include "../include/menu.h"


Combo combos[MAX_COMBOS]; // Define the shared array of combos
int comboCount = 0;       // Define the shared counter for the number of combos
pthread_mutex_t comboMutex = PTHREAD_MUTEX_INITIALIZER; // Define the mutex for thread safety

int nextComboID = 1; // Global counter for generating unique IDs
int indexPool[MAX_COMBOS]; // Pool of reusable IDs
int indexPoolCount = 0;    // Number of IDs in the pool

ComboQueue comboQueue = {{{0}}, 0, -1, 0};

int EnqueueCombo(Combo* combo) {
    if (comboQueue.size == QUEUE_MAX) {
        printf("La cola de combos está llena. No se puede agregar más órdenes.\n");
        return -1; // Queue is full
    }

    comboQueue.rear = (comboQueue.rear + 1) % QUEUE_MAX; // Move rear to the next position
    comboQueue.queue[comboQueue.rear] = *combo; // Add the combo to the queue
    comboQueue.size++; // Increment the size
    return 0; // Success
}

int DequeueCombo(Combo *combo) {
    if (comboQueue.size == 0) {
        printf("La cola de combos está vacía. No hay órdenes para procesar.\n");
        return -1; // Queue is empty
    }

    *combo = comboQueue.queue[comboQueue.front]; // Get the front combo
    comboQueue.front = (comboQueue.front + 1) % QUEUE_MAX; // Move front to the next position
    comboQueue.size--; // Decrement the size

   
    return 0; // Success
}

int IsQueueEmpty() {
    return comboQueue.size == 0;
}

int IsQueueFull() {
    return comboQueue.size == QUEUE_MAX;
}

void CreateComboOrder(int id) {
    if (id < 1 || id > 3) {
        printf("ID de combo no válido.\n");
        return;
    }

    Combo *combo = (Combo *)malloc(sizeof(Combo));
    if (combo == 0) {
        printf("Error al asignar memoria para el combo.\n");
        return;
    }

    switch (id) {
        case 1:
            combo->type = SIMPLE;
            break;
        case 2:
            combo->type = DOBLE;
            break;
        case 3:
            combo->type = COMPLETO;
            break;
        default:
            printf("Opción no válida.\n");
            free(combo);
            return;
    }

    combo->status = PENDIENTE;

    pthread_mutex_lock(&comboMutex);
    // Reuse an ID from the pool if available
    if (indexPoolCount > 0) {
    combo->id = indexPool[--indexPoolCount];
    } else {
        combo->id = nextComboID++;
    }

    if (EnqueueCombo(combo) == 0){
        printf("\n=== Orden Creada ===\n");
        printf("ID: %d | Tipo: %s | Estado: %s\n", combo->id, GetComboTypeString(combo->type), GetComboStatusString(combo->status));
        printf("====================\n\n");
        printf("Presione Enter para continuar...\n");
        getchar();
        getchar();
    }
    
    pthread_mutex_unlock(&comboMutex);

    free(combo);
}

void OrdersReport(int option)
{
    MENU_CLEAN_SCREEN
    switch (option) {
        case 1:
            printf("=== Combos Pendientes ===\n");
            for (int i = 0; i < comboCount; i++) {
                if (combos[i].status == PENDIENTE) {
                    printf("ID: %d | Tipo: %s\n", combos[i].id, GetComboTypeString(combos[i].type));
                }
            }
            break;
        case 2:
            printf("=== Combos En Proceso ===\n");
            for (int i = 0; i < comboCount; i++) {
                if (combos[i].status == EN_PROCESO) {
                    printf("ID: %d | Tipo: %s\n", combos[i].id, GetComboTypeString(combos[i].type));
                }
            }
            break;
        case 3:
            printf("=== Combos Terminados ===\n");
            for (int i = 0; i < comboCount; i++) {
                if (combos[i].status == TERMINADO) {
                    printf("ID: %d | Tipo: %s\n", combos[i].id, GetComboTypeString(combos[i].type));
                }
            }
            break;
        case 4:
            printf("=== Todas las Ordenes ===\n");
            for (int i = 0; i < comboCount; i++) {
                printf("ID: %d | Tipo: %s | Estado: %s\n", combos[i].id, GetComboTypeString(combos[i].type), GetComboStatusString(combos[i].status));
            }
            break;
        default:
            printf("Opción no válida.\n");
            break;
    }

    WaitForUserInput();
}

const char* GetComboTypeString(ComboType type) {
    switch (type) {
        case SIMPLE: return "Simple";
        case DOBLE: return "Doble";
        case COMPLETO: return "Completo";
        default: return "Desconocido";
    }
}

const char* GetComboStatusString(ComboStatus status) {
    switch (status) {
        case PENDIENTE: return "Pendiente";
        case EN_PROCESO: return "En Proceso";
        case TERMINADO: return "Terminado";
        default: return "Desconocido";
    }
}

void LogCombo(Combo *combo) {
    FILE *logFile = fopen("combo_log.txt", "a"); // Open the log file in append mode
    if (logFile != NULL) {
        fprintf(logFile, "Combo - ID: %d, Tipo: %s, Estado: %s\n",
                combo->id,
                GetComboTypeString(combo->type),
                GetComboStatusString(combo->status));
        fclose(logFile); // Close the file
    } else {
        printf("Error al abrir el archivo de registro.\n");
    }
}