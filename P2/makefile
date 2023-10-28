CC=gcc
CFLAGS=-I -Wall -Wshadow -pthread -lm
DEPS = 

%.o: %.c $(DEPS)
	$(CC) -c -o  $@ $< $(CFLAGS)

cyclictestURJC: cyclictestURJC.o
	gcc -o cyclictestURJC cyclictestURJC.o -lm -pthread
clean:
	rm -f *.o cyclictestURJC ./-lpthread cyclictestURJC.csv
