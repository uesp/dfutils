
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>


void main (void) {
  FILE *fi, *fo;
  int i;
  char name[32];
  unsigned short pic1, pic2;

  fi = fopen ("d:\\dagger\\fall.exe", "rb");
  fo = fopen ("itemhack.txt", "wt");


	/* Move to start of item data */
  fseek (fi, 0x1b682al, SEEK_SET);

  for (i = 0; i < 288; i++) {

	/* Read in item values */
    fread (name, sizeof(char), 24, fi);
    fseek (fi, 20, SEEK_CUR);
    pic1 = ((unsigned short) fgetc(fi)) + ((unsigned short)fgetc(fi) << 8);
    pic2 = ((unsigned short) fgetc(fi)) + ((unsigned short)fgetc(fi) << 8);

	/* Output data */
    if (pic2 && pic1) {
      fprintf (fo, "   { 0x%04X, \"%s #1\" },\n", pic1, name);
      fprintf (fo, "   { 0x%04X, \"%s #2\" },\n", pic2, name);
     }
    else if (pic1)
      fprintf (fo, "   { 0x%04X, \"%s\" },\n", pic1, name);
    else if (pic2)
      fprintf (fo, "   { 0x%04X, \"%s\" },\n", pic2, name);

   }


  fclose (fo);
  fclose (fi);
 }