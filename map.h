#ifndef MAP_H
#define MAP_H
#define WORLD_WIDTH 401
#define WORLD_HEIGHT 401
#include "pc.h"
#include "npc.h"
#define MAX_TRAINERS 50


struct map {
  struct npc npcs[MAX_TRAINERS];
  char terrain[21][80];
  char terrain_with_npcs[21][80];
  int hiker_costmap[21][80];
  int rival_costmap[21][80];
  int is_generated;
  int path_north;
  int path_south;
  int path_east;
  int path_west;
  int x_pos;
  int y_pos;
};

int generate_map(struct map *m, struct map* world[WORLD_HEIGHT][WORLD_WIDTH], struct pc* player, int numtrainers);

#endif // MAP_H