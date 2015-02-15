#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <sys/time.h>
#include <string.h>
#include <math.h>

#define DUNGEON_X               160 //160
#define DUNGEON_Y               96 //96
#define MIN_ROOMS               12 //12
#define MAX_ROOMS               24
#define ROOM_MIN_X              8 //8
#define ROOM_MAX_X              38
#define ROOM_MIN_Y              5 //5
#define ROOM_MAX_Y              35
#define ROOM_SEPARATION         3
#define MAX_PLACEMENT_ATTEMPTS  1000
#define MAX_CELL_HARDNESS       255

#define rand_range(min, max) ((rand() % (((max) + 1) - (min))) + (min))

typedef enum terrain_type
{
  ter_wall,
  ter_wall_immutable,
  ter_floor_room,
  ter_floor_corridor
} terrain_type_t;

typedef struct cell
{
  terrain_type_t terrain;
  uint8_t hardness;
} cell_t;

typedef struct room
{
  uint8_t start_x;
  uint8_t start_y;
  uint8_t end_x;
  uint8_t end_y;
  uint32_t connected;
} room_t;

typedef struct dungeon
{
  uint32_t num_rooms;
  room_t rooms[MAX_ROOMS];
  cell_t cells[DUNGEON_Y][DUNGEON_X];
} dungeon_t;

typedef struct path {
  uint8_t pos[2];
  uint8_t from[2];
  uint16_t cost;
} path_t;

typedef struct priority_queue
{
  int length;
  path_t path[DUNGEON_X * DUNGEON_Y];
} priority_queue_t;

void priority_queue_init(priority_queue_t *pq)
{
  pq->length = 0;
}

void priority_queue_perc_up(priority_queue_t *pq, int i_child, int i_parent)
{
  path_t temp_path;

  while (pq->path[i_parent].cost >= pq->path[i_child].cost && i_child != 0)
  {
    temp_path = pq->path[i_parent];
    pq->path[i_parent] = pq->path[i_child];
    pq->path[i_child] = temp_path;

    i_child = i_parent;
    i_parent = (i_child - 1) / 2;
  }

  return;
}

void priority_queue_add_with_priority(priority_queue_t *pq, path_t *path)
{
  printf("add node\n");
  int i_child, i_parent;
  pq->length++;

  i_child = pq->length - 1;
  pq->path[i_child] = *path;
  i_parent = (i_child - 1) / 2;

  priority_queue_perc_up(pq, i_child, i_parent);
}

void priority_queue_decrease_priority(priority_queue_t *pq, path_t *p, int cost)
{
  printf("decrease priority\n");
  int i;

  for (i = 0; i < pq->length; i++)
  {
    if (pq->path[i].pos[0] == p->pos[0] && pq->path[i].pos[1] == p->pos[1])
    {
      int i_child = i;
      int i_parent = (i - 1) / 2;

      pq->path[i].cost = cost;

      priority_queue_perc_up(pq, i_child, i_parent);
      return;
    }
  }
  // error
}

int empty_dungeon(dungeon_t *d)
{
  int x, y;

  for (y = 0; y < DUNGEON_Y; y++)
  {
    for (x = 0; x < DUNGEON_X; x++)
    {
      if (x == 0 || x == DUNGEON_X - 1 ||
          y == 0 || y == DUNGEON_Y - 1)
      {
        d->cells[y][x].terrain = ter_wall_immutable;
        d->cells[y][x].hardness = MAX_CELL_HARDNESS;
      }
      else
      {
        d->cells[y][x].terrain = ter_wall;
        d->cells[y][x].hardness = rand_range(1, MAX_CELL_HARDNESS);
      }
    }
  }

  return 0;
}

int render_dungeon(dungeon_t *d)
{
  int x, y;

  for (y = 0; y < DUNGEON_Y; y++)
  {
    for (x = 0; x < DUNGEON_X; x++)
    {
      switch (d->cells[y][x].terrain)
      {
        case ter_wall_immutable:
          putchar('#');
          break;
        case ter_wall:
          putchar('#');
          break;
        case ter_floor_room:
          putchar('.');
          break;
        case ter_floor_corridor:
          putchar('.');
          break;
      }
    }
    putchar('\n');
  }
  putchar('\n');

  return 0;
}

int is_outermost_wall(int x, int y)
{
  if (x == 0 || x == DUNGEON_X - 1 || y == 0 || y == DUNGEON_Y - 1)
  {
    return 0;
  }

  return -1;
}

int passes_hausdorff(dungeon_t *d, room_t *test_room)
{
  int i, j, k, l, m;

  if (d->num_rooms == 0)
  {
    return 0;
  }

  for (i = 0; i < d->num_rooms; i++)
  {
    for (j = d->rooms[i].start_x; j <= d->rooms[i].end_x; j++)
    {
      for (k = d->rooms[i].start_y; k <= d->rooms[i].end_y; k++)
      {
        for (l = test_room->start_x; l <= test_room->end_x; l++)
        {
          for (m = test_room->start_y; m <= test_room->end_y; m++)
          {
            if (sqrt(pow(j - l, 2) + pow(k - m, 2)) <= ROOM_SEPARATION)
            {
              return -1;
            }
          }
        }
      }
    }
  }
  return 0;
}

