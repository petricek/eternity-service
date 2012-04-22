#include <stdio.h>

void main()
{
  FILE *f;
  char c[1024];

  f = fopen("/dev/random", "r");
  fread(c, 40, 1, f);
  printf("Random string:\n%s\n", c);
  fread(c, 40, 1, f);
  printf("Another one:\n%s\n", c);
  fclose(f);
}
