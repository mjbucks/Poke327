#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <unistd.h>
#include <ncurses.h>


#include "map.h"
#include "pc.h"
#include "heap.h"

#define WORLD_WIDTH 401
#define WORLD_HEIGHT 401
#define MAP_WIDTH 80
#define MAP_HEIGHT 21
#define PLAYER_TYPE_PC 1
#define PLAYER_TYPE_HIKER 2
#define PLAYER_TYPE_RIVAL 3
#define PLAYER_TYPE_PACER 4
#define PLAYER_TYPE_WANDERER 5
#define PLAYER_TYPE_SENTRY 6
#define PLAYER_TYPE_EXPLORER 7
#define DIR_NORTH 0
#define DIR_SOUTH 2
#define DIR_EAST 1
#define DIR_WEST 3
#define MAX_TRAINERS 50
#define DEFAULT_TRAINERS 10

// CHARACTER TURN STRUCT
typedef struct character_turn {
  heap_node_t *hn;
  int32_t time;
  int id;
  int player_type;
  int dir;
} character_turn_t;

// CHARACTER TURN COMPARE
static int32_t path_cmp(const void *key, const void *with) {
    int32_t time_difference = ((character_turn_t *)key)->time - ((character_turn_t *)with)->time;
    if (time_difference != 0) {
        return time_difference;
    } else {
        return ((character_turn_t *)key)->id - ((character_turn_t *)with)->id;
    }
}

// PRINT MAP USING NCURSES
void print_map(struct pc* player, struct map* world[WORLD_HEIGHT][WORLD_WIDTH]) {
    move(1, 0);

    for (int row = 0; row < MAP_HEIGHT; row++) {
        for (int col = 0; col < MAP_WIDTH; col++) {
            printw("%c", world[player->y][player->x]->terrain_with_npcs[row][col]);
        }
        printw("\n");
    }
    refresh();
}