int place_room(dungeon_t *d)
{
  int x, y;

  room_t new_room;
  new_room.start_x = rand_range(0, DUNGEON_X);
  new_room.start_y = rand_range(0, DUNGEON_Y);
  new_room.end_x = new_room.start_x + ROOM_MIN_X + rand() % ROOM_MAX_X;
  new_room.end_y = new_room.start_y + ROOM_MIN_Y + rand() % ROOM_MIN_Y;

  if (new_room.start_x < 0 || new_room.start_y < 0)
  {
    return -1;
  }

  if (new_room.end_x >= DUNGEON_X || new_room.end_y >= DUNGEON_Y)
  {
    return -1;
  }

  if (is_outermost_wall(new_room.start_x, new_room.start_y) == 0 ||
      is_outermost_wall(new_room.end_x, new_room.end_y) == 0)
  {
    return -1;
  }

  if (passes_hausdorff(d, &new_room) == -1)
  {
    return -1;
  }

  for (y = new_room.start_y; y <= new_room.end_y; y++)
  {
    for (x = new_room.start_x; x <= new_room.end_x; x++)
    {
      d->cells[y][x].terrain = ter_floor_room;
      d->cells[y][x].hardness = 0;
    }
  }

  d->rooms[d->num_rooms++] = new_room;

  return 0;
}

int generate_rooms(dungeon_t *d)
{
  int attempts;

  for (attempts = 0; attempts < MAX_PLACEMENT_ATTEMPTS; attempts++)
  {
    if (d->num_rooms == MAX_ROOMS)
    {
      return 0;
    }
    place_room(d);
  }

  if (d->num_rooms >= MIN_ROOMS)
  {
    return 0;
  }
  else
  {
    fprintf(stderr, "Max room placement attempts reached. Please try to generate another dungeon\n");
    exit(0);
  }
}

int point_in_room(int x, int y, room_t *r)
{
  return (x >= r->start_x && x <= r->end_x &&
          y >= r->start_y && y <= r->end_y);
}

void connect_points(dungeon_t *dungeon, int x1, int y1, int x2, int y2)
{
  int x_diff = x1 - x2;
  int y_diff = y1 - y2;

  while (x_diff != 0)
  {
    if (x_diff < 0)
    {
      x1++;
    }
    else
    {
      x1--;
    }
    if (dungeon->cells[y1][x1].terrain == ter_floor_room)
    {

    }
    else
    {
      dungeon->cells[y1][x1].terrain = ter_floor_corridor;
      dungeon->cells[y1][x1].hardness = 0;
    }
    x_diff = x1 - x2;
  }

  while (y_diff != 0)
  {
    if (y_diff < 0)
    {
      y1++;
    }
    else
    {
      y1--;
    }
    if (dungeon->cells[y1][x1].terrain == ter_floor_room)
    {

    }
    else
    {
      dungeon->cells[y1][x1].terrain = ter_floor_corridor;
      dungeon->cells[y1][x1].hardness = 0;
    }
    y_diff = y1 - y2;
  }
}

void generate_corridors(dungeon_t *dungeon)
{
  int i;
  int room1X, room1Y, room2X, room2Y;

  for (i = 1; i < dungeon->num_rooms; i++)
  {
    room1X = rand_range(dungeon->rooms[i - 1].start_x, dungeon->rooms[i - 1].end_x);
    room1Y = rand_range(dungeon->rooms[i - 1].start_y, dungeon->rooms[i - 1].end_y);
    room2X = rand_range(dungeon->rooms[i].start_x, dungeon->rooms[i].end_x);
    room2Y = rand_range(dungeon->rooms[i].start_y, dungeon->rooms[i].end_y);
    connect_points(dungeon, room1X, room1Y, room2X, room2Y);
  }
}

int generate_dungeon(dungeon_t *d)
{
  empty_dungeon(d);
  generate_rooms(d);
  generate_corridors(d);

  return 0;
}

