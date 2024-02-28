#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>

#include "map.h"
#include "pc.h"
#include "heap.h"

#define WORLD_WIDTH 401
#define WORLD_HEIGHT 401
#define MAP_WIDTH 80
#define MAP_HEIGHT 21
#define MAX_TRAINERS 10

typedef struct character_turn {
  heap_node_t *hn;
  int32_t time;
  int id;
  int player_type;
} character_turn_t;

static int32_t path_cmp(const void *key, const void *with) {
    return ((character_turn_t *) key)->time - ((character_turn_t *) with)->time;
}

int main(int argc, char *argv[])
{
    int numtrainers = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--numtrainers") == 0 && i + 1 < argc) {
            if (atoi(argv[i + 1]) > MAX_TRAINERS) {
                printf("Number of trainers cannot exceed %d\n", MAX_TRAINERS);
                printf("Setting number of trainers to %d\n", MAX_TRAINERS);
                break;
            }
            else{
                numtrainers = atoi(argv[i + 1]); // convert the next argument to an integer
                break;
            }
        }
    }

    if(numtrainers <= 0){
        numtrainers = MAX_TRAINERS;
    }

    printf("Number of trainers: %d\n", numtrainers);

    srand(time(NULL));
    // ALLOCATE MEMORY FOR WORLD
    struct map* world[WORLD_HEIGHT][WORLD_WIDTH] = {NULL};

    // ALLOCATE MEMORY FOR PLAYER CHARACTER
    struct pc* player = malloc(sizeof(struct pc));
    player->x = 200;
    player->y = 200;

    // Allocate and generate the initial map
    world[player->y][player->x] = malloc(sizeof(struct map));
    if (world[player->y][player->x] == NULL) {
        fprintf(stderr, "Failed to allocate memory for map\n");
        return 1;
    }
    world[player->y][player->x]->is_generated = 0;
    world[player->y][player->x]->path_east = 0;
    world[player->y][player->x]->path_west = 0;
    world[player->y][player->x]->path_north = 0;
    world[player->y][player->x]->path_south = 0;
    world[player->y][player->x]->x_pos = player->x;
    world[player->y][player->x]->y_pos = player->y;
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            world[player->y][player->x]->terrain[i][j] = ' ';
        }
    }
    
    // GENERATE AND PRINT CENTER MAP
    generate_map(world[player->y][player->x], world, player, numtrainers);
    world[player->y][player->x]->is_generated = 1;

    for(int i = 0; i < MAP_HEIGHT; i++){
            for(int j = 0; j < MAP_WIDTH; j++){
                printf("%c ", world[player->y][player->x]->terrain_with_npcs[i][j]);
            }
            printf("\n");
    }

    // HEAP FOR MAIN GAME LOOP
    heap_t heap;
    heap_init(&heap, path_cmp, NULL);

    character_turn_t characters[numtrainers + 1], *cur;

    characters[0].time = 0;
    characters[0].id = 0;
    characters[0].player_type = 1;
    characters[0].hn = heap_insert(&heap, &characters[0]);


    for(int i = 0; i < numtrainers; i++){
        characters[i+1].time = 0;
        characters[i+1].id = i + 1;
        characters[i+1].player_type = world[player->map_y][player->map_x]->npcs[i].player_type;
        characters[i+1].hn = heap_insert(&heap, &characters[i+1]);
    }

    while((cur = heap_peek_min(&heap))){
        printf("%d", cur->time);
        printf("%d", cur->id);
        printf("%d", cur->player_type);
        break;
    }



    heap_delete(&heap);



        // FREE ALL MEMORY USED IN THE WORLD
    for (int i = 0; i < WORLD_HEIGHT; i++) {
        for (int j = 0; j < WORLD_WIDTH; j++) {
            if (world[i][j] != NULL) {
                free(world[i][j]);
            }
        }
    }

    // FREE PC MEMORY
    free(player);

    return 0;



    // OLD MAIN GAME LOOP!
    // char command;
    // while (1) {
    //     printf("Current position: (%d, %d)\n", player->x-200, player->y-200);
    //     printf("Enter a command (n/s/e/w/f (fly) to move, q to quit): ");
    //     scanf(" %c", &command);

    //     if (command == 'q') {
    //         break;
    //     } if (command == 'n') {
    //         if (player->y + 1 < WORLD_HEIGHT) {
    //             if (world[player->y+1][player->x] == NULL) {
    //                 world[player->y+1][player->x] = malloc(sizeof(struct map));
    //                 if (world[player->y+1][player->x] == NULL) {
    //                     fprintf(stderr, "Failed to allocate memory for map\n");
    //                     return 1;
    //                 }
    //                 world[player->y+1][player->x]->is_generated = 0;
    //                 world[player->y+1][player->x]->path_east = 0;
    //                 world[player->y+1][player->x]->path_west = 0;
    //                 world[player->y+1][player->x]->path_north = 0;
    //                 world[player->y+1][player->x]->path_south = world[player->y][player->x]->path_north;
    //                 world[player->y+1][player->x]->x_pos = player->x;
    //                 world[player->y+1][player->x]->y_pos = player->y+1;
    //                 for (int i = 0; i < MAP_HEIGHT; i++) {
    //                     for (int j = 0; j < MAP_WIDTH; j++) {
    //                         world[player->y+1][player->x]->terrain[i][j] = ' ';
    //                     }
    //                 }
    //                 generate_map(world[player->y+1][player->x], world, player, numtrainers);
    //                 world[player->y+1][player->x]->is_generated = 1;
    //             }
    //             player->y++;
    //         } else {
    //             printf("You can't go further north!\n");
    //         }
    //     } else if (command == 's') {
    //         if (player->y - 1 >= 0) {
    //             if (world[player->y-1][player->x] == NULL) {
    //                 world[player->y-1][player->x] = malloc(sizeof(struct map));
    //                 if (world[player->y-1][player->x] == NULL) {
    //                     fprintf(stderr, "Failed to allocate memory for map\n");
    //                     return 1;
    //                 }
    //                 world[player->y-1][player->x]->is_generated = 0;
    //                 world[player->y-1][player->x]->path_east = 0;
    //                 world[player->y-1][player->x]->path_west = 0;
    //                 world[player->y-1][player->x]->path_south = 0;
    //                 world[player->y-1][player->x]->path_north = world[player->y][player->x]->path_south;
    //                 world[player->y-1][player->x]->x_pos = player->x;
    //                 world[player->y-1][player->x]->y_pos = player->y-1;
    //                 for (int i = 0; i < MAP_HEIGHT; i++) {
    //                     for (int j = 0; j < MAP_WIDTH; j++) {
    //                         world[player->y-1][player->x]->terrain[i][j] = ' ';
    //                     }
    //                 }
    //                 generate_map(world[player->y-1][player->x], world, player, numtrainers);
    //                 world[player->y-1][player->x]->is_generated = 1;
    //             }
    //             player->y--;
    //         } else {
    //             printf("You can't go further south!\n");
    //         }
    //     } else if (command == 'e') {
    //         if (player->x + 1 < WORLD_WIDTH) {
    //             if (world[player->y][player->x+1] == NULL) {
    //                 world[player->y][player->x+1] = malloc(sizeof(struct map));
    //                 if (world[player->y][player->x+1] == NULL) {
    //                     fprintf(stderr, "Failed to allocate memory for map\n");
    //                     return 1;
    //                 }
    //                 world[player->y][player->x+1]->is_generated = 0;
    //                 world[player->y][player->x+1]->path_east = 0;
    //                 world[player->y][player->x+1]->path_north = 0;
    //                 world[player->y][player->x+1]->path_south = 0;
    //                 world[player->y][player->x+1]->path_west = world[player->y][player->x]->path_east;
    //                 world[player->y][player->x+1]->x_pos = player->x+1;
    //                 world[player->y][player->x+1]->y_pos = player->y;
    //                 for (int i = 0; i < MAP_HEIGHT; i++) {
    //                     for (int j = 0; j < MAP_WIDTH; j++) {
    //                         world[player->y][player->x+1]->terrain[i][j] = ' ';
    //                     }
    //                 }
    //                 generate_map(world[player->y][player->x+1], world, player, numtrainers);
    //                 world[player->y][player->x+1]->is_generated = 1;
    //             }
    //             player->x++;
    //         } else {
    //             printf("You can't go further east!\n");
    //         }
    //     } else if (command == 'w') {
    //         if (player->x - 1 >= 0) {
    //             if (world[player->y][player->x-1] == NULL) {
    //                 world[player->y][player->x-1] = malloc(sizeof(struct map));
    //                 if (world[player->y][player->x-1] == NULL) {
    //                     fprintf(stderr, "Failed to allocate memory for map\n");
    //                     return 1;
    //                 }
    //                 world[player->y][player->x-1]->is_generated = 0;
    //                 world[player->y][player->x-1]->path_west = 0;
    //                 world[player->y][player->x-1]->path_north = 0;
    //                 world[player->y][player->x-1]->path_south = 0;
    //                 world[player->y][player->x-1]->path_east = world[player->y][player->x]->path_west;
    //                 world[player->y][player->x-1]->x_pos = player->x-1;
    //                 world[player->y][player->x-1]->y_pos = player->y;
    //                 for (int i = 0; i < MAP_HEIGHT; i++) {
    //                     for (int j = 0; j < MAP_WIDTH; j++) {
    //                         world[player->y][player->x-1]->terrain[i][j] = ' ';
    //                     }
    //                 }
    //                 generate_map(world[player->y][player->x-1], world, player, numtrainers);
    //                 world[player->y][player->x-1]->is_generated = 1;
    //             }
    //             player->x--;
    //         } else {
    //             printf("You can't go further west!\n");
    //         }
    //     } else if (command == 'f') {
    //         int new_x, new_y;
    //         printf("Enter the x and y coordinates of the destination: ");
    //         scanf("%d %d", &new_x, &new_y);
    //         new_x += 200;
    //         new_y += 200;
    //         if (new_x >= 0 && new_x < WORLD_WIDTH && new_y >= 0 && new_y < WORLD_HEIGHT) {
    //             if (world[new_y][new_x] == NULL) {
    //                 world[new_y][new_x] = malloc(sizeof(struct map));
    //                 if (world[new_y][new_x] == NULL) {
    //                     fprintf(stderr, "Failed to allocate memory for map\n");
    //                     return 1;
    //                 }
    //                 world[new_y][new_x]->is_generated = 0;
    //                 world[new_y][new_x]->path_west = 0;
    //                 world[new_y][new_x]->path_north = 0;
    //                 world[new_y][new_x]->path_south = 0;
    //                 world[new_y][new_x]->path_east = 0;
    //                 world[new_y][new_x]->x_pos = new_x;
    //                 world[new_y][new_x]->y_pos = new_y;
    //                 for (int i = 0; i < MAP_HEIGHT; i++) {
    //                     for (int j = 0; j < MAP_WIDTH; j++) {
    //                         world[new_y][new_x]->terrain[i][j] = ' ';
    //                     }
    //                 }
    //                 generate_map(world[new_y][new_x], world, player, numtrainers);
    //                 world[new_y][new_x]->is_generated = 1;
    //             }
    //             player->x--;
    //             player->x = new_x;
    //             player->y = new_y;
    //         } else {
    //             printf("Invalid coordinates! Please try again.\n");
    //         }
    //     }

    //     if (world[player->y][player->x]->is_generated == 0) {
    //         if (world[player->y][player->x] == NULL) {
    //             fprintf(stderr, "Failed to allocate memory for world[%d][%d]\n", player->y, player->x);
    //             return 1;
    //         }
    //         generate_map(world[player->y][player->x], world, player, numtrainers);
    //         world[player->y][player->x]->is_generated = 1;
    //     }

    //     for(int i = 0; i < MAP_HEIGHT; i++){
    //         for(int j = 0; j < MAP_WIDTH; j++){
    //             printf("%c ", world[player->y][player->x]->terrain[i][j]);
    //         }
    //         printf("\n");
    //     }

    // }
    
    // PRINT HIKER AND RIVAL MAPS
    // printf("Hiker map: \n");
    // for (int i = 0; i < 21; i++) {
    //     for (int j = 0; j < 80; j++) {
    //         if (world[player->y][player->x]->hiker_costmap[i][j] == INT16_MAX){
    //             printf("   ");
    //         }
    //         else{
    //             printf("%02d ", abs(world[player->y][player->x]->hiker_costmap[i][j]% 100)); // Adjust to print the hiker cost map
    //         }
    //     }
    //     printf("\n");
    // }
    // printf("Rival map: \n");
    // for (int i = 0; i < 21; i++) {
    //     for (int j = 0; j < 80; j++) {
    //         if (world[player->y][player->x]->rival_costmap[i][j] == INT16_MAX){
    //             printf("   ");
    //         }
    //         else{
    //             printf("%02d ", abs(world[player->y][player->x]->rival_costmap[i][j]% 100)); // Adjust to print the hiker cost map
    //         }
    //     }
    //     printf("\n");
    // }
}