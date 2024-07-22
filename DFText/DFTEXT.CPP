	/* Standard C Includes */
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <ctype.h>
#include <string.h>

	/* User Defined Includes */
#include "fileutil.h"
#include "daggen.h"
#include "dagfile1.h"

	/* Size of string arrays */
#define MAX_STRING_LENGTH 256

	/* Version information */
#define DFTEXT_VERSION "DFText v0.2b"

	/* Debugging status */
#define DEBUG FALSE

	/* Release version or for debugging ?*/
#define RELEASE FALSE

#if RELEASE
  #define RSC_FILE "arena2\\text.rsc"
#else
  #define RSC_FILE "text.rsc"
#endif

#define TEMP_FILE "text.ttt"


RSC_TYPE rscfile;



/*========== Begin Main Program ===========================================*/
short main (short argv, char *argc[]) {
  char buffer[MAX_STRING_LENGTH + 1], *file_ptr, ch;
  boolean del = FALSE, prompt = TRUE, replace = FALSE;
  boolean extract = TRUE;
  short index, i, j;

	/* Open log file for debugging */
  open_log_file ("dftext.log");


	/* Parse the parameters */
  for (i = 1, j = 0; i < argv; i++) {
    file_ptr = strchr (argc[i], '/');

    if (file_ptr) {
      if (!stricmp (argc[i], "/d"))
	del = TRUE;
      else if (!stricmp (argc[i], "/y"))
	prompt = FALSE;
      else if (!stricmp(argc[i], "/s"))
	extract = FALSE;
     }
    else {
      if (j == 1)
	strcpy (buffer, argc[i]);
      else if (j == 0)
	index = (short) strtol(argc[i], NULL, 0);

      j++;
     }
   }

	/* Check arguments */
  if (argv < 2 || (j < 1 && del) || (j < 2 && !del)) {
    printf ("Need at least two arguments on command line!\n\n");
    printf ("%s, March 1998 - Created by Dave Humphrey\n", DFTEXT_VERSION);
    printf ("   DFTEXT [/d] [/y] [/s] <index> [filename]\n\n");
    printf ("      [/d] ....... Delete the text index from the file.\n");
    printf ("      [/y] ....... Default Yes to all prompts.\n");
    printf ("      [/s] ....... Save text file to text index.\n");
    printf ("      [filename] . Specifies the text file to save as or read from.\n");
    printf ("                   Required for all operations except delete.\n");
    printf ("      [index] .... Specifies the text index to add, modify, delete\n");
    printf ("                   or extract. Required for all operations.\n\n");
    printf ("   Examples:\n");
    printf ("      Add/Modify Text:  dftext 8751 newtext.txt /s\n");
    printf ("      Extract Text:     dftext 8775 newtext.txt\n");
    printf ("      Delete Text:      dftext 1265 /d\n\n");
    printf ("Any problems, comments etc can be emailed to me at aj589@freenet.carleton.ca\n\n");

    exit (FALSE);
   }
  else if (j >= 2 && del) {
    printf ("Too many arguments...Ignoring some\n");
   }

	/* Attempt to read the text file header */
  if (!rscfile.read_header (RSC_FILE)) {
    printf ("Error: Could not read RSC header from file '%s'!\n", RSC_FILE);
    printf ("       %s\n\n", df_err_msg[df_err_code]);
    exit (FALSE);
   }

	/* Read in the specified text file */
  if (j > 1 && !del && !extract) {
    file_ptr = rscfile.read_textfile (buffer);

	/* Check for errors */
    if (!file_ptr) {
      printf ("Error: Failed to read text file '%s'!\n", buffer);
      printf ("       %s\n", df_err_msg[df_err_code]);
      return (FALSE);
     }

	/* Prompt user if index already exists */
    if (rscfile.find_type(index) != -1) replace = TRUE;

    if (prompt) {
      if (replace)
	printf ("Text Index of %d already exists, Replace with New Text [Y/N]?", index);
      else
	printf ("Add text index %d to file [Y/N]?", index);

      ch = 0;

      while (ch != 'y' && ch != 'n')
	ch = tolower(getch());

      printf ("\n");
      if (ch == 'n') return (FALSE);
     }

	  /* Attempt to add and write the file */
    if (!rscfile.add_record (RSC_FILE, index, file_ptr)) {
      printf ("Error: Failed to save RSC file '%s'!\n", RSC_FILE);
      printf ("       %s\n\n", df_err_msg[df_err_code]);
      return (FALSE);
     }

    if (replace)
      printf ("   Replaced text index %d with file '%s'\n\n", index, buffer);
    else
      printf ("   Added text index %d with file '%s'\n\n", index, buffer);
   }

  else if (del) {

    if (rscfile.find_type(index) == -1) {
      printf ("Error: No text entry with index of %d was found!\n\n", index);
      return (FALSE);
     }

    if (prompt) {
      printf ("Text Index of %d found, Delete [Y/N]?", index);
      ch = 0;

      while (ch != 'y' && ch != 'n')
	ch = tolower(getch());

      printf ("\n");
      if (ch == 'n') return (FALSE);
     }

    if (!rscfile.del_record (RSC_FILE, index)) {
      printf ("Error: Could not delete index %d from file '%s'\n", index, RSC_FILE);
      printf ("       %s\n\n", df_err_msg[df_err_code]);
     }
    else
      printf ("   Deleted text index %d from file '%s'\n\n", index, RSC_FILE);
   }
	/* Merely extract text */
  else if (extract && j >= 2) {

    if (rscfile.find_type(index) == -1) {
      printf ("Error: Text index %d not found in file '%s'!\n\n", index, RSC_FILE);
      return (FALSE);
     }

	/* Get the text */
    file_ptr = rscfile.extract_text (RSC_FILE, index);

    if (!file_ptr) {
      printf ("   Error: Could not extract text index %d!\n", index);
      printf ("          %s\n\n", df_err_msg[df_err_code]);
      return (FALSE);
     }

    if (fileexists(buffer) && prompt) {
      printf ("   File '%s' already exists, overwrite [Y/N]?", buffer);
      ch = 0;

      while (ch != 'y' && ch != 'n')
	ch = tolower(getch());

      printf ("\n");
      if (ch == 'n') return (FALSE);
     }

    if (write_textfile (buffer, file_ptr))
      printf ("   Text index %d saved as file %s.\n\n", index, buffer);
    else
      printf ("   Error: Failed to save file %s!\n\n", buffer);
   }

  return (TRUE);
 }
/*========== End Main Program =============================================*/
