CC = gcc
CFLAGS = -Wall -g
OBJECTS = main.o list.o pcb.o scheduler.o commands.o semaphore.o utils.o
HEADERS = list.h pcb.h scheduler.h commands.h semaphore.h utils.h

all: run

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

run: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o run

clean:
	del *.o run.exe
