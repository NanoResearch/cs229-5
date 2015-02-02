#include <endian.h>
#include <stdio.h>
#include <math.h>

int main(int argc, char *argv[])
{
  float f;
  union {
    int i;
    char c[4];
    float f;
  } u;

  u.i = 0x01020304;

  /* Format string explaination: %x prints in integer in hexidecimal. *
   * The modifier '#' tells printf to add a '0x' in front of it.  'h' *
   * signifies that the following integer is a short, and hh signals  *
   * a byte.  You're already seen field width (2 here), but putting   *
   * a zero before the field width means to zero-pad the output.      */
  printf("%#x == %d\nByte order: 0x%02hhx%02hhx%02hhx%02hhx\n\n",
         u.i, u.i, u.c[0], u.c[1], u.c[2], u.c[3]);

#if 0

  /* htobe32 == host format to big-endian 32 bits */
  u.i = htobe32(u.i);

  printf("%#x == %d\nBig-endian byte order: 0x%02hhx%02hhx%02hhx%02hhx\n\n",
         u.i, u.i, u.c[0], u.c[1], u.c[2], u.c[3]);

  /* be32toh == big-endian 32 bit to host format */
  u.i = be32toh(u.i);

  printf("%#x == %d\nBack to system default: 0x%02hhx%02hhx%02hhx%02hhx\n\n",
         u.i, u.i, u.c[0], u.c[1], u.c[2], u.c[3]);

  /* htole32 == host format to little-endian 32 bit */
  u.i = htole32(u.i);

  printf("%#x == %d\nLittle-endian byte order: 0x%02hhx%02hhx%02hhx%02hhx\n\n",
         u.i, u.i, u.c[0], u.c[1], u.c[2], u.c[3]);

  /* This all works exactly the same way for other data sizes. *
   * See endian(3) for function specifics.                     */

  /* What about floating point? */

  u.f = M_PI; /* Value of pi, from math.h */
  printf("%f\n%#x == %d\nByte order: 0x%02hhx%02hhx%02hhx%02hhx\n\n",
         u.f, u.i, u.i, u.c[0], u.c[1], u.c[2], u.c[3]);

  u.f = htobe32(u.f);
  printf("%f\n%#x == %d\nLittle endian: 0x%02hhx%02hhx%02hhx%02hhx\n\n",
         u.f, u.i, u.i, u.c[0], u.c[1], u.c[2], u.c[3]);

  u.f = be32toh(u.f);
  printf("%f\n%#x == %d\nHost order: 0x%02hhx%02hhx%02hhx%02hhx\n\n",
         u.f, u.i, u.i, u.c[0], u.c[1], u.c[2], u.c[3]);


























  u.f = M_PI;
  printf("%f\n%#x == %d\nByte order: 0x%02hhx%02hhx%02hhx%02hhx\n\n",
         u.f, u.i, u.i, u.c[0], u.c[1], u.c[2], u.c[3]);

  u.i = htobe32(u.i);
  printf("%f\n%#x == %d\nLittle endian: 0x%02hhx%02hhx%02hhx%02hhx\n\n",
         u.f, u.i, u.i, u.c[0], u.c[1], u.c[2], u.c[3]);

  u.i = be32toh(u.i);
  printf("%f\n%#x == %d\nHost order: 0x%02hhx%02hhx%02hhx%02hhx\n\n",
         u.f, u.i, u.i, u.c[0], u.c[1], u.c[2], u.c[3]);

















  /* Or, without using a union: */
  f = M_PI;
  *((int *) &f) = htobe32(*((int *) &f));
  *((int *) &f) = be32toh(*((int *) &f));
  printf("%f\n\n", f);

#endif
  return 0;
}