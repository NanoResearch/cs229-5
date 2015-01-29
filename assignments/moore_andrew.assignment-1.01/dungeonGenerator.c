#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

/*
 * Structure:   cell
 * ---------------------
 * the cell contains a symbol that can be printed to the console,
 * a hardness rating, and a mutable flag
 */
typedef struct cell{
    char symbol;
    int hardness;
    int mutable;
} cell;

/*
 * Structure:   room
 * ---------------------
 * the room contains x and y values (coordinates) for both the start
 * of the room and the end of the room
 */
typedef struct room{
    int startX;
    int startY;
    int endX;
    int endY;
} room;

/*
 * Structure:   dungeon
 * ---------------------
 * the dungeon structure contains a 160 x 96 array of cells and 
 * an array of 12 rooms
 */
typedef struct dungeon{
    int numRooms;
    cell cell[160][96];
    room rooms[12];
} dungeon;

/*
 * Function:    print_dungeon
 * --------------------------
 * prints the dungeon map to the console
 *  
 *  dungeon: the dungeon to print to the console
 */
void printDungeon(dungeon *dungeon)
{
    int x, y;
    for (y = 0; y < 96; y++)
    {
        for (x = 0; x < 160; x++)
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
int isOutermostWall(int x, int y)
{
    if (x == 0 || x == 159 || y == 0 || y == 95)
    {
        return 1;
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
 *  testRoom: room to compare with current dungeon rooms
 *
 *  returns: 1 if the rooms hausdorff distance is greater than 3 from all
 *           other rooms
 *           in the dungeon
 */
int passesHausdorff(dungeon *dungeon, room *testRoom)
{
    int i, j, k, l, m;

    if (dungeon->numRooms <= 0)
    {
        return 1;
    }

    for (i = 0; i < dungeon->numRooms; i++)
    {
        for (j = dungeon->rooms[i].startX; j <= dungeon->rooms[i].endX; j++)
        {
            for (k = dungeon->rooms[i].startY; k <= dungeon->rooms[i].endY; k++)
            {
                for (l = testRoom->startX; l <= testRoom->endX; l++)
                {
                    for (m = testRoom->startY; m <= testRoom->endY; m++)
                    {
                        if (sqrt(pow(j - l, 2) + pow(k - m, 2)) <= 6)
                        {
                            return -1;
                        }
                    }
                }
            }
        }
    }
    return 1;
}

/*
 * Function:    create_room
 * ------------------------
 * creates a room and adds it to the given dungeon if it has all the valid
 * criterion
 *  
 *  startX: starting x coordinate of the room
 *  startY: starting y coordinate of the room
 *  dungeon: dungeon that will contain the new room
 *
 *  returns: 1 if the room is added successfully, -1 otherwise
 */
int createRoom(int startX, int startY, dungeon *dungeon)
{
    int x, y;
    int endX = startX + 8 + rand() % 35;
    int endY = startY + 5 + rand() % 10;

    if (dungeon->numRooms == 12)
    {
        return -1;
    }

    if (startX < 0 || startY < 0)
    {
        return -1;
    }

    if (endX >= 160 || endY >= 96)
    {
        return -1;
    }

    if (isOutermostWall(startX, startY) == 1 ||
        isOutermostWall(endX, endY) == 1)
    {
        return -1;
    }

    room testRoom;
    testRoom.startX = startX;
    testRoom.startY = startY;
    testRoom.endX = endX;
    testRoom.endY = endY;

    if (passesHausdorff(dungeon, &testRoom) == -1)
    {
        return -1;
    }

    for (y = startY; y <= endY; y++)
    {
        for (x = startX; x <= endX; x++)
        {
            dungeon->cell[x][y].symbol = '.';
        }
    }

    dungeon->rooms[dungeon->numRooms] = testRoom;
    dungeon->numRooms++;

    return 1;
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
void connect_points(dungeon *dungeon, int x1, int y1, int x2, int y2)
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
void create_corridors(dungeon *dungeon)
{
    int i;
    int room1X, room1Y, room2X, room2Y;

    for (i = 1; i < 12; i++)
    {
        room1X = dungeon->rooms[i - 1].startX + (dungeon->rooms[i - 1].endX - dungeon->rooms[i - 1].startX) / 2;
        room1Y = dungeon->rooms[i - 1].startY + (dungeon->rooms[i - 1].endY - dungeon->rooms[i - 1].startY) / 2;
        room2X = dungeon->rooms[i].startX + (dungeon->rooms[i].endX - dungeon->rooms[i].startX) / 2;
        room2Y = dungeon->rooms[i].startY + (dungeon->rooms[i].endY - dungeon->rooms[i].startY) / 2;
        connect_points(dungeon, room1X, room1Y, room2X, room2Y);
    }
}

/*
 * Function:    generate_dungeon
 * -----------------------------
 * generates a random dungeon
 */
void generateDungeon()
{
    /* initialize dungeon */
    dungeon dungeon;
    dungeon.numRooms = 0;

    /* fill entire dungeon with rock (#) */
    int x, y;
    for (y = 0; y < 96; y++)
    {
        for (x = 0; x < 160; x++)
        {
            dungeon.cell[x][y].symbol = '#';
            dungeon.cell[x][y].mutable = isOutermostWall(x, y);
            dungeon.cell[x][y].hardness = rand() % 7;
        }
    }
    
    /* carve out 12 rooms into the dungeon */
    int roomCount = 0;
    while (roomCount < 12)
    {
        int startX = rand() % 160;
        int startY = rand() % 96;
        if (createRoom(startX, startY, &dungeon) == 1)
        {
            roomCount++;
        }
    }

    create_corridors(&dungeon);

    printDungeon(&dungeon);
}

int main (int argc, char *argv[])
{
    srand(time(NULL));
    generateDungeon();
    return 0;
}
