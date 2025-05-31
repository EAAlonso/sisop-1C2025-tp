#include "../headers/cocina.hpp"

Pedido colaPedidos; // Instancia global de la cola de pedidos

Cocina::Cocina() {
    cout << "Iniciando cocina..." << endl;
    colaPedidos = Pedido();
    colaPedidos.InitColaPedidos();
}

void Cocina::LlamarCocineros(int cantidadCocineros) {

    cout << "Llamando a " << cantidadCocineros << " cocineros..." << endl;

    for (int i = 0; i < cantidadCocineros; ++i) {
        pid_t pid = fork();

        if (pid == 0) {
            atenderPedidos();
            exit(EXIT_SUCCESS);
        } else if (pid > 0) {
            // Proceso padre: guarda el PID del cocinero
            cocineros.push_back(pid);
            cout << "Cocinero con PID " << pid << " creado.\n";
        } else {
            // Error al crear el proceso
            cerr << "Error al crear el proceso del cocinero." << endl;
            exit(EXIT_FAILURE);
        }
    }
    
    cout << "Todos los cocineros están activos y esperando pedidos.\n";
}


void Cocina::atenderPedidos() {
    while (true) { // TODO: FLAG SIG
        s_Pedido pedido = s_Pedido();
        if (!colaPedidos.PopPedido(pedido)) {
            cout << "No hay pedidos disponibles. Cocinero " << getpid() << " esperando..." << endl;
            sleep(1); // Espera antes de volver a intentar
            continue; // Volver al inicio del bucle para esperar un nuevo pedido
        }
        pedido.estado = EstadoPedido::EN_PROCESO;
        // Procesar el pedido
        cout << "PID " << getpid() << " procesando pedido ID: " << pedido.id << endl;
        sleep(pedido.combo.tiempoPreparacion); // Simula el tiempo de preparación del pedido
        pedido.estado = EstadoPedido::TERMINADO;
        cout << "PID " << getpid() << " completó pedido ID: " << pedido.id << endl;
    }
}


Cocina::~Cocina() {

    cout << "Cerrando cocina...\n";
    
    // Matar procesos hijos
    /*for (pid_t pid : cocineros) {
        kill(pid, SIGTERM);  // Podés usar SIGKILL si no responden
        waitpid(pid, nullptr, 0); // Esperamos que terminen
        cout << "Cocinero con PID " << pid << " terminado.\n";
    }*/

    cout << "Cocina cerrada correctamente.\n";
}