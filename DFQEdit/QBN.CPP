/* QBN0.CPP - Displays Contents of 1st QBN Section */
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <dir.h>


	/* User Defined Includes */
#include "genutil.h"
#include "fileutil.h"

boolean main (void) {
  FILE *qbn_log;	/* File pointers to output file */
  FILE *f;		/* File pointer to current input file */
  struct ffblk file_block;	/* DOS File Control Block Structure */
  int done, i;		/* Loop counters */

	/* Open the output files */
  qbn_log = openfile ("qbn.log", "wt");

	/* Find each QBN in current directory and parse */
  done = findfirst("*.qbn", &file_block, 0);

  while (!done) {
	/* Open the QBN file for input */
    printf ("Reading File: '%s'\n", file_block.ff_name);
    f = openfile (file_block.ff_name, "rb");

    fprintf (qbn_log, "%13s: ", file_block.ff_name);

	/* Move to offset of 1st Offset/Size Values and read values */
    for (i = 0; i < 16; i++) {
      fprintf (qbn_log, "%02X ", fgetc(f));
     }

    fseek (f, 58, SEEK_SET);
    fprintf (qbn_log, " - %04X\n", read_int (f));
    fclose (f);	/* Close QBN file */
    done = findnext(&file_block);	/* Find next QBN file */
  } while (!done);

  fclose (qbn_log);	/* Close output files */

  return (TRUE);
 }