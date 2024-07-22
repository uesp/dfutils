	/* Standard C Includes */
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <alloc.h>
#include <dos.h>
#include <string.h>
#include <time.h>
#include <dir.h>

	/* Graphics Includes */
#include "xlib_all.h"
#include "pbm.h"

	/* Used Defined Includes */
#include "gutil.h"
#include "pcx.h"
#include "dagputil.h"

void put_pix (const int x1, const int y1, const int data) {
  x_put_pix (x1, y1, VisiblePageOffs, data);
 }

/*========== Begin Main Program ===========================================*/
boolean main (void) {
  TEXTURE_TYPE texture;
  struct ffblk file_block;
  unsigned char *temp_ptr;
  int i = 0, j, y, sum;
  boolean done = FALSE;

	/* Open the Log File */
  open_log_file ("debugtx5.log");
  write_log_entry ("Picture Headers for Texture Files #1");

	/* Change Directory */
  chdir ("d:\\dagger\\arena2");
  setdisk (3);

//  texture.load_texture ("texture.006");

//  chdir ("c:\\tc\\dagpic");
//  setdisk (2);
//  printf ("\n");

//  return (TRUE);


	/* Find All Texture Files in the DF ARENA2 Directory */
  if (findfirst ("texture.*", &file_block, 0)) done = TRUE;
  printf ("\n");
  y = wherey();

  while (!done) {
    i++;
    gotoxy (1, y);
    printf ("Found %d Textures", i);

	/* Attempt to Load the Texture File */
    if (!texture.load_texture (file_block.ff_name)) {
      write_log_entry ("     ERROR: Failed to Load Texture!");
     }
    else {

      for (j = 0; j < texture.num_images; j++) {
	temp_ptr = (unsigned char*) texture.pics[j]->extra_header;
	write_log_entry ("     0x%02X 0x%02X  0x%02X 0x%02X", (unsigned int)temp_ptr[0], (unsigned int)temp_ptr[1], (unsigned int)temp_ptr[2], (unsigned int)temp_ptr[3]);
       }

      //write_log_entry ("     Num Images = %d", texture.num_images);

      /*
      for (j = 0; j < texture.num_images; j++) {
	temp_ptr = texture.header[j]->extra_header;
	sum = temp_ptr[0] + temp_ptr[1] + temp_ptr[2] + temp_ptr[3] + temp_ptr[4] + temp_ptr[5] + temp_ptr[6] + temp_ptr[7] + temp_ptr[8] + temp_ptr[9] + temp_ptr[10] + temp_ptr[11];
	write_log_entry ("     T1=0x%04X  T2=0x%04X  EX=%d", texture.header[j]->type1, texture.header[j]->type2, sum);
       }
      */
     }

    if (findnext(&file_block)) done = TRUE;
   }

	/* Return to Orig Directory */
  chdir ("c:\\tc\\dagpic");
  setdisk (2);
  printf ("\n");

  return (TRUE);
 }

