/**************************************************************
Pontificia Universidad Javeriana
Facultad de Ingeniería - Ingeniería de Sistemas
Materia: Sistemas Operativos
Autores: 
  Samuel Jesus Mejía Díaz
  Jair Santiago Vargas Saenz
  Johan López Arciniegas
  Juan Diego Rojas Vargas
  Ismael Santiago Forero Romero
Fecha original: Febrero 2024
Fecha edición: 26 de mayo de 2025
Tema: Sistema de préstamo de libros - Solicitante
Fichero: solicitante_main.c
Descripción:
Proceso principal del solicitante, encargado de la interacción
con el usuario para realizar operaciones como préstamo,
devolución o renovación, y comunicarlas al receptor.
****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include "solicitante.h"

int main(int argc, char *argv[]) {
    char *archivoEntrada = NULL;
    char *pipeName = NULL;
    int opt;

    while ((opt = getopt(argc, argv, "i:p:")) != -1) {
        switch (opt) {
            case 'i': archivoEntrada = optarg; break;
            case 'p': pipeName = optarg; break;
            default:
                fprintf(stderr, "Uso: %s [-i archivo] -p pipe\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (!pipeName) {
        fprintf(stderr, "Debe especificar el nombre del pipe con -p\n");
        exit(EXIT_FAILURE);
    }

    if (archivoEntrada) leerArchivo(archivoEntrada, pipeName);
    else menuInteractivo(pipeName);

    return 0;
}
