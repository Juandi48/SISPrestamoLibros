
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
Fichero: receptor.h
Descripción:
Archivo de cabecera con las definiciones y estructuras
utilizadas por el proceso receptor para la gestión de
memoria compartida, semáforos y operaciones del sistema.
****************************************************************/
#ifndef RECEPTOR_H
#define RECEPTOR_H

#include <semaphore.h>
#include <pthread.h>

#define BUFFER_SIZE 10
#define MAX_LIBROS 100

typedef struct {
    char operacion;
    char nombreLibro[100];
    int isbn;
    char pipeRespuesta[100];
} Solicitud;

typedef struct {
    Solicitud buffer[BUFFER_SIZE];
    int in, out, count;
    pthread_mutex_t mutex;
    sem_t empty, full;
} BufferCircular;

typedef struct {
    char nombreLibro[100];
    int isbn;
    int ejemplares;
    char estado[10][2];
    char fecha[10][20];
} Libro;

extern BufferCircular buffer;
extern Libro libros[MAX_LIBROS];
extern int totalLibros;
extern int receptorActivo;
extern int verbose;
extern char archivoSalida[100];

void cargarBD(const char *nombreArchivo);
void guardarBD(const char *nombreArchivo);
void imprimirReporte();
void actualizarEstado(const Solicitud *sol);
void procesarSolicitud(Solicitud sol);
void *hiloAuxiliar1(void *arg);
void *hiloConsola(void *arg);

#endif
