/*
 ******************************************************************
 * Unless otherwise noted, All code provided below was created by *
 * Professor Jeremy Sheaffer at Iowa State University. To see my  *
 * solution to making a random dungeon please see:                *
 * assignments/moore_andrew.assignment-1.01/dungeonGenerator.c    *
 ******************************************************************
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <sys/time.h>
#include <string.h>

/* Adds a "room shrinking" phase if it has trouble placing all the rooms. *
 * This allows rooms to become smaller than are specified in the problem  *
 * statement as a minimum room size, so strictly speaking, this is not    *
 * compliant.                                                             */

#define DUNGEON_X              160
#define DUNGEON_Y              96
#define MIN_ROOMS              12
#define MAX_ROOMS              24
#define ROOM_MIN_X             10
#define ROOM_MIN_Y             8
#define ROOM_SEPARATION        5
#define MAX_PLACEMENT_ATTEMPTS 1000

#define rand_under(numerator, denominator) \
  (rand() < ((RAND_MAX / denominator) * numerator))
#define rand_range(min, max) ((rand() % (((max) + 1) - (min))) + (min))

typedef enum dim {
  dim_x,
  dim_y,
  num_dims
} dim_t;

typedef int16_t pair_t[num_dims];

typedef enum terrain_type {
  ter_debug,
  ter_wall,
  ter_wall_no_room,
  ter_wall_no_floor,
  ter_wall_immutable,
  ter_floor,
  ter_floor_room,
  ter_floor_hall,
  ter_floor_tentative
} terrain_type_t;

typedef struct room {
  pair_t position;
  pair_t size;
  uint32_t connected;
  uint8_t exists;
} room_t;

typedef struct dungeon {
  uint32_t num_rooms;
  room_t rooms[MAX_ROOMS];
  uint8_t map[DUNGEON_Y][DUNGEON_X];
} dungeon_t;

int point_in_room_p(room_t *r, pair_t p)
{
  return ((r->position[dim_x] <= p[dim_x]) &&
          ((r->position[dim_x] + r->size[dim_x]) > p[dim_x]) &&
          (r->position[dim_y] <= p[dim_y]) &&
          ((r->position[dim_y] + r->size[dim_y]) > p[dim_y]));
}

/* Randomly decides whether to choose the x or the y direction first,   *
 * then draws the corridor connecting two points with one or two lines. */
int connect_two_points(dungeon_t *d,
                       pair_t min_x, pair_t max_x,
                       pair_t min_y, pair_t max_y)
{
  uint32_t i;
  uint32_t x_first;

  x_first = rand() & 1;

  for (i = min_x[dim_x]; i <= max_x[dim_x]; i++) {
    if (d->map[x_first ? min_y[dim_y] : max_y[dim_y]][i] == ter_wall) {
      d->map[x_first ? min_y[dim_y] : max_y[dim_y]][i] = ter_floor_hall;
    }
  }
  for (i = min_y[dim_y]; i <= max_y[dim_y]; i++) {
    if (d->map[i][x_first ? max_y[dim_x] : min_y[dim_x]] == ter_wall) {
      d->map[i][x_first ? max_y[dim_x] : min_y[dim_x]] = ter_floor_hall;
    }
  }

  return 0;
}

/* Recursively subdivides the space between two points until some minimum *
 * closeness is reached, then draws the corridors connecting them.        */
int connect_two_points_recursive(dungeon_t *d, pair_t e1, pair_t e2)
{
  pair_t mid;
  int16_t *min_x, *min_y, *max_x, *max_y;

  min_x = ((e1[dim_x] < e2[dim_x]) ? e1 : e2);
  max_x = ((min_x == e1) ? e2 : e1);
  min_y = ((e1[dim_y] < e2[dim_y]) ? e1 : e2);
  max_y = ((min_y == e1) ? e2 : e1);

  if ((max_x[dim_x] - min_x[dim_x]) + 
      (max_y[dim_y] - min_y[dim_y]) < 15
) {
    return connect_two_points(d, min_x, max_x, min_y, max_y);
  }

  mid[dim_x] = rand_range(min_x[dim_x], max_x[dim_x]);
  mid[dim_y] = rand_range(min_y[dim_y], max_y[dim_y]);

  connect_two_points_recursive(d, e1, mid);
  connect_two_points_recursive(d, mid, e2);

  return 0;
}

