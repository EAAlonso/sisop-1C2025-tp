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
        auto cocinero = std::make_unique<Cocinero>(i + 1, *this);
        cocinero->iniciar();
        cocineros.push_back(std::move(cocinero));
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

void Cocina::cerrarCocina() {
    servidorActivo = false;
    close(socketServidor);
    cvPedidos.notify_all();

    if (hiloAceptador.joinable()) hiloAceptador.join();

    for (auto& cocinero : cocineros) {
        cocinero->unirse();
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
