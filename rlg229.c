#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>

#include "dungeon.h"
#include "character.h"
#include "utils.h"
#include "pc.h"
#include "npc.h"
#include "move.h"
#include "io.h"
#include "parse.h"
static void usage(char *name)
{
  fprintf(stderr,
          "Usage: %s [-r|--rand <seed>]\n"
          "%*c[-n|--nummon <number of monsters>]\n",
          name, (int) strlen(name) + 8, ' ');

  exit(-1);
}

int main(int argc, char *argv[])
{
  return 0;
}
