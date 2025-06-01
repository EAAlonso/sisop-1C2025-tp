#pragma once
#include <string>
#include "../headers/json.hpp"
#include <fstream>
#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

void simularClientesDesdeArchivo(const string& archivo, int puerto, const string host);
