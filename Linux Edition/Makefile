CC = gcc
CFLAGS = -Wall -Wextra -g -Wno-unused-parameter
# switch -Wall prints out the warning messages

ifeq ($(OS), Windows_NT)
	RM = del
else
	RM = rm
endif

all: main

main: main.o
	$(CC) $(CFLAGS) -o main main.o 

# It actually is: 'gcc (FLAGS) -o main main.o readfile.o'
# main stands for the name of the executable file

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

clean:
	rm -f *.o