int save_dungeon(dungeon_t *d)
{
  int x, y;

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

  free(path);

  // file-type marker with the value RLG229
  char file_marker[] = "RLG229";
  fwrite(&file_marker, 1, 6, f);


  // An unsigned 32-bit integer file version marker with the value 0
  uint32_t version_marker = 0;
  version_marker = htobe32(version_marker);
  fwrite(&version_marker, 4, 1, f);


  // An unsigned 32-bit integer size of the rest of the file
  unsigned int size_of_rest = 61455 + (d->num_rooms * 4) - 14;
  size_of_rest = htobe32(size_of_rest);
  fwrite(&size_of_rest, 4, 1, f);


  // terrain values for the map
  uint8_t open, room, corridor, hardness;
  
  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      switch (d->cells[y][x].terrain) {
      case ter_wall_immutable:
        open = 0;
        room = 0;
        corridor = 0;
        break;
      case ter_wall:
        open = 0;
        room = 0;
        corridor = 0;
        break;
      case ter_floor_room:
        open = 1;
        room = 1;
        corridor = 0;
        break;
      case ter_floor_corridor:
        open = 1;
        room = 0;
        corridor = 1;
        break;
      }

      hardness = d->cells[y][x].hardness;

      fwrite(&open, 1, 1, f);
      fwrite(&room, 1, 1, f);
      fwrite(&corridor, 1, 1, f);
      fwrite(&hardness, 1, 1, f);
    }
  }


  // An unsigned 16-bit integer containing the number of rooms in the dungeon
  uint16_t number_of_rooms = d->num_rooms;
  number_of_rooms = htobe16(number_of_rooms);
  fwrite(&number_of_rooms, 2, 1, f);
  

  // start and ending x and y for all the rooms
  int i;

  for (i = 0; i < d->num_rooms; i++)
  {
    fwrite(&d->rooms[i].start_x, 4, 1, f);
    fwrite(&d->rooms[i].start_y, 4, 1, f);
    fwrite(&d->rooms[i].end_x, 4, 1, f);
    fwrite(&d->rooms[i].end_y, 4, 1, f);
  }

  fclose(f);
  return 0;
}

int load_dungeon(dungeon_t *d)
{
  int i, x, y;
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

  free(path);

  // file-type marker with the value RLG229
  char *file_marker;
  file_marker = malloc(7);

  for (i = 0; i < 6; i++)
  {
    fread(&file_marker[i], 1, 1, f);
  }
  file_marker[6] = '\0';

  printf("file-type: %s\n", file_marker);


  // An unsigned 32-bit integer file version marker with the value 0
  uint32_t version_marker;
  fread(&version_marker, 4, 1, f);
  version_marker = be32toh(version_marker);
  printf("version number: %d\n", version_marker);


  // An unsigned 32-bit integer size of the rest of the file
  uint32_t size_of_rest;
  fread(&size_of_rest, 4, 1, f);
  size_of_rest = be32toh(size_of_rest);
  printf("file Size: %d bytes\n", size_of_rest + 14);
  

  // terrain values for the map
  uint8_t open, room, corridor, hardness;
  
  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      fread(&open, 1, 1, f);
      fread(&room, 1, 1, f);
      fread(&corridor, 1, 1, f);
      fread(&hardness, 1, 1, f);
      
      if (hardness == 255)
      {
        d->cells[y][x].terrain = ter_wall_immutable;
      }
      else if (room != 0)
      {
        d->cells[y][x].terrain = ter_floor_room;
      }
      else if (corridor != 0)
      {
        d->cells[y][x].terrain = ter_floor_corridor;
      }
      else
      {
        d->cells[y][x].terrain = ter_wall;
      }
    }
  }


  // An unsigned 16-bit integer containing the number of rooms in the dungeon
  uint16_t number_of_rooms;
  fread(&number_of_rooms, 2, 1, f);
  number_of_rooms = be16toh(number_of_rooms);
  d->num_rooms = number_of_rooms;


  // Position, width and height of all the rooms
  for (i = 0; i < d->num_rooms; i++)
  {
    fread(&d->rooms[i].start_x, 4, 1, f);
    fread(&d->rooms[i].start_y, 4, 1, f);
    fread(&d->rooms[i].end_x, 4, 1, f);
    fread(&d->rooms[i].end_y, 4, 1, f);
  }

  return 0;
}

int main (int argc, char *argv[])
{
  dungeon_t d;

  srand(time(NULL));
  // srand(1);

  if (argc == 1)
  {
    // generate a new dungeon, render it, and exit
    generate_dungeon(&d);
    render_dungeon(&d);
  }
  else if (argc == 2)
  {
    if (!strcmp(argv[1], "--save"))
    {
      // generate a new dungeon, render it, save it, and exit
      generate_dungeon(&d);
      render_dungeon(&d);
      save_dungeon(&d);
    }
    else if (!strcmp(argv[1], "--load"))
    {
      // load the dungeon from disk, render it, and exit
      load_dungeon(&d);
      render_dungeon(&d);
    }
  }
  else if (argc == 3)
  {
    if ((!strcmp(argv[1], "--save") && !strcmp(argv[2], "--load")) ||
        (!strcmp(argv[1], "--load") && !strcmp(argv[2], "--save")))
    {
      // load the dungeon from disk, render it, save it, and exit
      load_dungeon(&d);
      render_dungeon(&d);
      save_dungeon(&d);
    }
  }
  else
  {
    // there were no valid arguments
    fprintf(stderr, "Bad argument format\n");
    fprintf(stderr, "'--save' and/or '--load' are valid arguments\n");
  }
  return 0;

}
