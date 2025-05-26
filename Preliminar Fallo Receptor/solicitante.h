#ifndef SOLICITANTE_H
#define SOLICITANTE_H

#define MAX_LIBRO 100

typedef struct {
    char operacion;
    char nombreLibro[MAX_LIBRO];
    int isbn;
    char pipeRespuesta[100];
} Solicitud;

void enviarSolicitud(const char *pipeName, Solicitud solicitud);
void recibirRespuesta(const char *pipeRespuesta);
void leerArchivo(const char *archivo, const char *pipeName);
void menuInteractivo(const char *pipeName);

#endif
