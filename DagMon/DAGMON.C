/* DAGMON - Hacking the MONSTER.BSA file for useful data
 * Copyright (c) 1996 Dave Humphrey - aj589@freenet.carelton.ca
 *
 * Monster Data Begins at Hex Offset 2A15h from beginning of MONSTER.BSA
 */

// Standard Include files
#include <conio.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct monster_type MONSTER_TYPE;

struct monster_type {
   unsigned char name[22];
   unsigned int hitdice;
   unsigned char rest[50];
};


//========= Exits program and displays error message ========================
void bug(char *string, char *file) {
  clrscr();
  printf ("DAGMON ERROR: %s\n", string);
  if (file != NULL) printf ("Filename = %s\n\n", file);
  exit(1); }
//========= End of procedure bug() ==========================================


//========= Main Program ====================================================
int main (void) {
  char filename[20] = "monster.bsa";
  int i, n; //Loop counters
  MONSTER_TYPE monster[43];
  FILE *f;

  if ((f = fopen(filename, "rb")) == NULL) bug("Could not open file!", filename);
  fseek(f, 0x2A15, SEEK_SET);  //Seek to beginning of monster data, Gargoyle


  for (n = 0; n < 32; n++) { //Read monsters
    fread(&monster[n].name[0], sizeof(char), 23, f); //Read name
    fseek(f, 1, SEEK_CUR);  //Jump ahead one byte
    fread(&monster[n].hitdice, sizeof(int), 1, f);  //Read number of d8 hit dice

    for (i = 0; i < 48; i++) { //Read remaining stuff
      monster[n].rest[i] = (int) fgetc(f);
     }
   } //End of for loop

  fseek(f, 0x5D4E, SEEK_SET);  //Jump to 2nd set of monster data

  for (n = 32; n < 41; n++) { //Read more monsters
    fread(&monster[n].name[0], sizeof(char), 23, f); //Read name
    fseek(f, 1, SEEK_CUR);  //Jump ahead one byte
    fread(&monster[n].hitdice, sizeof(int), 1, f);  //Read number of d8 hit dice

    for (i = 0; i < 48; i++) { //Read remaining stuff
      monster[n].rest[i] = (int) fgetc(f);
     }
   } //End of for loop

  fseek(f, 0x6138, SEEK_SET);  //Jump to 3rd set of monster data

  for (n = 41; n < 42; n++) { //Read more monsters
    fread(&monster[n].name[0], sizeof(char), 23, f); //Read name
    fseek(f, 1, SEEK_CUR);  //Jump ahead one byte
    fread(&monster[n].hitdice, sizeof(int), 1, f);  //Read number of d8 hit dice

    for (i = 0; i < 48; i++) { //Read remaining stuff
      monster[n].rest[i] = (int) fgetc(f);
     }
   } //End of for loop

  fclose(f);

  if ((f = fopen("temp.dat", "wt")) == NULL) bug("Could not open file!", "temp.txt");
  fprintf (f, "MONSTER.BSA Stuff\n\n");
  fprintf (f, "%23s %2s\n", "MONSTER NAME", "HP");
  fprintf (f, "%23s %2s\n", "=================", "==");

  for (n = 0; n < 42; n++) { //Write all monsters
    fprintf (f, "%23s %2d", monster[n].name, monster[n].hitdice);

    for (i = 0; i < 48; i++) { //Write all stuff
      fprintf (f, " %3d", (unsigned int) monster[n].rest[i]);
     }

    fprintf (f, "\n");
   } //End of for loop


  fclose(f);

  return(0); }
//========= End Main Program ================================================