CC=gcc
CFLAGS=-I -Wall -Wshadow -pthread
DEPS = 

%.o: %.c $(DEPS)
	$(CC) -c -o  $@ $< $(CFLAGS)

practica1: practica1.o
	gcc -o practica1 practica1.o	
clean:
	rm -f *.o practica1 ./-lpthread
