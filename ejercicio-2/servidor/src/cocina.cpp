#include "../headers/cocina.hpp"

Cocina::Cocina(int puerto) : puerto(puerto), servidorActivo(true) {}

Cocina::~Cocina() {
    cerrarCocina();
}

void Cocina::abrirCocina() {
    socketServidor = socket(AF_INET, SOCK_STREAM, 0); // creación de socket TCP (sock_stream) para IPv4 (af_inet)
    if (socketServidor < 0) 
    {
        perror("Error al crear socket");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(socketServidor, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)); // permite reutilizar el puerto rápidamente tras reiniciar el servidor
                                                                             // sino el SO le pone un time_wait y demora minutos para liberar el socket
                                                                             // y aparece un error 'Address already in use'

    // preparación del servidor
    sockaddr_in direccionServidor{};
    direccionServidor.sin_family = AF_INET;
    direccionServidor.sin_addr.s_addr = INADDR_ANY;
    direccionServidor.sin_port = htons(puerto);

    if (bind(socketServidor, (struct sockaddr*)&direccionServidor, sizeof(direccionServidor)) < 0) // asocia el socket a esa dirección IP y puerto.
    { 
        perror("Error en bind");
        exit(EXIT_FAILURE);
    }

    // empieza a escuchar conexiones entrantes (clientes que se conectan a este socket)
    // hasta 10 en espera (backlog de conexiones no aceptadas aún)
    if (listen(socketServidor, 10) < 0) 
    {
        perror("Error en listen");
        exit(EXIT_FAILURE);
    }

    Cocina::mostrarBienvenidaConsola();

    // creación de 5 hilos cocineros, cada uno ejecutando el método cocineroLoop(int id)
    // los cocineros toman pedidos y los procesan
    for (int i = 0; i < 5; ++i) {
        hilosCocineros.emplace_back(&Cocina::cocineroLoop, this, i + 1);
    }

    // Lanzar hilo aceptador
    hiloAceptador = thread(&Cocina::aceptarClientes, this);
}

void Cocina::aceptarClientes() {
    while (servidorActivo) {
        sockaddr_in direccionCliente;
        socklen_t tam = sizeof(direccionCliente);
        int socketCliente = accept(socketServidor, (struct sockaddr*)&direccionCliente, &tam);
        if (socketCliente < 0) {
            if (servidorActivo) perror("[Cocina] Error en accept");
            continue;
        }

        char buffer[1024] = {0};
        ssize_t bytes = recv(socketCliente, buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0) {
            close(socketCliente);
            continue;
        }

        try {
            json pedidoJson = json::parse(buffer);
            Pedido pedido;
            pedido.combo = pedidoJson.value("combo", "");
            pedido.clienteSocket = socketCliente;

            {
                lock_guard<mutex> lock(mutexColaPedidos);
                colaPedidos.push(pedido);
            }
            cvPedidos.notify_one();

            cout << "[Cocina] Pedido recibido: " << pedido.combo << endl;

        } catch (...) {
            cerr << "[Cocina] Error al parsear JSON del cliente." << endl;
            close(socketCliente);
        }
    }
}

void Cocina::cocineroLoop(int id) {
    while (servidorActivo) {
        Pedido pedido;
        {
            unique_lock<mutex> lock(mutexColaPedidos);
            cvPedidos.wait(lock, [&]() { return !colaPedidos.empty() || !servidorActivo; });

            if (!servidorActivo && colaPedidos.empty()) return;

            pedido = colaPedidos.front();
            colaPedidos.pop();
        }

        auto logPaso = [&](const string& paso) {
            cout << "[Cocinero " << id << "] " << paso << endl;
        };

        logPaso("Tomando pedido...");
        sleep(2);

        logPaso("Cocinando...");
        sleep(2);

        if (pedido.combo == "S") {
            logPaso("Armando combo: 1 carne, 1 queso, 2 panes");
            sleep(3);
        } else if (pedido.combo == "D") {
            logPaso("Armando combo: 2 carnes, 2 quesos, 3 panes");
            sleep(5);
        } else {
            logPaso("Armando combo: 2 carnes, 2 quesos, lechuga, tomate");
            sleep(7);
        }

        logPaso("Empaquetando...");
        sleep(3);

        logPaso("Entregado.");
        sleep(2);

        close(pedido.clienteSocket);
    }
}


void Cocina::cerrarCocina() {
    servidorActivo = false;
    close(socketServidor);
    cvPedidos.notify_all();

    if (hiloAceptador.joinable()) hiloAceptador.join();

    for (auto& hilo : hilosCocineros) {
        if (hilo.joinable()) hilo.join();
    }

    cout << "[Cocina] Apagado completo." << endl;
}

void Cocina::mostrarBienvenidaConsola() 
{
    cout << endl;
    cout << "\033[38;5;179m---------------------------------------" << endl;
    cout << "¡¡¡ Fork & Burger abrió sus puertas !!!\033[0m (Servidor escuchando en puerto " << puerto << ")" <<endl;
    cout << "\033[38;5;179m---------------------------------------\033[0m" << endl;
    cout << endl;
    cout << "\033[38;5;179m        █████████        \033[0m\n";
    cout << "\033[38;5;34m      █████████████      \033[0m\n";
    cout << "\033[1;31m     ███████████████     \033[0m\n";
    cout << "\033[1;93m     ███████████████     \033[0m\n";
    cout << "\033[38;5;88m      █████████████      \033[0m\n";
    cout << "\033[38;5;179m        █████████        \033[0m\n";
    cout << endl;

}
