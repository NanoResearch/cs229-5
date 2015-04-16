#ifndef PC_H
# define PC_H

# include <stdint.h>

# include "dims.h"
# include "object.h"
# include "dungeon.h"

typedef struct dungeon dungeon_t;

typedef struct pc {
  char name[40];
  char catch_phrase[80];
  object_t a, b, c, d, e, f, g, h, i, j, k, l;
  object_t *carry[PC_CARRY];
} pc_t;

void pc_delete(pc_t *pc);
uint32_t pc_is_alive(dungeon_t *d);
void config_pc(dungeon_t *d);
uint32_t pc_next_pos(dungeon_t *d, pair_t dir);
void place_pc(dungeon_t *d);

#endif
