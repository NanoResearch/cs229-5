#ifndef MOVE_H
# define MOVE_H

# include "dungeon.h"

void dijkstra(dungeon_t *d, pair_t from, pair_t to, uint16_t next[]);

#endif
