#include <stdio.h>

/* int main(void)   <--- perfectly legal */

void times_tables(void);

int main(int argc, char *argv[])
{
  times_tables();

  return 0;
}

void times_tables(void)
{
  int table[10][10];
  int i, j;

  for (i = 1; i <= 10; i++)  {
    for (j = 1; j <= 10; j++) {
      table[i - 1][j - 1] = i * j;
    }
  }

  for (i = 1; i <= 10; i++)  {
    for (j = 1; j <= 10; j++) {
      printf("%3d ", table[i - 1][j - 1]);
    }
    printf("\n");
  }
  
}
