#ifndef CHARACTER_H
#define CHARACTER_H

typedef enum char_type {
  pc,
  npc
} char_type_t;

typedef struct pc {
  
} pc_t;

typedef struct npc {
  uint32_t smart;
  uint32_t tele;
} npc_t;

typedef struct character {
  char_type_t type;
  pc_t pc;
  npc_t npc;
  char symbol;
  uint8_t speed;
  uint16_t pos[2];
  uint32_t alive;
} character_t;

#endif
