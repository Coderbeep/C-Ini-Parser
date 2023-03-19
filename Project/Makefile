CC = gcc
CFLAGS = -Wall -Wextra -g
# switch -Wall prints out the warning messages

ifeq ($(OS), Windows_NT)
	RM = del
else
	RM = rm
endif


all: main

main: main.o readfile.o
	$(CC) $(CFLAGS) -o main main.o readfile.o  

# It actually is: 'gcc (FLAGS) -o main main.o readfile.o'
# main stands for the name of the executable file

main.o: main.c readfile.h
	$(CC) $(CFLAGS) -c main.c

readfile.o: readfile.c readfile.h
	$(CC) $(CFLAGS) -c readfile.c

clean:
	del -f *.o
	