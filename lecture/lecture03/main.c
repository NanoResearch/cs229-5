#include <stdio.h>

#include "cf.h"

int main(int argc, char *argv[])
{
  int f = 72;

  printf("%d Fahrenheit is %d celcius.\n", f, f2c(f));

  return 0;
}