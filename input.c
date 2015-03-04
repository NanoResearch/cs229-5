# include <stdio.h>
# include <stdint.h>
# include <stdlib.h>
# include <ncurses.h>

# include "input.h"
# include "dims.h"
# include "dungeon.h"
# include "move.h"
# include "character.h"

void io_init_terminal()
{
  initscr();
  raw();
  noecho();
  curs_set(0);
  keypad(stdscr, TRUE);
}

void io_kill_terminal()
{
  endwin();
}

uint32_t get_input_terminal(dungeon_t *d, pair_t dir)
{
  int ctrl_mode = 1;
  int player_moved = 0;

  dir[dim_y] = dir[dim_x] = 0;

  while(1)
  {
    short input = getch();

    if (ctrl_mode == 1)
    {
      switch(input) {
        case '7':
        case 'y':
          if (d->pc->position[dim_x] != 1 && d->pc->position[dim_y] != 1)
          {
            dir[dim_y] = -1;
            dir[dim_x] = -1;
          }
          player_moved = 1;
          break;
        case '8':
        case 'k':
          if (d->pc->position[dim_y] != 1)
          {
            dir[dim_y] = -1;
          }
          player_moved = 1;
          break;
        case '9':
        case 'u':
          if (d->pc->position[dim_x] != DUNGEON_X - 2 && d->pc->position[dim_y] != 1)
          {
            dir[dim_y] = -1;
            dir[dim_x] = 1;
          }
          player_moved = 1;
          break;
        case '6':
        case 'l':
          if (d->pc->position[dim_x] != DUNGEON_X - 2)
          {
            dir[dim_x] = 1;
          }
          player_moved = 1;
          break;
        case '3':
        case 'n':
          if (d->pc->position[dim_x] != DUNGEON_X - 2 && d->pc->position[dim_y] != DUNGEON_Y - 2)
          {
            dir[dim_y] = 1;
            dir[dim_x] = 1;
          }
          player_moved = 1;
          break;
        case '2':
        case 'j':
          if (d->pc->position[dim_y] != DUNGEON_Y - 2)
          {
            dir[dim_y] = 1;
          }
          player_moved = 1;
          break;
        case '1':
        case 'b':
          if (d->pc->position[dim_x] != 1 && d->pc->position[dim_y] != DUNGEON_Y - 2)
          {
            dir[dim_y] = 1;
            dir[dim_x] = -1;
          }
          player_moved = 1;
          break;
        case '4':
        case 'h':
          if (d->pc->position[dim_x] != 1)
          {
            dir[dim_x] = -1;
          }
          player_moved = 1;
          break;
        case '>':
          break;
        case '<':
          break;
        case ' ':
          player_moved = 1;
          break;
        case 'L':
          break;
        case 'S':
          delete_dungeon(d);
          io_kill_terminal();
          exit(0);
      }
    }
    else
    {
      switch(input) {
        case '8':
        case 'k':
          break;
        case '6':
        case 'l':
          break;
        case '2':
        case 'j':
          break;
        case '4':
        case 'h':
          break;
        case 27:
          break;
        case 'S':
          delete_dungeon(d);
          io_kill_terminal();
          exit(0);
      }
    }

    if (player_moved == 1)
    {
      return 0;
    }
  }
}









//     if ()
    
//     else if (input == 'S')
//     {
//       delete_dungeon(d);
//       io_kill_terminal();
//       exit(0);
//     }

//     else if (input == 'c')
//     {
//       dir[dim_y] = dir[dim_x] = 0;

//       /* Tunnel to the nearest dungeon corner, then move around in hopes *
//        * of killing a couple of monsters before we die ourself.          */

//       if (in_corner(d, d->pc)) {
        
//         dir[dim_x] = (mapxy(d->pc->position[dim_x] - 1,
//                             d->pc->position[dim_y]) ==
//                       ter_wall_immutable) ? 1 : -1;
        
//         dir[dim_y] = (mapxy(d->pc->position[dim_x],
//                             d->pc->position[dim_y] - 1) ==
//                       ter_wall_immutable) ? 1 : -1;
//       } else {
//         dir_nearest_wall(d, d->pc, dir);
//       }

//       return 0;
//     }
//   }
// }