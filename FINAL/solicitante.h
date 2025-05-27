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
Fichero: solicitante.h
Descripción:
Archivo de cabecera con constantes, estructuras y definiciones
compartidas por los procesos solicitante y receptor del sistema
de préstamo de libros.
****************************************************************/

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
