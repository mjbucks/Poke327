main: MapGeneration.o main.o
	gcc MapGeneration.o main.o -o main

MapGeneration.o: MapGeneration.c map.h
	gcc -Wall -Werror -g MapGeneration.c -c

main.o: main.c map.h
	gcc -Wall -Werror -g main.c -c

clean:
	rm -f main *.o
