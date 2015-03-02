#ifndef CHARACTER_H
# define CHARACTER_H

# include <stdint.h>

# include "dims.h"

typedef struct dungeon dungeon_t;
typedef struct npc npc_t;
typedef struct pc pc_t;

typedef struct character {
  char symbol;
  pair_t position;
  uint32_t speed;
  uint32_t next_turn;
  uint32_t alive;
  npc_t *npc;
  pc_t *pc;
} character_t;

int32_t compare_characters_by_next_turn(const void *character1,
                                        const void *character2);
uint32_t can_see(dungeon_t *d, character_t *voyeur, character_t *exhibitionist);
void character_delete(void *c);

#endif
