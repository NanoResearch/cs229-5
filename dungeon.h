#ifndef DUNGEON_H
# define DUNGEON_H

# include <stdint.h>
# include <stdio.h>

# include "heap.h"
# include "macros.h"
# include "character.h"

typedef enum dim {
  dim_x,
  dim_y,
  num_dims
} dim_t;

typedef int16_t pair_t[num_dims];

#define DUNGEON_X              160
#define DUNGEON_Y              96
#define MIN_ROOMS              12
#define MAX_ROOMS              24
#define ROOM_MIN_X             10
#define ROOM_MIN_Y             8
#define ROOM_SEPARATION        5
#define MAX_PLACEMENT_ATTEMPTS 1000
#define SAVE_DIR               ".rlg229"
#define DUNGEON_SAVE_FILE      "dungeon"
#define DUNGEON_SAVE_SEMANTIC  "RLG229"
#define DUNGEON_SAVE_VERSION   0U
#define VISUAL_RANGE           30
#define PC_SPEED               10

#define mappair(pair) (d->map[pair[dim_y]][pair[dim_x]])
#define mapxy(x, y) (d->map[y][x])
#define hardnesspair(pair) (d->hardness[pair[dim_y]][pair[dim_x]])
#define hardnessxy(x, y) (d->hardness[y][x])

typedef enum __attribute__ ((__packed__)) terrain_type {
  ter_debug,
  ter_wall,
  ter_wall_no_room,
  ter_wall_no_floor,
  ter_wall_immutable,
  ter_floor,
  ter_floor_room,
  ter_floor_hall,
  ter_floor_tentative
} terrain_type_t;

typedef struct room {
  pair_t position;
  pair_t size;
  uint32_t connected;
  uint8_t exists;
} room_t;

typedef struct dungeon {
  uint32_t num_rooms;
  room_t rooms[MAX_ROOMS];
  terrain_type_t map[DUNGEON_Y][DUNGEON_X];
  /* Since hardness is usually not used, it would be expensive to pull it *
   * into cache every time we need a map cell, so we store it in a        *
   * parallel array, rather than using a structure to represent the       *
   * cells.  We may want a cell structure later, but from a performanace  *
   * perspective, it would be a bad idea to ever have the map be part of  *
   * that structure.  Pathfinding will require efficient use of the map,  *
   * and pulling in unnecessary data with each map cell would add a lot   *
   * of overhead to the memory system.                                    */
  uint8_t hardness[DUNGEON_Y][DUNGEON_X];
  uint32_t num_char;
  character_t chars[DUNGEON_Y * DUNGEON_X];
  heap_t turn_list;
} dungeon_t;

int read_dungeon(dungeon_t *dungeon, char *filename);
int gen_dungeon(dungeon_t *dungeon);
int gen_characters(dungeon_t *dungeon);
void move_pc(dungeon_t *dungeon);
void render_dungeon(dungeon_t *dungeon);
int write_dungeon(dungeon_t *dungeon);
void init_dungeon(dungeon_t *d);
void delete_dungeon(dungeon_t *d);

#endif
