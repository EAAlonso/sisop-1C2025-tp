# Trabajo Práctico 2 - Coordinación de Tareas con Procesos y Memoria Compartida
# GRUPO 12

## Descripción

Para poder generar input el proceso padre funciona como menú, se crean los “combos” usando las opciones del menú y al ir creando los pedidos se puede ver en los logs (/logs/) como se van pasando entre cada proceso. Para salir se usa la opción “3” del menú.

En el header “cocina.hpp” se encuentran los define para parametrizar cantidad de procesos hijos y tiempo de preparación de cada combo (Con este tiempo simulamos trabajo y probamos concurrencia).

En el header “colaMemCompartida.hpp” se encuentra el define que parametriza la cantidad de pedidos máxima.

## 

## Ejecución

1. Servidor
./bin/ejercicio_1

##

## Importante antes de ejecutar:

1. Ejecutar el comando "make" en la carpeta de servidor y cliente
2. Tal vez sea necesario instalar g++ (sudo apt install g++)
3. Si no le deja instalar g++, correr el comando: sudo apt update --allow-releaseinfo-change

##
