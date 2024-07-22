	/* Standard 'C' Includes */
#include <stdlib.h>
#include <conio.h>
#include <stdio.h>
#include <string.h>
#include <dos.h>
#include <direct.h>
#include <dir.h>
#include <alloc.h>

	/* User Defined Includes */
#include "genutil.h"
#include "fileutil.h"


/*========== Converts a Message Name into a Hash Value ===================*/
long msg_hash (const char far *msg) {
  long int val = 0;

  while ((*msg == '_' || *msg == '=') && *msg)
    msg++;

  while(*msg && *msg != '_')
    val = (val<<1) + *msg++;

  return (val);
 }
/*========== End of function msg_hash() ==================================*/



int main (int argv, char *argc[]) {
  FILE *f;	/* File pointer */
  boolean done = FALSE;	/* Loop counter */
  char *buf, *temp_ptr;	/* Temp input buffers */
  unsigned long find;


  if (argv > 1)
    find = strtoul (argc[1], NULL, 0);
  else
    return (-1);

  printf ("Searching for value %08lx...\n", find);

	/* Try and open file... */
  if (!(f = fopen ("msg.dat", "rt")) ) return(FALSE);

	/* Attempt to allocate temp input buffer */
  buf = create_ptr (100 + 1);

  while (!done) {
	/* Read line */
    done = (boolean) !read_eol (f, buf);

	/* Remove trailing/leading spaces...if any */
    temp_ptr = lstrip(buf);
    rstrip(temp_ptr);

	/* Create the hash numerical value */
    if (msg_hash(temp_ptr) == find) {
      printf ("Found a match: '%s'\n\n", temp_ptr);
      break;
     };

   }; /* End of while loop */

  fclose (f);	/* Close file */
  delete buf;
  return (0);
 }