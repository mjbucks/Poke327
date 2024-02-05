#ifndef MAP_H
#define MAP_H

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

int generate_map(struct map *m);

#endif // MAP_H
