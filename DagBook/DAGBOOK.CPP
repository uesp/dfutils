/* Program for parsing the Daggerfall Book File Format */

	/* Standard C Includes */
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>

	/* User Defined Includes */
#include "genutil.h"
#include "fileutil.h"


char buffer[201];	/* Temp input buffer */
int undef[256];


boolean main(int argv, char *argc[]) {
  FILE *f, *f1;	/* File pointer */
  boolean done = FALSE, output = TRUE, undef_found = FALSE;
  int ch;
  int center_line = 0, i, num_headers = 0;

  long offset_to_text = 0;

  if (argv < 2) {
    printf ("Usage: DAGBOOK <filename>\n\n");
    return (FALSE);
   }
  else if (argv >= 3) {
    if (stricmp(argc[3], "/N")) output = FALSE;
   }

	/* Attempt to open the file */
  if (!(f = fopen (argc[1], "rb"))) {
    printf ("Could not open file %s!\n", argc[1]);
    return (FALSE);
   }

  f1 = fopen ("undef.log", "at");

  if (output) printf ("\n/*========== Begin Book =======================================*/\n");
  fprintf (f1, "Reading file '%s'...\n", argc[1]);

	/* Read in the Title and authors name */
  if (fread (buffer, sizeof(char), 64, f) != 64) done = TRUE;
  if (output) printf ("Title: %s\n", buffer);
  if (fread (buffer, sizeof(char), 160, f) != 160) done = TRUE;
  if (output) printf ("Written by %s\n", buffer);

	/* Skip some of the header */
  if (fread (buffer, sizeof(char), 10, f) != 10) done = TRUE;
  num_headers = read_int(f);
  offset_to_text = read_long(f);
  if (output) printf ("Headers = %d,    Offset to Text = 0x%04lX\n", num_headers, offset_to_text);

	/* Skip to start of text */
  fseek (f, offset_to_text, SEEK_SET);

  i = 0;
  buffer[0] = 0;

	/* Parse the file */
  while (!done) {
    if ((ch = fgetc(f)) == EOF) done = TRUE;

	/* Means next line is centered on screen */
    if (ch == 0xFD)
      center_line = 1;

	/* Unknown */
    else if (ch == 0xFB) {
      ch = fgetc(f);
      ch = fgetc(f);
      if (ch != 0) fprintf (f1, "   0xFB value Read: 3rd Byte = 0x%02X\n", ch);
     }

	/* Font selection char */
    else if (ch == 0xF9) {
      ch = fgetc(f);

      if (ch == 0x02) {
	if (output) printf ("{ Start Fancy Font }\n");
       }
      else if (ch == 0x04) {
	if (output) printf ("{ Start Regular Font }\n");
       }
      else
	fprintf (f1, "   { Unknown Font #%d }\n", ch);

     }

	/* Pause display output */
    else if (ch == 0xF6) {

      if (output) {
	printf ("Press any key to continue...");
	if (getch() == 27) done = TRUE;
	delline();
	gotoxy (1, wherey());
       }
     }

	/* End of line...display current line in memory */
    else if (ch == 0 || i >= 200) {

      if (output) {
	if (center_line)
	  printf_centx ("%s\n", buffer);
	else
	  printf ("%s\n", buffer);
       }

      center_line = 0;
      i = 0;
      buffer[0] = 0;
     }
	/* Add character to end of current line */
    else if (ch >= ' ' && ch <= '~') {
      buffer[i] = ch;
      buffer[i + 1] = 0;
      i++;
     }
	/* An undefined value */
    else if (ch != EOF) {
      undef[ch]++;
      undef_found = TRUE;
     }
   }

  fclose (f);
  if (output) printf ("/*========== End of Book =======================================*/\n");

  if (undef_found) {
    fprintf (f1, "   Undefined Characters found in file '%s'...\n", argc[1]);

    for (i = 0; i < 256; i++)
      if (undef[i]) fprintf (f1, "     Char = 0x%02X, Counts = %d\n", i, undef[i]);
   }

  fclose (f1);

	/* Output the undefined Characters */
  return (TRUE);
 }