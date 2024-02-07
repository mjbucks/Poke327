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
    struct map* world[WORLD_HEIGHT][WORLD_WIDTH] = {NULL};

    // Allocate and generate the initial map
    world[player_y][player_x] = malloc(sizeof(struct map));
    if (world[player_y][player_x] == NULL) {
        fprintf(stderr, "Failed to allocate memory for map\n");
        return 1;
    }
    world[player_y][player_x]->is_generated = 0;
    world[player_y][player_x]->path_east = 0;
    world[player_y][player_x]->path_west = 0;
    world[player_y][player_x]->path_north = 0;
    world[player_y][player_x]->path_south = 0;
    world[player_y][player_x]->x_pos = player_x;
    world[player_y][player_x]->y_pos = player_y;
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            world[player_y][player_x]->terrain[i][j] = ' ';
        }
    }


    // GENERATE AND PRINT CENTER MAP
    generate_map(world[player_y][player_x], world);
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
        } if (command == 'n') {
            if (player_y + 1 < WORLD_HEIGHT) {
                if (world[player_y+1][player_x] == NULL) {
                    world[player_y+1][player_x] = malloc(sizeof(struct map));
                    if (world[player_y+1][player_x] == NULL) {
                        fprintf(stderr, "Failed to allocate memory for map\n");
                        return 1;
                    }
                    world[player_y+1][player_x]->is_generated = 0;
                    world[player_y+1][player_x]->path_east = 0;
                    world[player_y+1][player_x]->path_west = 0;
                    world[player_y+1][player_x]->path_north = 0;
                    world[player_y+1][player_x]->path_south = world[player_y][player_x]->path_north;
                    world[player_y+1][player_x]->x_pos = player_x;
                    world[player_y+1][player_x]->y_pos = player_y+1;
                    for (int i = 0; i < MAP_HEIGHT; i++) {
                        for (int j = 0; j < MAP_WIDTH; j++) {
                            world[player_y+1][player_x]->terrain[i][j] = ' ';
                        }
                    }
                    printf("Path south in main: %d\n", world[player_y+1][player_x]->path_south);

                    generate_map(world[player_y+1][player_x], world);
                    world[player_y+1][player_x]->is_generated = 1;
                }
                player_y++;
            } else {
                printf("You can't go further north!\n");
            }
        } else if (command == 's') {
            if (player_y - 1 >= 0) {
                if (world[player_y-1][player_x] == NULL) {
                    world[player_y-1][player_x] = malloc(sizeof(struct map));
                    if (world[player_y-1][player_x] == NULL) {
                        fprintf(stderr, "Failed to allocate memory for map\n");
                        return 1;
                    }
                    world[player_y-1][player_x]->is_generated = 0;
                    world[player_y-1][player_x]->path_east = 0;
                    world[player_y-1][player_x]->path_west = 0;
                    world[player_y-1][player_x]->path_south = 0;
                    world[player_y-1][player_x]->path_north = world[player_y][player_x]->path_south;
                    world[player_y-1][player_x]->x_pos = player_x;
                    world[player_y-1][player_x]->y_pos = player_y-1;
                    for (int i = 0; i < MAP_HEIGHT; i++) {
                        for (int j = 0; j < MAP_WIDTH; j++) {
                            world[player_y-1][player_x]->terrain[i][j] = ' ';
                        }
                    }
                    printf("Path south in main: %d\n", world[player_y-1][player_x]->path_south);

                    generate_map(world[player_y-1][player_x], world);
                    world[player_y-1][player_x]->is_generated = 1;
                }
                player_y--;
            } else {
                printf("You can't go further south!\n");
            }
        } else if (command == 'e') {
            if (player_x + 1 < WORLD_WIDTH) {
                if (world[player_y][player_x+1] == NULL) {
                    world[player_y][player_x+1] = malloc(sizeof(struct map));
                    if (world[player_y][player_x+1] == NULL) {
                        fprintf(stderr, "Failed to allocate memory for map\n");
                        return 1;
                    }
                    world[player_y][player_x+1]->is_generated = 0;
                    world[player_y][player_x+1]->path_east = 0;
                    world[player_y][player_x+1]->path_north = 0;
                    world[player_y][player_x+1]->path_south = 0;
                    world[player_y][player_x+1]->path_west = world[player_y][player_x]->path_east;
                    world[player_y][player_x+1]->x_pos = player_x+1;
                    world[player_y][player_x+1]->y_pos = player_y;
                    for (int i = 0; i < MAP_HEIGHT; i++) {
                        for (int j = 0; j < MAP_WIDTH; j++) {
                            world[player_y][player_x+1]->terrain[i][j] = ' ';
                        }
                    }
                    printf("Path south in main: %d\n", world[player_y][player_x+1]->path_south);

                    generate_map(world[player_y][player_x+1], world);
                    world[player_y][player_x+1]->is_generated = 1;
                }
                player_x++;
            } else {
                printf("You can't go further east!\n");
            }
        } else if (command == 'w') {
            if (player_x - 1 >= 0) {
                if (world[player_y][player_x-1] == NULL) {
                    world[player_y][player_x-1] = malloc(sizeof(struct map));
                    if (world[player_y][player_x-1] == NULL) {
                        fprintf(stderr, "Failed to allocate memory for map\n");
                        return 1;
                    }
                    world[player_y][player_x-1]->is_generated = 0;
                    world[player_y][player_x-1]->path_west = 0;
                    world[player_y][player_x-1]->path_north = 0;
                    world[player_y][player_x-1]->path_south = 0;
                    world[player_y][player_x-1]->path_east = world[player_y][player_x]->path_west;
                    world[player_y][player_x-1]->x_pos = player_x-1;
                    world[player_y][player_x-1]->y_pos = player_y;
                    for (int i = 0; i < MAP_HEIGHT; i++) {
                        for (int j = 0; j < MAP_WIDTH; j++) {
                            world[player_y][player_x-1]->terrain[i][j] = ' ';
                        }
                    }
                    printf("Path  in main: %d\n", world[player_y][player_x-1]->path_east);

                    generate_map(world[player_y][player_x-1], world);
                    world[player_y][player_x-1]->is_generated = 1;
                }
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
            generate_map(world[player_y][player_x], world);
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
    for (int i = 0; i < WORLD_HEIGHT; i++) {
        for (int j = 0; j < WORLD_WIDTH; j++) {
            if (world[i][j] != NULL) {
                free(world[i][j]);
            }
        }
    }

    return 0;
}