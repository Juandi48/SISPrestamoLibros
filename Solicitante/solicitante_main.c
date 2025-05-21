// solicitante_main.c

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
            case 'i':
                archivoEntrada = optarg;
                break;
            case 'p':
                pipeName = optarg;
                break;
            default:
                fprintf(stderr, "Uso: %s [-i archivo] -p pipe\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (!pipeName) {
        fprintf(stderr, "Debe especificar el nombre del pipe con -p\n");
        exit(EXIT_FAILURE);
    }

    if (archivoEntrada) {
        leerArchivo(archivoEntrada, pipeName);
    } else {
        menuInteractivo(pipeName);
    }

    printf("Proceso solicitante finalizado.\n");
    return 0;
}
