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
  FILE *null;
  FILE *reward;
  FILE *msg, *msg1;
  FILE *f;		/* File pointer to current input file */
  struct ffblk file_block;	/* DOS File Control Block Structure */
  int done, i;		/* Loop counters */
  unsigned offset, size;
  unsigned char ch1, ch2, ch3, ch4;

	/* Open the output files */
  qbn_log = openfile ("qbn0.log", "wt");
  reward = openfile ("reward0.log", "wt");
  msg = openfile ("msg0.log", "wt");
  msg1 = openfile ("msg0.dat", "wt");
  null = openfile ("null0.log", "wt");

	/* Find each QBN in current directory and parse */
  done = findfirst("*.qbn", &file_block, 0);

  while (!done) {
	/* Open the QBN file for input */
    printf ("Reading File: '%s'\n", file_block.ff_name);
    f = openfile (file_block.ff_name, "rb");

	/* Move to offset of 1st Offset/Size Values and read values */
    fseek (f, 16, SEEK_SET);
    size = read_int (f);
    fseek (f, 36, SEEK_SET);
    offset = read_int (f);

    fprintf (qbn_log, "%s: #%s, offs 0x%X - ", file_block.ff_name, size, offset);
    fprintf (reward, "%13s:\n", file_block.ff_name);
    fprintf (msg, "%13s:\n", file_block.ff_name);
    fprintf (null, "%13s:\n", file_block.ff_name);


	/* Jump to offset of 1st Section */
    if (size == 0) {
      fprintf (qbn_log, "NO SECTION\n");
     }
    else if (!fseek (f, offset, SEEK_SET)) {
	/* Read in all the sections */
      for (i = 0; i < size; i++) {
	fprintf (qbn_log, "\n   SECTION #%02X (i = %02X)\n", fgetc(f), i);
	ch1 = fgetc (f);
	ch2 = fgetc (f);
	fprintf (qbn_log, "          Reward = %02X %02X\n", ch1, ch2);
	fprintf (reward,  "     %02X %02X  -  ", ch1, ch2);
	ch1 = fgetc (f);
	ch2 = fgetc (f);
	ch3 = fgetc (f);
	ch4 = fgetc (f);
	fprintf (qbn_log, "            Type = %02X %02X %02X %02X\n", ch1, ch2, ch3, ch4);
	fprintf (reward,  "%02X %02X %02X %02X\n", ch1, ch2, ch3, ch4);
	ch1 = fgetc (f);
	ch2 = fgetc (f);
	ch3 = fgetc (f);
	ch4 = fgetc (f);
	fprintf (qbn_log, "        Msg Type = %02X %02X %02X %02X\n", ch1, ch2, ch3, ch4);
	fprintf (msg, "     %02X %02X %02X %02X  -  ", ch1, ch2, ch3, ch4);
	fprintf (msg1, " %02X%02X%02X%02X\n", ch4, ch3, ch2,  ch1);
	ch1 = fgetc (f);
	ch2 = fgetc (f);
	ch3 = fgetc (f);
	ch4 = fgetc (f);
	fprintf (qbn_log, "            NULL = %02X %02X %02X %02X\n", ch1, ch2, ch3, ch4);
	fprintf (null, "     %02X %02X %02X %02X\n", ch1, ch2, ch3, ch4);
	ch1 = fgetc (f);
	ch2 = fgetc (f);
	fprintf (qbn_log, "          Msg ID #1 = %02X %02X\n", ch1, ch2);
	fprintf (msg, "%02X %02X  -  ", ch1, ch2);
	ch1 = fgetc (f);
	ch2 = fgetc (f);
	fprintf (qbn_log, "          Msg ID #2 = %02X %02X\n", ch1, ch2);
	fprintf (msg, "%02X %02X\n", ch1, ch2);
       } /* End of for loop */
     }
    else
     fprintf (qbn_log, "BAD OFFSET!\n");

    fclose (f);	/* Close QBN file */
    done = findnext(&file_block);	/* Find next QBN file */
  } while (!done);

  fclose (qbn_log);	/* Close output files */
  fclose (reward);
  fclose (msg);
  fclose (msg1);
  fclose (null);

  return (TRUE);
 }