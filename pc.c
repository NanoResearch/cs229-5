#include <stdlib.h>
#include <ncurses.h> /* for COLOR_WHITE */

#include "string.h"

#include "dungeon.h"
#include "pc.h"
#include "utils.h"
#include "move.h"
#include "io.h"
#include "dice.h"

void pc_delete(dungeon_t *d)
{
  destroy_dice((dice_t *) d->pc.damage);
  if (d->pc.pc)
  {
    destroy_pc_objects(d);
    free(d->pc.pc);
  }
}

uint32_t pc_is_alive(dungeon_t *d)
{
  return d->pc.alive;
}

void place_pc(dungeon_t *d)
{
  d->pc.position[dim_y] = rand_range(d->rooms->position[dim_y],
                                     (d->rooms->position[dim_y] +
                                      d->rooms->size[dim_y] - 1));
  d->pc.position[dim_x] = rand_range(d->rooms->position[dim_x],
                                     (d->rooms->position[dim_x] +
                                      d->rooms->size[dim_x] - 1));
}

void config_pc(dungeon_t *d)
{
  int i;

  d->pc.symbol = '@';
  d->pc.color = COLOR_WHITE;

  place_pc(d);

  d->pc.speed = PC_SPEED;
  d->pc.hp = PC_HP;
  d->pc.damage = new_dice(0, 1, 4);
  d->pc.next_turn = 0;
  d->pc.alive = 1;
  d->pc.sequence_number = 0;
  d->pc.pc = malloc(sizeof (*d->pc.pc));
  strncpy(d->pc.pc->name, "Isabella Garcia-Shapiro", sizeof (d->pc.pc->name));
  strncpy(d->pc.pc->catch_phrase,
          "Whatcha doin'?", sizeof (d->pc.pc->name));
  d->pc.pc->weapon = NULL;
  d->pc.pc->offhand = NULL;
  d->pc.pc->ranged = NULL;
  d->pc.pc->armor = NULL;
  d->pc.pc->helmet = NULL;
  d->pc.pc->cloak = NULL;
  d->pc.pc->gloves = NULL;
  d->pc.pc->boots = NULL;
  d->pc.pc->amulet = NULL;
  d->pc.pc->light = NULL;
  d->pc.pc->ring1 = NULL;
  d->pc.pc->ring2 = NULL;
  d->pc.pc->speed = PC_SPEED;

  for (i = 0; i < PC_CARRY; i++)
  {
    d->pc.pc->carry_slots[i] = NULL;
  }

  d->pc.npc = NULL;

  d->character[d->pc.position[dim_y]][d->pc.position[dim_x]] = &d->pc;

  io_calculate_offset(d);

  dijkstra(d);
  dijkstra_tunnel(d);
}

uint32_t pc_next_pos(dungeon_t *d, pair_t dir)
{
  dir[dim_y] = dir[dim_x] = 0;

  /* Tunnel to the nearest dungeon corner, then move around in hopes *
   * of killing a couple of monsters before we die ourself.          */

  if (in_corner(d, &d->pc)) {
    /*
    dir[dim_x] = (mapxy(d->pc.position[dim_x] - 1,
                        d->pc.position[dim_y]) ==
                  ter_wall_immutable) ? 1 : -1;
    */
    dir[dim_y] = (mapxy(d->pc.position[dim_x],
                        d->pc.position[dim_y] - 1) ==
                  ter_wall_immutable) ? 1 : -1;
  } else {
    dir_nearest_wall(d, &d->pc, dir);
  }

  return 0;
}
