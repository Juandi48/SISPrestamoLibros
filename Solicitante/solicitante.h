// solicitante.h

#ifndef SOLICITANTE_H
#define SOLICITANTE_H

#define MAX_LIBRO 100

typedef struct {
    char operacion;              // 'D', 'R', 'P', 'Q'
    char nombreLibro[MAX_LIBRO];
    int isbn;
} Solicitud;

// Prototipos de funciones
void enviarSolicitud(const char *pipeName, Solicitud solicitud);
void recibirRespuesta();
void leerArchivo(const char *archivo, const char *pipeName);
void menuInteractivo(const char *pipeName);

#endif
