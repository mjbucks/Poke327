#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "map.h"

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MAP_WIDTH 80
#define MAP_HEIGHT 21

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

void place_paths(struct map *m){
    int path_west = rand()%15+3;
    int path_east = rand()%15+3;
    int path_north = rand()%75+3;
    int path_south = rand()%75+3;

    int randx = rand()%75+3;
    int randy = rand()%15+3;

    for(int i = 0; i <= randy; i++){
        m->terrain[i][path_north] = '#';
    }
    for(int i = MAP_HEIGHT; i >= randy; i--){
        m->terrain[i][path_south] = '#';
    }
    for(int i = 0; i <= randx; i++){
        m->terrain[path_east][i] = '#';
    }
    for(int i = MAP_WIDTH; i >= randx; i--){
        m->terrain[path_west][i] = '#';
    }
    for(int i = MIN(path_north, path_south); i < MAX(path_south, path_north); i++){
        m->terrain[randy][i] = '#';
    }
    for(int i = MIN(path_east, path_west); i < MAX(path_east, path_west); i++){
        m->terrain[i][randx] = '#';
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

int generate_map(struct map *m)
{
    srand(time(NULL));

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
    place_paths(m);
    place_pcenter(m);
    place_pmart(m);

    return 0;
}