// MAIN METHOD
int main(int argc, char *argv[])
{
    int numtrainers = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--numtrainers") == 0 && i + 1 < argc) {
            if (atoi(argv[i + 1]) > MAX_TRAINERS) {
                printf("Number of trainers cannot exceed %d\n", MAX_TRAINERS);
                printf("Setting number of trainers to %d\n", DEFAULT_TRAINERS);
                break;
            }
            else{
                numtrainers = atoi(argv[i + 1]);
                break;
            }
        }
    }

    if(numtrainers <= 0){
        numtrainers = DEFAULT_TRAINERS;
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
    
    // Initialize ncurses
    initscr();

    // Disable line buffering and echoing
    raw();

    // Enable special keys recognition
    keypad(stdscr, TRUE);

    // GENERATE AND PRINT CENTER MAP
    generate_map(world[player->y][player->x], world, player, numtrainers);
    world[player->y][player->x]->is_generated = 1;
    print_map(player, world);

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
        characters[i+1].player_type = world[player->y][player->x]->npcs[i].player_type;
        characters[i+1].dir = rand()%4;
        characters[i+1].hn = heap_insert(&heap, &characters[i+1]);
    }

    char ch;
    while((cur = heap_peek_min(&heap))){
        // printf("%d\n", cur->time);
        // printf("%d\n", cur->id);
        // printf("%d\n", cur->player_type);
        int min_cost = INT16_MAX;
        int cur_xpos = world[player->y][player->x]->npcs[cur->id - 1].x_pos;
        int cur_ypos = world[player->y][player->x]->npcs[cur->id - 1].y_pos;
        int next_x = 0;
        int next_y = 0;

        if(cur->player_type != PLAYER_TYPE_HIKER && cur->player_type != PLAYER_TYPE_RIVAL && cur->player_type != PLAYER_TYPE_PACER &&
        cur->player_type != PLAYER_TYPE_WANDERER && cur->player_type != PLAYER_TYPE_EXPLORER && cur->player_type != PLAYER_TYPE_PC){
            characters[cur->id].time = cur->time + 20;
            characters[cur->id].id = cur->id;
            characters[cur->id].dir = cur->dir;
            characters[cur->id].player_type = cur->player_type;
            characters[cur->id].hn = heap_insert(&heap, &characters[cur->id]);
            heap_remove_min(&heap);
            continue;
        } else if (cur->player_type == PLAYER_TYPE_HIKER) {
            for (int i = -1; i <= 1; i++){
                for (int j = -1; j <= 1; j++){
                    if (world[player->y][player->x]->hiker_costmap[cur_ypos + i][cur_xpos + j] < min_cost && 
                    (world[player->y][player->x]->terrain_with_npcs[cur_ypos + i][cur_xpos + j] != '@' && world[player->y][player->x]->terrain_with_npcs[cur_ypos + i][cur_xpos + j] != 'h' && 
                    world[player->y][player->x]->terrain_with_npcs[cur_ypos + i][cur_xpos + j] != 'r' && world[player->y][player->x]->terrain_with_npcs[cur_ypos + i][cur_xpos + j] != 'p' && 
                    world[player->y][player->x]->terrain_with_npcs[cur_ypos + i][cur_xpos + j] != 'w' && world[player->y][player->x]->terrain_with_npcs[cur_ypos + i][cur_xpos + j] != 's' && 
                    world[player->y][player->x]->terrain_with_npcs[cur_ypos + i][cur_xpos + j] != 'e')) {

                        min_cost = world[player->y][player->x]->hiker_costmap[cur_ypos + i][cur_xpos + j];
                        next_y = cur_ypos + i;
                        next_x = cur_xpos + j;

                    } else if (world[player->y][player->x]->terrain_with_npcs[cur_ypos + i][cur_xpos + j] == '@') {
                        cur->time += 100;
                    }
                }
            }
            cur->time += abs(world[player->y][player->x]->hiker_costmap[cur_ypos][cur_xpos] - min_cost);
            world[player->y][player->x]->npcs[cur->id - 1].x_pos = next_x;
            world[player->y][player->x]->npcs[cur->id - 1].y_pos = next_y;
            world[player->y][player->x]->terrain_with_npcs[next_y][next_x] = world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos];
            world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos] = world[player->y][player->x]->terrain[cur_ypos][cur_xpos];
        } else if (cur->player_type == PLAYER_TYPE_RIVAL) {
            for (int i = -1; i <= 1; i++){
                for (int j = -1; j <= 1; j++){
                    if (world[player->y][player->x]->rival_costmap[cur_ypos + i][cur_xpos + j] < min_cost && 
                    (world[player->y][player->x]->terrain_with_npcs[cur_ypos + i][cur_xpos + j] != '@' && world[player->y][player->x]->terrain_with_npcs[cur_ypos + i][cur_xpos + j] != 'h' && 
                    world[player->y][player->x]->terrain_with_npcs[cur_ypos + i][cur_xpos + j] != 'r' && world[player->y][player->x]->terrain_with_npcs[cur_ypos + i][cur_xpos + j] != 'p' && 
                    world[player->y][player->x]->terrain_with_npcs[cur_ypos + i][cur_xpos + j] != 'w' && world[player->y][player->x]->terrain_with_npcs[cur_ypos + i][cur_xpos + j] != 's' && 
                    world[player->y][player->x]->terrain_with_npcs[cur_ypos + i][cur_xpos + j] != 'e')) {

                        min_cost = world[player->y][player->x]->rival_costmap[cur_ypos + i][cur_xpos + j];
                        next_y = cur_ypos + i;
                        next_x = cur_xpos + j;

                    } else if (world[player->y][player->x]->terrain_with_npcs[cur_ypos + i][cur_xpos + j] == '@') {
                        cur->time += 100;
                    }
                }
            }
            cur->time += abs(world[player->y][player->x]->rival_costmap[cur_ypos][cur_xpos] - min_cost);
            world[player->y][player->x]->npcs[cur->id - 1].x_pos = next_x;
            world[player->y][player->x]->npcs[cur->id - 1].y_pos = next_y;
            world[player->y][player->x]->terrain_with_npcs[next_y][next_x] = world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos];
            world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos] = world[player->y][player->x]->terrain[cur_ypos][cur_xpos];
        } else if (cur->player_type == PLAYER_TYPE_PACER) {
            if(cur->dir == DIR_NORTH){ // DIR IS NORTH
                if (world[player->y][player->x]->rival_costmap[cur_ypos + 1][cur_xpos] < INT16_MAX && 
                (world[player->y][player->x]->terrain_with_npcs[cur_ypos + 1][cur_xpos] != '@' && world[player->y][player->x]->terrain_with_npcs[cur_ypos + 1][cur_xpos] != 'h' && 
                    world[player->y][player->x]->terrain_with_npcs[cur_ypos + 1][cur_xpos] != 'r' && world[player->y][player->x]->terrain_with_npcs[cur_ypos + 1][cur_xpos] != 'p' && 
                    world[player->y][player->x]->terrain_with_npcs[cur_ypos + 1][cur_xpos] != 'w' && world[player->y][player->x]->terrain_with_npcs[cur_ypos + 1][cur_xpos] != 's' && 
                    world[player->y][player->x]->terrain_with_npcs[cur_ypos + 1][cur_xpos] != 'e')){
                        next_y = cur_ypos + 1;
                        next_x = cur_xpos;
                        cur->time += abs(world[player->y][player->x]->rival_costmap[cur_ypos][cur_xpos] - world[player->y][player->x]->rival_costmap[next_y][next_x]);
                        world[player->y][player->x]->npcs[cur->id - 1].x_pos = next_x;
                        world[player->y][player->x]->npcs[cur->id - 1].y_pos = next_y;
                        world[player->y][player->x]->terrain_with_npcs[next_y][next_x] = world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos];
                        world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos] = world[player->y][player->x]->terrain[cur_ypos][cur_xpos];

                } else{
                    cur->time += 30;
                    // Switch to south
                    cur->dir = DIR_SOUTH;
                }
            } else if(cur->dir == DIR_SOUTH){ // DIR IS SOUTH
                if (world[player->y][player->x]->rival_costmap[cur_ypos - 1][cur_xpos] < INT16_MAX && 
                (world[player->y][player->x]->terrain_with_npcs[cur_ypos - 1][cur_xpos] != '@' && world[player->y][player->x]->terrain_with_npcs[cur_ypos - 1][cur_xpos] != 'h' && 
                    world[player->y][player->x]->terrain_with_npcs[cur_ypos - 1][cur_xpos] != 'r' && world[player->y][player->x]->terrain_with_npcs[cur_ypos - 1][cur_xpos] != 'p' && 
                    world[player->y][player->x]->terrain_with_npcs[cur_ypos - 1][cur_xpos] != 'w' && world[player->y][player->x]->terrain_with_npcs[cur_ypos - 1][cur_xpos] != 's' && 
                    world[player->y][player->x]->terrain_with_npcs[cur_ypos - 1][cur_xpos] != 'e')){
                        next_y = cur_ypos - 1;
                        next_x = cur_xpos;
                        cur->time += abs(world[player->y][player->x]->rival_costmap[cur_ypos][cur_xpos] - world[player->y][player->x]->rival_costmap[next_y][next_x]);
                        world[player->y][player->x]->npcs[cur->id - 1].x_pos = next_x;
                        world[player->y][player->x]->npcs[cur->id - 1].y_pos = next_y;
                        world[player->y][player->x]->terrain_with_npcs[next_y][next_x] = world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos];
                        world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos] = world[player->y][player->x]->terrain[cur_ypos][cur_xpos];

                } else{
                    cur->time += 30;
                    // Switch to north
                    cur->dir = DIR_NORTH;
                }
            } else if(cur->dir == DIR_EAST){ // DIR IS EAST
                if (world[player->y][player->x]->rival_costmap[cur_ypos][cur_xpos + 1] < INT16_MAX && 
                (world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos + 1] != '@' && world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos + 1] != 'h' && 
                    world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos + 1] != 'r' && world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos + 1] != 'p' && 
                    world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos + 1] != 'w' && world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos + 1] != 's' && 
                    world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos + 1] != 'e')){
                        next_y = cur_ypos;
                        next_x = cur_xpos + 1;
                        cur->time += abs(world[player->y][player->x]->rival_costmap[cur_ypos][cur_xpos] - world[player->y][player->x]->rival_costmap[next_y][next_x]);
                        world[player->y][player->x]->npcs[cur->id - 1].x_pos = next_x;
                        world[player->y][player->x]->npcs[cur->id - 1].y_pos = next_y;
                        world[player->y][player->x]->terrain_with_npcs[next_y][next_x] = world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos];
                        world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos] = world[player->y][player->x]->terrain[cur_ypos][cur_xpos];

                } else{
                    cur->time += 30;
                    // Switch to west
                    cur->dir = DIR_WEST;
                }
            } else{
                if (world[player->y][player->x]->rival_costmap[cur_ypos][cur_xpos - 1] < INT16_MAX && 
                (world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos - 1] != '@' && world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos - 1] != 'h' && 
                    world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos - 1] != 'r' && world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos - 1] != 'p' && 
                    world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos - 1] != 'w' && world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos - 1] != 's' && 
                    world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos - 1] != 'e')){
                        next_y = cur_ypos;
                        next_x = cur_xpos - 1;
                        cur->time += abs(world[player->y][player->x]->rival_costmap[cur_ypos][cur_xpos] - world[player->y][player->x]->rival_costmap[next_y][next_x]);
                        world[player->y][player->x]->npcs[cur->id - 1].x_pos = next_x;
                        world[player->y][player->x]->npcs[cur->id - 1].y_pos = next_y;
                        world[player->y][player->x]->terrain_with_npcs[next_y][next_x] = world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos];
                        world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos] = world[player->y][player->x]->terrain[cur_ypos][cur_xpos];

                } else{
                    cur->time += 30;
                    // Switch to eest
                    cur->dir = DIR_EAST;
                }
            }
        } else if (cur->player_type == PLAYER_TYPE_WANDERER){
            if(cur->dir == DIR_NORTH){ // DIR IS NORTH
                if (world[player->y][player->x]->rival_costmap[cur_ypos + 1][cur_xpos] < INT16_MAX && 
                (world[player->y][player->x]->terrain_with_npcs[cur_ypos + 1][cur_xpos] != '@' && world[player->y][player->x]->terrain_with_npcs[cur_ypos + 1][cur_xpos] != 'h' && 
                    world[player->y][player->x]->terrain_with_npcs[cur_ypos + 1][cur_xpos] != 'r' && world[player->y][player->x]->terrain_with_npcs[cur_ypos + 1][cur_xpos] != 'p' && 
                    world[player->y][player->x]->terrain_with_npcs[cur_ypos + 1][cur_xpos] != 'w' && world[player->y][player->x]->terrain_with_npcs[cur_ypos + 1][cur_xpos] != 's' && 
                    world[player->y][player->x]->terrain_with_npcs[cur_ypos + 1][cur_xpos] != 'e') && 
                    world[player->y][player->x]->terrain[cur_ypos + 1][cur_xpos] == world[player->y][player->x]->terrain[cur_ypos][cur_xpos]){
                        next_y = cur_ypos + 1;
                        next_x = cur_xpos;
                        cur->time += abs(world[player->y][player->x]->rival_costmap[cur_ypos][cur_xpos] - world[player->y][player->x]->rival_costmap[next_y][next_x]);
                        world[player->y][player->x]->npcs[cur->id - 1].x_pos = next_x;
                        world[player->y][player->x]->npcs[cur->id - 1].y_pos = next_y;
                        world[player->y][player->x]->terrain_with_npcs[next_y][next_x] = world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos];
                        world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos] = world[player->y][player->x]->terrain[cur_ypos][cur_xpos];

                } else{
                    cur->time += 30;
                    // Switch to random dir
                    cur->dir = rand()%4;
                }
            } else if (cur->dir == DIR_SOUTH) {
                if (world[player->y][player->x]->rival_costmap[cur_ypos - 1][cur_xpos] < INT16_MAX && 
                (world[player->y][player->x]->terrain_with_npcs[cur_ypos - 1][cur_xpos] != '@' && world[player->y][player->x]->terrain_with_npcs[cur_ypos - 1][cur_xpos] != 'h' && 
                    world[player->y][player->x]->terrain_with_npcs[cur_ypos - 1][cur_xpos] != 'r' && world[player->y][player->x]->terrain_with_npcs[cur_ypos - 1][cur_xpos] != 'p' && 
                    world[player->y][player->x]->terrain_with_npcs[cur_ypos - 1][cur_xpos] != 'w' && world[player->y][player->x]->terrain_with_npcs[cur_ypos - 1][cur_xpos] != 's' && 
                    world[player->y][player->x]->terrain_with_npcs[cur_ypos - 1][cur_xpos] != 'e') && 
                    world[player->y][player->x]->terrain[cur_ypos - 1][cur_xpos] == world[player->y][player->x]->terrain[cur_ypos][cur_xpos]){
                        next_y = cur_ypos - 1;
                        next_x = cur_xpos;
                        cur->time += abs(world[player->y][player->x]->rival_costmap[cur_ypos][cur_xpos] - world[player->y][player->x]->rival_costmap[next_y][next_x]);
                        world[player->y][player->x]->npcs[cur->id - 1].x_pos = next_x;
                        world[player->y][player->x]->npcs[cur->id - 1].y_pos = next_y;
                        world[player->y][player->x]->terrain_with_npcs[next_y][next_x] = world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos];
                        world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos] = world[player->y][player->x]->terrain[cur_ypos][cur_xpos];

                } else{
                    cur->time += 30;
                    // Switch to random dir
                    cur->dir = rand()%4;
                }
            } else if (cur->dir == DIR_EAST) {
                if (world[player->y][player->x]->rival_costmap[cur_ypos][cur_xpos + 1] < INT16_MAX && 
                (world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos + 1] != '@' && world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos + 1] != 'h' && 
                    world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos + 1] != 'r' && world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos + 1] != 'p' && 
                    world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos + 1] != 'w' && world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos + 1] != 's' && 
                    world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos + 1] != 'e') && 
                    world[player->y][player->x]->terrain[cur_ypos][cur_xpos + 1] == world[player->y][player->x]->terrain[cur_ypos][cur_xpos]){
                        next_y = cur_ypos;
                        next_x = cur_xpos + 1;
                        cur->time += abs(world[player->y][player->x]->rival_costmap[cur_ypos][cur_xpos] - world[player->y][player->x]->rival_costmap[next_y][next_x]);
                        world[player->y][player->x]->npcs[cur->id - 1].x_pos = next_x;
                        world[player->y][player->x]->npcs[cur->id - 1].y_pos = next_y;
                        world[player->y][player->x]->terrain_with_npcs[next_y][next_x] = world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos];
                        world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos] = world[player->y][player->x]->terrain[cur_ypos][cur_xpos];

                } else{
                    cur->time += 30;
                    // Switch to random dir
                    cur->dir = rand()%4;
                }
            } else {
                if (world[player->y][player->x]->rival_costmap[cur_ypos][cur_xpos - 1] < INT16_MAX && 
                (world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos - 1] != '@' && world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos - 1] != 'h' && 
                    world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos - 1] != 'r' && world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos - 1] != 'p' && 
                    world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos - 1] != 'w' && world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos - 1] != 's' && 
                    world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos - 1] != 'e') && 
                    world[player->y][player->x]->terrain[cur_ypos][cur_xpos - 1] == world[player->y][player->x]->terrain[cur_ypos][cur_xpos]){
                        next_y = cur_ypos;
                        next_x = cur_xpos - 1;
                        cur->time += abs(world[player->y][player->x]->rival_costmap[cur_ypos][cur_xpos] - world[player->y][player->x]->rival_costmap[next_y][next_x]);
                        world[player->y][player->x]->npcs[cur->id - 1].x_pos = next_x;
                        world[player->y][player->x]->npcs[cur->id - 1].y_pos = next_y;
                        world[player->y][player->x]->terrain_with_npcs[next_y][next_x] = world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos];
                        world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos] = world[player->y][player->x]->terrain[cur_ypos][cur_xpos];

                } else{
                    cur->time += 30;
                    // Switch to random dir
                    cur->dir = rand()%4;
                }
            }
        } else if (cur->player_type == PLAYER_TYPE_EXPLORER){
            if(cur->dir == DIR_NORTH){ // DIR IS NORTH
                if (world[player->y][player->x]->rival_costmap[cur_ypos + 1][cur_xpos] < INT16_MAX && 
                (world[player->y][player->x]->terrain_with_npcs[cur_ypos + 1][cur_xpos] != '@' && world[player->y][player->x]->terrain_with_npcs[cur_ypos + 1][cur_xpos] != 'h' && 
                    world[player->y][player->x]->terrain_with_npcs[cur_ypos + 1][cur_xpos] != 'r' && world[player->y][player->x]->terrain_with_npcs[cur_ypos + 1][cur_xpos] != 'p' && 
                    world[player->y][player->x]->terrain_with_npcs[cur_ypos + 1][cur_xpos] != 'w' && world[player->y][player->x]->terrain_with_npcs[cur_ypos + 1][cur_xpos] != 's' && 
                    world[player->y][player->x]->terrain_with_npcs[cur_ypos + 1][cur_xpos] != 'e')){
                        next_y = cur_ypos + 1;
                        next_x = cur_xpos;
                        cur->time += abs(world[player->y][player->x]->rival_costmap[cur_ypos][cur_xpos] - world[player->y][player->x]->rival_costmap[next_y][next_x]);
                        world[player->y][player->x]->npcs[cur->id - 1].x_pos = next_x;
                        world[player->y][player->x]->npcs[cur->id - 1].y_pos = next_y;
                        world[player->y][player->x]->terrain_with_npcs[next_y][next_x] = world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos];
                        world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos] = world[player->y][player->x]->terrain[cur_ypos][cur_xpos];

                } else{
                    cur->time += 30;
                    // Switch to random dir
                    cur->dir = rand()%4;
                }
            } else if (cur->dir == DIR_SOUTH) {
                if (world[player->y][player->x]->rival_costmap[cur_ypos - 1][cur_xpos] < INT16_MAX && 
                (world[player->y][player->x]->terrain_with_npcs[cur_ypos - 1][cur_xpos] != '@' && world[player->y][player->x]->terrain_with_npcs[cur_ypos - 1][cur_xpos] != 'h' && 
                    world[player->y][player->x]->terrain_with_npcs[cur_ypos - 1][cur_xpos] != 'r' && world[player->y][player->x]->terrain_with_npcs[cur_ypos - 1][cur_xpos] != 'p' && 
                    world[player->y][player->x]->terrain_with_npcs[cur_ypos - 1][cur_xpos] != 'w' && world[player->y][player->x]->terrain_with_npcs[cur_ypos - 1][cur_xpos] != 's' && 
                    world[player->y][player->x]->terrain_with_npcs[cur_ypos - 1][cur_xpos] != 'e')){
                        next_y = cur_ypos - 1;
                        next_x = cur_xpos;
                        cur->time += abs(world[player->y][player->x]->rival_costmap[cur_ypos][cur_xpos] - world[player->y][player->x]->rival_costmap[next_y][next_x]);
                        world[player->y][player->x]->npcs[cur->id - 1].x_pos = next_x;
                        world[player->y][player->x]->npcs[cur->id - 1].y_pos = next_y;
                        world[player->y][player->x]->terrain_with_npcs[next_y][next_x] = world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos];
                        world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos] = world[player->y][player->x]->terrain[cur_ypos][cur_xpos];

                } else{
                    cur->time += 30;
                    // Switch to random dir
                    cur->dir = rand()%4;
                }
            } else if (cur->dir == DIR_EAST) {
                if (world[player->y][player->x]->rival_costmap[cur_ypos][cur_xpos + 1] < INT16_MAX && 
                (world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos + 1] != '@' && world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos + 1] != 'h' && 
                    world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos + 1] != 'r' && world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos + 1] != 'p' && 
                    world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos + 1] != 'w' && world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos + 1] != 's' && 
                    world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos + 1] != 'e')){
                        next_y = cur_ypos;
                        next_x = cur_xpos + 1;
                        cur->time += abs(world[player->y][player->x]->rival_costmap[cur_ypos][cur_xpos] - world[player->y][player->x]->rival_costmap[next_y][next_x]);
                        world[player->y][player->x]->npcs[cur->id - 1].x_pos = next_x;
                        world[player->y][player->x]->npcs[cur->id - 1].y_pos = next_y;
                        world[player->y][player->x]->terrain_with_npcs[next_y][next_x] = world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos];
                        world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos] = world[player->y][player->x]->terrain[cur_ypos][cur_xpos];

                } else{
                    cur->time += 30;
                    // Switch to random dir
                    cur->dir = rand()%4;
                }
            } else {
                if (world[player->y][player->x]->rival_costmap[cur_ypos][cur_xpos - 1] < INT16_MAX && 
                (world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos - 1] != '@' && world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos - 1] != 'h' && 
                    world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos - 1] != 'r' && world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos - 1] != 'p' && 
                    world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos - 1] != 'w' && world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos - 1] != 's' && 
                    world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos - 1] != 'e')){
                        next_y = cur_ypos;
                        next_x = cur_xpos - 1;
                        cur->time += abs(world[player->y][player->x]->rival_costmap[cur_ypos][cur_xpos] - world[player->y][player->x]->rival_costmap[next_y][next_x]);
                        world[player->y][player->x]->npcs[cur->id - 1].x_pos = next_x;
                        world[player->y][player->x]->npcs[cur->id - 1].y_pos = next_y;
                        world[player->y][player->x]->terrain_with_npcs[next_y][next_x] = world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos];
                        world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos] = world[player->y][player->x]->terrain[cur_ypos][cur_xpos];

                } else{
                    cur->time += 30;
                    // Switch to random dir
                    cur->dir = rand()%4;
                }
            }
        } else if (cur->player_type == PLAYER_TYPE_PC){
            while ((ch = getch()) != 'Q') { // Press 'Q' to quit
                switch(ch) {
                    case '7': case 'y':
                        if (world[player->y][player->x]->npcs[cur->id - 1].x_pos > 1 && world[player->y][player->x]->npcs[cur->id - 1].y_pos > 1 && world[player->y][player->x]->rival_costmap[cur_ypos - 1][cur_xpos - 1] < INT16_MAX &&
                        (world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos - 1] != '@' && world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos - 1] != 'h' && 
                        world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos - 1] != 'r' && world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos - 1] != 'p' && 
                        world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos - 1] != 'w' && world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos - 1] != 's' && 
                        world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos - 1] != 'e')) {
                            next_x--;
                            next_y--; 

                            cur->time += abs(world[player->y][player->x]->rival_costmap[cur_ypos][cur_xpos] - world[player->y][player->x]->rival_costmap[next_y][next_x]);
                            world[player->y][player->x]->npcs[cur->id - 1].x_pos = next_x;
                            world[player->y][player->x]->npcs[cur->id - 1].y_pos = next_y;
                            world[player->y][player->x]->terrain_with_npcs[next_y][next_x] = world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos];
                            world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos] = world[player->y][player->x]->terrain[cur_ypos][cur_xpos];
                        } else if (){

                        } // Move upper left
                        break;
                    case '8': case 'k':
                        if (world[player->y][player->x]->npcs[cur->id - 1].y_pos > 1 && world[player->y][player->x]->rival_costmap[cur_ypos - 1][cur_xpos] < INT16_MAX){
                            next_y--;
                        } // Move up
                        break;
                    case '9': case 'u':
                        if (world[player->y][player->x]->npcs[cur->id - 1].y_pos > 1 && world[player->y][player->x]->npcs[cur->id - 1].x_pos < 79 && world[player->y][player->x]->rival_costmap[cur_ypos - 1][cur_xpos + 1] < INT16_MAX) {
                            next_y--;
                            next_x++;
                        } // Move upper right
                        break;
                    case '6': case 'l':
                        if (world[player->y][player->x]->npcs[cur->id - 1].x_pos < 79 && world[player->y][player->x]->rival_costmap[cur_ypos][cur_xpos + 1] < INT16_MAX){
                            next_x++;
                        } // Move right
                        break;
                    case '3': case 'n':
                        if (world[player->y][player->x]->npcs[cur->id - 1].y_pos < 20 && world[player->y][player->x]->npcs[cur->id - 1].x_pos < 79 && world[player->y][player->x]->rival_costmap[cur_ypos + 1][cur_xpos + 1] < INT16_MAX) {
                            next_x++;
                            next_y++;
                        } // Move lower right
                        break;
                    case '2': case 'j':
                        if (world[player->y][player->x]->npcs[cur->id - 1].y_pos < 20 && world[player->y][player->x]->rival_costmap[cur_ypos + 1][cur_xpos] < INT16_MAX){
                            next_y++;
                        } // Move down
                        break;
                    case '1': case 'b':
                        if (world[player->y][player->x]->npcs[cur->id - 1].y_pos < 20 && world[player->y][player->x]->npcs[cur->id - 1].x_pos > 1 &&  world[player->y][player->x]->rival_costmap[cur_ypos + 1][cur_xpos - 1] < INT16_MAX) {
                            next_y++;
                            next_x--;
                        } // Move lower left
                        break;
                    case '4': case 'h':
                        if (world[player->y][player->x]->npcs[cur->id - 1].x_pos > 0 &&  world[player->y][player->x]->rival_costmap[cur_ypos][cur_xpos - 1] < INT16_MAX){
                            next_x--;
                        } // Move left
                        break;
                    case '>':
                        // Placeholder for entering a building interface
                        printw("Entering building interface...");
                        getch(); // Wait for user input to continue
                        break;
                    case '5': case ' ': case '.':
                        // Rest for a turn
                        break;
                    case 't':
                        // Display a list of trainers
                        printw("Trainer list:\n");
                        // Placeholder for trainer list display
                        getch(); // Wait for user input to continue
                        break;
                }
            }
        }

        print_map(player, world);

        characters[cur->id].time = cur->time;
        characters[cur->id].id = cur->id;
        characters[cur->id].dir = cur->dir;
        characters[cur->id].player_type = cur->player_type;
        characters[cur->id].hn = heap_insert(&heap, &characters[cur->id]);
        heap_remove_min(&heap);

        usleep(250000);

    }

    // END CURSES
    endwin();

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


        // next->id = cur->id;
        // next->player_type = cur->player_type;
        // next->time += cur->time + 20;
        // heap_remove_min(&heap);
        // heap_insert(&heap, &next);


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
    
