#include "fileutil.h"
#include "dfgamma.h"
#include "math.h"



	/* List of DF palettes to change */
char *pal_list[] = {
  "arena2\\pal.raw",
  "arena2\\ART_PAL.COL",
  "arena2\\DANKBMAP.COL",
  "ARENA2\\FMAP_PAL.COL",
  "ARENA2\\NIGHTSKY.COL",
  "DATA\\DAGGER.COL",
  "ARENA2\\MAP.PAL",
  "ARENA2\\OLDMAP.PAL",
  "ARENA2\\OLDPAL.PAL",
  "ARENA2\\PAL.PAL",
 };

short num_pals = 10;

	/* The Main Palette Object */
DAGPAL_TYPE pal;


/*========== Class DAGPAL_TYPE Constructor ================================*/
DAGPAL_TYPE::DAGPAL_TYPE (void) {
  is_col = FALSE;
  filename[0] = 0;
 }
/*========== End of Class DAGPAL_TYPE Constructor =========================*/


/*========== Changes the Filename for the Palette Data ====================*/
void DAGPAL_TYPE::change_filename (const char *new_file) {
  strncpy (filename, new_file, 100);
  filename[100] = 0;
 }
/*========== End of Procedure DAGPAL_TYPE::change_filename() ==============*/


/*========== Attempts to Load a Palette File ==============================*/
boolean DAGPAL_TYPE::load_palette (const char *filename) {
  FILE *f;		/* File pointer */
  short i, j;		/* Loop counters */
  unsigned char buffer[800];	/* Temp input buffer */


	/* Attempt to open file for input */
  if (!(f = openfile(filename, "rb"))) {
    err_code = ERR_FILE;
    return (FALSE);
   }

	/* Change the filename */
  change_filename (filename);

	/* What type of file is it? */
  switch (get_filesize(f)) {
    case 768:
      is_col = FALSE;
      break;
    case 776:
      is_col = TRUE;
      break;
    default:
      fclose (f);
      err_code = DF_ERR_NOTPAL;
      return (FALSE);
   };


	/* Read the header information if it is a COL format */
  if (is_col) {
    if (fread (col_data, sizeof(char), 8, f) != 8) {
      err_code = ERR_READ;
      fclose (f);
      return (FALSE);
     }
   }

	/* Read in all the palette data */
  if (fread (buffer, sizeof(char), 768, f) != 768) {
    err_code = ERR_READ;
    fclose (f);
    return (FALSE);
   }

  for (i = 0, j = 0; i < 256; i++) {
    pal_data[i].r = (unsigned short) buffer[j++];
    pal_data[i].g = (unsigned short) buffer[j++];
    pal_data[i].b = (unsigned short) buffer[j++];
   }

  fclose (f);
  err_code = ERR_NONE;
  return (TRUE);
 }
/*========== End of Function DAGPAL_TYPE::load_palette() ==================*/


/*========== Attempts to Save a Palette File ==============================*/
boolean DAGPAL_TYPE::save_palette (const char *filename) {
  FILE *f;		/* File pointer */
  short i, j;		/* Loop counter */
  unsigned char buffer[800];	/* Temp input buffer */

	/* Attempt to open file for input */
  if (!(f = openfile(filename, "wb"))) {
    err_code = ERR_FILE;
    return (FALSE);
   }

	/* Write the header information if it is a COL format */
  if (is_col) {
    if (fwrite (col_data, sizeof(char), 8, f) != 8) {
      err_code = ERR_WRITE;
      fclose (f);
      return (FALSE);
     }
   }

  for (i = 0, j = 0; i < 256; i++) {
    buffer[j++] = pal_data[i].r;
    buffer[j++] = pal_data[i].g;
    buffer[j++] = pal_data[i].b;
   }

	/* Write all the palette data */
  fwrite (buffer, sizeof(char), 768, f);

  fclose (f);
  err_code = ERR_NONE;
  return (TRUE);
 }
/*========== End of Function DAGPAL_TYPE::load_palette() ==================*/


