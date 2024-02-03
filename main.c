#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "map.h"

#define WORLD_WIDTH 401
#define WORLD_HEIGHT 401
#define MAP_WIDTH 80
#define MAP_HEIGHT 21

int main(int argc, char *argv[])
{
    srand(time(NULL));
    int player_x = 200;
    int player_y = 200;

    // THIS INITIALIZES THE WORLD
    struct map *world[WORLD_HEIGHT][WORLD_WIDTH] = {NULL};

    // THIS INITIALIZES THE CENTER OF THE WORLD
    world[player_y][player_x] = malloc(sizeof(struct map));
    if (world[player_y][player_x] == NULL) {
        fprintf(stderr, "Failed to allocate memory for world[%d][%d]\n", player_y, player_x);
        return 1;
    }

    // GENERATE AND PRINT CENTER MAP
    generate_map(world[player_y][player_x]);
    for(int i = 0; i < MAP_HEIGHT; i++){
            for(int j = 0; j < MAP_WIDTH; j++){
                printf("%c ", world[player_y][player_x]->terrain[i][j]);
            }
            printf("\n");
    }

    // START GAME!
    char command;
    while (1) {
        printf("Current position: (%d, %d)\n", player_x-200, player_y-200);
        printf("Enter a command (n/s/e/w to move, q to quit): ");
        scanf(" %c", &command);

        if (command == 'q') {
            break;
        } else if (command == 'n') {
            player_y++;
        } else if (command == 's') {
            player_y--;
        } else if (command == 'e') {
            player_x++;
        } else if (command == 'w') {
            player_x--;
        }

        if (world[player_y][player_x] == NULL) {
            world[player_y][player_x] = malloc(sizeof(struct map));
            if (world[player_y][player_x] == NULL) {
                fprintf(stderr, "Failed to allocate memory for world[%d][%d]\n", player_y, player_x);
                return 1;
            }
            generate_map(world[player_y][player_x]);
        }


        // THIS DISPLAYS THE MAP THE USER IS CURRENTLY ON
        for(int i = 0; i < MAP_HEIGHT; i++){
            for(int j = 0; j < MAP_WIDTH; j++){
                printf("%c ", world[player_y][player_x]->terrain[i][j]);
            }
            printf("\n");
        }

    }

    // FREE ALL MEMORY USED IN THE WORLD
    for (int i = 0; i < WORLD_HEIGHT; i++) {
        for (int j = 0; j < WORLD_WIDTH; j++) {
            if (world[i][j] != NULL) {
                free(world[i][j]);
            }
        }
    }

    return 0;
}
