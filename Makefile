main: MapGeneration.o main.o heap.o
	gcc MapGeneration.o main.o heap.o -o main -lncurses

MapGeneration.o: MapGeneration.c map.h npc.h pc.h heap.h
	gcc -Wall -Werror -g MapGeneration.c -c

main.o: main.c map.h pc.h npc.h heap.h
	gcc -Wall -Werror -g main.c -c -lncurses

heap.o: heap.c heap.h
	gcc -Wall -Werror -g heap.c -c

clean:
	rm -f main *.o
