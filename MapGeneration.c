#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include "map.h"
#include "pc.h"
#include "heap.h"

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MAP_WIDTH 80
#define MAP_HEIGHT 21
#define WORLD_WIDTH 401
#define WORLD_HEIGHT 401
#define PLAYER_TYPE_PC 1
#define PLAYER_TYPE_HIKER 2
#define PLAYER_TYPE_RIVAL 3
#define PLAYER_TYPE_SWIMMER 4
#define PLAYER_TYPE_OTHER 5

typedef struct path {
  heap_node_t *hn;
  uint8_t pos[2];
  uint8_t from[2];
  int32_t cost;
} path_t;

typedef enum dim {
  dim_x,
  dim_y,
  num_dims
} dim_t;

int isBorder(int y, int x) {
    if (x == 0 || x == MAP_WIDTH-1 || y == 0 || y == MAP_HEIGHT-1){
        return 1;
    }
    else {
        return 0;
    }
}

int canPlaceP(struct map *m,int y, int x){
    if(m->terrain[y][x] != '#' && (m->terrain[y+1][x] == '#' || m->terrain[y-1][x] == '#' || m->terrain[y][x+1] == '#' || m->terrain[y][x-1] == '#')){
        return 1;
    }
    else{
        return 0;
    }
}

int canPlacePC(struct map *m,int y, int x){
    if(m->terrain[y][x] == '#' && (y != 0 && x != 0 && y != MAP_HEIGHT - 1 && x != MAP_WIDTH - 1)){
        return 1;
    }
    else{
        return 0;
    }
}

void fill_board_with_short_grass(struct map *m){
    for(int i = 0; i < MAP_HEIGHT; i++){
        for(int j = 0; j < MAP_WIDTH; j++){
            if(isBorder(i, j) == 1){
                m->terrain[i][j] = '%';
            }
            else{
                m->terrain[i][j] = '.';
            }
        }
    }
}

void place_water(struct map *m){
    int water_height = rand()%15;
    int water_width = rand()%70;
    int water_x = rand()%(MAP_WIDTH-water_width);
    int water_y = rand()%(MAP_HEIGHT-water_height);
    for(int i = water_y; i < water_height + water_y; i++){
        for(int j = water_x; j < water_width + water_x; j++){
            if(isBorder(i, j) == 0){
                m->terrain[i][j] = '~';
            }
        }
    }
}

void place_mountains(struct map *m){
    int mountain_height = rand()%15;
    int mountain_width = rand()%70;
    int water_x = rand()%(MAP_WIDTH-mountain_width);
    int water_y = rand()%(MAP_HEIGHT-mountain_height);
    for(int i = water_y; i < mountain_height + water_y; i++){
        for(int j = water_x; j < mountain_width + water_x; j++){
            if(isBorder(i, j) == 0){
                m->terrain[i][j] = '%';
            }
        }
    }
}

void place_tall_grass(struct map *m){
    int grass_height = rand()%15;
    int grass_width = rand()%70;
    int water_x = rand()%(MAP_WIDTH-grass_width);
    int water_y = rand()%(MAP_HEIGHT-grass_height);
    for(int i = water_y; i < grass_height + water_y; i++){
        for(int j = water_x; j < grass_width + water_x; j++){
            if(isBorder(i, j) == 0){
                m->terrain[i][j] = ':';
            }
        }
    }
}

void place_short_grass(struct map *m){
    int grass_height = rand()%10;
    int grass_width = rand()%50;
    int water_x = rand()%(MAP_WIDTH-grass_width);
    int water_y = rand()%(MAP_HEIGHT-grass_height);
    for(int i = water_y; i < grass_height + water_y; i++){
        for(int j = water_x; j < grass_width + water_x; j++){
            if(isBorder(i, j) == 0){
                m->terrain[i][j] = '.';
            }
        }
    }
}

void place_trees(struct map *m){
    int tree_height = rand()%10;
    int tree_width = rand()%50;
    int tree_x = rand()%(MAP_WIDTH-tree_width);
    int tree_y = rand()%(MAP_HEIGHT-tree_height);
    for(int i = tree_y; i < tree_height + tree_y; i++){
        for(int j = tree_x; j < tree_width + tree_x; j++){
            if(isBorder(i, j) == 0){
                m->terrain[i][j] = '^';
            }
        }
    }
}

