/* QBN4.CPP - Displays Contents of 4rd QBN Section */
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <dir.h>


	/* User Defined Includes */
#include "genutil.h"
#include "fileutil.h"

boolean main (void) {
  FILE *qbn_log;	/* File pointers to output file */
  FILE *null;
  FILE *f;		/* File pointer to current input file */
  struct ffblk file_block;	/* DOS File Control Block Structure */
  int done, i;		/* Loop counters */
  unsigned offset, size;
  unsigned char ch1, ch2, ch3, ch4;

	/* Open the output files */
  qbn_log = openfile ("qbn3.log", "wt");

	/* Find each QBN in current directory and parse */
  done = findfirst("*.qbn", &file_block, 0);

  while (!done) {
	/* Open the QBN file for input */
    printf ("Reading File: '%s'\n", file_block.ff_name);
    f = openfile (file_block.ff_name, "rb");

	/* Move to offset of 3rd Offset/Size Values and read values */
    fseek (f, 22, SEEK_SET);
    size = read_int (f);
    fseek (f, 42, SEEK_SET);
    offset = read_int (f);

    fprintf (qbn_log, "\n%s: #%d, offs 0x%X - ", file_block.ff_name, size, offset);

	/* Jump to offset of 1st Section */
    if (size == 0) {
      //fprintf (qbn_log, "NO SECTION\n");
     }
    else if (!fseek (f, offset, SEEK_SET)) {
	/* Read in all the sections */
      for (i = 0; i < size; i++) {
//	fprintf (qbn_log, "\n   SECTION #%04X (i = %04X)", read_int(f), i);
	read_int(f);

	ch1 = fgetc (f);
	ch2 = fgetc (f);
	fprintf (qbn_log, "\n       1: %02X %02X", ch1, ch2);
	ch1 = fgetc (f);
	ch2 = fgetc (f);
	fprintf (qbn_log, " %02X %02X", ch1, ch2);
	ch1 = fgetc (f);
	ch2 = fgetc (f);
	fprintf (qbn_log, " %02X %02X", ch1, ch2);

		/* Message Type...Ignore */
	ch1 = fgetc (f);
	ch2 = fgetc (f);
	ch3 = fgetc (f);
	ch4 = fgetc (f);
//	fprintf (qbn_log, "\n       ID: %02X %02X %02X %02X", ch1, ch2, ch3, ch4);

		/* NULL Bytes...ignore */
	ch1 = fgetc (f);
	ch2 = fgetc (f);
	ch3 = fgetc (f);
	ch4 = fgetc (f);

		/* Msg ID's...ignore */
	ch1 = fgetc (f);
	ch2 = fgetc (f);
	ch3 = fgetc (f);
	ch4 = fgetc (f);
	//fprintf (qbn_log, "\n       N2: %02X %02X %02X %02X", ch1, ch2, ch3, ch4);
       } /* End of for loop */
     }
    else
     fprintf (qbn_log, "BAD OFFSET!\n");

    fclose (f);	/* Close QBN file */
    done = findnext(&file_block);	/* Find next QBN file */
  } while (!done);

  fclose (qbn_log);	/* Close output files */

  return (TRUE);
 }