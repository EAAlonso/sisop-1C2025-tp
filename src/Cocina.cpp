#include "../headers/Cocina.h"


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

    // se agrega un nuevo pedido
    struct sembuf pedido_up = {1, 1, 0};
    semop(semid, &pedido_up, 1);
}

Pedido Cocina::desencolarPedido() {
    struct sembuf pedirMutex = {0, -1, 0};
    struct sembuf liberarMutex  = {0, 1, 0};

    // Espera un pedido disponible
    semop(semid, &pedirMutex, 1);

    Pedido pedido = colaPedidos->pedidos[colaPedidos->pri];
    colaPedidos->pri = (colaPedidos->pri + 1) % MAX_PEDIDOS;
    colaPedidos->cantidad--;

    semop(semid, &liberarMutex, 1);

    return pedido;
}

bool Cocina::colaVacia(){
    return colaPedidos->cantidad = 0;
}

void Cocina::llamarCocineros(int cantidadCocineros) {

    cout << "Llamando a " << cantidadCocineros << " cocineros..." << endl;

    for (int i = 0; i < cantidadCocineros; ++i) {
        pid_t pid = fork();

        if (pid < 0) {
            cerr << "Error al crear el proceso del cocinero." << endl;
            exit(EXIT_FAILURE);

        } else if (pid == 0) {

            // Proceso hijo: se queda esperando pedidos
            cout << "Cocinero " << i + 1 << " listo. PID: " << getpid() << endl;
            cocineros.push_back(pid);
            //atenderPedidos(); // este método no debe salir

            exit(EXIT_SUCCESS); // por si acaso
        }
    }

    cout << "Todos los cocineros están activos y esperando pedidos.\n";
}


void Cocina::atenderPedidos() {

    while (true) {
        
        // Espera a que haya un pedido
        struct sembuf wait_pedido = {1, -1, 0};
        semop(semid, &wait_pedido, 1);

        // Toma mutex para leer de la cola
        struct sembuf pedirMutex = {0, -1, 0};
        semop(semid, &pedirMutex, 1);

        Pedido pedido;
        if (colaPedidos->cantidad > 0) {
            pedido = desencolarPedido();
            // PROCESAR PEDIDO
        }

        // Libera mutex
        struct sembuf liberarMutex = {0, 1, 0};
        semop(semid, &liberarMutex, 1);

        cout << "PID " << getpid() << " procesando pedido ID: " << pedido.id << endl;
        sleep(5);
        cout << "PID " << getpid() << " completó pedido ID: " << pedido.id << endl;
    }
}


Cocina::~Cocina() {
    std::cout << "Cerrando cocina...\n";

    // Matar procesos hijos
    for (pid_t pid : cocineros) {
        kill(pid, SIGTERM);  // Podés usar SIGKILL si no responden
        waitpid(pid, nullptr, 0); // Esperamos que terminen
        std::cout << "Cocinero con PID " << pid << " terminado.\n";
    }

    // Liberar semáforos
    if (semctl(semid, 0, IPC_RMID) == -1) {
        perror("Error al eliminar semáforos");
    } else {
        std::cout << "Semáforos eliminados.\n";
    }

    // Liberar memoria compartida
    if (shmdt(colaPedidos) == -1) {
        perror("Error al desadjuntar memoria compartida");
    }

    if (shmctl(shmid, IPC_RMID, nullptr) == -1) {
        perror("Error al eliminar segmento de memoria compartida");
    } else {
        std::cout << "Memoria compartida eliminada.\n";
    }

    std::cout << "Cocina finalizada correctamente.\n";
}