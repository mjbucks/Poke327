#ifndef MAP_H
#define MAP_H
#define WORLD_WIDTH 401
#define WORLD_HEIGHT 401

struct map {
  char terrain[21][80];
  int is_generated;
  int path_north;
  int path_south;
  int path_east;
  int path_west;
  int x_pos;
  int y_pos;
};

int generate_map(struct map *m, struct map* world[WORLD_HEIGHT][WORLD_WIDTH]);

#endif // MAP_H
