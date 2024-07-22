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
  int done;		/* Loop counter */
  long filesize, size;

	/* Open the output files */
  qbn_log = openfile ("qbnsize.log", "wt");

	/* Find each QBN in current directory and parse */
  done = findfirst("*.qbn", &file_block, 0);

  while (!done) {
	/* Open the QBN file for input */
    printf ("Reading File: '%s' \n", file_block.ff_name);
    f = openfile (file_block.ff_name, "rb");
    filesize = get_filesize (f);
    fprintf (qbn_log, "%s: ", file_block.ff_name);

	/* Skip the 16-Byte Header */
    fseek (f, 16, SEEK_SET);
    size = 60;

    size += read_int(f) * 19;
    fprintf (qbn_log, "%d, ", read_int (f));
    fprintf (qbn_log, "%d, ", read_int (f));
    size += read_int(f) * 20;
    size += read_int(f) * 24;
    fprintf (qbn_log, "%d     ", read_int (f));
    size += read_int(f) * 33;
    size += read_int(f) * 14;
    size += read_int(f) * 87;
    size += read_int(f) * 8;

    if (size == filesize)
      fprintf (qbn_log, "Size OK\n");
    else
      fprintf (qbn_log, "Size %ld != %ld\n", size, filesize);

    fclose (f);	/* Close QBN file */
    done = findnext(&file_block);	/* Find next QBN file */
  } while (!done);

  fclose (qbn_log);	/* Close output files */

  return (TRUE);
 }