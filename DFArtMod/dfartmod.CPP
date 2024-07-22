	/* Standard C Includes */
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <time.h>
#include <alloc.h>
#include <string.h>
#include <process.h>
#include <dos.h>

	/* User Defined Includes */
#include "fileutil.h"
#include "guitext.h"
#include "daggen.h"
#include "dagfile1.h"
#include "dfart.h"
#include "dfartmod.h"

	/* Is this a public release, or for debugging */
#define RELEASE TRUE
#define DEBUG TRUE

	/* Define the appropiate filenames we will need */
#if RELEASE
  #define TEXT_RSC "Arena2\\Text.rsc"
  #define TEXT_BAK "arena2\\text.bak"
  #define MAGIC_BAK "arena2\\magic.bak"
  #define MAGIC_DEF "Arena2\\Magic.def"
#else
  #define TEXT_RSC "Text.rsc"
  #define TEXT_BAK "text.bak"
  #define MAGIC_BAK "magic.bak"
  #define MAGIC_DEF "Magic.def"
#endif

	/* Holds all the important data */
AMD_TYPE amdfile;
MAGICDEF_TYPE deffile;
RSC_TYPE rscfile;


	/* Main loop counter and redraw variable */
boolean done = FALSE;

	/* Flag options set by user on command line */
boolean verbose = TRUE;
boolean prompt = TRUE;
boolean check = FALSE;
boolean view = FALSE;
boolean file = FALSE;
boolean backup = TRUE;

	/* The amd file to operate on */
char filename[MAX_STRING_LENGTH + 1];


/*========== Displays a Simple Help Screen ================================*/
void display_help (void) {
  printf ("%s, March 1998 - by Dave Humphrey, aj589@freenet.carleton.ca\n", DFARTMOD_VERSION);
  printf ("   Extracts artifact modules (AMD) for adding custom artifacts to the\n");
  printf ("   game of Daggerfall.\n\n");

  printf ("   Syntax: DFArtMod <file.amd> [/?] [/v] [/c] [/y]\n");
  printf ("       <file.amd> .. The AMD file to extract from, Required.\n");
  printf ("       [/?] ........ View this help screen.\n");
  printf ("       [/v] ........ View info on the AMD file, do not extract.\n");
  printf ("       [/c] ........ Check the AMD for errors/conflicts, do not extract.\n");
  printf ("       [/y] ........ Default 'Yes' to all prompts.\n\n");

  printf ("Any problems, comments etc... please contact me at the above email.\n\n");
 }
/*========== End of Procedure display_help() ==============================*/


/*========== Checks for Errors / Conflicts in the AMD File ================*/
void check_amd (void) {
  short err = 1, i, j, art_count;

  printf ("Checking AMD for internal errors and conflicts...\n");

	/* Find and display all internal errors */
  while (!amdfile.check(err)) {
    err++;

    if (dfart_check_text)
      printf ("   Text %d, %s\n", dfart_check_index + 1, get_errmsg(err_code));
    else
      printf ("   Artifact %d, %s\n", dfart_check_index + 1, get_errmsg(err_code));

   }

	/* Display total number of internal errors found */
  if (err - 1)
    printf ("      Warning: Found %d internal AMD errors!\n", err - 1);
  else
    printf ("      No internal AMD errors/conflicts found.\n");

	/* Check for external conflicts */
  printf ("Checking AMD, DEF, and RSC for external errors and conflicts...\n");

	/* Look at the DEF file - Artifacts */
  for (j = 0; j < amdfile.num_records; j++) {
    art_count = 0;

		/* Look for existing artifact with the same index */
    for (i = 0; i < deffile.num_records; i++) {

		/* Is it an artifact? */
      if (deffile.records[i]->magic_type) {

		/* Does the index match? */
	if (deffile.records[i]->u1 && amdfile.records[j]->index == art_count) {
	  err++;
	  printf ("   Artifact %d, Warning: Artifact index %d already exists in DEF File!\n", j + 1, art_count);
	  break;
	 }

	art_count++;
       }
     }
   }

	/* Look at the RSC file */
  for (j = 0; j < amdfile.num_texts; j++) {
    art_count = 0;

    for (i = 0; i < rscfile.num_texts; i++) {
      if (rscfile.header[i].type == amdfile.text_index[j] + 8700) {
	err++;
	printf ("   Text %d, Warning: Text index %d already exists in RSC file!\n", j + 1, amdfile.text_index[j]);
       }
     }
   }

  if (err - 1)
    printf ("Warning: A Total of %d errors/conflicts were found!\n\n", err - 1);
  else
    printf ("No errors or conflicts found...\n\n");
 }
/*========== End of Procedure check_amd() =================================*/