/* Chooses a random point inside each room and connects them with a *
 * corridor.  Random internal points prevent corridors from exiting *
 * rooms in predictable locations.                                  */
int connect_two_rooms(dungeon_t *d, room_t *r1, room_t *r2)
{
  pair_t e1, e2;

  e1[dim_y] = rand_range(r1->position[dim_y],
                         r1->position[dim_y] + r1->size[dim_y] - 1);
  e1[dim_x] = rand_range(r1->position[dim_x],
                         r1->position[dim_x] + r1->size[dim_x] - 1);
  e2[dim_y] = rand_range(r2->position[dim_y],
                         r2->position[dim_y] + r2->size[dim_y] - 1);
  e2[dim_x] = rand_range(r2->position[dim_x],
                         r2->position[dim_x] + r2->size[dim_x] - 1);

  return connect_two_points_recursive(d, e1, e2);
}

int connect_rooms(dungeon_t *d)
{
  uint32_t i, j;
  uint32_t dist, dist_tmp;
  uint32_t connected;
  uint32_t r1, r2;
  uint32_t min_con, max_con;

  connected = 0;

  /* Find the closest pair and connect them.  Do this until everybody *
   * is connected to room zero.  Because of the nature of the path    *
   * drawing algorithm, it's possible for two paths to cross, or for  *
   * a path to touch another room, but what this produces is *almost* *
   * a free tree.                                                     */
  while (!connected) {
    dist = DUNGEON_X + DUNGEON_Y;
    for (i = 0; i < d->num_rooms; i++) {
      for (j = i + 1; j < d->num_rooms; j++) {
        if (d->rooms[i].connected != d->rooms[j].connected) {
          dist_tmp = (abs(d->rooms[i].position[dim_x] -
                          d->rooms[j].position[dim_x]) +
                      abs(d->rooms[i].position[dim_y] -
                          d->rooms[j].position[dim_y]));
          if (dist_tmp < dist) {
            r1 = i;
            r2 = j;
            dist = dist_tmp;
          }
        }
      }
    }
    connect_two_rooms(d, d->rooms + r1, d->rooms + r2);
    min_con = (d->rooms[r1].connected < d->rooms[r2].connected ?
               d->rooms[r1].connected : d->rooms[r2].connected);
    max_con = (d->rooms[r1].connected > d->rooms[r2].connected ?
               d->rooms[r1].connected : d->rooms[r2].connected);
    for (connected = 1, i = 1; i < d->num_rooms; i++) {
      if (d->rooms[i].connected == max_con) {
        d->rooms[i].connected = min_con;
      }
      if (d->rooms[i].connected) {
        connected = 0;
      }
    }
  }

  /* Now let's add a handful of random, extra connections to create *
   * loops and a more exciting look for the dungeon.  Use a do loop *
   * to guarantee that it always adds at least one such path.       */

  do {
    r1 = rand_range(0, d->num_rooms - 1);
    while ((r2 = rand_range(0, d->num_rooms - 1)) == r1)
      ;
    connect_two_rooms(d, d->rooms + r1, d->rooms + r2);
  } while (rand_under(1, 2));

  return 0;
}

