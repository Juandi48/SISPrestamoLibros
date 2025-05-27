
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
Fichero: receptor.c
Descripción:
Contiene la lógica principal del proceso receptor,
que gestiona la recepción de solicitudes de préstamo,
devolución y renovación de libros, procesando mensajes
a través de memoria compartida y semáforos.
****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/stat.h>
#include "receptor.h"

BufferCircular buffer;
Libro libros[MAX_LIBROS];
int totalLibros = 0;
int receptorActivo = 1;
int verbose = 0;
char archivoSalida[100] = "";

int main(int argc, char *argv[]) {
    char pipeNombre[100] = "", archivoBD[100] = "";

    if (argc < 5) {
        fprintf(stderr, "Uso: %s -p <pipe> -f <bd.txt> [-s salida.txt] [-v]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-p")) strcpy(pipeNombre, argv[++i]);
        else if (!strcmp(argv[i], "-f")) strcpy(archivoBD, argv[++i]);
        else if (!strcmp(argv[i], "-v")) verbose = 1;
        else if (!strcmp(argv[i], "-s")) strcpy(archivoSalida, argv[++i]);
    }

    cargarBD(archivoBD);

    unlink(pipeNombre);
    if (mkfifo(pipeNombre, 0666) == -1) {
        perror("Error creando el FIFO");
        exit(EXIT_FAILURE);
    }

    buffer.in = buffer.out = buffer.count = 0;
    pthread_mutex_init(&buffer.mutex, NULL);
    sem_init(&buffer.empty, 0, BUFFER_SIZE);
    sem_init(&buffer.full, 0, 0);

    pthread_t tid1, tid2;
    pthread_create(&tid1, NULL, hiloAuxiliar1, NULL);
    pthread_create(&tid2, NULL, hiloConsola, NULL);

    int fd = open(pipeNombre, O_RDONLY);
    if (fd == -1) {
        perror("Error abriendo el pipe");
        exit(EXIT_FAILURE);
    }

    while (receptorActivo) {
        Solicitud sol;
        int bytes = read(fd, &sol, sizeof(Solicitud));

        if (bytes == sizeof(Solicitud)) {
            if (sol.operacion == 'Q') {
                int respfd = open(sol.pipeRespuesta, O_WRONLY);
                if (respfd != -1) {
                    write(respfd, "Solicitante finalizado", 23);
                    close(respfd);
                }
                continue;
            }

            procesarSolicitud(sol);
        } else {
            usleep(100000); // prevenir CPU alta si no hay datos
        }
    }

    close(fd);
    if (strlen(archivoSalida) > 0) guardarBD(archivoSalida);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    unlink(pipeNombre);

    return 0;
}


