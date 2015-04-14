#ifndef DESCRIPTIONS_H
# define DESCRIPTIONS_H

# include <stdint.h>

typedef struct dungeon dungeon_t;

# ifdef __cplusplus

# include <string>
# include "dice.h"

extern "C" {
# endif

uint32_t parse_descriptions(dungeon_t *d);
uint32_t print_descriptions(dungeon_t *d);
uint32_t destroy_descriptions(dungeon_t *d);
uint32_t create_objects(dungeon_t *d);
uint32_t destroy_objects(dungeon_t *d);

typedef enum object_type {
  objtype_no_type,
  objtype_WEAPON,
  objtype_OFFHAND,
  objtype_RANGED,
  objtype_ARMOR,
  objtype_HELMET,
  objtype_CLOAK,
  objtype_GLOVES,
  objtype_BOOTS,
  objtype_RING,
  objtype_AMULET,
  objtype_LIGHT,
  objtype_SCROLL,
  objtype_BOOK,
  objtype_FLASK,
  objtype_GOLD,
  objtype_AMMUNITION,
  objtype_FOOD,
  objtype_WANT,
  objtype_CONTAINER
} object_type_t;

# ifdef __cplusplus
} /* extern "C" */

class monster_description {
 private:
  std::string name, description;
  char symbol;
  uint32_t color, abilities;
  dice speed, hitpoints, damage;
 public:
  monster_description() : name(),       description(), symbol(0),   color(0),
                          abilities(0), speed(),       hitpoints(), damage()
  {
  }
  void set(const std::string &name,
           const std::string &description,
           const char symbol,
           const uint32_t color,
           const dice &speed,
           const uint32_t abilities,
           const dice &hitpoints,
           const dice &damage);
  std::ostream &print(std::ostream &o);
};

class object {
private:
  std::string name, description;
  object_type_t type;
  uint32_t color, hit, dodge, defence, weight, speed, attribute, value;
  dice damage;
public:
  object() : name(),    description(),  type(objtype_no_type),
             color(0),  hit(0),         dodge(0),
             defence(0),     weight(0), speed(0),
             attribute(0),   value(0),  damage()
  {
  }
  void set(const std::string &name,
           const std::string &description,
           const object_type_t type,
           const uint32_t color,
           const uint32_t &hit,
           const uint32_t &dodge,
           const uint32_t &defence,
           const uint32_t &weight,
           const uint32_t &speed,
           const uint32_t &attrubute,
           const uint32_t &value,
           const dice &damage);
  std::ostream &print(std::ostream &o);
};

class object_description {
 private:
  std::string name, description;
  object_type_t type;
  uint32_t color;
  dice hit, damage, dodge, defence, weight, speed, attribute, value;
 public:
  object_description() : name(),    description(), type(objtype_no_type),
                         color(0),  hit(),         damage(),
                         dodge(),   defence(),     weight(),
                         speed(),   attribute(),   value()
  {
  }
  void set(const std::string &name,
           const std::string &description,
           const object_type_t type,
           const uint32_t color,
           const dice &hit,
           const dice &damage,
           const dice &dodge,
           const dice &defence,
           const dice &weight,
           const dice &speed,
           const dice &attrubute,
           const dice &value);
  object create_object();
  std::ostream &print(std::ostream &o);
};

# endif

#endif
