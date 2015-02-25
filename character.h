#ifndef CHARACTER_H
#define CHARACTER_H

#include "heap.h"

typedef enum char_type {
  pc,
  npc
} char_type_t;

typedef struct pc {
  uint8_t target_pos[2];
} pc_t;

typedef struct npc {
  uint32_t smart;
  uint32_t tele;
} npc_t;

typedef struct character {
  heap_node_t *hn;
  uint32_t char_num;
  uint32_t alive;
  char_type_t type;
  pc_t pc;
  npc_t npc;
  char symbol;
  uint8_t speed;
  uint16_t pos[2];
  uint32_t turn;
} character_t;

#endif
