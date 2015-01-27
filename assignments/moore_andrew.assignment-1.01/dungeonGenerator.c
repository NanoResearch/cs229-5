#include <stdlib.h>
#include <stdio.h>

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
        }
    }
    printDungeon(&dungeon);
}

int main (int argc, char *argv[])
{
    generateDungeon();

    return 0;
}