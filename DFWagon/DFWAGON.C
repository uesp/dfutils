#include <stdlib.h>
#include <conio.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define FALSE 0
#define TRUE  1

#define WEIGHT_213 0xDFE13l
#define STRING_213 0x1B06A9l


/*========== Displays Basic Help Screen ===================================*/
void display_help (void) {
  puts ("DFWagon v1.0 - 27 May 1998, by Dave Humphrey (aj589@freenet.carleton.ca)");
  puts ("     Changes the maximum weight limit of the wagon in TES:Daggerfall. Copy");
  puts ("the file dfwagon.exe into your Daggerfall directory to use.  Requires patch");
  puts ("version 2.13 (e-mail me for earlier version info).\n");

  puts ("   Format: DFWagon [new weight] [/s\"string\"] [/d]");
  puts ("      New Weight ....... The new maximum weight of wagon in pounds. Can");
  puts ("                         range from 1 up to 16000.");
  puts ("      /s\"String\" ....... The string which you would like to replace the");
  puts ("                         default ' / 750' displayed above the wagon. Can");
  puts ("                         be up to six characters long with additional");
  puts ("                         ones ignored.");
  puts ("              /d ....... Display the current weight limit and string.");
  puts ("   Ex: DFWagon 2000            (changes max weight to 2000 lbs)");
  puts ("       DFWagon 2000 \"/2000\"    (also changes the text)\n");

  puts ("This file modifies FALL.EXE which you may wish to backup. It will not");
  puts ("modify the file if it isn't v2.13 (checks for proper file size).");
  puts ("Any problems, comments, or questions, just e-mail me.\n");
 }
/*========== End of Procedure display_help() ==============================*/


/*========== Begin Main Program ===========================================*/
int main (int argv, char *argc[]) {
  FILE *f;
  short display = FALSE;
  short replace_string = FALSE;
  short i;
  unsigned short new_weight = 0, j;
  char new_string[10] = "";
  char old_string[10] = "";


  if (argv < 2){
    display_help();
    return (0);
   }

	/* Parse command line parameters */
  for (i = 1; i < argv; i++) {

    if (!stricmp(argc[i], "/s")) {
      i++;
      strncpy (new_string, argc[i], 6);
      replace_string = TRUE;
     }
    else if (!stricmp(argc[i], "/?") || !stricmp(argc[i], "?")) {
      display_help();
      return (TRUE);
     }
    else if (!stricmp(argc[i], "/d")) {
      display = TRUE;
     }
    else {	/* Must be a weight value */
      j = (unsigned short) atoi(argc[i]);
      if (j) new_weight = j;
     }
   }

	/* Make sure the string isn't too long */
  new_string[6] = 0;

  if (replace_string) {

	/* Make sure only valid characters are present */
    for (i = 0; i < strlen(new_string); i++) {
      if (!isprint(new_string[i])) new_string[i] = ' ';
     }
   }

	/* Make sure there is something to do */
  if (!display && !new_weight && !replace_string) {
    display_help();
    puts ("      Nothing to do!!\n");
    return (TRUE);
   }

	/* Attempt to open file for reading/writing */
  if (!(f = fopen ("fall.exe", "r+b"))) {
    puts ("   Error: Could not open file FALL.EXE!\n");
    return (FALSE);
   }

	/* Is it the correct size? */
  fseek (f, 0, SEEK_END);

  if (ftell(f) != 1864183l) {
    fclose (f);
    puts ("   Error: FALL.EXE is incorrect size, v2.13 required!\n");
    return (FALSE);
   }

	/* Get the current settings */
  fseek (f, WEIGHT_213, SEEK_SET);
  j = (fgetc(f) + (fgetc(f) << 8)) >> 2;
  fseek (f, STRING_213, SEEK_SET);
  fread (old_string, sizeof(char), 6, f);
  old_string[6] = 0;

	/* Output results if required */
  if (display) {
    fclose (f);
    printf ("     Current Weight Limit = %u lbs\n", j);
    printf ("     Current String = '%s'\n", old_string);
    puts ("   No changes made to file.");
    return (TRUE);
   }

	/* Change things if required */
  if (new_weight) {

    if (new_weight > 16000) {
      fclose (f);
      puts ("   Error: New weight limit must be less than 16,000!");
      return (TRUE);
     }
    else if (new_weight > 8000) {
      puts ("   Warning: If this doesn't work try a value less than 8000.");
     }

    fseek (f, WEIGHT_213, SEEK_SET);
    fputc ((new_weight << 2) & 0xff, f);
    fputc ((new_weight >> 6) & 0xff, f);
    printf ("   Changed weight limit from %u to %u lbs.\n", j, new_weight);
   }

  if (replace_string) {
    fseek (f, STRING_213, SEEK_SET);
    fwrite (new_string, sizeof(char), strlen(new_string), f);
    printf ("   Changed string from '%s' to '%s'.\n", old_string, new_string);
   }

  puts ("\n");
  fclose (f);
  return (TRUE);
 }
/*========== End of Main Program ==========================================*/