// CURSES CODE:




// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <time.h>
// #include <limits.h>
// #include <unistd.h>
// #include <ncurses.h>

// #include "map.h"
// #include "pc.h"
// #include "heap.h"

// #define WORLD_WIDTH 401
// #define WORLD_HEIGHT 401
// #define MAP_WIDTH 80
// #define MAP_HEIGHT 21
// #define MAX_TRAINERS 50
// #define DEFAULT_TRAINERS 10

// typedef struct character_turn {
//   heap_node_t *hn;
//   int32_t time;
//   int id;
//   int player_type;
// } character_turn_t;

// static int32_t path_cmp(const void *key, const void *with) {
//     int32_t time_difference = ((character_turn_t *)key)->time - ((character_turn_t *)with)->time;
//     if (time_difference != 0) {
//         return time_difference;
//     } else {
//         return ((character_turn_t *)key)->id - ((character_turn_t *)with)->id;
//     }
// }

// void print_map(struct pc* player, struct map* world[WORLD_HEIGHT][WORLD_WIDTH]) {
//     move(1, 0);

//     for (int row = 0; row < MAP_HEIGHT; row++) {
//         for (int col = 0; col < MAP_WIDTH; col++) {
//             printw(world[player->y][player->x]->terrain_with_npcs);
//         }
//         printw("\n");
//     }
//     refresh();
// }

