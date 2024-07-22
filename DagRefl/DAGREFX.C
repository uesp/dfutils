/* DAGREFX - Program to change the Monster's Reflex's in Daggerfall
 * Copyright (c) 1996 Dave Humphrey - aj589@freenet.carleton.ca
 *
 * This program is based on the fact that the monster reflex chosen at
 * character creation is 130 bytes (82h) from the beginning of the
 * character's name in the SAVETREE.DAT file.  The character in this
 * position gives the value chosen by the following
 *
 * 	Highest - 0
 * 	High	- 1
 *	Average - 2
 *	Low     - 3
 *      Lowest  - 4
 *
 * This was determined by creating several as exact as possible characters
 * with different monster reflex settings and then comparing the files.
 * Eventually, by eliminating byte difference locations one can determine
 * this (it took over an hour or so to determine this, including the actual
 * character creation in DF).
 *
 * This program may be freely modified/copied/distributed.
 */

	//Standard Include files
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <sys/stat.h>
#include <ctype.h>

	//Set the version number
#define version 0.10

	//Standard boolean type
typedef enum {TRUE = 1, FALSE = 0} boolean;


//========= Notifies User of a bug/error ====================================
void bug(char *string, char *filename) {
  /* General error message routine notifying user of an error */

  printf ("\n\nDAGREFX.C v%3.2f, 16 November 1996, Dave Humphrey\n", version);
  printf ("%s\n", string);
  if (filename != NULL) printf ("Filename = %s\n", filename);
  printf ("\n\n");
  exit(FALSE); //Exit program
 }
//========= End of procedure bug() ==========================================


//========= Displays a simple help screen ===================================
void display_help(void) {
  printf ("\n");
  printf ("DAGREFX v%3.2f  Copyright (c) 1996 - Dave Humphrey\n", version);
  printf ("Edits the ES:DAGGERFALL file savetree.dat in the specified savegame directory\n");
  printf ("and displays/changes the monster reflex value chosen during character creation.\n\n");
  printf ("  DAGREFX savegame character [/R:new reflex]\n\n");

  printf ("         savegame - The savegame number, 0-5, you wish to change.\n");
  printf ("        character - The name of the character in the savegame.\n");
  printf ("  [/R:new reflex] - Change the monster reflex value to this. The acceptable\n");
  printf ("                    values are as follows...\n");
  printf ("	     		      	  Highest - 0\n");
  printf ("			      	     High - 1\n");
  printf ("			      	  Average - 2\n");
  printf ("			      	      Low - 3\n");
  printf ("			      	   Lowest - 4\n\n");

  printf ("If a new reflex value is not given, the current value will simply\n");
  printf ("be displayed. Comments can be E-Mailed to: aj589@freenet.carleton.ca\n\n");
 }
//========= End of procedure display_help() =================================


//========= Function which checks if a string is the character '0' to '5' ===
boolean is_savegame(const char *string) {

  while (*string == ' ')  //Ignore leading spaces, just in case
    string++;

  if (*string >= '0' && *string <= '5') {
    string++;

    while (*string == ' ')  //Ignore trailing spaces...just in case
      string++;

    if (*string == 0) return(TRUE);  //We must be at end of string if it's a savegame #
   }

  return(FALSE); }
//========= End of function is_savegame() ===================================



