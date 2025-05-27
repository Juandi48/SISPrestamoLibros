CC = gcc
CFLAGS = -Wall -pthread
OBJS_SOL = solicitante_main.o solicitante_funciones.o
OBJS_REC = receptor.o receptor_funciones.o

all: solicitante receptor

solicitante: $(OBJS_SOL)
	$(CC) $(CFLAGS) -o solicitante $(OBJS_SOL)

receptor: $(OBJS_REC)
	$(CC) $(CFLAGS) -o receptor $(OBJS_REC)

solicitante_main.o: solicitante_main.c solicitante.h
	$(CC) $(CFLAGS) -c solicitante_main.c

solicitante_funciones.o: solicitante_funciones.c solicitante.h
	$(CC) $(CFLAGS) -c solicitante_funciones.c

receptor.o: receptor.c receptor.h
	$(CC) $(CFLAGS) -c receptor.c

receptor_funciones.o: receptor_funciones.c receptor.h
	$(CC) $(CFLAGS) -c receptor_funciones.c

clean:
	rm -f *.o solicitante receptor /tmp/resp_*
