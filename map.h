#ifndef MAP_H
#define MAP_H

struct map {
  double x_pos;
  double y_pos;
  int gates[4]; //N, S, E, W
  char terrain[21][80];
  int is_generated;
};

int generate_map(struct map *m);

#endif // MAP_H