/*========== Extracts the AMD file to the RSC and DEF files ===============*/
void extract_amd (void) {
  char buf[MAX_STRING_LENGTH + 1], ch, *temp_ptr;
  short i, j, art_count;
  boolean added;

	/* Fix the DEF file first, count number of artifacts */
  for (i = 0, art_count = 0; i < deffile.num_records; i++) {
    if (deffile.records[i]->magic_type) art_count++;
   }

	/* Find the highest index in AMD file */
  for (i = 0, j = 0; i < amdfile.num_records; i++) {
    if (amdfile.records[i]->index > j) j = amdfile.records[i]->index;
   }

	/* Make sure we have a valid number */
  if (j > MAX_ART_INDEX) j = MAX_ART_INDEX;

	/* Create 'fake' records in def if we don't have enough */
  for (i = 0; i <= j - art_count; i++) {
    deffile.add_record (deffile.num_records);
    deffile.records[deffile.num_records - 1]->magic_type = 2;
    deffile.records[deffile.num_records - 1]->u1 = 0;
   }

	/* Copy the new artifact records over */
  for (i = 0; i < amdfile.num_records; i++) {
    art_count = 0;
    if (verbose) printf ("   Adding Artifact %d of %d (Index = %d)...\n", i + 1, amdfile.num_records, amdfile.records[i]->index);
    added = FALSE;

		/* Find the position to place it */
    for (j = 0; j < deffile.num_records; j++) {

		/* Is the item an artifact? */
      if (deffile.records[j]->magic_type) {

		/* Found the position? */
	if (art_count == amdfile.records[i]->index) {

		/* Replace it if one already exists */
	  if (prompt && deffile.records[j]->u1) {
	    printf ("      Artifact index %d already exists in DEF file, Replace [Y/N]?\n", amdfile.records[i]->index);

	    do {
	      ch = toupper(getch());
	    } while (ch != 'Y' && ch != 'N');

	    if (ch != 'Y') break;
	   }

	  delete deffile.records[j];
	  deffile.records[j] = amdfile.records[i];
	  amdfile.records[i] = NULL;
	  added = TRUE;
	  break;
	 }

	art_count++;
       }
     }

    if (verbose && added)
      printf ("      Artifact successfully added...\n");
    else if (verbose)
      printf ("      Warning: Artifact was not added!\n");

   }

	/* Backup the DEF file if required */
  if (backup) {
    strcpy (buf, "copy ");
    strcat (buf, MAGIC_DEF);
    chrcat (buf, ' ');
    strcat (buf, MAGIC_BAK);
    strcat (buf, " > nul");

    if (system (buf)) {
      printf ("Warning: Failed to created a backup of  file '%s'!\n", MAGIC_DEF);
      getch();
     }
   }

	/* Attempt to save the DEF file */
  if (!deffile.save (MAGIC_DEF)) {
    printf ("   Error: Failed to save DEF file '%s'!\n", MAGIC_DEF);
    printf ("          %s\n\n", get_errmsg(err_code));
    exit (FALSE);
   }
  else if (verbose)
    printf ("DEF File '%s' was successfully saved.\n", MAGIC_DEF);

	/* Backup the RSC file if required */
  if (backup) {
    strcpy (buf, "copy ");
    strcat (buf, TEXT_RSC);
    chrcat (buf, ' ');
    strcat (buf, TEXT_BAK);
    strcat (buf, " > nul");

    if (system (buf)) {
      printf ("Warning: Failed to created a backup of file '%s'!\n", TEXT_RSC);
      getch();
     }
   }

  write_log_entry ("FarCoreLeft() = %ld", farcoreleft());

	/* Attempt to save the RSC file */
  for (i = 0; i < amdfile.num_texts; i++) {
    j = 0;
    temp_ptr = amdfile.texts[i];

	/* Parse the text */
    while (temp_ptr[j]) {
      switch (temp_ptr[j]) {
	case 10:
	case 13:
	  temp_ptr[j] = 0xFC;
	  break;
	case 9:
	  temp_ptr[j] = 32;
	  break;
	 }

      j++;
     }

	/* Add the end of text marker to end of string */
    chrcat (temp_ptr, 0xFE);

    if (!rscfile.add_record (TEXT_RSC, amdfile.text_index[i] + 8700, amdfile.texts[i], FALSE)) {
      printf ("   Error: Failed to save RSC file '%s'!\n", TEXT_RSC);
      printf ("          %s\n", get_errmsg(err_code));
      exit (FALSE);
     }
    else if (verbose)
      printf ("   Added text index %d to RSC file...\n", amdfile.text_index[i]);
   }

  write_log_entry ("FarCoreLeft() = %ld", get_memleft());
  printf ("Finished...\n\n");
 }
