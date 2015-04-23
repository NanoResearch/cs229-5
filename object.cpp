#include <vector>
#include <ncurses.h> /* for COLOR_BLACK and COLOR_WHITE */

#include "object.h"
#include "dungeon.h"
#include "utils.h"
#include "pc.h"

object::object(const object_description &o, pair_t p, object *next) :
  name(o.get_name()),
  description(o.get_description()),
  type(o.get_type()),
  color(o.get_color()),
  damage(o.get_damage()),
  hit(o.get_hit().roll()),
  dodge(o.get_dodge().roll()),
  defence(o.get_defence().roll()),
  weight(o.get_weight().roll()),
  speed(o.get_speed().roll()),
  attribute(o.get_attribute().roll()),
  value(o.get_value().roll()),
  next(next)
{
  position[dim_x] = p[dim_x];
  position[dim_y] = p[dim_y];
}

object::~object()
{
  if (next) {
    delete next;
  }
}

void gen_object(dungeon_t *d)
{
  object *o;
  const std::vector<object_description> &v =
    *((std::vector<object_description> *) d->object_descriptions);
  const object_description &od = v[rand_range(0, v.size() - 1)];
  uint32_t room;
  pair_t p;

  room = rand_range(0, d->num_rooms - 1);
  p[dim_y] = rand_range(d->rooms[room].position[dim_y],
                        (d->rooms[room].position[dim_y] +
                         d->rooms[room].size[dim_y] - 1));
  p[dim_x] = rand_range(d->rooms[room].position[dim_x],
                        (d->rooms[room].position[dim_x] +
                         d->rooms[room].size[dim_x] - 1));

  o = new object(od, p, (object *) d->object[p[dim_y]][p[dim_x]]);

  d->object[p[dim_y]][p[dim_x]] = (object_t *) o;  
}

void gen_objects(dungeon_t *d, uint32_t numobj)
{
  uint32_t i;

  d->num_objects = numobj;
  for (i = 0; i < numobj; i++) {
    gen_object(d);
  }
}

char get_symbol(object_t *o)
{
  return ((object *) o)->next ? '&' : object_symbol[((object *) o)->type];
}

uint32_t get_color(object_t *o)
{
  return (((object *) o)->color == COLOR_BLACK ?
          COLOR_WHITE                          :
          ((object *) o)->color);
}

const char * get_obj_name(object_t *o)
{
  return ((object *) o)->name.c_str();
}

void destroy_objects(dungeon_t *d)
{
  uint32_t y, x;

  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      if (d->object[y][x]) {
        delete (object *) d->object[y][x];
        d->object[y][x] = 0;
      }
    }
  }
}

void destroy_pc_objects(dungeon_t *d)
{
  uint32_t i;

  for (i = 0; i < PC_CARRY; i++)
  {
    if (d->pc.pc->carry_slots[i]) {
      delete (object *) d->pc.pc->carry_slots[i];
      d->pc.pc->carry_slots[i] = 0;
    }
  }

  if (d->pc.pc->weapon)
  {
    delete (object *) d->pc.pc->weapon;
    d->pc.pc->weapon = 0;
  }
  if (d->pc.pc->offhand)
  {
    delete (object *) d->pc.pc->offhand;
    d->pc.pc->offhand = 0;
  }
  if (d->pc.pc->ranged)
  {
    delete (object *) d->pc.pc->ranged;
    d->pc.pc->ranged = 0;
  }
  if (d->pc.pc->armor)
  {
    delete (object *) d->pc.pc->armor;
    d->pc.pc->armor = 0;
  }
  if (d->pc.pc->helmet)
  {
    delete (object *) d->pc.pc->helmet;
    d->pc.pc->helmet = 0;
  }
  if (d->pc.pc->cloak)
  {
    delete (object *) d->pc.pc->cloak;
    d->pc.pc->cloak = 0;
  }
  if (d->pc.pc->gloves)
  {
    delete (object *) d->pc.pc->gloves;
    d->pc.pc->gloves = 0;
  }
  if (d->pc.pc->boots)
  {
    delete (object *) d->pc.pc->boots;
    d->pc.pc->boots = 0;
  }
  if (d->pc.pc->amulet)
  {
    delete (object *) d->pc.pc->amulet;
    d->pc.pc->amulet = 0;
  }
  if (d->pc.pc->light)
  {
    delete (object *) d->pc.pc->light;
    d->pc.pc->light = 0;
  }
  if (d->pc.pc->ring1)
  {
    delete (object *) d->pc.pc->ring1;
    d->pc.pc->ring1 = 0;
  }
  if (d->pc.pc->ring2)
  {
    delete (object *) d->pc.pc->ring2;
    d->pc.pc->ring2 = 0;
  }
}

uint32_t location_contains_object(dungeon_t *d, pair_t next)
{
  if (d->object[next[dim_y]][next[dim_x]])
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

void pickup_object(dungeon_t *d, pair_t next)
{
  uint32_t i;

  for (i = 0; i < PC_CARRY; i++)
  {
    if (!d->pc.pc->carry_slots[i])
    {
      d->pc.pc->carry_slots[i] = d->object[next[dim_y]][next[dim_x]];
      d->object[next[dim_y]][next[dim_x]] = 0;
      return;
    }
  }
}

// Wear an item. If an item of that type is already equipped, items are swapped.
uint32_t wear_object(dungeon_t *d, char key)
{
  return 0;
}

// Take off an item. Item goes to an open carry slot.
uint32_t remove_object(dungeon_t *d, char key)
{
  return 0;
}

// Drop an item. Item goes to floor.
uint32_t drop_object(dungeon_t *d, char key)
{
  return 0;
}

// Expunge an item from the game. Item is permanently removed from the game.
uint32_t expunge_object(dungeon_t *d, char key)
{
  return 0;
}
