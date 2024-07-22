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
  puts ("DFSkills v1.0 - 29 May 1998, by Dave Humphrey (aj589@freenet.carleton.ca)");
  puts ("     Modifies the FALL.EXE file of TES:Daggerfall to allow skills and");
  puts ("attributes to exceed 100. Copy the file dfskills.exe into your Daggerfall");
  puts ("directory to use.  Requires patch version 2.13.\n");

  puts ("   Format: DFSkills [/u]");
  puts ("      /u ....... Replaces values with the orginal v2.13 ones.");
  puts ("   Ex:");
  puts ("      DFSkills		(change fall.exe to allow skills > 100)");
  puts ("      DFSkills /u	(return to default skills <= 100)\n");

  puts ("This file modifies FALL.EXE which you may wish to backup. It will not");
  puts ("modify the file if it isn't v2.13 (checks for proper file size).");
  puts ("Any problems, comments, or questions, just e-mail me.\n");
 }
/*========== End of Procedure display_help() ==============================*/


/*========== Begin Main Program ===========================================*/
int main (int argv, char *argc[]) {
  FILE *f;
  short ch = 0, i;
  short uninstall = FALSE;

	/* Parse command line parameters */
  for (i = 1; i < argv; i++) {

    if (!stricmp(argc[i], "/u")) {
      uninstall = TRUE;
     }
    else if (!stricmp(argc[i], "/?") || !stricmp(argc[i], "?")) {
      display_help();
      return (TRUE);
     }
   }

	/* Make sure the user wishes to proceed */
  if (uninstall)
    puts ("   Values in FALL.EXE will be replaced by default v2.13 values.");
  else
    puts ("   Values in FALL.EXE will be replaced by modified values.");

  puts ("       Proceed [Y/N]?");

  while (ch != 'Y' && ch != 'N')
    ch = toupper(getch());

  if (ch == 'N') return (FALSE);


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


	/* Replace values? */
  if (uninstall) {

		/* Allow attributes to reach 100 when leveling */
    fseek (f, 0x87f24l, SEEK_SET);

    if (fputc (0x64, f) == EOF) {
      puts ("   Error: Could not write to file FALL.EXE!\n");
      fclose (f);
      return (FALSE);
     }

    fputc (0x7C, f);

		/* Allow attributes to reach 200 */
    fseek (f, 0x88485l, SEEK_SET);
    fputc (0x64, f);
    fputc (0x7F, f);

    fseek (f, 0x8849al, SEEK_SET);
    fputc (0x64, f);
    fputc (0x7e, f);

    fseek (f, 0x884adl, SEEK_SET);
    fputc (0x64, f);

		/* Allow skills to reach 200 */
    fseek (f, 0x884f0l, SEEK_SET);
    fputc (0x64, f);
    fputc (0x7e, f);

    fseek (f, 0x88506l, SEEK_SET);
    fputc (0x64, f);

		/* Skills increase to 127 naturally */
    fseek (f, 0x886f1l, SEEK_SET);
    fputc (0x64, f);
    fseek (f, 0x88381l, SEEK_SET);
    fputc (0x64, f);

    puts ("   Values successfully replaced with v2.13 defaults.");
   }
  else {

		/* Allow attributes to reach 200 when leveling */
    fseek (f, 0x87f24l, SEEK_SET);

    if (fputc (0xc8, f) == EOF) {
      puts ("   Error: Could not write to file FALL.EXE!\n");
      fclose (f);
      return (FALSE);
     }

    fputc (0x72, f);

		/* Allow attributes to reach 200 */
    fseek (f, 0x88485l, SEEK_SET);
    fputc (0xc8, f);
    fputc (0x77, f);

    fseek (f, 0x8849al, SEEK_SET);
    fputc (0xc8, f);
    fputc (0x76, f);

    fseek (f, 0x884adl, SEEK_SET);
    fputc (0xc8, f);

		/* Allow skills to reach 200 */
    fseek (f, 0x884f0l, SEEK_SET);
    fputc (0xc8, f);
    fputc (0x76, f);

    fseek (f, 0x88506l, SEEK_SET);
    fputc (0xc8, f);

		/* Skills increase to 127 naturally */
    fseek (f, 0x886f1l, SEEK_SET);
    fputc (0xc8, f);
    fseek (f, 0x88381l, SEEK_SET);
    fputc (0x7f, f);
    puts ("   Values successfully modified.");
   }

  fclose (f);
  puts ("\n");
  return (TRUE);
 }
/*========== End of Main Program ==========================================*/