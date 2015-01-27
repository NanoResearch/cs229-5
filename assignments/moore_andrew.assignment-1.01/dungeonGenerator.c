#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

typedef struct cell{
    char symbol;
    int hardness;
    int mutable;
} cell;

typedef struct room{
    int startX;
    int startY;
    int endX;
    int endY;
} room;

typedef struct dungeon{
    int numRooms;
    cell cell[160][96];
    room rooms[12];
} dungeon;

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
                        if (sqrt((j - l) * (j - l) + (k - m) * (k - m)) <= 6)
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

    if (isOutermostWall(startX, startY) == 1 || isOutermostWall(endX, endY) == 1)
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

void generateDungeon()
{    
    dungeon dungeon;
    dungeon.numRooms = 0;

    int x, y;

    for (y = 0; y < 96; y++)
    {
        for (x = 0; x < 160; x++)
        {
            cell *cell;
            if (!(cell = malloc(sizeof (*cell)))) {
                perror("malloc");
                exit(1);
            }
            cell = &dungeon.cell[x][y];
            cell->symbol = '#'; 
            cell->mutable = isOutermostWall(x, y);
            
            /* assigns the cell a hardness between 0 and 6 this isn't the */
            /* most random, but works for our purposes */
            cell->hardness = rand() % 7;
        }
    }
    int roomCount = 0;
    while (roomCount < 12)
    {
        int startX = rand() % 160;
        int startY = rand() % 96;
        // printf("about to create room\n");
        if (createRoom(startX, startY, &dungeon) == 1)
        {
            roomCount++;
        }
    }
    printDungeon(&dungeon);
}

int main (int argc, char *argv[])
{
    srand(time(NULL));
    generateDungeon();

    return 0;
}
