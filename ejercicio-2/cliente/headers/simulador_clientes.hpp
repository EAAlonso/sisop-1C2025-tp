#pragma once
#include <string>
#include "../headers/json.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <fstream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>

using namespace std;
using nlohmann::json;

void enviarPedido(const string& combo, int sock);
string leerComboDesdeArchivo(const string& rutaArchivo);
