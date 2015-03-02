#include <stdlib.h>

#include "string.h"

#include "dungeon.h"
#include "pc.h"
#include "utils.h"
#include "move.h"
#include "character.h"

void pc_delete(pc_t *pc)
{
  free(pc);
}

uint32_t pc_is_alive(dungeon_t *d)
{
  return d->pc->alive;
}

void config_pc(dungeon_t *d)
{
  d->pc = malloc(sizeof (*d->pc));
  d->pc->symbol = '@';
  d->pc->position[dim_y] = rand_range(d->rooms->position[dim_y],
                                     (d->rooms->position[dim_y] +
                                      d->rooms->size[dim_y] - 1));
  d->pc->position[dim_x] = rand_range(d->rooms->position[dim_x],
                                     (d->rooms->position[dim_x] +
                                      d->rooms->size[dim_x] - 1));
  d->pc->speed = PC_SPEED;
  d->pc->next_turn = 0;
  d->pc->alive = 1;
  d->pc->pc = malloc(sizeof (*d->pc->pc));
  strncpy(d->pc->pc->name, "Isabella Garcia-Shapiro", sizeof (d->pc->pc->name));
  strncpy(d->pc->pc->catch_phrase,
          "Whatcha doin'?", sizeof (d->pc->pc->name));
  d->pc->npc = NULL;

  d->character[d->pc->position[dim_y]][d->pc->position[dim_x]] = d->pc;

  heap_insert(&d->next_turn, d->pc);
}

uint32_t pc_next_pos(dungeon_t *d, pair_t dir)
{
  dir[dim_y] = dir[dim_x] = 0;

  /* Tunnel to the nearest dungeon corner, then move around in hopes *
   * of killing a couple of monsters before we die ourself.          */

  if (in_corner(d, d->pc)) {
    /*
    dir[dim_x] = (mapxy(d->pc->position[dim_x] - 1,
                        d->pc->position[dim_y]) ==
                  ter_wall_immutable) ? 1 : -1;
    */
    dir[dim_y] = (mapxy(d->pc->position[dim_x],
                        d->pc->position[dim_y] - 1) ==
                  ter_wall_immutable) ? 1 : -1;
  } else {
    dir_nearest_wall(d, d->pc, dir);
  }

  return 0;
}