// int main(int argc, char *argv[])
// {
//     int numtrainers = 0;

//     // DETERMINE THE NUMBER OF TRAINERS
//     for (int i = 1; i < argc; i++) {
//         if (strcmp(argv[i], "--numtrainers") == 0 && i + 1 < argc) {
//             if (atoi(argv[i + 1]) > MAX_TRAINERS) {
//                 printf("Number of trainers cannot exceed %d\n", MAX_TRAINERS);
//                 printf("Setting number of trainers to %d\n", DEFAULT_TRAINERS);
//                 break;
//             }
//             else{
//                 numtrainers = atoi(argv[i + 1]);
//                 break;
//             }
//         }
//     }

//     if(numtrainers <= 0){
//         numtrainers = DEFAULT_TRAINERS;
//     }
//     // printf("Number of trainers: %d\n", numtrainers);

//     srand(time(NULL));
//     // ALLOCATE MEMORY FOR WORLD
//     struct map* world[WORLD_HEIGHT][WORLD_WIDTH] = {NULL};

//     // ALLOCATE MEMORY FOR PLAYER CHARACTER
//     struct pc* player = malloc(sizeof(struct pc));
//     player->x = 200;
//     player->y = 200;

//     // Allocate and generate the initial map
//     world[player->y][player->x] = malloc(sizeof(struct map));
//     if (world[player->y][player->x] == NULL) {
//         fprintf(stderr, "Failed to allocate memory for map\n");
//         return 1;
//     }
//     world[player->y][player->x]->is_generated = 0;
//     world[player->y][player->x]->path_east = 0;
//     world[player->y][player->x]->path_west = 0;
//     world[player->y][player->x]->path_north = 0;
//     world[player->y][player->x]->path_south = 0;
//     world[player->y][player->x]->x_pos = player->x;
//     world[player->y][player->x]->y_pos = player->y;
//     for (int i = 0; i < MAP_HEIGHT; i++) {
//         for (int j = 0; j < MAP_WIDTH; j++) {
//             world[player->y][player->x]->terrain[i][j] = ' ';
//         }
//     }
    