//========== Begin Main Program =============================================
int main (int argv, char *argc[]) {
  char reflex_name[5][20] = { //The names of the monster reflex value
	"Highest", "High", "Average", "Low", "Lowest" };

  boolean savegame = FALSE, character = FALSE, reflex = FALSE;  //Toggles for what the parser finds in the supplied arguments
  boolean found = FALSE;  //Did we find the character's name in the current file?
  char char_name[81];  //String buffer to hold the character's name
  char filename[20];   //Filename of the save game
  char ch, *temp_str;  //Some temporary variables
  int save_number = -1;
  int i;  	       //Loop counter
  int current_reflex = -1, new_reflex = -1;  //What the reflex in the save game file is currently set at, and what the user wants to change it too
  int search_location = 0;  //Where are we in the string search
  long int j;	       //Loop counter
  struct stat statbuf;   //Location to hold statistics on opened file
  FILE *f; 	       //File pointer

  printf("\n");

  if (argv > 1) { //Two arguments

    for (i = 1; i < argv; i++) { //Go through argument list and get useful stuff
      if (is_savegame(argc[i]) && !savegame) {
	savegame = TRUE;
	save_number = atoi(argc[i]);
       } //End of if savegame
      else if ((temp_str = strstr(strupr(argc[i]), "/R:")) != NULL) {
	temp_str += 3;  //Skip the characters "/R:" and point to start of new reflex value

	if (strlen(temp_str) == 1) { //Only 1 character, must be a number 0-4
	  if (*temp_str >= '0' && *temp_str <= '4') {
	    new_reflex = (int)*temp_str - (int)'0';
	    reflex = TRUE;
	   }
	  else //An invalid reflex number
	    printf ("'%s' is an invalid reflex value!\n");
	 }
	else {
	  for (j = 0; j < 5; j++) { //Compare string with each reflex name
	    if (stricmp(reflex_name[(int)j], temp_str) == 0) { // A match!
	      reflex = TRUE;
	      new_reflex = (int)j;
	      break;  //Get out of for loop
	     }
	   } //End of for loop

	  if (new_reflex == -1) //We didn't match the given reflex value!
	    printf ("'%s' is an invalid reflex value!\n");
	 }
       } //end of if /R:
      else if (!character) { //Must be the character's name...we hope
	character = TRUE;
	strcpy (char_name, argc[i]);
       }
      else if (strchr(argc[i], '?') != NULL) {
	display_help();
	exit(0);
       }
     } //End of for argument loop

    if (!savegame || !character) {  //User didn't specify enough arguments...
      printf ("Syntax Error: Must specify save game number and character name!\n\n");
      exit(FALSE);
     }
			//We're assuming we're in the DAGGERFALL directory
    sprintf (&filename[0], "save%1d\\savetree.dat", save_number);
    if ((f = fopen(filename, "r+b")) == NULL) bug ("Could not open specified file!", filename);

			 //Get the files statistics
    if (fstat(fileno(f), &statbuf) == -1) bug("Could not retrieve file information!", filename);
    printf ("Searching");

    for (j = 0; j < statbuf.st_size; j++) { //Search for given name
      ch = toupper(fgetc(f));

      if (j % (statbuf.st_size/40) == 0) printf(".");  //Just a simple display routine to show using we're doing something
			//We cannot use the strstr() function as this would stop searching at a terminated NULL
			//or 0 character, which the data will have a plenthera of, so we make our own crude search

      if (ch == char_name[search_location]) { //One character match
	search_location++;

	if (search_location == strlen(char_name)) { //We found a complete match
	  found = TRUE;
	  printf("Found!\n");
	  if (fseek(f, 130 - strlen(char_name), SEEK_CUR) != 0) bug("fseek() returned an error!", char_name);
	  current_reflex = fgetc(f);
	  printf ("Save Game #%1d: %s\n", save_number, char_name);

	  if (current_reflex >= 0 && current_reflex <= 4) //Valid reflex read?
	    printf ("     Monster Reflex is currently set at %s(%1d)\n\n", reflex_name[current_reflex], current_reflex);
	  else { //Invalid reflex read...error!
	    printf ("	  ERROR: Reflex value of %d in %s is invalid!\n\n", current_reflex, filename);
	    exit(FALSE);
	   } //End of if invalid reflex

	  if (reflex) { //Do we want to modify this value?
	    printf ("Do you wish to change this to %s(%1d) (Y/N)?", reflex_name[new_reflex], new_reflex);

	    do {   	//Standard get input from keyboard routine...
	      ch = toupper(getch());
	    } while (ch != 'Y' && ch != 'N' && ch != 27);  //27 = ESCape

	    if (ch == 'Y') { //Yes...change it
	      if (fseek(f, -1, SEEK_CUR) != 0) bug("fseek() returned an error!", "Relative -1");
	      if (fputc(new_reflex ,f) != new_reflex) bug("fputc(new_reflex) returned an error.  Could not write to file!", filename);
	      printf ("\n\nMonster reflex value in file %s has been successfully\nchanged to %s(%1d)", filename, reflex_name[new_reflex], new_reflex);
	     } //End of it change it

	    printf ("\n\n");
	   } //End of if change reflex

	  break;  //Exit for loop
	 }
       }
      else
	search_location = 0;
     } //End of looking for character name

    fclose(f);

    if (!found) {  //We didn't find the specified name?
      printf ("Not Found!\n");
      printf ("The character '%s' was not found in Save Game #%1d!\n\n", char_name, save_number);
     }
   } //End of if any arguments
  else { //No arguments?
    display_help();
   }

  return(TRUE); }
//========== End of Main Program main() =====================================
