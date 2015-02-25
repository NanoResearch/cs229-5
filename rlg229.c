#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>

#include "heap.h"
#include "dungeon.h"
#include "utils.h"
#include "move.h"

static void usage(char *name)
{
  fprintf(stderr,
          "Usage: %s [-r|--rand <seed>] [-l|--load [<file>]] [-s|--store]\n"
          "%*c[-n|--nummon <number of monsters>]\n",
          name, (int) strlen(name) + 8, ' ');

  exit(-1);
}

static int32_t turn_cmp(const void *key, const void *with) {
  return ((character_t *) key)->turn - ((character_t *) with)->turn;
}

int main(int argc, char *argv[])
{
  dungeon_t d;
  time_t seed;
  uint32_t nummon;
  struct timeval tv;
  uint32_t i;
  uint32_t do_load, do_store, do_seed;
  uint32_t long_arg;
  char *save_file;
  // uint32_t game_turn = 0;

  /* Default behavior: Seed with the time, generate a new dungeon, *
   * and don't write to disk.                                      */
  do_load = do_store = 0;
  do_seed = 1;
  save_file = NULL;
  nummon = 10;

  /* The project spec requires '--load' and '--store'.  It's common *
   * to have short and long forms of most switches (assuming you    *
   * don't run out of letters).  For now, we've got plenty.  Long   *
   * forms use whole words and take two dashes.  Short forms use an *
   * abbreviation after a single dash.  We'll add '--rand' (to      *
   * specify a random seed), which will take an argument of it's    *
   * own, and we'll add short forms for all three commands, '-l',   *
   * '-s', and '-r', respectively.  We're also going to allow an    *
   * optional argument to load to allow us to load non-default save *
   * files.  No means to store to non-default locations, however.   */
  if (argc > 1) {
    for (i = 1, long_arg = 0; i < argc; i++, long_arg = 0) {
      if (argv[i][0] == '-') { /* All switches start with a dash */
        if (argv[i][1] == '-') {
          argv[i]++;    /* Make the argument have a single dash so we can */
          long_arg = 1; /* handle long and short args at the same place.  */
        }
        switch (argv[i][1]) {
        case 'r':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-rand")) ||
              argc < ++i + 1 /* No more arguments */ ||
              !sscanf(argv[i], "%lu", &seed) /* Argument is not an integer */) {
            usage(argv[0]);
          }
          do_seed = 0;
          break;
        case 'l':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-load"))) {
            usage(argv[0]);
          }
          do_load = 1;
          if ((argc > i + 1) && argv[i + 1][0] != '-') {
            /* There is another argument, and it's not a switch, so *
             * we'll treat it as a save file and try to load it.    */
            save_file = argv[++i];
          }
          break;
        case 's':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-save"))) {
            usage(argv[0]);
          }
          do_store = 1;
          break;
        case 'n':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-nummon")) ||
              argc < ++i + 1 /* No more arguments */ ||
              !sscanf(argv[i], "%u", &nummon) /* Argument is not an int */) {
            usage(argv[0]);
          }
          break;
        default:
          usage(argv[0]);
        }
      } else { /* No dash */
        usage(argv[0]);
      }
    }
  }

  if (do_seed) {
    /* Allows me to generate more than one dungeon *
     * per second, as opposed to time().           */
    gettimeofday(&tv, NULL);
    seed = (tv.tv_usec ^ (tv.tv_sec << 20)) & 0xffffffff;
  }

  printf("Seed is %ld.\n", seed);
  srand(seed);

  d.num_char = nummon + 1;

  init_dungeon(&d);

  if (do_load) {
    read_dungeon(&d, save_file);
  } else {
    gen_dungeon(&d);
    heap_init(&d.turn_list, turn_cmp, NULL);
    gen_characters(&d, &d.turn_list);
  }

  render_dungeon(&d);

  int char_num;
  while (1)
  {
    for (char_num = 0; char_num < d.num_char; char_num++)
    {
      if (d.chars[char_num].alive == 1)
      {
        if (char_num == 0)
        {
          if (move_pc(&d) == 1)
          {
            render_dungeon(&d);
            printf("\nGame over. PC killed all the monsters\n");
            return 0;
          }
        }
        else
        {
          if (move_npc(&d, char_num) == 1)
          {
            render_dungeon(&d);
            printf("\nGame over. PC killed.\n");
            return 0;
          }
        }
      }
    }

    render_dungeon(&d);
    usleep(100000);
  }


  // TODO: Implement turn list using heap
  // character_t *character = heap_peek_min(&d.turn_list);

  // while (1)
  // {
  //   character = heap_peek_min(&d.turn_list);
  //   if (character->turn == game_turn)
  //   {
  //     character = heap_remove_min(&d.turn_list);
  //     if (character->char_num == 0)
  //     {
  //       move_pc(&d);
  //       heap_insert(&d)
  //     }
  //     else
  //     {
  //       move_npc(&d, character->char_num);
  //     }
  //   }
  // }

  if (do_store) {
    write_dungeon(&d);
  }

  delete_dungeon(&d);

  return 0;
}
