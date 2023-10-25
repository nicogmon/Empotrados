CC=gcc
CFLAGS=-I -Wall -Wshadow -pthread
DEPS = 

%.o: %.c $(DEPS)
	$(CC) -c -o  $@ $< $(CFLAGS)

cyclictestURJC: cyclictestURJC.o
	gcc -o cyclictestURJC cyclictestURJC.o	
clean:
	rm -f *.o cyclictestURJC ./-lpthread cyclictestURJC.csv