/* QBN7.CPP - Displays Contents of 7th QBN Section */
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <dir.h>


	/* User Defined Includes */
#include "genutil.h"
#include "fileutil.h"

boolean main (void) {
  FILE *qbn_log;	/* File pointers to output file */
  FILE *mob;
  FILE *null1;
  FILE *null2;
  FILE *f;		/* File pointer to current input file */
  struct ffblk file_block;	/* DOS File Control Block Structure */
  int done, i;		/* Loop counters */
  unsigned offset, size;
  unsigned char ch1, ch2, ch3, ch4;
  int mob_count[300];

  for (i = 0; i < 300; i++)
    mob_count[i] = 0;

	/* Open the output files */
  qbn_log = openfile ("qbn7.log", "wt");
  mob = openfile ("mob7.log", "wt");
  null1 = openfile ("null7a.log", "wt");
  null2 = openfile ("null7b.log", "wt");

	/* Find each QBN in current directory and parse */
  done = findfirst("*.qbn", &file_block, 0);

  while (!done) {
	/* Open the QBN file for input */
    printf ("Reading File: '%s'\n", file_block.ff_name);
    f = openfile (file_block.ff_name, "rb");

	/* Move to offset of 7th Offset/Size Values and read values */
    fseek (f, 30, SEEK_SET);
    size = read_int (f);
    fseek (f, 50, SEEK_SET);
    offset = read_int (f);

    fprintf (qbn_log, "\n%s: #%d, offs 0x%X - ", file_block.ff_name, size, offset);
    fprintf (null1, "\n%s: #%d", file_block.ff_name, size);
    fprintf (null2, "\n%s: #%d", file_block.ff_name, size);
    fprintf (mob, "\n%s: #%d", file_block.ff_name, size);

	/* Jump to offset of 1st Section */
    if (size == 0) {
      /* fprintf (qbn_log, "NO SECTION\n"); */
     }
    else if (!fseek (f, offset, SEEK_SET)) {
	/* Read in all the sections */
      for (i = 0; i < size; i++) {
		/* Read the section index */
	ch1 = fgetc (f);

		/* Read in 1st null section */
	ch1 = fgetc (f);
	ch2 = fgetc (f);
	fprintf (null1, "\n       %02X %02X", ch1, ch2);

		/* Read in mob index */
	ch1 = fgetc(f);
	fprintf (mob, "\n       %02X  (%d)", ch1, ch1);
	mob_count[ch1]++;

		/* Read in number ??? */
	ch1 = fgetc (f);
	ch2 = fgetc (f);
	fprintf (qbn_log, "\n       %02X %02X", ch1, ch2);

		/* Read and ignore message hash value */
	ch1 = fgetc (f);
	ch2 = fgetc (f);
	ch1 = fgetc (f);
	ch2 = fgetc (f);

		/* NULL Bytes? */
	ch1 = fgetc (f);
	ch2 = fgetc (f);
	ch3 = fgetc (f);
	ch4 = fgetc (f);
	fprintf (null2, "\n       %02X %02X %02X %02X", ch1, ch2, ch3, ch4);

       } /* End of for loop */
     }
    else
     fprintf (qbn_log, "BAD OFFSET!\n");

    fclose (f);	/* Close QBN file */
    done = findnext(&file_block);	/* Find next QBN file */
  } while (!done);

  fclose (qbn_log);	/* Close output files */
  fclose (null1);
  fclose (null2);

  fprintf (mob, "\n\nMob Count Statistics\n");

  for (i = 0; i < 256; i++) {
    fprintf (mob, "\n  %d: %d", i, mob_count[i]);
   }
  fclose (mob);

  return (TRUE);
 }