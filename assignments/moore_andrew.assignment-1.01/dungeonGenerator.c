#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#define DUNGEON_X       160
#define DUNGEON_Y       96
#define MIN_ROOMS       12
#define MAX_ROOMS       20
#define ROOM_MIN_X      8
#define ROOM_MIN_Y      5
#define ROOM_MAX_X      35  
#define ROOM_MAX_Y      10
#define ROOM_SEPARATION 6

/*
 * Structure:   cell
 * ---------------------
 * the cell contains a symbol that can be printed to the console,
 * and a mutable flag
 */
typedef struct cell{
    char symbol;
    int mutable;
} cell_t;

/*
 * Structure:   room
 * ---------------------
 * the room contains x and y values (coordinates) for both the start
 * of the room and the end of the room
 */
typedef struct room{
    int start_x;
    int start_y;
    int end_x;
    int end_y;
} room_t;

/*
 * Structure:   dungeon
 * ---------------------
 * the dungeon structure contains a grid of cells and 
 * a list of rooms
 */
typedef struct dungeon{
    int num_rooms;
    cell_t cell[DUNGEON_X][DUNGEON_Y];
    room_t rooms[MIN_ROOMS];
} dungeon_t;

/*
 * Function:    print_dungeon
 * --------------------------
 * prints the dungeon map to the console
 *  
 *  dungeon: the dungeon to print to the console
 */
void print_dungeon(dungeon_t *dungeon)
{
    int x, y;
    for (y = 0; y < DUNGEON_Y; y++)
    {
        for (x = 0; x < DUNGEON_X; x++)
        {
            printf("%c", dungeon->cell[x][y].symbol);
        }
        printf("\n");
    }
    printf("\n");
}

/*
 * Function:    is_outermost_wall
 * ------------------------------
 * checks whether the given x and y values are part of the dungeon border
 *  
 *  x: x coordinate to check
 *  y: y coordinate to check
 *
 *  returns: 1 if the coordinates are part of the wall, -1 otherwise
 */
int is_outermost_wall(int x, int y)
{
    if (x == 0 || x == DUNGEON_X - 1 || y == 0 || y == DUNGEON_Y - 1)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

/*
 * Function:    passes_hausdorff
 * -----------------------------
 * checks whether the given room is further than hausdorff's distance from all
 * other rooms in the given dungeon
 *  
 *  dungeon: dungeon with rooms to check against
 *  test_room: room to compare with current dungeon rooms
 *
 *  returns: 0 if the rooms hausdorff distance is greater than 3 from all
 *           other rooms
 *           in the dungeon
 */
int passes_hausdorff(dungeon_t *dungeon, room_t *test_room)
{
    int i, j, k, l, m;

    if (dungeon->num_rooms <= 0)
    {
        return 0;
    }

    for (i = 0; i < dungeon->num_rooms; i++)
    {
        for (j = dungeon->rooms[i].start_x; j <= dungeon->rooms[i].end_x; j++)
        {
            for (k = dungeon->rooms[i].start_y; k <= dungeon->rooms[i].end_y; k++)
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

/*
 * Function:    create_room
 * ------------------------
 * creates a room and adds it to the given dungeon if it has all the valid
 * criterion
 *  
 *  start_x: starting x coordinate of the room
 *  start_y: starting y coordinate of the room
 *  dungeon: dungeon that will contain the new room
 *
 *  returns: 0 if the room is added successfully, -1 otherwise
 */
int create_room(int start_x, int start_y, dungeon_t *dungeon)
{
    int x, y;
    int end_x = start_x + ROOM_MIN_X + rand() % ROOM_MAX_X;
    int end_y = start_y + ROOM_MIN_Y + rand() % ROOM_MAX_Y;

    if (dungeon->num_rooms == MIN_ROOMS)
    {
        return -1;
    }

    if (start_x < 0 || start_y < 0)
    {
        return -1;
    }

    if (end_x >= DUNGEON_X || end_y >= DUNGEON_Y)
    {
        return -1;
    }

    if (is_outermost_wall(start_x, start_y) == 0 ||
        is_outermost_wall(end_x, end_y) == 0)
    {
        return -1;
    }

    room_t test_room;
    test_room.start_x = start_x;
    test_room.start_y = start_y;
    test_room.end_x = end_x;
    test_room.end_y = end_y;

    if (passes_hausdorff(dungeon, &test_room) == -1)
    {
        return -1;
    }

    for (y = start_y; y <= end_y; y++)
    {
        for (x = start_x; x <= end_x; x++)
        {
            dungeon->cell[x][y].symbol = '.';
        }
    }

    dungeon->rooms[dungeon->num_rooms] = test_room;
    dungeon->num_rooms++;

    return 0;
}

/*
 * Function:    connect_points
 * ----------------------------
 * given a dungeon and two points, connects the two points in the dungeon
 *  
 *  dungeon: starting x coordinate of the room
 *  x1: x location of point one
 *  y1: y location of point one
 *  x2: x location of point two
 *  y2: y location of point two
 */
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
        dungeon->cell[x1][y1].symbol = '.';
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
        dungeon->cell[x1][y1].symbol = '.';
        y_diff = y1 - y2;
    }
}

/*
 * Function:    create_corridors
 * ------------------------------
 * loops through the rooms stored in a given dungeon and connects
 * the rooms one by one
 *
 *  dungeon: dungeon the corridors will be added to
 *
 */
void create_corridors(dungeon_t *dungeon)
{
    int i;
    int room1X, room1Y, room2X, room2Y;

    for (i = 1; i < MIN_ROOMS; i++)
    {
        room1X = dungeon->rooms[i - 1].start_x + (dungeon->rooms[i - 1].end_x - dungeon->rooms[i - 1].start_x) / 2;
        room1Y = dungeon->rooms[i - 1].start_y + (dungeon->rooms[i - 1].end_y - dungeon->rooms[i - 1].start_y) / 2;
        room2X = dungeon->rooms[i].start_x + (dungeon->rooms[i].end_x - dungeon->rooms[i].start_x) / 2;
        room2Y = dungeon->rooms[i].start_y + (dungeon->rooms[i].end_y - dungeon->rooms[i].start_y) / 2;
        connect_points(dungeon, room1X, room1Y, room2X, room2Y);
    }
}

/*
 * Function:    generate_dungeon
 * -----------------------------
 * generates a random dungeon
 */
void generate_dungeon()
{
    /* initialize dungeon */
    dungeon_t dungeon;
    dungeon.num_rooms = 0;

    /* fill entire dungeon with rock (#) */
    int x, y;
    for (y = 0; y < DUNGEON_Y; y++)
    {
        for (x = 0; x < DUNGEON_X; x++)
        {
            dungeon.cell[x][y].symbol = '#';
            dungeon.cell[x][y].mutable = is_outermost_wall(x, y);
        }
    }
    
    /* carve out 12 rooms into the dungeon */
    int roomCount = 0;
    while (roomCount < MIN_ROOMS)
    {
        int start_x = rand() % DUNGEON_X;
        int start_y = rand() % DUNGEON_Y;
        if (create_room(start_x, start_y, &dungeon) == 0)
        {
            roomCount++;
        }
    }

    create_corridors(&dungeon);

    print_dungeon(&dungeon);
}

int main (int argc, char *argv[])
{
    srand(time(NULL));
    generate_dungeon();
    return 0;
}
