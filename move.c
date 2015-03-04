#include <unistd.h>

#include "dungeon.h"
#include "heap.h"
#include "move.h"
#include "npc.h"
#include "pc.h"
#include "character.h"
#include "input.h"

/* Ugly hack: There is no way to pass a pointer to the dungeon into the *
 * heap's comparitor funtion without modifying the heap.  Copying the   *
 * pc_distance array is a possible solution, but that doubles the       *
 * bandwidth requirements for dijkstra, which would also be bad.        *
 * Instead, make a global pointer to the dungeon in this file,          *
 * initialize it in dijkstra, and use it in the comparitor to get to    *
 * pc_distance.  Otherwise, pretend it doesn't exist, because it really *
 * is ugly.                                                             */
static dungeon_t *dungeon;

typedef struct path {
  heap_node_t *hn;
  uint8_t pos[2];
} path_t;

void next_move(dungeon_t *d,
               character_t *c,
               pair_t goal_pos,
               pair_t next_pos)
{
  if ((goal_pos[dim_x] == c->position[dim_x]) &&
      (goal_pos[dim_y] == c->position[dim_y])) {
    next_pos[dim_x] = goal_pos[dim_x];
    next_pos[dim_y] = goal_pos[dim_y];
  }
  
  if (has_characteristic(c, SMART)) {
  }
}

static int32_t dist_cmp(const void *key, const void *with) {
  return ((int32_t) dungeon->pc_distance[((path_t *) key)->pos[dim_y]]
                                        [((path_t *) key)->pos[dim_x]] -
          (int32_t) dungeon->pc_distance[((path_t *) with)->pos[dim_y]]
                                        [((path_t *) with)->pos[dim_x]]);
}

void dijkstra(dungeon_t *d)
{
  /* Currently assumes that monsters only move on floors.  Will *
   * need to be modified for tunneling and pass-wall monsters.  */

  heap_t h;
  uint32_t x, y;
  static path_t p[DUNGEON_Y][DUNGEON_X], *c;
  static uint32_t initialized = 0;

  if (!initialized) {
    initialized = 1;
    dungeon = d;
    for (y = 0; y < DUNGEON_Y; y++) {
      for (x = 0; x < DUNGEON_X; x++) {
        p[y][x].pos[dim_y] = y;
        p[y][x].pos[dim_x] = x;
      }
    }
  }

  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      d->pc_distance[y][x] = 255;
    }
  }
  d->pc_distance[d->pc->position[dim_y]][d->pc->position[dim_x]] = 0;

  heap_init(&h, dist_cmp, NULL);

  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      if (mapxy(x, y) >= ter_floor) {
        p[y][x].hn = heap_insert(&h, &p[y][x]);
      }
    }
  }

  while ((c = heap_remove_min(&h))) {
    c->hn = NULL;
    if ((p[c->pos[dim_y] - 1][c->pos[dim_x] - 1].hn) &&
        (d->pc_distance[c->pos[dim_y] - 1][c->pos[dim_x] - 1] >
         d->pc_distance[c->pos[dim_y]][c->pos[dim_x]] + 1)) {
      d->pc_distance[c->pos[dim_y] - 1][c->pos[dim_x] - 1] =
        d->pc_distance[c->pos[dim_y]][c->pos[dim_x]] + 1;
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y] - 1][c->pos[dim_x] - 1].hn);
    }
    if ((p[c->pos[dim_y] - 1][c->pos[dim_x]    ].hn) &&
        (d->pc_distance[c->pos[dim_y] - 1][c->pos[dim_x]    ] >
         d->pc_distance[c->pos[dim_y]][c->pos[dim_x]] + 1)) {
      d->pc_distance[c->pos[dim_y] - 1][c->pos[dim_x]    ] =
        d->pc_distance[c->pos[dim_y]][c->pos[dim_x]] + 1;
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y] - 1][c->pos[dim_x]    ].hn);
    }
    if ((p[c->pos[dim_y] - 1][c->pos[dim_x] + 1].hn) &&
        (d->pc_distance[c->pos[dim_y] - 1][c->pos[dim_x] + 1] >
         d->pc_distance[c->pos[dim_y]][c->pos[dim_x]] + 1)) {
      d->pc_distance[c->pos[dim_y] - 1][c->pos[dim_x] + 1] =
        d->pc_distance[c->pos[dim_y]][c->pos[dim_x]] + 1;
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y] - 1][c->pos[dim_x] + 1].hn);
    }
    if ((p[c->pos[dim_y]    ][c->pos[dim_x] - 1].hn) &&
        (d->pc_distance[c->pos[dim_y]    ][c->pos[dim_x] - 1] >
         d->pc_distance[c->pos[dim_y]][c->pos[dim_x]] + 1)) {
      d->pc_distance[c->pos[dim_y]    ][c->pos[dim_x] - 1] =
        d->pc_distance[c->pos[dim_y]][c->pos[dim_x]] + 1;
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y]    ][c->pos[dim_x] - 1].hn);
    }
    if ((p[c->pos[dim_y]    ][c->pos[dim_x] + 1].hn) &&
        (d->pc_distance[c->pos[dim_y]    ][c->pos[dim_x] + 1] >
         d->pc_distance[c->pos[dim_y]][c->pos[dim_x]] + 1)) {
      d->pc_distance[c->pos[dim_y]    ][c->pos[dim_x] + 1] =
        d->pc_distance[c->pos[dim_y]][c->pos[dim_x]] + 1;
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y]    ][c->pos[dim_x] + 1].hn);
    }
    if ((p[c->pos[dim_y] + 1][c->pos[dim_x] - 1].hn) &&
        (d->pc_distance[c->pos[dim_y] + 1][c->pos[dim_x] - 1] >
         d->pc_distance[c->pos[dim_y]][c->pos[dim_x]] + 1)) {
      d->pc_distance[c->pos[dim_y] + 1][c->pos[dim_x] - 1] =
        d->pc_distance[c->pos[dim_y]][c->pos[dim_x]] + 1;
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y] + 1][c->pos[dim_x] - 1].hn);
    }
    if ((p[c->pos[dim_y] + 1][c->pos[dim_x]    ].hn) &&
        (d->pc_distance[c->pos[dim_y] + 1][c->pos[dim_x]    ] >
         d->pc_distance[c->pos[dim_y]][c->pos[dim_x]] + 1)) {
      d->pc_distance[c->pos[dim_y] + 1][c->pos[dim_x]    ] =
        d->pc_distance[c->pos[dim_y]][c->pos[dim_x]] + 1;
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y] + 1][c->pos[dim_x]    ].hn);
    }
    if ((p[c->pos[dim_y] + 1][c->pos[dim_x] + 1].hn) &&
        (d->pc_distance[c->pos[dim_y] + 1][c->pos[dim_x] + 1] >
         d->pc_distance[c->pos[dim_y]][c->pos[dim_x]] + 1)) {
      d->pc_distance[c->pos[dim_y] + 1][c->pos[dim_x] + 1] =
        d->pc_distance[c->pos[dim_y]][c->pos[dim_x]] + 1;
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y] + 1][c->pos[dim_x] + 1].hn);
    }
  }
  heap_delete(&h);
}

