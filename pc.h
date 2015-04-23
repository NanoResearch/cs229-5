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
  object_t * weapon;
  object_t * offhand;
  object_t * ranged;
  object_t * armor;
  object_t * helmet;
  object_t * cloak;
  object_t * gloves;
  object_t * boots;
  object_t * amulet;
  object_t * light;
  object_t * ring1;
  object_t * ring2;
  object_t * carry_slots[PC_CARRY];
  int32_t speed;
} pc_t;

void pc_delete(dungeon_t *d);
uint32_t pc_is_alive(dungeon_t *d);
void config_pc(dungeon_t *d);
uint32_t pc_next_pos(dungeon_t *d, pair_t dir);
void place_pc(dungeon_t *d);

#endif