//     // GENERATE AND PRINT CENTER MAP
//     generate_map(world[player->y][player->x], world, player, numtrainers);
//     world[player->y][player->x]->is_generated = 1;
//     print_map(player, world);
//     // for(int i = 0; i < MAP_HEIGHT; i++){
//     //         for(int j = 0; j < MAP_WIDTH; j++){
//     //             printf("%c ", world[player->y][player->x]->terrain_with_npcs[i][j]);
//     //         }
//     //         printf("\n");
//     // }



//     // HEAP FOR MAIN GAME LOOP
//     heap_t heap;
//     heap_init(&heap, path_cmp, NULL);

//     character_turn_t characters[numtrainers + 1], *cur;

//     characters[0].time = 0;
//     characters[0].id = 0;
//     characters[0].player_type = 1;
//     characters[0].hn = heap_insert(&heap, &characters[0]);


//     for(int i = 0; i < numtrainers; i++){
//         characters[i+1].time = 0;
//         characters[i+1].id = i + 1;
//         characters[i+1].player_type = world[player->y][player->x]->npcs[i].player_type;
//         characters[i+1].hn = heap_insert(&heap, &characters[i+1]);
//     }

//     while((cur = heap_peek_min(&heap))){
//         printf("%d\n", cur->time);
//         printf("%d\n", cur->id);
//         printf("%d\n", cur->player_type);
//         int min_cost = INT16_MAX;
//         int cur_xpos = world[player->y][player->x]->npcs[cur->id - 1].x_pos;
//         int cur_ypos = world[player->y][player->x]->npcs[cur->id - 1].y_pos;
//         int next_x = 0;
//         int next_y = 0;

