#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include "genutil.h"
#include "fileutil.h"


void main (int argv, char *argc[]) {
  FILE *f, *f1;
  int i[5];

  if (argv <= 1) return;

  if (!(f = fopen (argc[1], "rb")))
    return;

  f1 = fopen ("bookhead.log", "at");

	/* Skip to start of header */
  fseek (f, 224, SEEK_SET);

	/* Read header */
  i[0] = read_int (f);
  i[1] = read_int (f);
  i[2] = read_int (f);
  i[3] = read_int (f);
  i[4] = read_int (f);
  fprintf (f1, "File '%s':  0x%04X  0x%04X  0x%04X  0x%04X  0x%04X\n", argc[1], i[0], i[1], i[2], i[3], i[4]);


  fclose (f);
  fclose (f1);
 }