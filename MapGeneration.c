#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MAP_WIDTH 80
#define MAP_HEIGHT 21

char map[MAP_HEIGHT][MAP_WIDTH];

int isBorder(int y, int x) {
    if (x == 0 || x == MAP_WIDTH-1 || y == 0 || y == MAP_HEIGHT-1){
        return 1;
    }
    else {
        return 0;
    }
}

int canPlaceP(int y, int x){
    if(map[y][x] != '#' && (map[y+1][x] == '#' || map[y-1][x] == '#' || map[y][x+1] == '#' || map[y][x-1] == '#')){
        return 1;
    }
    else{
        return 0;
    }
}

void fill_board_with_short_grass(){
    for(int i = 0; i < MAP_HEIGHT; i++){
        for(int j = 0; j < MAP_WIDTH; j++){
            if(isBorder(i, j) == 1){
                map[i][j] = '%';
            }
            else{
                map[i][j] = '.';
            }
        }
    }
}

void place_water(){
    int water_height = rand()%15;
    int water_width = rand()%70;
    int water_x = rand()%(MAP_WIDTH-water_width);
    int water_y = rand()%(MAP_HEIGHT-water_height);
    for(int i = water_y; i < water_height + water_y; i++){
        for(int j = water_x; j < water_width + water_x; j++){
            if(isBorder(i, j) == 0){
                map[i][j] = '~';
            }
        }
    }
}

void place_mountains(){
    int mountain_height = rand()%15;
    int mountain_width = rand()%70;
    int water_x = rand()%(MAP_WIDTH-mountain_width);
    int water_y = rand()%(MAP_HEIGHT-mountain_height);
    for(int i = water_y; i < mountain_height + water_y; i++){
        for(int j = water_x; j < mountain_width + water_x; j++){
            if(isBorder(i, j) == 0){
                map[i][j] = '%';
            }
        }
    }
}

void place_tall_grass(){
    int grass_height = rand()%15;
    int grass_width = rand()%70;
    int water_x = rand()%(MAP_WIDTH-grass_width);
    int water_y = rand()%(MAP_HEIGHT-grass_height);
    for(int i = water_y; i < grass_height + water_y; i++){
        for(int j = water_x; j < grass_width + water_x; j++){
            if(isBorder(i, j) == 0){
                map[i][j] = ':';
            }
        }
    }
}

void place_short_grass(){
    int grass_height = rand()%10;
    int grass_width = rand()%50;
    int water_x = rand()%(MAP_WIDTH-grass_width);
    int water_y = rand()%(MAP_HEIGHT-grass_height);
    for(int i = water_y; i < grass_height + water_y; i++){
        for(int j = water_x; j < grass_width + water_x; j++){
            if(isBorder(i, j) == 0){
                map[i][j] = '.';
            }
        }
    }
}

void place_trees(){
    int tree_height = rand()%10;
    int tree_width = rand()%50;
    int tree_x = rand()%(MAP_WIDTH-tree_width);
    int tree_y = rand()%(MAP_HEIGHT-tree_height);
    for(int i = tree_y; i < tree_height + tree_y; i++){
        for(int j = tree_x; j < tree_width + tree_x; j++){
            if(isBorder(i, j) == 0){
                map[i][j] = '.';
            }
        }
    }
}

void place_paths(){
    int path_west = rand()%15+3;
    int path_east = rand()%15+3;
    int path_north = rand()%75+3;
    int path_south = rand()%75+3;

    int randx = rand()%75+3;
    int randy = rand()%15+3;

    for(int i = 0; i <= randy; i++){
        map[i][path_north] = '#';
    }
    for(int i = MAP_HEIGHT; i >= randy; i--){
        map[i][path_south] = '#';
    }
    for(int i = 0; i <= randx; i++){
        map[path_east][i] = '#';
    }
    for(int i = MAP_WIDTH; i >= randx; i--){
        map[path_west][i] = '#';
    }
    for(int i = MIN(path_north, path_south); i < MAX(path_south, path_north); i++){
        map[randy][i] = '#';
    }
    for(int i = MIN(path_east, path_west); i < MAX(path_east, path_west); i++){
        map[i][randx] = '#';
    }
}

void place_pmart(){
    int isPlaced = 0;
    int randx;
    int randy;
    while(isPlaced == 0){
        randx = rand()%75+3;
        randy = rand()%15+3;
        if(canPlaceP(randy, randx)){
            map[randy][randx] = 'M';
            isPlaced = 1;
        }
    }
}

void place_pcenter(){
    int isPlaced = 0;
    int randx;
    int randy;
    while(isPlaced == 0){
        randx = rand()%75+3;
        randy = rand()%15+3;
        if(canPlaceP(randy, randx)){
            map[randy][randx] = 'C';
            isPlaced = 1;
        }
    }
}

int main(int argc, char *argv[])
{
    srand(time(NULL));

    fill_board_with_short_grass();
    place_trees();
    place_tall_grass();
    place_water();
    place_mountains();
    place_tall_grass();
    place_short_grass();
    place_water();
    place_mountains();
    place_tall_grass();
    place_paths();
    place_pcenter();
    place_pmart();

    for(int i = 0; i < MAP_HEIGHT; i++){
        for(int j = 0; j < MAP_WIDTH; j++){
            printf("%c ", map[i][j]);
        }
        printf("\n");
    }
}