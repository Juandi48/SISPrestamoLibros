CC = gcc
CFLAGS = -Wall -pthread

all: solicitante receptor

solicitante: solicitante_main.o solicitante_funciones.o
	$(CC) $(CFLAGS) -o solicitante solicitante_main.o solicitante_funciones.o

receptor: receptor.o receptor_funciones.o
	$(CC) $(CFLAGS) -o receptor receptor.o receptor_funciones.o

solicitante_main.o: solicitante_main.c solicitante.h
	$(CC) $(CFLAGS) -c solicitante_main.c

solicitante_funciones.o: solicitante_funciones.c solicitante.h
	$(CC) $(CFLAGS) -c solicitante_funciones.c

receptor.o: receptor.c receptor.h
	$(CC) $(CFLAGS) -c receptor.c

receptor_funciones.o: receptor_funciones.c receptor.h
	$(CC) $(CFLAGS) -c receptor_funciones.c

clean:
	rm -f *.o solicitante receptor
