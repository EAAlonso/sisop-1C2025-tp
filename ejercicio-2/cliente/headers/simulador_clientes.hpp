#pragma once
#include <string>
#include "../headers/json.hpp"
#include <fstream>
#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>

using namespace std;

void clienteEscuchador(const string& combo, int puerto, const string& host);
void simularClientesDesdeArchivo(const string& archivo, int puerto, const string& host);
