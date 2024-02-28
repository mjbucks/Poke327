main: MapGeneration.o main.o heap.o
	gcc MapGeneration.o main.o heap.o -o main

MapGeneration.o: MapGeneration.c map.h npc.h
	gcc -Wall -Werror -g MapGeneration.c -c

main.o: main.c map.h pc.h npc.h
	gcc -Wall -Werror -g main.c -c

heap.o: heap.c heap.h
	gcc -Wall -Werror -g heap.c -c

clean:
	rm -f main *.o