int place_room(dungeon_t *d, room_t *r)
{
  pair_t p;
  uint32_t tries;
  uint32_t qualified;

  for (;;) {
    for (tries = 0; tries < MAX_PLACEMENT_ATTEMPTS; tries++) {
      p[dim_x] = rand() % DUNGEON_X;
      p[dim_y] = rand() % DUNGEON_Y;
      if (p[dim_x] - ROOM_SEPARATION < 0                      ||
          p[dim_x] + r->size[dim_x] + ROOM_SEPARATION > DUNGEON_X ||
          p[dim_y] - ROOM_SEPARATION < 0                      ||
          p[dim_y] + r->size[dim_y] + ROOM_SEPARATION > DUNGEON_Y) {
        continue;
      }
      r->position[dim_x] = p[dim_x];
      r->position[dim_y] = p[dim_y];
      for (qualified = 1, p[dim_y] = r->position[dim_y] - ROOM_SEPARATION;
           p[dim_y] < r->position[dim_y] + r->size[dim_y] + ROOM_SEPARATION;
           p[dim_y]++) {
        for (p[dim_x] = r->position[dim_x] - ROOM_SEPARATION;
             p[dim_x] < r->position[dim_x] + r->size[dim_x] + ROOM_SEPARATION;
             p[dim_x]++) {
          if (d->map[p[dim_y]][p[dim_x]] >= ter_floor) {
            qualified = 0;
          }
        }
      }
    }
    if (!qualified) {
      if (rand_under(1, 2)) {
        if (r->size[dim_x] > 2) {
          r->size[dim_x]--;
        } else if (r->size[dim_y] > 2) {
          r->size[dim_y]--;
        }
      } else {
        if (r->size[dim_y] > 2) {
          r->size[dim_y]--;
        } else if (r->size[dim_x] > 2) {
          r->size[dim_x]--;
        }
      }
    } else {
      for (p[dim_y] = r->position[dim_y];
           p[dim_y] < r->position[dim_y] + r->size[dim_y];
           p[dim_y]++) {
        for (p[dim_x] = r->position[dim_x];
             p[dim_x] < r->position[dim_x] + r->size[dim_x];
             p[dim_x]++) {
          d->map[p[dim_y]][p[dim_x]] = ter_floor_room;
        }
      }
      for (p[dim_x] = r->position[dim_x] - 1;
           p[dim_x] < r->position[dim_x] + r->size[dim_x] + 1;
           p[dim_x]++) {
        d->map[r->position[dim_y] - 1][p[dim_x]]                =
          d->map[r->position[dim_y] + r->size[dim_y] + 1][p[dim_x]] =
          ter_wall;
      }
      for (p[dim_y] = r->position[dim_y] - 1;
           p[dim_y] < r->position[dim_y] + r->size[dim_y] + 1;
           p[dim_y]++) {
        d->map[p[dim_y]][r->position[dim_x] - 1]                =
          d->map[p[dim_y]][r->position[dim_x] + r->size[dim_x] + 1] =
          ter_wall;
      }

      return 0;
    }
  }

  return 0;
}

int place_rooms(dungeon_t *d)
{
  uint32_t i;

  for (i = 0; i < d->num_rooms; i++) {
    place_room(d, d->rooms + i);
  }

  return 0;
}

int make_rooms(dungeon_t *d)
{
  uint32_t i;

  for (i = 0; i < MIN_ROOMS; i++) {
    d->rooms[i].exists = 1;
  }
  for (; i < MAX_ROOMS && rand_under(6, 8); i++) {
    d->rooms[i].exists = 1;
  }
  d->num_rooms = i;

  for (; i < MAX_ROOMS; i++) {
    d->rooms[i].exists = 0;
  }

  for (i = 0; i < d->num_rooms; i++) {
    d->rooms[i].size[dim_x] = ROOM_MIN_X;
    d->rooms[i].size[dim_y] = ROOM_MIN_Y;
    while (rand_under(7, 8)) {
      d->rooms[i].size[dim_x]++;
    }
    while (rand_under(3, 4)) {
      d->rooms[i].size[dim_y]++;
    }
    /* Initially, every room is connected only to itself. */
    d->rooms[i].connected = i;
  }

  return 0;
}

int empty_dungeon(dungeon_t *d)
{
  pair_t p;

  for (p[dim_y] = 0; p[dim_y] < DUNGEON_Y; p[dim_y]++) {
    for (p[dim_x] = 0; p[dim_x] < DUNGEON_X; p[dim_x]++) {
      d->map[p[dim_y]][p[dim_x]] = ter_wall;
      if (p[dim_y] == 0 || p[dim_y] == DUNGEON_Y - 1 ||
          p[dim_x] == 0 || p[dim_x] == DUNGEON_X - 1) {
        d->map[p[dim_y]][p[dim_x]] = ter_wall_immutable;
      }
    }
  }

  return 0;
}

