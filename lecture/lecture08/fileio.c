#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum action {
  read_binary,
  read_text,
  write_binary,
  write_text
} action_t;

int main(int argc, char *argv[])
{
  action_t action;
  FILE *f;
  char *path; /* Absolute path to the file */
  char *home; /* The name of our home directory */
  char *file; /* The name of the file */

  /* This is called an "anonymous structure".  Notice how it doesn't *
   * have a name.  The instance, s, has a name (s), but the type     *
   * does not!                                                       */
  struct {
    int i;
    int j;
  } s = { 1, 2 };

  if (argc != 3) {
    fprintf(stderr, "Wrong number of paramenters\n");
    return -1;
  }

  if (argv[1][0] != '-') {
    fprintf(stderr, "Bad argument format\n");
    return -1;
  }

  /* I did it this way (below) to illustrate indexing individual *
   * characters in the argument vector.  Far more robust would   *
   * be to use strcmp(), for example:                            *
   *    if (!strcmp(argv[1], "-wt")) {                           *
   *      action = write_text;                                   *
   *    } else if ...                                            *
   * Using the above technique obviates the need for the if      *
   * statement above.  We can instead have a final else to       *
   * handle errors.                                              */
  if (argv[1][1] == 'w') {
    if (argv[1][2] == 't') {
      action = write_text;
    } else {
      action = write_binary;
    }
    /*    f = fopen(argv[2], "w");*/
    file = argv[2];
  } else {
    if (argv[1][2] == 't') {
      action = read_text;
    } else {
      action = read_binary;
    }
    /*    f = fopen(argv[2], "r");*/
    file = argv[2];
  }

  home = getenv("HOME");
  path = malloc(strlen(home) + strlen(file) + 2 /* NULL and internal / */);
  if (!path) {
    fprintf(stderr, "malloc failed!\n");
    return 1;
  }

  sprintf(path, "%s/%s", home, file);

  if (action == read_binary || action == read_text) {
    f = fopen(path, "r");
  } else {
    f = fopen(path, "w");
  }                                             
  if (!f) {
    perror(path);
    return 1;
  }
  free(path);

  /* We almost finished in class, but three of these cases were not *
   * filled in.  Go ahead and try this out.  We'll look at the      *
   * other three and take questions on Friday.                      */
  switch (action) {
  case read_text:
    fscanf(f, "%d %d", &s.i, &s.j);
    printf("%d %d\n", s.i, s.j);
    break;
  case read_binary:
    fread(&s, sizeof (s), 1, f);
    printf("%d %d\n", s.i, s.j);
  break;
  case write_text:
    fprintf(f, "%d %d\n", s.i, s.j);
    break;
  case write_binary:
    /* We did this one in class, but not the three above. */
    fwrite(&s, sizeof (s), 1, f);
    break;
  }

  /* What happens if you write a file as binary, then try to read it as *
   * text?  If you write as text and try to read as binary?  In either  *
   * case, nothing good.  There is nothing to protect you from this.    */

  return 0;
}