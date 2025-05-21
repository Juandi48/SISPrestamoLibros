// solicitante_funciones.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "solicitante.h"

void enviarSolicitud(const char *pipeName, Solicitud solicitud) {
    int fd = open(pipeName, O_WRONLY);
    if (fd == -1) {
        perror("Error al abrir el pipe para escribir");
        exit(EXIT_FAILURE);
    }

    if (write(fd, &solicitud, sizeof(Solicitud)) == -1) {
        perror("Error al escribir en el pipe");
        close(fd);
        exit(EXIT_FAILURE);
    }

    close(fd);
}

void recibirRespuesta() {
    // Esta función es simulada. En implementación final, usar pipe de respuesta o mecanismo IPC real.
    printf("Esperando respuesta del Receptor...\n");
    sleep(1);
    printf("Respuesta recibida (simulada)\n\n");
}

void leerArchivo(const char *archivo, const char *pipeName) {
    FILE *f = fopen(archivo, "r");
    if (!f) {
        perror("No se pudo abrir el archivo de entrada");
        exit(EXIT_FAILURE);
    }

    char linea[256];
    while (fgets(linea, sizeof(linea), f)) {
        Solicitud solicitud;
        char op;
        if (sscanf(linea, " %c , %99[^,] , %d", &op, solicitud.nombreLibro, &solicitud.isbn) < 3) {
            fprintf(stderr, "Línea mal formada: %s", linea);
            continue;
        }
        solicitud.operacion = op;
        enviarSolicitud(pipeName, solicitud);
        if (op == 'Q') break;
        recibirRespuesta();
    }

    fclose(f);
}

void menuInteractivo(const char *pipeName) {
    while (1) {
        Solicitud solicitud;
        char buffer[MAX_LIBRO];

        printf("Seleccione operación:\nD = Devolver\nR = Renovar\nP = Prestar\nQ = Salir\n> ");
        scanf(" %c", &solicitud.operacion);

        if (solicitud.operacion == 'Q') {
            strcpy(solicitud.nombreLibro, "Salir");
            solicitud.isbn = 0;
            enviarSolicitud(pipeName, solicitud);
            break;
        }

        printf("Nombre del libro: ");
        scanf(" %[^\n]", buffer);
        strncpy(solicitud.nombreLibro, buffer, MAX_LIBRO);

        printf("ISBN: ");
        scanf("%d", &solicitud.isbn);

        enviarSolicitud(pipeName, solicitud);
        recibirRespuesta();
    }
}
