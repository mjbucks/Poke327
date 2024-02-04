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

    // ALLOCATE MEMORY FOR WORLD
    struct map *world_memory_pool = malloc(WORLD_HEIGHT * WORLD_WIDTH * sizeof(struct map));
    if (world_memory_pool == NULL) {
        fprintf(stderr, "Failed to allocate memory for world\n");
        return 1;
    }
    
    // THIS INITIALIZES THE WORLD
    struct map *world[WORLD_HEIGHT][WORLD_WIDTH];
    for (int i = 0; i < WORLD_HEIGHT; i++) {
        for (int j = 0; j < WORLD_WIDTH; j++) {
            world[i][j] = &world_memory_pool[i * WORLD_WIDTH + j];
            world[i][j]->is_generated = 0;
        }
    }

    // GENERATE AND PRINT CENTER MAP
    generate_map(world[player_y][player_x]);
    world[player_y][player_x]->is_generated = 1;
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
            if (player_y + 1 < WORLD_HEIGHT) {
                player_y++;
            } else {
                printf("You can't go further north!\n");
            }
        } else if (command == 's') {
            if (player_y - 1 >= 0) {
                player_y--;
            } else {
                printf("You can't go further south!\n");
            }
        } else if (command == 'e') {
            if (player_x + 1 < WORLD_WIDTH) {
                player_x++;
            } else {
                printf("You can't go further east!\n");
            }
        } else if (command == 'w') {
            if (player_x - 1 >= 0) {
                player_x--;
            } else {
                printf("You can't go further west!\n");
            }
        } else if (command == 'f') {
            int new_x, new_y;
            printf("Enter the x and y coordinates of the destination: ");
            scanf("%d %d", &new_x, &new_y);
            new_x += 200;
            new_y += 200;
            if (new_x >= 0 && new_x < WORLD_WIDTH && new_y >= 0 && new_y < WORLD_HEIGHT) {
                player_x = new_x;
                player_y = new_y;
            } else {
                printf("Invalid coordinates! Please try again.\n");
            }
        }

        if (world[player_y][player_x]->is_generated == 0) {
            if (world[player_y][player_x] == NULL) {
                fprintf(stderr, "Failed to allocate memory for world[%d][%d]\n", player_y, player_x);
                return 1;
            }
            generate_map(world[player_y][player_x]);
            world[player_y][player_x]->is_generated = 1;
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
    free(world_memory_pool);

    return 0;
}
