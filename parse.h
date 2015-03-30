#ifndef PARSE_H
# define PARSE_H

# ifdef __cplusplus
extern "C" {
# endif

struct dungeon_t;

void parse_monster_descriptions(dungeon_t *d);
void print_monster_descriptions(dungeon_t *d);
void destroy_monster_descriptions(dungeon_t *d);

class monster_description {
};

# ifdef __cplusplus
}
# endif

#endif