void place_paths(struct map *m, struct map* world[WORLD_HEIGHT][WORLD_WIDTH]){
    int x_pos = m->x_pos;
    int y_pos = m->y_pos;
    if((x_pos != WORLD_HEIGHT-1) && (world[y_pos][x_pos+1] != NULL)){

        m->path_east = world[y_pos][x_pos+1]->path_west;

    }
    if((x_pos != 0) && (world[y_pos][x_pos-1] != NULL)){

        m->path_west = world[y_pos][x_pos-1]->path_east;

    }
    if((y_pos != 0) && (world[y_pos-1][x_pos] != NULL)){

        m->path_south = world[y_pos-1][x_pos]->path_north;

    }
    if((y_pos != WORLD_HEIGHT-1) && (world[y_pos+1][x_pos] != NULL)){

        m->path_north = world[y_pos+1][x_pos]->path_south;

    }
    m->path_south = (m->path_south == 0) ? rand()%75+3 : m->path_south;
    m->path_east = (m->path_east == 0) ? rand()%15+3 : m->path_east;
    m->path_north = (m->path_north == 0) ? rand()%75+3 : m->path_north;
    m->path_west = (m->path_west == 0) ? rand()%15+3 : m->path_west;

    int path_north = m->path_north;
    int path_south = m->path_south;
    int path_east = m->path_east;
    int path_west = m->path_west;

    int randx = rand()%75+3;
    int randy = rand()%15+3;

    for(int i = 0; i <= randy; i++){
        m->terrain[i][path_north] = '#';
    }
    for(int i = MAP_HEIGHT - 1; i >= randy; i--){
        m->terrain[i][path_south] = '#';
    }
    for(int i = MAP_WIDTH - 1; i >= randx; i--){
        m->terrain[path_east][i] = '#';
    }
    for(int i = 0; i <= randx; i++){
        m->terrain[path_west][i] = '#';
    }
    for(int i = MIN(path_north, path_south); i <= MAX(path_south, path_north); i++){
        m->terrain[randy][i] = '#';
    }
    for(int i = MIN(path_east, path_west); i <= MAX(path_east, path_west); i++){
        m->terrain[i][randx] = '#';
    }
    if (x_pos == WORLD_HEIGHT - 1){
        m->terrain[path_east][MAP_WIDTH-1] = '%';
    }
    if (x_pos == 0){
        m->terrain[path_west][0] = '%';
    }
    if (y_pos == WORLD_HEIGHT - 1){
        m->terrain[0][path_north] = '%';
    }
    if (y_pos == 0){
        m->terrain[MAP_HEIGHT-1][path_south] = '%';
    }
}

void place_pmart(struct map *m){
    int isPlaced = 0;
    int randx;
    int randy;
    while(isPlaced == 0){
        randx = rand()%75+3;
        randy = rand()%15+3;
        if(canPlaceP(m, randy, randx)){
            m->terrain[randy][randx] = 'M';
            isPlaced = 1;
        }
    }
}

void place_pcenter(struct map *m){
    int isPlaced = 0;
    int randx;
    int randy;
    while(isPlaced == 0){
        randx = rand()%75+3;
        randy = rand()%15+3;
        if(canPlaceP(m, randy, randx)){
            m->terrain[randy][randx] = 'C';
            isPlaced = 1;
        }
    }
}

void place_pc(struct map *m, struct pc* player){
    int isPlaced = 0;
    int randx;
    int randy;
    while(isPlaced == 0){
        randx = rand()%75+3;
        randy = rand()%15+3;
        if(canPlacePC(m, randy, randx)){
            m->terrain[randy][randx] = '@';
            player->map_x = randx;
            player->map_y = randy;
            isPlaced = 1;
        }
    }
}

double manhattan(int y, int x){
    return abs(x) + abs(y);
}


// START HERE ------------------------------------------------------------
// Function to compare nodes for the heap
static int32_t path_cmp(const void *key, const void *with) {
  return ((path_t *) key)->cost - ((path_t *) with)->cost;
}

// Function to get the cost of traversing a terrain type
int get_cost_of_terrain(char terrain_type, int player_type) {
    if (terrain_type == '#'){ // PATH
        if (player_type != PLAYER_TYPE_SWIMMER){
            return 10;
        }
        else {
            return INT_MAX;
        }
    }
    if (terrain_type == 'M'){ // POKEMART
        if (player_type == PLAYER_TYPE_PC){
            return 10;
        }
        else if(player_type == PLAYER_TYPE_SWIMMER){
            return INT_MAX;
        }
        else {
            return 50;
        }
    }
    if (terrain_type == 'C'){ // POKECENTER
        if (player_type == PLAYER_TYPE_PC){
            return 10;
        }
        else if(player_type == PLAYER_TYPE_SWIMMER){
            return INT_MAX;
        }
        else {
            return 50;
        }
    }
    if (terrain_type == '.'){ // SHORT GRASS
        if (player_type != PLAYER_TYPE_SWIMMER){
            return 10;
        }
        else {
            return INT_MAX;
        }
    }
    if (terrain_type == ':'){ // TALL GRASS
        if (player_type == PLAYER_TYPE_PC || player_type == PLAYER_TYPE_RIVAL || player_type == PLAYER_TYPE_OTHER){
            return 20;
        }
        else if(player_type == PLAYER_TYPE_HIKER){
            return 15;
        }
        else {
            return INT_MAX;
        }
    }
    if (terrain_type == '~'){ // WATER
        if (player_type == PLAYER_TYPE_SWIMMER){
            return 7;
        }
        else {
            return INT_MAX;
        }
    }
    if (terrain_type == '%'){ // MOUNTAINS
        if (player_type == PLAYER_TYPE_HIKER){
            return 15;
        }
        else {
            return INT_MAX - 1;
        }
    }
    if (terrain_type == '^'){ // TREES
        if (player_type == PLAYER_TYPE_HIKER){
            return 15;
        }
        else {
            return INT_MAX;
        }
    }
    return INT_MAX;
}

