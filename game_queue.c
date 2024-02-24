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