//         if(cur->player_type != 2){
//             characters[cur->id].time = cur->time + 20;
//             characters[cur->id].id = cur->id;
//             characters[cur->id].player_type = cur->player_type;
//             characters[cur->id].hn = heap_insert(&heap, &characters[cur->id]);
//             heap_remove_min(&heap);
//             continue;
//         } else if (cur->player_type == 2) {
//             for (int i = -1; i <= 1; i++){
//                 for (int j = -1; j <= 1; j++){
//                     if (world[player->y][player->x]->hiker_costmap[cur_ypos + i][cur_xpos + j] < min_cost && 
//                     (world[player->y][player->x]->terrain_with_npcs[cur_ypos + i][cur_xpos + j] != '@' && world[player->y][player->x]->terrain_with_npcs[cur_ypos + i][cur_xpos + j] != 'h' && 
//                     world[player->y][player->x]->terrain_with_npcs[cur_ypos + i][cur_xpos + j] != 'r' && world[player->y][player->x]->terrain_with_npcs[cur_ypos + i][cur_xpos + j] != 'p' && 
//                     world[player->y][player->x]->terrain_with_npcs[cur_ypos + i][cur_xpos + j] != 'w' && world[player->y][player->x]->terrain_with_npcs[cur_ypos + i][cur_xpos + j] != 's' && 
//                     world[player->y][player->x]->terrain_with_npcs[cur_ypos + i][cur_xpos + j] != 'e')) {

