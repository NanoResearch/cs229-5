#ifndef OBJECT_H
# define OBJECT_H

# ifdef __cplusplus

# include <string>

# include "descriptions.h"
# include "dims.h"

extern "C" {
# endif

typedef struct object_t {
} object_t;

void gen_objects(dungeon_t *d, uint32_t numobj);
char get_symbol(object_t *o);
uint32_t get_color(object_t *o);
const char * get_obj_name(object_t *o);
void destroy_objects(dungeon_t *d);
void destroy_pc_objects(dungeon_t *d);
uint32_t location_contains_object(dungeon_t *d, pair_t next);
void pickup_object(dungeon_t *d, pair_t next);
void calculate_pc_speed(dungeon_t *d);
uint32_t wear_object(dungeon_t *d, char key);
uint32_t remove_object(dungeon_t *d, char key);
uint32_t drop_object(dungeon_t *d, char key);
uint32_t expunge_object(dungeon_t *d, char key);

# ifdef __cplusplus

} /* extern "C" */

class object {
 private:
  const std::string &name;
  const std::string &description;
  object_type_t type;
  uint32_t color;
  pair_t position;
  const dice &damage;
  int32_t hit, dodge, defence, weight, speed, attribute, value;
  object *next;
 public:
  object(const object_description &o, pair_t p, object *next);
  ~object();

  friend char get_symbol(object_t *o);
  friend uint32_t get_color(object_t *o);
  friend const char * get_obj_name(object_t *o);
  friend object_type_t get_obj_type(object_t *o);
  friend int32_t get_obj_speed(object_t *o);
};

# endif

#endif
