#include <stdio.h>
#include <stdlib.h>
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

int main(int argc, char *argv[])
{
    fill_board_with_short_grass();
    for(int i = 0; i < MAP_HEIGHT; i++){
        for(int j = 0; j < MAP_WIDTH; j++){
            printf("%d ", map[i][j]);
        }
        printf("\n");
    }
}