/*========== Brighten the Palette by a Certain Degree =====================*/
void DAGPAL_TYPE::modify (const float f) {
  float a;
  float f1 = 1/f;
  short i;		/* Loop counter */

  a = MAX_PAL / pow (MAX_PAL + 1, f1);

  for (i = 0; i < 256; i++) {
//    write_log_entry ("  %3d) %3d %3d %3d", i, pal_data[i].r, pal_data[i].g, pal_data[i].b);
    pal_data[i].r = a * pow (pal_data[i].r + 1, f1);
    pal_data[i].g = a * pow (pal_data[i].g + 1, f1);
    pal_data[i].b = a * pow (pal_data[i].b + 1, f1);


	/* Make sure the values don't exceed the maximum */
    if (is_col) {
      if (pal_data[i].r > 255) pal_data[i].r = 255;
      if (pal_data[i].g > 255) pal_data[i].g = 255;
      if (pal_data[i].b > 255) pal_data[i].b = 255;
     }
    else {
      if (pal_data[i].r > MAX_PAL) pal_data[i].r = MAX_PAL;
      if (pal_data[i].g > MAX_PAL) pal_data[i].g = MAX_PAL;
      if (pal_data[i].b > MAX_PAL) pal_data[i].b = MAX_PAL;
     }

//     write_log_entry ("       %3d %3d %3d", pal_data[i].r, pal_data[i].g, pal_data[i].b);
   }

 }
/*========== End of Procedure DAGPAL_TYPE::modify() =======================*/


/*========== Displays a Simple Help Screen ================================*/
void display_help (void) {
  puts ("DFGamma v1.0, 14 June 1998 - by Dave Humphrey (aj589@freenet.carleton.ca)");
  puts ("     Brightens or darkens TES:Daggerfall's palettes to increase the games");
  puts ("playability on dark monitors.  Merely copy this file into your Daggerfall");
  puts ("directory.  This is a totally unofficial product and I am in no way");
  puts ("connected or related to Bethesda.\n");

  puts ("   Format:  DFGamma [gamma] [/u<drive>] [/p]");
  puts ("           Gamma .... The factor by which you wish to modify the palettes.");
  puts ("                      A value of 1.5 is reasonable to increase brightness.");
  puts ("       /u<drive> .... Replaces all the palettes with the originals from the");
  puts ("                      cdrom. Drive is your cd-rom drive with the Daggerfall");
  puts ("                      disk in it.  ex: /ud or /uF");
  puts ("              /p .... Prompts you before modifying each palette. Useful if");
  puts ("                      you want to change some but not others.\n");

  puts ("It is entirely possible to mess up the palettes by specifying a too low or");
  puts ("too high modification factor (0 would make everything black and 10000 would");
  puts ("make everything white).  Try using a factor of 1.2 to 2 until the brightness");
  puts ("is satisfactory.  You may need to copy some palette files from the CD-ROM if");
  puts ("you are using the Small or Medium install size.  Any problems, comments,");
  puts ("etc...just e-mail me.\n");
 }
/*========== End of Procedure display_help() ==============================*/