int gen_dungeon(dungeon_t *d)
{
  /*
  pair_t p1, p2;

  p1[dim_x] = rand_range(1, 158);
  p1[dim_y] = rand_range(1, 94);
  p2[dim_x] = rand_range(1, 158);
  p2[dim_y] = rand_range(1, 94);
  */

  empty_dungeon(d);

  /*
  connect_two_points_recursive(d, p1, p2);
  return 0;
  */

  make_rooms(d);
  place_rooms(d);
  connect_rooms(d);

  return 0;
}

void render_dungeon(dungeon_t *d)
{
  pair_t p;

  for (p[dim_y] = 0; p[dim_y] < DUNGEON_Y; p[dim_y]++) {
    for (p[dim_x] = 0; p[dim_x] < DUNGEON_X; p[dim_x]++) {
      switch (d->map[p[dim_y]][p[dim_x]]) {
      case ter_wall:
      case ter_wall_no_room:
      case ter_wall_no_floor:
      case ter_wall_immutable:
        putchar('#');
        break;
      case ter_floor:
      case ter_floor_room:
      case ter_floor_hall:
      case ter_floor_tentative:
        putchar('.');
        break;
      case ter_debug:
        putchar('*');
        break;
      }
    }
    putchar('\n');
  }
}

/*
 * Author: Andrew Moore
 */
int save_dungeon(dungeon_t *d)
{
  FILE *f;
  char *path; // Absolute path to the save file
  char *home; // The name of home directory
  char *path_to_file; // The path inside home to the save file

  printf("saving dungeon...\n");

  home = getenv("HOME"); 
  path_to_file = ".rlg229/dungeon";

  path = malloc(strlen(home) + strlen(path_to_file) + 2 /* NULL and internal / */);
  if (!path)
  {
    fprintf(stderr, "malloc failed!\n");
    return 1;
  }

  sprintf(path, "%s/%s", home, path_to_file);

  f = fopen(path, "w");

  /*
   * A semantic file-type marker with the value RLG229
   */
   char file_marker[] = "RLG229";
   fwrite(&file_marker, 1, 6, f);

  /*
   * An unsigned 32-bit integer file version marker with the value 0
   */
  uint32_t version_marker = 6;
  version_marker = htobe32(version_marker);
  fwrite(&version_marker, 4, 1, f);

  /*
   * An unsigned 32-bit integer size of the rest of the file (total size of the file minus
   * 14)
   */
  unsigned int size_of_rest = 61455 + (d->num_rooms * 4) - 14;
  size_of_rest = htobe32(size_of_rest);
  fwrite(&size_of_rest, 4, 1, f);

  /*
   * The row-major dungeon matrix from top to bottom, with four bytes for each cell. In
   * each cell, the first byte is non-zero if the cell represents open space, zero otherwise;
   * the second byte is non-zero if the cell is part of a room, zero otherwise; the third
   * byte is non-zero if the cell is part of a corridor, zero otherwise; and the fourth byte
   * is the unsigned, 8-bit integer hardness value of the cell, where open space has a
   * hardness of 0 and immutable material has a hardness of 255.
   */

  /*
   * An unsigned 16-bit integer containing the number of rooms in the dungeon
   */
  uint16_t number_of_rooms = d->num_rooms;
  printf("room count: %d\n", d->num_rooms);
  number_of_rooms = htobe16(number_of_rooms);
  fwrite(&number_of_rooms, 2, 1, f);
  
  /*
   * The positions of all of the rooms in the dungeon, given with 4 unsigned 8-bit integers
   * each. The first byte is the x position of the upper left corner of the room; the second
   * byte is the y position of the upper left corner of the room; the third byte is the width
   * of the room; and the fourth byte is the height of the room.
   */


  fclose(f);

  return 0;
}

/*
 * Author: Andrew Moore
 */