void compute_cost_map(struct map *m, struct pc* player, int player_type) {
    // Create a heap to store nodes with their tentative distances
    heap_t heap;
    uint32_t x, y;
    char terrain = ' ';
    int cur_cost = 0;
    heap_init(&heap, path_cmp, NULL);


    // START
    static path_t path[MAP_HEIGHT][MAP_WIDTH], *p;
    static uint32_t initialized = 0;

  if (!initialized) {
    for (y = 0; y < MAP_HEIGHT; y++) {
      for (x = 0; x < MAP_WIDTH; x++) {
        path[y][x].pos[dim_y] = y;
        path[y][x].pos[dim_x] = x;
      }
    }
    initialized = 1;
  }
  
  for (y = 0; y < MAP_HEIGHT; y++) {
    for (x = 0; x < MAP_WIDTH; x++) {
      path[y][x].cost = INT_MAX - 1;
    }
  }

  path[player->map_y][player->map_x].cost = 0;

  heap_init(&heap, path_cmp, NULL);

  for (y = 1; y < MAP_HEIGHT - 1; y++) {
    for (x = 1; x < MAP_WIDTH - 1; x++) {
      path[y][x].hn = heap_insert(&heap, &path[y][x]);
    }
  }

  while ((p = heap_remove_min(&heap))) {
    p->hn = NULL;

    for (int i = -1; i <= 1; i++){
        for (int j = -1; j <= 1; j++){
            terrain = m->terrain[p->pos[dim_y] + i][p->pos[dim_x] + j];
            cur_cost =  get_cost_of_terrain(terrain, player_type);
            if((i == 0 && j == 0) || p->pos[dim_y] + i < 0 || p->pos[dim_y] + i >= MAP_HEIGHT || p->pos[dim_y] + j < 0 || p->pos[dim_y] + j >= MAP_WIDTH){
                continue;
            }
            if(path[p->pos[dim_y] + i][p->pos[dim_x] + j].cost > path[p->pos[dim_y]][p->pos[dim_x]].cost + cur_cost){
                path[p->pos[dim_y ] + i][p->pos[dim_x] + j].cost = path[p->pos[dim_y]][p->pos[dim_x]].cost + cur_cost;
            }
        }
    }
  }

  if (player_type == PLAYER_TYPE_HIKER){
    for(int i = 0; i < MAP_HEIGHT; i++){
        for(int j = 0; j < MAP_WIDTH; j++){
            m->hiker_costmap[i][j] = path[i][j].cost;
        }
    }
  }

    // Free dynamically allocated memory
    heap_delete(&heap);
}
// END HERE----------------------------------------------------


int generate_map(struct map *m, struct map* world[WORLD_HEIGHT][WORLD_WIDTH], struct pc* player)
{
    srand(time(NULL));
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            m->rival_costmap[i][j] = INT_MAX;
        }
    }
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            m->hiker_costmap[i][j] = INT_MAX;
        }
    }

    fill_board_with_short_grass(m);
    place_trees(m);
    place_tall_grass(m);
    place_water(m);
    place_mountains(m);
    place_tall_grass(m);
    place_short_grass(m);
    place_water(m);
    place_mountains(m);
    place_tall_grass(m);
    place_paths(m, world);

    double manhattanDistance = manhattan(m->y_pos - 200.0, m->x_pos - 200.0);
    int rand100 = rand()%100;
    double probability = ((-45.0 * manhattanDistance)/200.0 + 50.0)/100.0;

    if((m->x_pos == 200 && m->y_pos == 200) || (probability > rand100/100.0)){
        place_pcenter(m);
    }
    rand100 = rand()%100;
    if((m->x_pos == 200 && m->y_pos == 200) || (probability > rand100/100.0)){
        place_pmart(m);
    }

    place_pc(m, player);

    for(int i = 0; i < MAP_HEIGHT; i++){
            for(int j = 0; j < MAP_WIDTH; j++){
                printf("%c ", m->terrain[i][j]);
            }
            printf("\n");
    }

    // Initialize the graph and compute the cost map
    compute_cost_map(m, player, PLAYER_TYPE_HIKER);
    // compute_cost_map(m, player, PLAYER_TYPE_RIVAL);
    return 0;
}