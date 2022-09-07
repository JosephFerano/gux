P=main
OBJECTS=
CFLAGS=-g -Wall -Wextra -pedantic -O0
LDLIBS=
CC=gcc
RM=rm -vf

$(P): clean
	$(CC) $(CFLAGS) $(P).c -o $(P)

clean:
	$(RM) $(P)

run:$(P)
	./$(P)

all:$(P)