/*========== Begin Main Program ===========================================*/
short main (int argv, char *argc[]) {
  boolean uninstall = FALSE;	/* User wants to replace orginal palettes? */
  boolean prompt = FALSE;	/* User wants to be prompted */
  boolean ok = TRUE;
  float factor = 1;		/* The modification factor */
  char udrive = 'd', ch;
  short i;

//  open_log_file ("dagpal.log");

	/* Create the program specific error codes */
  error_handler.add_error (DF_ERR_NOTPAL, "File is not a known COL or PAL format!");

	/* Not enough command line parameters? */
  if (argv < 2) {
    display_help();
    return (TRUE);
   }

	/* Parse the command line parameters */
  for (i = 1; i < argv; i++) {

    if (!stricmp(argc[i], "/p"))
      prompt = TRUE;
    else if (!strnicmp(argc[i], "/u", 2)) {

      if (strlen(argc[i]) < 3) {
	printf ("   Error: You must specify the CD-ROM path (ex: /ud)\n");
	return (FALSE);
       }

      udrive = argc[i][2];
      uninstall = TRUE;
     }
    else if (strchr(argc[i], '?')) {
      display_help ();
      return (TRUE);
     }
    else {
      factor = (float) atof(argc[i]);
     }
   }

	/* What do we want to do? */
  if (uninstall) {
    char buffer[101];

	/* Make sure the path exists */
    sprintf (buffer, "%c:\\dagger\\%s", udrive, pal_list[0], pal_list[0]);

    if (!fileexists(buffer)) {
      printf ("  Error: File '%s' not found!\n", buffer);
      printf ("     Make sure you specify the correct CD-ROM path and that the Daggerfall\n");
      printf ("     CD is in the drive.\n\n");
      return (FALSE);
     }
    else if (!isdir("arena2")) {
      printf ("  Error: Path 'ARENA2' not found!\n");
      printf ("     Make sure you are in the Daggerfall directory when running DFGamma.\n\n");
      return (FALSE);
     }

    for (i = 0; i < num_pals; i++) {

      if (prompt) {
	printf ("  Replace palette file '%s' (Y/N/A/Q)?\n", pal_list[i]);

	do {
	  ch = toupper(getch());
	} while (ch != 'Y' && ch != 'N' && ch != 'A' && ch != 'Q' && ch != 27);

	if (ch == 'Q' || ch == 27) {
	  puts ("      Aborting...\n");
	  return (FALSE);
	 }
	else if (ch == 'Y')
	  ok = TRUE;
	else if (ch == 'A') {
	  prompt = FALSE;
	  ok = TRUE;
	 }
	else
	  ok = FALSE;

       }
      else
	ok = TRUE;

      if (ok) {

	if (strnicmp(pal_list[i], "data", 4))
	  sprintf (buffer, "copy %c:\\dagger\\%s %s /Y > NUL", udrive, pal_list[i], pal_list[i]);
	else
	  sprintf (buffer, "copy %c:\\%s %s /Y > NUL", udrive, pal_list[i], pal_list[i]);

	//printf (buffer);
	printf ("     Copying file %s\n", pal_list[i]);

	if (system (buffer)) {
	  printf ("   Error: Could not copy files!\n\n");
	  return (FALSE);
	 }
       }

     }

    return (TRUE);
   }

	/* Valid factor received? */
  if (factor <= 0) {
    printf ("   The factor of %f is not valid, aborting!\n", factor);
    return (FALSE);
   }

	/* Load in all the palettes, modify them and save them again */
  for (i = 0; i < num_pals; i++) {

    if (prompt) {
      printf ("  Modify palette file '%s' by %fx (Y/N/A/Q)?\n", pal_list[i], factor);

      do {
	ch = toupper(getch());
      } while (ch != 'Y' && ch != 'N' && ch != 'A' && ch != 'Q' && ch != 27);

      if (ch == 'Q' || ch == 27) {
	puts ("      Aborting...\n");
	return (FALSE);
       }
      else if (ch == 'Y')
	ok = TRUE;
      else if (ch == 'A') {
	prompt = FALSE;
	ok = TRUE;
       }
      else
	ok = FALSE;
     }
    else
      ok = TRUE;


    if (ok) {

		/* Attempt to load pal data */
      if (!pal.load_palette (pal_list[i])) {
	printf ("   Error loading palette '%s'", pal_list[i]);
	printf ("       %s\n", get_errmsg(err_code));
       }
      else {
	pal.modify (factor);

	if (!pal.save_palette (pal_list[i])) {
	  printf ("   Error saving palette '%s'", pal_list[i]);
	  printf ("       %s\n", get_errmsg(err_code));
	 }
	else
	  printf ("     Palette '%s' modified and saved.\n", pal_list[i]);
       }

     }

   }

  printf ("\n");
  return (TRUE);
 }
/*========== End Main Program =============================================*/
