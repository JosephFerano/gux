CFLAGS=-g -Wall -Wextra -pedantic -O0

all:main mylib.so

mylib.so: mylib.o
	$(CC) -shared -Wl,-soname,lib$@.1 mylib.o -o lib$@.1.1.1
	ln -sf lib$@.1.1.1 lib$@

main: clean
	$(CC) $(CFLAGS) main.c -o main

clean:
	$(RM) main mylib.o libmylib.so*
