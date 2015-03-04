#ifndef INPUT_H
# define INPUT_H

#include <stdio.h>
#include <ncurses.h>

#include "dungeon.h"
#include "dims.h"

void io_init_terminal(void);
void io_kill_terminal(void);
uint32_t get_input_terminal(dungeon_t *d, pair_t dir);

#endif