//                         min_cost = world[player->y][player->x]->hiker_costmap[cur_ypos + i][cur_xpos + j];
//                         next_y = cur_ypos + i;
//                         next_x = cur_xpos + j;

//                     }
//                 }
//             }
//             cur->time += abs(world[player->y][player->x]->hiker_costmap[cur_ypos][cur_xpos] - min_cost);
//             world[player->y][player->x]->npcs[cur->id - 1].x_pos = next_x;
//             world[player->y][player->x]->npcs[cur->id - 1].y_pos = next_y;
//             world[player->y][player->x]->terrain_with_npcs[next_y][next_x] = world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos];
//             world[player->y][player->x]->terrain_with_npcs[cur_ypos][cur_xpos] = world[player->y][player->x]->terrain[cur_ypos][cur_xpos];
//         }

//         for(int i = 0; i < MAP_HEIGHT; i++){
//             for(int j = 0; j < MAP_WIDTH; j++){
//                 printf("%c ", world[player->y][player->x]->terrain_with_npcs[i][j]);
//             }
//             printf("\n");
//         }

//         characters[cur->id].time = cur->time;
//         characters[cur->id].id = cur->id;
//         characters[cur->id].player_type = cur->player_type;
//         characters[cur->id].hn = heap_insert(&heap, &characters[cur->id]);
//         heap_remove_min(&heap);

//         usleep(250000);

//     }



//     heap_delete(&heap);



//         // FREE ALL MEMORY USED IN THE WORLD
//     for (int i = 0; i < WORLD_HEIGHT; i++) {
//         for (int j = 0; j < WORLD_WIDTH; j++) {
//             if (world[i][j] != NULL) {
//                 free(world[i][j]);
//             }
//         }
//     }

//     // FREE PC MEMORY
//     free(player);

//     return 0;


//         // next->id = cur->id;
//         // next->player_type = cur->player_type;
//         // next->time += cur->time + 20;
//         // heap_remove_min(&heap);
//         // heap_insert(&heap, &next);


//     // OLD MAIN GAME LOOP!
//     // char command;
//     // while (1) {
//     //     printf("Current position: (%d, %d)\n", player->x-200, player->y-200);
//     //     printf("Enter a command (n/s/e/w/f (fly) to move, q to quit): ");
//     //     scanf(" %c", &command);