void do_moves(dungeon_t *d)
{
  pair_t next;
  character_t *c;
  uint32_t pc_dead;

  pc_dead = 0;

  do {
    c = heap_remove_min(&d->next_turn);

    if (!c->alive) {
      if (c == d->pc) {
        pc_dead = 1;
        d->pc = NULL;
      }
      character_delete(c);

      if (pc_dead) {
        break;
      }

      continue;
    }

    c->next_turn += (100 / c->speed);

    if (c == d->pc) {
      get_input_terminal(d, next);
      next[dim_x] += c->position[dim_x];
      next[dim_y] += c->position[dim_y];
      if (mappair(next) <= ter_floor) {
        mappair(next) = ter_floor_hall;
      }
    } else {
      npc_next_pos(d, c, next);
    }

    d->character[c->position[dim_y]][c->position[dim_x]] = NULL;
    c->position[dim_y] = next[dim_y];
    c->position[dim_x] = next[dim_x];
    if (d->character[c->position[dim_y]][c->position[dim_x]]) {
      d->character[c->position[dim_y]][c->position[dim_x]]->alive = 0;
      d->num_monsters--;
    }
    d->character[c->position[dim_y]][c->position[dim_x]] = c;
    if (c == d->pc) {
      dijkstra(d);
    }
    heap_insert(&d->next_turn, c);
  } while (heap_peek_min(&d->next_turn) != d->pc);
}

void dir_nearest_wall(dungeon_t *d, character_t *c, pair_t dir)
{
  dir[dim_x] = dir[dim_y] = 0;

  if (c->position[dim_x] != 1 && c->position[dim_x] != DUNGEON_X - 2) {
    dir[dim_x] = (c->position[dim_x] > DUNGEON_X - c->position[dim_x] ? 1 : -1);
  }
  if (c->position[dim_y] != 1 && c->position[dim_y] != DUNGEON_Y - 2) {
    dir[dim_y] = (c->position[dim_y] > DUNGEON_Y - c->position[dim_y] ? 1 : -1);
  }
}

uint32_t in_corner(dungeon_t *d, character_t *c)
{
  uint32_t num_immutable;

  num_immutable = 0;

  num_immutable += (mapxy(c->position[dim_x] - 1,
                          c->position[dim_y]    ) == ter_wall_immutable);
  num_immutable += (mapxy(c->position[dim_x] + 1,
                          c->position[dim_y]    ) == ter_wall_immutable);
  num_immutable += (mapxy(c->position[dim_x]    ,
                          c->position[dim_y] - 1) == ter_wall_immutable);
  num_immutable += (mapxy(c->position[dim_x]    ,
                          c->position[dim_y] + 1) == ter_wall_immutable);

  return num_immutable > 1;
}
