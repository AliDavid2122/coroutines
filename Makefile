main: main.o coroutine.o
	gcc -o main main.o coroutine.o -g 

main.o: main.c
	gcc -c main.c -g

coroutine.o: coroutine.c coroutine.h
	gcc -c coroutine.c -g

clear:
	rm -f coroutine.o main.o main

run: main
	./main