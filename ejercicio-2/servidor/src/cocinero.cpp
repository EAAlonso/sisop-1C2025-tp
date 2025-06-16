#include "../headers/cocinero.hpp"

mutex mutexLog;

Cocinero::Cocinero(int id, Cocina& cocina) : id(id), cocina(cocina), activo(true) {}

Cocinero::~Cocinero() {
    unirse();
}

void Cocinero::iniciar() {
    hilo = thread(&Cocinero::trabajar, this);
}

void Cocinero::unirse() {
    activo = false;
    if (hilo.joinable()) {
        hilo.join();
    }
}

void Cocinero::trabajar() {
    try {
        while (true) {
            Pedido pedido;

            {
                unique_lock<mutex> lock(cocina.mutexColaPedidos);
                cocina.cvPedidos.wait(lock, [&]() {
                    return !cocina.colaPedidos.empty() || !cocina.servidorActivo;
                });


                if (!cocina.servidorActivo && cocina.colaPedidos.empty()) {
                    logPedido("Nos vemoss 👋");
                    return;
                }

                pedido = cocina.colaPedidos.front();
                cocina.colaPedidos.pop();
            }

            auto logPaso = [&](const string& mensaje) {
                logPedido("Pedido #" + to_string(pedido.numeroPedido) + ": " + mensaje);
            };

            logPaso("\033[38;5;33mTomando pedido...\033[0m");
            sleep(2);

            logPaso("\033[38;5;208mCocinando...\033[0m");
            sleep(2);

            Combo combo(pedido.combo);
            logPaso(combo.getMensajeArmado());
            sleep(combo.getTiempoArmado());

            logPaso("\033[38;5;187mEmpaquetando...\033[0m");
            sleep(3);

            logPaso("\033[38;5;82mEntregado.\033[0m");
            sleep(2);

            if (pedido.clienteSocket != -1) {
                string respuesta = "Pedido #" + to_string(pedido.numeroPedido) + " entregado exitosamente.\n";
                send(pedido.clienteSocket, respuesta.c_str(), respuesta.size(), 0);
            }
        }
    } catch (const exception& e) {
        //cerr << "[Cocinero " << id << "] Error inesperado: " << e.what() << endl;
        return;
    }
}


string obtenerFechaActual() {
    using namespace chrono;

    auto ahora = system_clock::now();
    time_t tiempo = system_clock::to_time_t(ahora);
    tm* tm_local = localtime(&tiempo);

    ostringstream oss;
    oss << put_time(tm_local, "%Y-%m-%d");  // Solo fecha
    return oss.str();
}

void Cocinero::logPedido(const string& mensaje) {
    try {
        lock_guard<mutex> lock(mutexLog);

        auto now = system_clock::now();
        time_t now_time = system_clock::to_time_t(now);
        setenv("TZ", "America/Argentina/Buenos_Aires", 1);
        tzset();
        tm* tm_ptr = localtime(&now_time);

        stringstream headerStream;
        headerStream << "[" << put_time(tm_ptr, "%H:%M:%S") << "] "
                     << "[Cocinero " << this->id << "] ";

        string encabezado = headerStream.str();

        cout << encabezado << mensaje << endl;

        string mensajeLimpio;
        bool inEscape = false;
        for (char c : mensaje) {
            if (c == '\033') inEscape = true;
            else if (inEscape && c == 'm') inEscape = false;
            else if (!inEscape) mensajeLimpio += c;
        }

        string fileName = "logs_" + obtenerFechaActual() + ".txt";
        ofstream archivo(fileName, ios::app);
        if (archivo.is_open()) {
            archivo << encabezado << mensajeLimpio << endl;
        }
    } catch (const system_error& e) {
        cerr << "[Log] Error al escribir log: " << e.what() << endl;
    }
}