//     //     if (command == 'q') {
//     //         break;
//     //     } if (command == 'n') {
//     //         if (player->y + 1 < WORLD_HEIGHT) {
//     //             if (world[player->y+1][player->x] == NULL) {
//     //                 world[player->y+1][player->x] = malloc(sizeof(struct map));
//     //                 if (world[player->y+1][player->x] == NULL) {
//     //                     fprintf(stderr, "Failed to allocate memory for map\n");
//     //                     return 1;
//     //                 }
//     //                 world[player->y+1][player->x]->is_generated = 0;
//     //                 world[player->y+1][player->x]->path_east = 0;
//     //                 world[player->y+1][player->x]->path_west = 0;
//     //                 world[player->y+1][player->x]->path_north = 0;
//     //                 world[player->y+1][player->x]->path_south = world[player->y][player->x]->path_north;
//     //                 world[player->y+1][player->x]->x_pos = player->x;
//     //                 world[player->y+1][player->x]->y_pos = player->y+1;
//     //                 for (int i = 0; i < MAP_HEIGHT; i++) {
//     //                     for (int j = 0; j < MAP_WIDTH; j++) {
//     //                         world[player->y+1][player->x]->terrain[i][j] = ' ';
//     //                     }
//     //                 }
//     //                 generate_map(world[player->y+1][player->x], world, player, numtrainers);
//     //                 world[player->y+1][player->x]->is_generated = 1;
//     //             }
//     //             player->y++;
//     //         } else {
//     //             printf("You can't go further north!\n");
//     //         }
//     //     } else if (command == 's') {
//     //         if (player->y - 1 >= 0) {
//     //             if (world[player->y-1][player->x] == NULL) {
//     //                 world[player->y-1][player->x] = malloc(sizeof(struct map));
//     //                 if (world[player->y-1][player->x] == NULL) {
//     //                     fprintf(stderr, "Failed to allocate memory for map\n");
//     //                     return 1;
//     //                 }
//     //                 world[player->y-1][player->x]->is_generated = 0;
//     //                 world[player->y-1][player->x]->path_east = 0;
//     //                 world[player->y-1][player->x]->path_west = 0;
//     //                 world[player->y-1][player->x]->path_south = 0;
//     //                 world[player->y-1][player->x]->path_north = world[player->y][player->x]->path_south;
//     //                 world[player->y-1][player->x]->x_pos = player->x;
//     //                 world[player->y-1][player->x]->y_pos = player->y-1;
//     //                 for (int i = 0; i < MAP_HEIGHT; i++) {
//     //                     for (int j = 0; j < MAP_WIDTH; j++) {
//     //                         world[player->y-1][player->x]->terrain[i][j] = ' ';
//     //                     }
//     //                 }
//     //                 generate_map(world[player->y-1][player->x], world, player, numtrainers);
//     //                 world[player->y-1][player->x]->is_generated = 1;
//     //             }
//     //             player->y--;
//     //         } else {
//     //             printf("You can't go further south!\n");
//     //         }
//     //     } else if (command == 'e') {
//     //         if (player->x + 1 < WORLD_WIDTH) {
//     //             if (world[player->y][player->x+1] == NULL) {
//     //                 world[player->y][player->x+1] = malloc(sizeof(struct map));
//     //                 if (world[player->y][player->x+1] == NULL) {
//     //                     fprintf(stderr, "Failed to allocate memory for map\n");
//     //                     return 1;
//     //                 }
//     //                 world[player->y][player->x+1]->is_generated = 0;
//     //                 world[player->y][player->x+1]->path_east = 0;
//     //                 world[player->y][player->x+1]->path_north = 0;
//     //                 world[player->y][player->x+1]->path_south = 0;
//     //                 world[player->y][player->x+1]->path_west = world[player->y][player->x]->path_east;
//     //                 world[player->y][player->x+1]->x_pos = player->x+1;
//     //                 world[player->y][player->x+1]->y_pos = player->y;
//     //                 for (int i = 0; i < MAP_HEIGHT; i++) {
//     //                     for (int j = 0; j < MAP_WIDTH; j++) {
//     //                         world[player->y][player->x+1]->terrain[i][j] = ' ';
//     //                     }
//     //                 }
//     //                 generate_map(world[player->y][player->x+1], world, player, numtrainers);
//     //                 world[player->y][player->x+1]->is_generated = 1;
//     //             }
//     //             player->x++;
//     //         } else {
//     //             printf("You can't go further east!\n");
//     //         }
//     //     } else if (command == 'w') {
//     //         if (player->x - 1 >= 0) {
//     //             if (world[player->y][player->x-1] == NULL) {
//     //                 world[player->y][player->x-1] = malloc(sizeof(struct map));
//     //                 if (world[player->y][player->x-1] == NULL) {
//     //                     fprintf(stderr, "Failed to allocate memory for map\n");
//     //                     return 1;
//     //                 }
//     //                 world[player->y][player->x-1]->is_generated = 0;
//     //                 world[player->y][player->x-1]->path_west = 0;
//     //                 world[player->y][player->x-1]->path_north = 0;
//     //                 world[player->y][player->x-1]->path_south = 0;
//     //                 world[player->y][player->x-1]->path_east = world[player->y][player->x]->path_west;
//     //                 world[player->y][player->x-1]->x_pos = player->x-1;
//     //                 world[player->y][player->x-1]->y_pos = player->y;
//     //                 for (int i = 0; i < MAP_HEIGHT; i++) {
//     //                     for (int j = 0; j < MAP_WIDTH; j++) {
//     //                         world[player->y][player->x-1]->terrain[i][j] = ' ';
//     //                     }
//     //                 }
//     //                 generate_map(world[player->y][player->x-1], world, player, numtrainers);
//     //                 world[player->y][player->x-1]->is_generated = 1;
//     //             }
//     //             player->x--;
//     //         } else {
//     //             printf("You can't go further west!\n");
//     //         }
//     //     } else if (command == 'f') {
//     //         int new_x, new_y;
//     //         printf("Enter the x and y coordinates of the destination: ");
//     //         scanf("%d %d", &new_x, &new_y);
//     //         new_x += 200;
//     //         new_y += 200;
//     //         if (new_x >= 0 && new_x < WORLD_WIDTH && new_y >= 0 && new_y < WORLD_HEIGHT) {
//     //             if (world[new_y][new_x] == NULL) {
//     //                 world[new_y][new_x] = malloc(sizeof(struct map));
//     //                 if (world[new_y][new_x] == NULL) {
//     //                     fprintf(stderr, "Failed to allocate memory for map\n");
//     //                     return 1;
//     //                 }
//     //                 world[new_y][new_x]->is_generated = 0;
//     //                 world[new_y][new_x]->path_west = 0;
//     //                 world[new_y][new_x]->path_north = 0;
//     //                 world[new_y][new_x]->path_south = 0;
//     //                 world[new_y][new_x]->path_east = 0;
//     //                 world[new_y][new_x]->x_pos = new_x;
//     //                 world[new_y][new_x]->y_pos = new_y;
//     //                 for (int i = 0; i < MAP_HEIGHT; i++) {
//     //                     for (int j = 0; j < MAP_WIDTH; j++) {
//     //                         world[new_y][new_x]->terrain[i][j] = ' ';
//     //                     }
//     //                 }
//     //                 generate_map(world[new_y][new_x], world, player, numtrainers);
//     //                 world[new_y][new_x]->is_generated = 1;
//     //             }
//     //             player->x--;
//     //             player->x = new_x;
//     //             player->y = new_y;
//     //         } else {
//     //             printf("Invalid coordinates! Please try again.\n");
//     //         }
//     //     }

//     //     if (world[player->y][player->x]->is_generated == 0) {
//     //         if (world[player->y][player->x] == NULL) {
//     //             fprintf(stderr, "Failed to allocate memory for world[%d][%d]\n", player->y, player->x);
//     //             return 1;
//     //         }
//     //         generate_map(world[player->y][player->x], world, player, numtrainers);
//     //         world[player->y][player->x]->is_generated = 1;
//     //     }

//     //     for(int i = 0; i < MAP_HEIGHT; i++){
//     //         for(int j = 0; j < MAP_WIDTH; j++){
//     //             printf("%c ", world[player->y][player->x]->terrain[i][j]);
//     //         }
//     //         printf("\n");
//     //     }

//     // }
    
//     // PRINT HIKER AND RIVAL MAPS
//     // printf("Hiker map: \n");
//     // for (int i = 0; i < 21; i++) {
//     //     for (int j = 0; j < 80; j++) {
//     //         if (world[player->y][player->x]->hiker_costmap[i][j] == INT16_MAX){
//     //             printf("   ");
//     //         }
//     //         else{
//     //             printf("%02d ", abs(world[player->y][player->x]->hiker_costmap[i][j]% 100)); // Adjust to print the hiker cost map
//     //         }
//     //     }
//     //     printf("\n");
//     // }
//     // printf("Rival map: \n");
//     // for (int i = 0; i < 21; i++) {
//     //     for (int j = 0; j < 80; j++) {
//     //         if (world[player->y][player->x]->rival_costmap[i][j] == INT16_MAX){
//     //             printf("   ");
//     //         }
//     //         else{
//     //             printf("%02d ", abs(world[player->y][player->x]->rival_costmap[i][j]% 100)); // Adjust to print the hiker cost map
//     //         }
//     //     }
//     //     printf("\n");
//     // }
// }