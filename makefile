CC = gcc
CFLAGS = -Wall -g
OBJECTS = main.o list.o pcb.o scheduler.o commands.o semaphore.o utils.o
HEADERS = list.h pcb.h scheduler.h commands.h semaphore.h utils.h

all: my_program

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

my_program: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o my_program

clean:
	del *.o my_program.exe