int load_dungeon(dungeon_t *d)
{
  int i;
  FILE *f;
  char *path; // Absolute path to the save file
  char *home; // The name of home directory
  char *path_to_file; // The path inside home to the save file

  printf("loading dungeon...\n");

  home = getenv("HOME"); 
  path_to_file = ".rlg229/dungeon";

  path = malloc(strlen(home) + strlen(path_to_file) + 2 /* NULL and internal / */);
  if (!path)
  {
    fprintf(stderr, "malloc failed!\n");
    return 1;
  }

  sprintf(path, "%s/%s", home, path_to_file);
  f = fopen(path, "r");

  /*
   * A semantic file-type marker with the value RLG229
   */
  char *file_marker;
  file_marker = malloc(7);

  for (i = 0; i < 6; i++)
  {
    fread(&file_marker[i], 1, 1, f);
  }
  file_marker[6] = '\0';

  printf("file-type: %s\n", file_marker);

  /*
   * An unsigned 32-bit integer file version marker with the value 0
   */
  uint32_t version_marker;
  fread(&version_marker, 4, 1, f);
  version_marker = be32toh(version_marker);
  printf("version number: %d\n", version_marker);

  /*
   * An unsigned 32-bit integer size of the rest of the file (total size of the file minus
   * 14)
   */
  uint32_t size_of_rest;
  fread(&size_of_rest, 4, 1, f);
  size_of_rest = be32toh(size_of_rest);
  printf("file Size: %d bytes\n", size_of_rest + 14);
  
  /*
   * The row-major dungeon matrix from top to bottom, with four bytes for each cell. In
   * each cell, the first byte is non-zero if the cell represents open space, zero otherwise;
   * the second byte is non-zero if the cell is part of a room, zero otherwise; the third
   * byte is non-zero if the cell is part of a corridor, zero otherwise; and the fourth byte
   * is the unsigned, 8-bit integer hardness value of the cell, where open space has a
   * hardness of 0 and immutable material has a hardness of 255.
   */

  /*
   * An unsigned 16-bit integer containing the number of rooms in the dungeon
   */
   uint16_t number_of_rooms;
   fread(&number_of_rooms, 2, 1, f);
   number_of_rooms =  be16toh(number_of_rooms);
   d->num_rooms = number_of_rooms;
   printf("number of rooms: %d\n", number_of_rooms);

  /*
   * The positions of all of the rooms in the dungeon, given with 4 unsigned 8-bit integers
   * each. The first byte is the x position of the upper left corner of the room; the second
   * byte is the y position of the upper left corner of the room; the third byte is the width
   * of the room; and the fourth byte is the height of the room.
   */


  gen_dungeon(d); //delete when done
  return 0;
}

/*
 * Author: Andrew Moore
 */
int main(int argc, char *argv[])
{
  dungeon_t d;

  srand(time(NULL));

  // if (argc == 1)
  // {
  //   // generate a new dungeon, render it, and exit
  //   printf("generate a new dungeon, render it, and exit\n");
  //   gen_dungeon(&d);
  //   render_dungeon(&d);
  //   return 0;
  // }
  // if (argc == 2)
  // {
  //   if (!strcmp(argv[1], "--save"))
  //   {
      // generate a new dungeon, render it, save it, and exit
      printf("generate a new dungeon, render it, save it, and exit\n");
      gen_dungeon(&d);
      render_dungeon(&d);
      save_dungeon(&d);
  //     return 0;
  //   }
  //   if (!strcmp(argv[1], "--load"))
  //   {
  //     // load the dungeon from disk, render it, and exit
  //     printf("load the dungeon from disk, render it, and exit\n");
      load_dungeon(&d);
  //     render_dungeon(&d);
  //     return 0;
  //   }
  // }
  // if (argc == 3)
  // {
  //   if ((!strcmp(argv[1], "--save") && !strcmp(argv[2], "--load")) ||
  //       (!strcmp(argv[1], "--load") && !strcmp(argv[2], "--save")))
  //   {
  //     // load the dungeon from disk, render it, save it, and exit
  //     printf("load the dungeon from disk, render it, rewrite it, and exit\n");
  //     load_dungeon(&d);
  //     render_dungeon(&d);
  //     save_dungeon(&d);
  //     return 0;
  //   }
  // }
  
  // // there were no valid arguments
  // fprintf(stderr, "Bad argument format\n '--save' and '--load' are valid arguments\n");

  return 0;
}