#include <stdlib.h>
#include <stdio.h>
#include <time.h>

typedef struct cell{
    char symbol;
    int hardness;
    int mutable;
} cell;

typedef struct dungeon{
    cell map[160][96];
} dungeon;

void printDungeon(dungeon *dungeon)
{
    int x, y;
    for (y = 0; y < 96; y++)
    {
        for (x = 0; x < 160; x++)
        {
            printf("%c", dungeon->map[x][y].symbol);
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

void generateDungeon()
{    
    dungeon dungeon;

    int x, y;

    for (y = 0; y < 96; y++)
    {
        for (x = 0; x < 160; x++)
        {
            cell *cell = &dungeon.map[x][y];
            cell->symbol = '#';
            cell->mutable = isOutermostWall(x, y);
            
            /* assigns the cell a hardness between 0 and 6 this isn't the */
            /* most random, but works for our purposes */
            cell->hardness = rand() % 7;
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