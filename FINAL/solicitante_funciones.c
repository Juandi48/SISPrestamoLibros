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
Fichero: solicitante_funciones.c
Descripción:
Implementación de funciones de soporte para el proceso
solicitante. Incluye validaciones, construcción de mensajes,
y manejo de entrada/salida del usuario.
****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <ctype.h>
#include "solicitante.h"

void enviarSolicitud(const char *pipeName, Solicitud solicitud) {
    int fd = open(pipeName, O_WRONLY);
    if (fd == -1) {
        perror("No se pudo abrir el pipe del receptor");
        exit(EXIT_FAILURE);
    }
    write(fd, &solicitud, sizeof(Solicitud));
    close(fd);
}

void recibirRespuesta(const char *pipeRespuesta) {
    char buffer[256];
    int fd = open(pipeRespuesta, O_RDONLY);
    if (fd == -1) {
        perror("No se pudo abrir el pipe de respuesta");
        exit(EXIT_FAILURE);
    }

    int n = read(fd, buffer, sizeof(buffer) - 1);
    if (n > 0) {
        buffer[n] = '\0';
        printf("Respuesta del receptor: %s\n\n", buffer);
    } else {
        printf("No se recibió respuesta del receptor.\n\n");
    }

    close(fd);
}

void leerArchivo(const char *archivo, const char *pipeName) {
    FILE *f = fopen(archivo, "r");
    if (!f) {
        perror("Error al abrir el archivo de entrada");
        exit(EXIT_FAILURE);
    }

    char linea[256];
    char pipeRespuesta[100];
    sprintf(pipeRespuesta, "/tmp/resp_%d", getpid());
    mkfifo(pipeRespuesta, 0666);

    while (fgets(linea, sizeof(linea), f)) {
        Solicitud solicitud;
        char op;
        if (sscanf(linea, " %c , %99[^,] , %d", &op, solicitud.nombreLibro, &solicitud.isbn) < 3)
            continue;

        solicitud.operacion = toupper(op);
        strcpy(solicitud.pipeRespuesta, pipeRespuesta);
        enviarSolicitud(pipeName, solicitud);

        if (op == 'Q') break;

        recibirRespuesta(pipeRespuesta);
    }

    fclose(f);
    unlink(pipeRespuesta);
}

void menuInteractivo(const char *pipeName) {
    char pipeRespuesta[100];
    sprintf(pipeRespuesta, "/tmp/resp_%d", getpid());
    mkfifo(pipeRespuesta, 0666);

    while (1) {
        Solicitud solicitud;
        char buffer[MAX_LIBRO];
        char op;

        do {
            printf("Seleccione operacion (P=Prestar, D=Devolver, R=Renovar, Q=Salir): ");
            scanf(" %c", &op);
            op = toupper(op);
            if (strchr("PDRQ", op) == NULL) {
                printf("Operacion invalida. Intente de nuevo.\n");
            }
        } while (strchr("PDRQ", op) == NULL);

        solicitud.operacion = op;

        if (op == 'Q') {
            strcpy(solicitud.nombreLibro, "Salir");
            solicitud.isbn = 0;
            strcpy(solicitud.pipeRespuesta, pipeRespuesta);
            enviarSolicitud(pipeName, solicitud);
            break;
        }

        while (1) {
            printf("Nombre del libro: ");
            scanf(" %[^\n]", buffer);
            if (strlen(buffer) > 0) break;
            printf("El nombre no puede estar vacío.\n");
        }
        strncpy(solicitud.nombreLibro, buffer, MAX_LIBRO);

        while (1) {
            printf("ISBN: ");
            if (scanf("%d", &solicitud.isbn) == 1 && solicitud.isbn > 0) break;
            else {
                printf("ISBN inválido. Intente nuevamente.\n");
                while (getchar() != '\n');
            }
        }

        strcpy(solicitud.pipeRespuesta, pipeRespuesta);
        enviarSolicitud(pipeName, solicitud);
        recibirRespuesta(pipeRespuesta);
    }

    unlink(pipeRespuesta);
}

