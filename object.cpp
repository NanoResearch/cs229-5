#include <vector>
#include <ncurses.h> /* for COLOR_BLACK and COLOR_WHITE */

#include "object.h"
#include "dungeon.h"
#include "utils.h"
#include "pc.h"
#include "descriptions.h"

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
  uint32_t room, blocked;
  pair_t p;

  do
  {
    room = rand_range(0, d->num_rooms - 1);
    p[dim_y] = rand_range(d->rooms[room].position[dim_y],
                          (d->rooms[room].position[dim_y] +
                           d->rooms[room].size[dim_y] - 1));
    p[dim_x] = rand_range(d->rooms[room].position[dim_x],
                        (d->rooms[room].position[dim_x] +
                         d->rooms[room].size[dim_x] - 1));
    if (d->object[p[dim_y]][p[dim_x]])
    {
      blocked = 1;
    }
    else
    {
      blocked = 0;
    }

  } while (blocked);

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

object_type get_obj_type(object_t *o)
{
  return ((object *) o)->type;
}

int32_t get_obj_speed(object_t *o)
{
  return ((object *) o)->speed;
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

void calculate_pc_speed(dungeon_t *d)
{
  int32_t speed;

  speed = PC_SPEED;

  if (d->pc.pc->weapon)
  {
    speed += get_obj_speed(d->pc.pc->weapon);
  }
  if (d->pc.pc->offhand)
  {
    speed += get_obj_speed(d->pc.pc->offhand);
  }
  if (d->pc.pc->ranged)
  {
    speed += get_obj_speed(d->pc.pc->ranged);
  }
  if (d->pc.pc->armor)
  {
    speed += get_obj_speed(d->pc.pc->armor);
  }
  if (d->pc.pc->helmet)
  {
    speed += get_obj_speed(d->pc.pc->helmet);
  }
  if (d->pc.pc->cloak)
  {
    speed += get_obj_speed(d->pc.pc->cloak);
  }
  if (d->pc.pc->gloves)
  {
    speed += get_obj_speed(d->pc.pc->gloves);
  }
  if (d->pc.pc->boots)
  {
    speed += get_obj_speed(d->pc.pc->boots);
  }
  if (d->pc.pc->amulet)
  {
    speed += get_obj_speed(d->pc.pc->amulet);
  }
  if (d->pc.pc->light)
  {
    speed += get_obj_speed(d->pc.pc->light);
  }
  if (d->pc.pc->ring1)
  {
    speed += get_obj_speed(d->pc.pc->ring1);
  }
  if (d->pc.pc->ring2)
  {
    speed += get_obj_speed(d->pc.pc->ring2);
  }

  if (speed <= 1)
  {
    speed = 2;
  }

  d->pc.pc->speed = speed;
}

// Wear an item. If an item of that type is already equipped, items are swapped.
uint32_t wear_object(dungeon_t *d, char key)
{
  object_type type;
  uint32_t slot;
  object_t * temp;

  switch (key)
  {
    case '0':
      if (d->pc.pc->carry_slots[0])
      {
        type = get_obj_type(d->pc.pc->carry_slots[0]);
        slot = 0;
        break;
      }
    case '1':
      if (d->pc.pc->carry_slots[1])
      {
        type = get_obj_type(d->pc.pc->carry_slots[1]);
        slot = 1;
        break;
      }
    case '2':
      if (d->pc.pc->carry_slots[2])
      {
        type = get_obj_type(d->pc.pc->carry_slots[2]);
        slot = 2;
        break;
      }
    case '3':
      if (d->pc.pc->carry_slots[3])
      {
        type = get_obj_type(d->pc.pc->carry_slots[3]);
        slot = 3;
        break;
      }
    case '4':
      if (d->pc.pc->carry_slots[4])
      {
        type = get_obj_type(d->pc.pc->carry_slots[4]);
        slot = 4;
        break;
      }
    case '5':
      if (d->pc.pc->carry_slots[5])
      {
        type = get_obj_type(d->pc.pc->carry_slots[5]);
        slot = 5;
        break;
      }
    case '6':
      if (d->pc.pc->carry_slots[6])
      {
        type = get_obj_type(d->pc.pc->carry_slots[6]);
        slot = 6;
        break;
      }
    case '7':
      if (d->pc.pc->carry_slots[7])
      {
        type = get_obj_type(d->pc.pc->carry_slots[7]);
        slot = 7;
        break;
      }
    case '8':
      if (d->pc.pc->carry_slots[8])
      {
        type = get_obj_type(d->pc.pc->carry_slots[8]);
        slot = 8;
        break;
      }
    case '9':
      if (d->pc.pc->carry_slots[9])
      {
        type = get_obj_type(d->pc.pc->carry_slots[9]);
        slot = 9;
        break;
      }
    default:
      return 1;
  }

  switch (type)
  {
    case objtype_WEAPON:
      temp = d->pc.pc->carry_slots[slot];
      d->pc.pc->carry_slots[slot] = d->pc.pc->weapon;
      d->pc.pc->weapon = temp;
      break;
    case objtype_OFFHAND:
      temp = d->pc.pc->carry_slots[slot];
      d->pc.pc->carry_slots[slot] = d->pc.pc->offhand;
      d->pc.pc->offhand = temp;
      break;
    case objtype_RANGED:
      temp = d->pc.pc->carry_slots[slot];
      d->pc.pc->carry_slots[slot] = d->pc.pc->ranged;
      d->pc.pc->ranged = temp;
      break;
    case objtype_ARMOR:
      temp = d->pc.pc->carry_slots[slot];
      d->pc.pc->carry_slots[slot] = d->pc.pc->armor;
      d->pc.pc->armor = temp;
      break;
    case objtype_HELMET:
      temp = d->pc.pc->carry_slots[slot];
      d->pc.pc->carry_slots[slot] = d->pc.pc->helmet;
      d->pc.pc->helmet = temp;
      break;
    case objtype_CLOAK:
      temp = d->pc.pc->carry_slots[slot];
      d->pc.pc->carry_slots[slot] = d->pc.pc->cloak;
      d->pc.pc->cloak = temp;
      break;
    case objtype_GLOVES:
      temp = d->pc.pc->carry_slots[slot];
      d->pc.pc->carry_slots[slot] = d->pc.pc->gloves;
      d->pc.pc->gloves = temp;
      break;
    case objtype_BOOTS:
      temp = d->pc.pc->carry_slots[slot];
      d->pc.pc->carry_slots[slot] = d->pc.pc->boots;
      d->pc.pc->boots = temp;
      break;
    case objtype_RING:
      temp = d->pc.pc->carry_slots[slot];
      // ring 1 is the previous #2 ring
      d->pc.pc->carry_slots[slot] = d->pc.pc->ring1;
      d->pc.pc->ring1 = d->pc.pc->ring2;
      // ring 2 is the new ring
      d->pc.pc->ring2 = temp;
      break;
    case objtype_AMULET:
      temp = d->pc.pc->carry_slots[slot];
      d->pc.pc->carry_slots[slot] = d->pc.pc->amulet;
      d->pc.pc->amulet = temp;
      break;
    case objtype_LIGHT:
      temp = d->pc.pc->carry_slots[slot];
      d->pc.pc->carry_slots[slot] = d->pc.pc->light;
      d->pc.pc->light = temp;
      break;
    default:
      return 1;
  }

  calculate_pc_speed(d);

  return 0;
}

// Take off an item. Item goes to an open carry slot.
uint32_t remove_object(dungeon_t *d, char key)
{
  uint32_t i;

  for (i = 0; i < PC_CARRY; i++)
  {
    if (!d->pc.pc->carry_slots[i])
    {
      switch (key) {
        case 'a':
          d->pc.pc->carry_slots[i] = d->pc.pc->weapon;
          d->pc.pc->weapon = 0;
          break;
        case 'b':
          d->pc.pc->carry_slots[i] = d->pc.pc->offhand;
          d->pc.pc->offhand = 0;
          break;
        case 'c':
          d->pc.pc->carry_slots[i] = d->pc.pc->ranged;
          d->pc.pc->ranged = 0;
          break;
        case 'd':
          d->pc.pc->carry_slots[i] = d->pc.pc->armor;
          d->pc.pc->armor = 0;
          break;
        case 'e':
          d->pc.pc->carry_slots[i] = d->pc.pc->helmet;
          d->pc.pc->helmet = 0;
          break;
        case 'f':
          d->pc.pc->carry_slots[i] = d->pc.pc->cloak;
          d->pc.pc->cloak = 0;
          break;
        case 'g':
          d->pc.pc->carry_slots[i] = d->pc.pc->gloves;
          d->pc.pc->gloves = 0;
          break;
        case 'h':
          d->pc.pc->carry_slots[i] = d->pc.pc->boots;
          d->pc.pc->boots = 0;
          break;
        case 'i':
          d->pc.pc->carry_slots[i] = d->pc.pc->amulet;
          d->pc.pc->amulet = 0;
          break;
        case 'j':
          d->pc.pc->carry_slots[i] = d->pc.pc->light;
          d->pc.pc->light = 0;
          break;
        case 'k':
          d->pc.pc->carry_slots[i] = d->pc.pc->ring1;
          d->pc.pc->ring1 = 0;
          break;
        case 'l':
          d->pc.pc->carry_slots[i] = d->pc.pc->ring2;
          d->pc.pc->ring2 = 0;
          break;
        default:
          return 1;
      }
      calculate_pc_speed(d);
      return 0;
    }
  }

  return 1;
}

// Drop an item. Item goes to floor.
uint32_t drop_object(dungeon_t *d, char key)
{
  if (d->object[d->pc.position[dim_y]][d->pc.position[dim_x]])
  {
    return 1;
  }
  switch (key) {
    case '0':
      d->object[d->pc.position[dim_y]][d->pc.position[dim_x]] = d->pc.pc->carry_slots[0];
      d->pc.pc->carry_slots[0] = 0;
      break;
    case '1':
      d->object[d->pc.position[dim_y]][d->pc.position[dim_x]] = d->pc.pc->carry_slots[1];
      d->pc.pc->carry_slots[1] = 0;
      break;
    case '2':
      d->object[d->pc.position[dim_y]][d->pc.position[dim_x]] = d->pc.pc->carry_slots[2];
      d->pc.pc->carry_slots[2] = 0;
      break;
    case '3':
      d->object[d->pc.position[dim_y]][d->pc.position[dim_x]] = d->pc.pc->carry_slots[3];
      d->pc.pc->carry_slots[3] = 0;
      break;
    case '4':
      d->object[d->pc.position[dim_y]][d->pc.position[dim_x]] = d->pc.pc->carry_slots[4];
      d->pc.pc->carry_slots[4] = 0;
      break;
    case '5':
      d->object[d->pc.position[dim_y]][d->pc.position[dim_x]] = d->pc.pc->carry_slots[5];
      d->pc.pc->carry_slots[5] = 0;
      break;
    case '6':
      d->object[d->pc.position[dim_y]][d->pc.position[dim_x]] = d->pc.pc->carry_slots[6];
      d->pc.pc->carry_slots[6] = 0;
      break;
    case '7':
      d->object[d->pc.position[dim_y]][d->pc.position[dim_x]] = d->pc.pc->carry_slots[7];
      d->pc.pc->carry_slots[7] = 0;
      break;
    case '8':
      d->object[d->pc.position[dim_y]][d->pc.position[dim_x]] = d->pc.pc->carry_slots[8];
      d->pc.pc->carry_slots[8] = 0;
      break;
    case '9':
      d->object[d->pc.position[dim_y]][d->pc.position[dim_x]] = d->pc.pc->carry_slots[9];
      d->pc.pc->carry_slots[9] = 0;
      break;
    default:
      return 1;
      break;
  }
  return 0;
}

// Expunge an item from the game. Item is permanently removed from the game.
uint32_t expunge_object(dungeon_t *d, char key)
{
  switch (key) {
    case '0':
      delete (object *) d->pc.pc->carry_slots[0];
      d->pc.pc->carry_slots[0] = 0;
      break;
    case '1':
      delete (object *) d->pc.pc->carry_slots[1];
      d->pc.pc->carry_slots[1] = 0;
      break;
    case '2':
      delete (object *) d->pc.pc->carry_slots[2];
      d->pc.pc->carry_slots[2] = 0;
      break;
    case '3':
      delete (object *) d->pc.pc->carry_slots[3];
      d->pc.pc->carry_slots[3] = 0;
      break;
    case '4':
      delete (object *) d->pc.pc->carry_slots[4];
      d->pc.pc->carry_slots[4] = 0;
      break;
    case '5':
      delete (object *) d->pc.pc->carry_slots[5];
      d->pc.pc->carry_slots[5] = 0;
      break;
    case '6':
      delete (object *) d->pc.pc->carry_slots[6];
      d->pc.pc->carry_slots[6] = 0;
      break;
    case '7':
      delete (object *) d->pc.pc->carry_slots[7];
      d->pc.pc->carry_slots[7] = 0;
      break;
    case '8':
      delete (object *) d->pc.pc->carry_slots[8];
      d->pc.pc->carry_slots[8] = 0;
      break;
    case '9':
      delete (object *) d->pc.pc->carry_slots[9];
      d->pc.pc->carry_slots[9] = 0;
      break;
    default:
      return 1;
      break;
  } 
  return 0;
}
