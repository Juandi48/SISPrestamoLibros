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
Tema: Sistema de préstamo de libros - Receptor
Fichero: receptor_funciones.c
Descripción:
Implementación de funciones auxiliares para el proceso
receptor del sistema de préstamo de libros. Manejo de
validaciones, registros y operaciones sobre el archivo.
****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include "receptor.h"

void cargarBD(const char *nombreArchivo) {
    FILE *f = fopen(nombreArchivo, "r");
    if (!f) {
        perror("No se pudo abrir el archivo de BD");
        exit(EXIT_FAILURE);
    }

    char linea[256];
    while (fgets(linea, sizeof(linea), f)) {
        if (strlen(linea) <= 1) continue;

        Libro libro;
        if (sscanf(linea, " %[^\n,], %d, %d", libro.nombreLibro, &libro.isbn, &libro.ejemplares) != 3) continue;

        for (int i = 0; i < libro.ejemplares; i++) {
            if (!fgets(linea, sizeof(linea), f)) break;
            int ejemplar;
            if (sscanf(linea, " %d, %1s, %19s", &ejemplar, libro.estado[i], libro.fecha[i]) != 3) {
                strcpy(libro.estado[i], "D");
                strcpy(libro.fecha[i], "00-00-0000");
            }
        }

        libros[totalLibros++] = libro;
    }

    fclose(f);
}

void guardarBD(const char *nombreArchivo) {
    FILE *f = fopen(nombreArchivo, "w");
    if (!f) {
        perror("Error al guardar BD");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < totalLibros; i++) {
        fprintf(f, "%s, %d, %d\n", libros[i].nombreLibro, libros[i].isbn, libros[i].ejemplares);
        for (int j = 0; j < libros[i].ejemplares; j++) {
            fprintf(f, "%d, %s, %s\n", j + 1, libros[i].estado[j], libros[i].fecha[j]);
        }
        fprintf(f, "\n");
    }

    fclose(f);
}

void imprimirReporte() {
    printf("Status, Nombre del Libro, ISBN, ejemplar, fecha\n");
    for (int i = 0; i < totalLibros; i++) {
        for (int j = 0; j < libros[i].ejemplares; j++) {
            printf("%s, %s, %d, %d, %s\n",
                   libros[i].estado[j],
                   libros[i].nombreLibro,
                   libros[i].isbn,
                   j + 1,
                   libros[i].fecha[j]);
        }
    }
}

void procesarSolicitud(Solicitud sol) {
    int respfd = open(sol.pipeRespuesta, O_WRONLY);
    if (respfd == -1) return;

    if (verbose) {
        printf("[RP] Recibido: %c | Libro: %s | ISBN: %d\n", sol.operacion, sol.nombreLibro, sol.isbn);
    }

    int libroEncontrado = -1;
    for (int i = 0; i < totalLibros; i++) {
        if (libros[i].isbn == sol.isbn) {
            libroEncontrado = i;
            break;
        }
    }

    if (libroEncontrado == -1) {
        write(respfd, "Error: libro no encontrado", 27);
        close(respfd);
        return;
    }

    Libro *libro = &libros[libroEncontrado];

    if (strcmp(libro->nombreLibro, sol.nombreLibro) != 0) {
        write(respfd, "Error: nombre no coincide con ISBN", 34);
        close(respfd);
        return;
    }

    if (sol.operacion == 'P') {
        for (int j = 0; j < libro->ejemplares; j++) {
            if (strcmp(libro->estado[j], "D") == 0) {
                strcpy(libro->estado[j], "P");
                time_t t = time(NULL);
                struct tm *tm = localtime(&t);
                strftime(libro->fecha[j], sizeof(libro->fecha[j]), "%d-%m-%Y", tm);
                write(respfd, "Prestamo exitoso", 17);
                if (strlen(archivoSalida) > 0) guardarBD(archivoSalida);
                close(respfd);
                return;
            }
        }
        write(respfd, "No disponible", 13);
        close(respfd);
        return;
    }

    if (sol.operacion == 'D' || sol.operacion == 'R') {
        sem_wait(&buffer.empty);
        pthread_mutex_lock(&buffer.mutex);
        buffer.buffer[buffer.in] = sol;
        buffer.in = (buffer.in + 1) % BUFFER_SIZE;
        buffer.count++;
        pthread_mutex_unlock(&buffer.mutex);
        sem_post(&buffer.full);

        if (sol.operacion == 'D') {
            write(respfd, "Solicitud de devolucion recibida", 32);
        } else {
            write(respfd, "Solicitud de renovacion recibida", 32);
        }

        close(respfd);
        return;
    }

    write(respfd, "Operacion invalida", 19);
    close(respfd);
}

void *hiloAuxiliar1(void *arg) {
    while (receptorActivo) {
        sem_wait(&buffer.full);
        pthread_mutex_lock(&buffer.mutex);
        Solicitud sol = buffer.buffer[buffer.out];
        buffer.out = (buffer.out + 1) % BUFFER_SIZE;
        buffer.count--;
        pthread_mutex_unlock(&buffer.mutex);
        sem_post(&buffer.empty);

        int libroEncontrado = -1;
        for (int i = 0; i < totalLibros; i++) {
            if (libros[i].isbn == sol.isbn) {
                libroEncontrado = i;
                break;
            }
        }

        if (libroEncontrado == -1) continue;

        Libro *libro = &libros[libroEncontrado];
        int modificado = 0;

        for (int j = 0; j < libro->ejemplares; j++) {
            if (strcmp(libro->estado[j], "P") == 0) {
                if (sol.operacion == 'D') {
                    strcpy(libro->estado[j], "D");
                } else if (sol.operacion == 'R') {
                    // Solo actualiza la fecha
                }
                time_t t = time(NULL);
                struct tm *tm = localtime(&t);
                strftime(libro->fecha[j], sizeof(libro->fecha[j]), "%d-%m-%Y", tm);
                modificado = 1;
                break;
            }
        }

        if (modificado && strlen(archivoSalida) > 0) {
            guardarBD(archivoSalida);
        }
    }

    return NULL;
}

void *hiloConsola(void *arg) {
    char comando[10];
    while (receptorActivo) {
        printf("(R) Ver reporte | (S) Salir\n> ");
        fflush(stdout);
        if (fgets(comando, sizeof(comando), stdin)) {
            if (comando[0] == 'r' || comando[0] == 'R') {
                imprimirReporte();
            } else if (comando[0] == 's' || comando[0] == 'S') {
                receptorActivo = 0;
            }
        }
    }
    return NULL;
}