/*========== End of Procedure extract_amd() ===============================*/


/*========== Displays Information on the AMD File =========================*/
void view_amd (void) {
  short i, j, k;

  printf ("\n");
  printf ("AMD File '%s', Saved on %d %s %d at %d:%02d by %s\n", filename, amdfile.save_date.da_day, months[amdfile.save_date.da_mon - 1], amdfile.save_date.da_year,
										       amdfile.save_time.ti_hour, amdfile.save_time.ti_min, amdfile.author);
	/* Display information on the artifacts */
  printf ("   %d Artifacts:\n", amdfile.num_records);

  for (i = 0; i < amdfile.num_records; i++) {
    printf ("      %d) %s, Index = %d\n", i + 1, amdfile.records[i]->name, amdfile.records[i]->index);
   }

	/* Display information on the texts */
  printf ("   %d Texts:\n", amdfile.num_texts);

  for (i = 0; i < amdfile.num_texts; i++) {
    printf ("      %d) Index = %d\n", i + 1, amdfile.text_index[i]);
   }

	/* Display information on the DEF file */
  printf ("DEF File '%s', %d total records\n", MAGIC_DEF, deffile.num_records);

  for (i = 0, j = 0, k = 0; i < deffile.num_records; i++) {
    if (deffile.records[i]->magic_type) {
      j++;
      if (deffile.records[i]->u1) k++;
     }
   }

  printf ("   Contains %d Artifacts, %d are Valid\n", j, k);

	/* Display information on the RSC file */
  printf ("RSC File '%s', %d total records\n", TEXT_RSC, rscfile.num_texts);


  printf ("\n");
 }
/*========== End of Procedure view_amd() ==================================*/


/*========== Begin Main Program ===========================================*/
int main (short argv, char *argc[]) {
#if DEBUG
  unsigned long initial_mem = get_memleft();
#endif
  short i;

	/* Open the log file for debugging */
  open_log_file ("dfartmod.log");
  write_log_entry ("Sizeof(rscfile) = %d", sizeof(rscfile));
  write_log_entry ("Sizeof(amdfile) = %d", sizeof(amdfile));
  write_log_entry ("Sizeof(deffile) = %d", sizeof(deffile));

	/* Need at least 1 argument */
  if (argv < 2) {
    printf ("Error: Need at least one argument on the command line!\n");
    display_help();
    return (FALSE);
   }

	/* Parse the command line parameters */
  for (i = 1; i < argv; i++) {

    if (!stricmp (argc[i], "/v"))
      view = TRUE;
    else if (!stricmp(argc[i], "/c"))
      check = TRUE;
    else if (!stricmp(argc[i], "/y"))
      prompt = FALSE;
    else if (!stricmp(argc[i], "?")) {
      display_help();
      return (TRUE);
     }
    else if (!file) {
      strncpy (filename, argc[i], MAX_STRING_LENGTH);
      filename[MAX_STRING_LENGTH] = 0;
      file = TRUE;
     }
    else {
      printf ("   Unknown Argument: '%s'\n", argc[i]);
     }

   }

	/* Make sure a file was specified */
  if (!file) {
    printf ("   Error: No AMD file was specified!\n\n");
    return (FALSE);
   }

	/* Attempt to load the AMD file */
  if (!amdfile.load (filename)) {
    printf ("   Error: Could not load the AMD file '%s'!\n", filename);
    printf ("          %s\n\n", get_errmsg(err_code));
    return (FALSE);
   }

	/* Attempt to load the DEF File */
  if (!deffile.load (MAGIC_DEF)) {
    printf ("   Error: Could not load the DEF file '%s'!\n", MAGIC_DEF);
    printf ("          %s\n\n", get_errmsg(err_code));
    return (FALSE);
   }

	/* Attempt to load the RSC File */
  if (!rscfile.read_header (TEXT_RSC)) {
    printf ("   Error: Could not load the RSC file '%s'!\n", TEXT_RSC);
    printf ("          %s\n\n", get_errmsg(err_code));
    return (FALSE);
   }


	/* Decide what to do with it */
  if (view)
    view_amd();
  else if (check)
    check_amd();
  else
    extract_amd();

	/* Display finishing message */
#if DEBUG
  printf ("Finished %s: Created by Dave Humphrey - aj589@freenet.carleton.ca\n", DFARTMOD_VERSION);
  printf ("   Initial / Final Memory = %ld / %ld bytes\n", initial_mem, get_memleft());
  printf ("   Heap is %s\n", get_heapstring());
#endif

  return (TRUE);
 }
/*========== End Main Program =============================================*/


