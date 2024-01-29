#include <stdio.h>
#include <stdlib.h>
#include <time.h>
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

int main(int argc, char *argv[])
{
    srand(time(NULL));

    fill_board_with_short_grass();
    place_tall_grass();
    place_water();
    place_mountains();
    place_tall_grass();
    place_short_grass();
    place_water();
    place_mountains();
    place_tall_grass();

    for(int i = 0; i < MAP_HEIGHT; i++){
        for(int j = 0; j < MAP_WIDTH; j++){
            printf("%c ", map[i][j]);
        }
        printf("\n");
    }
}