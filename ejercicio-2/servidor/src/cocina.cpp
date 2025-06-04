#include "../headers/cocina.hpp"

Cocina::Cocina(int puerto) : puerto(puerto), servidorActivo(true) {}

Cocina::~Cocina() {
    cerrarCocina();
}

void Cocina::abrirCocina() {
    socketServidor = socket(AF_INET, SOCK_STREAM, 0);
    if (socketServidor < 0) {
        perror("Error al crear socket");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(socketServidor, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in direccionServidor{};
    direccionServidor.sin_family = AF_INET;
    direccionServidor.sin_addr.s_addr = INADDR_ANY;
    direccionServidor.sin_port = htons(puerto);

    if (bind(socketServidor, (struct sockaddr*)&direccionServidor, sizeof(direccionServidor)) < 0) {
        perror("Error en bind");
        exit(EXIT_FAILURE);
    }

    if (listen(socketServidor, 10) < 0) {
        perror("Error en listen");
        exit(EXIT_FAILURE);
    }

    Cocina::mostrarBienvenidaConsola();

    for (int i = 0; i < 5; ++i) {
        auto cocinero = make_unique<Cocinero>(i + 1, *this);
        cocinero->iniciar();
        cocineros.push_back(move(cocinero));
    }

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


        {
            unique_lock<mutex> lock(mutexClientes);
            if (clientesActivos >= MAX_CLIENTES) {
                cout << "[Cocina] Límite de clientes alcanzado. Rechazando conexión." << endl;
                close(socketCliente);
                continue;
            }
            clientesActivos++;
        }

        thread([this, socketCliente, direccionCliente]() {
            char ipCliente[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &direccionCliente.sin_addr, ipCliente, INET_ADDRSTRLEN);
            cout << "[Cocina] Conexión aceptada desde: " << ipCliente << endl;

            char buffer[1024] = {0};
            ssize_t bytes = recv(socketCliente, buffer, sizeof(buffer) - 1, 0);
            if (bytes <= 0) {
                cout << "[Cocina] Cliente desconectado sin enviar datos." << endl;
                close(socketCliente);
                liberarCliente(); 
                return;
            }

            buffer[bytes] = '\0';
            cout << "[Cocina] Mensaje recibido (crudo): " << buffer << endl;

            try {
                json pedidoJson = json::parse(buffer);

                Pedido pedido;
                pedido.combo = pedidoJson.value("combo", "");
                pedido.clienteSocket = socketCliente;

                {
                    lock_guard<mutex> lock(mutexColaPedidos);
                    pedido.numeroPedido = contadorPedidos++;
                    colaPedidos.push(pedido);
                    cout << "[Cocina] Pedido #" << pedido.numeroPedido << " recibido: " << pedido.combo << endl;
                }

                cvPedidos.notify_one();

            } catch (...) {
                cerr << "[Cocina] Error al parsear JSON del cliente." << endl;
                string errorMsg = "Error: JSON inválido\n";
                send(socketCliente, errorMsg.c_str(), errorMsg.size(), 0);
                close(socketCliente);
                liberarCliente();
                return;
            }
        }).detach();
    }
}



void Cocina::liberarCliente() {
    {
        lock_guard<mutex> lock(mutexClientes);
        clientesActivos--;
    }

    {
        lock_guard<mutex> lock(mutexColaPedidos);
        if (clientesActivos == 0 && colaPedidos.empty() && !cierreSolicitado) {
            cierreSolicitado = true;
            cout << "[Cocina] Todos los clientes se retiraron y no hay más pedidos. Cerrando cocina..." << endl;
            cerrarCocina();
            raise(SIGINT);
        }
    }
    cvClientes.notify_one();
}

void Cocina::cerrarCocina() {
    if (!servidorActivo) return;

    servidorActivo = false;

    close(socketServidor);

    int dummySock = socket(AF_INET, SOCK_STREAM, 0);
    if (dummySock >= 0) {
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(puerto);
        addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        connect(dummySock, (struct sockaddr*)&addr, sizeof(addr));
        close(dummySock);
    }

    cvPedidos.notify_all();

    if (hiloAceptador.joinable()) hiloAceptador.join();

    for (auto& cocinero : cocineros) {
        cocinero->unirse();
    }

    cout << "[Cocina] Apagado completo." << endl;
}

void Cocina::mostrarBienvenidaConsola() {
    cout << endl;
    cout << "\033[38;5;179m---------------------------------------" << endl;
    cout << "¡¡¡ Fork & Burger abrió sus puertas !!!\033[0m (Servidor escuchando en puerto " << puerto << ")" << endl;
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
