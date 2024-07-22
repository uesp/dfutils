
	/* General include files */
#include <sys\stat.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <alloc.h>
#include <ctype.h>
#include <stdarg.h>

	/* Include files for the graphics routines, can remove if your using
	 * or creating your own */
#include "xlib.h"
#include "xpal.h"
#include "xpoint.h"
#include "xrect.h"
#include "xtext.h"

	/* Some utility files of mine (e-mail me if you want 'em) */
#include "fileutil.h"
#include "keyboard.h"
#include "pic.h"
#include "pcx.h"

	/* Are we doing debugging currently? */
#define DEBUG FALSE

#define PIXEL_COLOR 1
#define TEXT_COLOR 9


	/* The picture data */
char *pic = NULL;
short width = 0, height = 0, offset = 0;
unsigned short image_size = 0;
short speed = 1;


	/* For converting to PCXs */
PCX_TYPE pcx;

	/* ModeX stuff */
unsigned char *font_ptr;
unsigned char *pal_ptr;


boolean done = FALSE;
boolean redraw = TRUE;

void put_pix (const int x1, const int y1, const int c) {
  x_put_pix(x1, y1, VisiblePageOffs, c);
 }

/*========== Loads the game pallette into memory ==========================*/
unsigned char *load_dagpal (const char *filename) {
  FILE *f;	/* File pointer */
  unsigned char *pal;	/* Temp pointer */

	/* Attempt and open the file */
  if (!(f = openfile (filename, "rb"))) {
    err_code = ERR_FILE;
    return (NULL);
   }

	/* If palette is a COL file skip header information */
  if (get_filesize(f) == 776) fseek (f, 8, SEEK_SET);

	/* Create the pointer */
  pal = (unsigned char *)create_ptr (769);

	/* Read in the colour values */
  if (fread (pal, sizeof(char), 768, f) != 768) {
    fclose (f);
    err_code = ERR_READ;
    delete pal;
    return (NULL);
   }

  fclose(f);
  err_code = ERR_NONE;
  return (pal);
 }
/*========= End of Function load_pal() ====================================*/


/*========== Loads a user defined font into memory ========================*/
unsigned char *load_font (const char *filename) {
  FILE *f;	/* File pointer */
  long l;
  unsigned short i;
  unsigned char *font;	/* Temp font pointer */

  if (!(f = openfile (filename, "rb"))) {
    err_code = ERR_FILE;
    return (NULL);
   }

  l = get_filesize(f);

  if (l > 64000l) {
    fclose (f);
    err_code = ERR_64KB;
    return (NULL);
   }

	/* Create the pointer */
  font = (unsigned char *) create_ptr ((unsigned short) l + 1);

  for (i = 0; i < l; i++) {
    font[i] = (unsigned char) fgetc(f);
   }

  if (l == 0) {
    fclose (f);
    err_code = ERR_READ;
    delete font;
    return (NULL);
   }

  fclose (f);
  err_code = ERR_NONE;
  return (font);
 }
/*========== End of Function load_font() ==================================*/


/*========== Puts a Daggerfall Image onto the Screen ======================*/
void put_dagpic(int x, int y, char *data) {
  register unsigned int x1, y1, i = offset;

	/* Print out the image */
  for (y1 = y; y1 < y + height && i < image_size; y1++)
    for (x1 = x; x1 < x + width && i < image_size; x1++, i++)
      x_put_pix(x1, y1, VisiblePageOffs, data[i]);

  /* Replace with put_pixel routine if not using XLIB library */
 }
/*========== End of procedure put_dagpic() ================================*/


/*========== Attempt to load a Daggerfall Picture =========================*/
boolean load_dagpic (const char *filename) {
  FILE *f;	/* File pointer */
  long l;

  if (!filename) {
    err_code = ERR_NULL;
    return (FALSE);
   }

	/* Attempt to open file */
  if (!(f = openfile(filename, "rb"))) {
    err_code = ERR_FILE;
    return (FALSE);
   }

	/* Get the filesize */
  image_size = l = get_filesize(f);

	/* Make sure its a good size */
  if (l > 65500l) {
    l = 64000;
//    fclose (f);
//    err_code = ERR_64KB;
//    return (FALSE);
   }

	/* Allocate pointer */
  DESTROY(pic);
  pic = create_ptr ((unsigned int)l);
  fseek (f, 96l*32000l, SEEK_SET);

	/* Otherwise, read in the file */
  if (fread(pic, sizeof(char), (unsigned int)l, f) != l) {
    fclose (f);
    DESTROY(pic);
    err_code = ERR_READ;
    return (FALSE);
   }

  width = 10;
  height = 10;
  offset = 0;

  fclose (f);
  err_code = ERR_NONE;
  return (TRUE);
 }
/*========== End of Function load_dagpic() ================================*/


/*========== Attempt to Save a Daggerfall Picture =========================*/
boolean save_dagpic (const char *filename) {
  FILE *f;	/* File pointer */

  if (!filename) {
    err_code = ERR_NULL;
    return (FALSE);
   }

	/* Attempt to open file */
  if (!(f = openfile(filename, "wb"))) {
    err_code = ERR_FILE;
    return (FALSE);
   }

	/* Write the data */
  fwrite (pic + offset, sizeof(char), (unsigned int) width * height, f);
  fclose (f);
  err_code = ERR_NONE;
  return (TRUE);
 }
/*========== End of Function load_dagpic() ================================*/


/*========== Begin main program ===========================================*/
int main (int argv, char *argc[]) {

	/* Open log file for debugging */
  open_log_file ("genpic.log");

	/* Assume the only argument is the filename to view */
  if (argv < 2) {
    printf ("Not enough command line arguments!\n");
    return (FALSE);
   }

	/* Attempt to load the picture */
  if (!load_dagpic(argc[1])) {
    bug (ERROR_MSG, "main() - load_dagpic(%s)", argc[1]);
   }

	/* Attempt to setup the ModeX stuff */
  if (!(font_ptr = load_font("smalthin.fnt")))
    bug (ERROR_MSG, "main() - load_font(smalthin.fnt)");

  if (!(pal_ptr = load_dagpal ("dagpic.pal")))
    bug (ERROR_MSG, "main() - load_pal(dagpic.pal)");

  if (x_set_mode (3, 320) == -1)
    bug ("Could not set ModeX video mode!");

  x_put_pal_raw (pal_ptr, 256, 0);
  x_text_init();
  x_register_userfont (font_ptr);
  x_set_font (2);

	/* Main loop */
  while (!done) {

    if (redraw) {
      redraw = FALSE;

		/* Clear the screen */
      x_rect_fill (0, 0, 360, 240, VisiblePageOffs, 0);

		/* Draw the picture */
      put_dagpic (1, 1, pic);
      x_put_pix (        0,          0, VisiblePageOffs, PIXEL_COLOR);
      x_put_pix (width + 2,          0, VisiblePageOffs, PIXEL_COLOR);
      x_put_pix (        0, height + 2, VisiblePageOffs, PIXEL_COLOR);
      x_put_pix (width + 2, height + 2, VisiblePageOffs, PIXEL_COLOR);

		/* Update text */
      x_printf (2, 210, VisiblePageOffs, TEXT_COLOR, "Image: '%s'", argc[1]);
      x_printf (2, 220, VisiblePageOffs, TEXT_COLOR, " Size: %3dx%3d", width, height);
      x_printf (2, 230, VisiblePageOffs, TEXT_COLOR, "Offset: %5d bytes", offset);
     }

	/* Check the keyboard status */
    keyboard.check();

    if (keyboard.get_esc_state())
      done = TRUE;
    else if (keyboard.last_code) {

      switch (keyboard.last_code) {
	case SCAN_KEYPAD_UP:

	  if (height > 1) {
	    height -= speed;
	    if (height < 1) height = 1;
	    redraw = TRUE;
	   }

	  break;
	case SCAN_KEYPAD_DOWN:

	  if (height < 200) {
	    height += speed;
	    if (height > 200) height = 200;
	    redraw = TRUE;
	   }

	  break;
	case SCAN_KEYPAD_LEFT:

	  if (width > 1) {
	    width -= speed;
	    if (width < 1) width = 1;
	    redraw = TRUE;
	   }

	  break;
	case SCAN_KEYPAD_RIGHT:

	  if (width < 320) {
	    width += speed;
	    if (width > 320) width = 320;
	    redraw = TRUE;
	   }

	  break;
	case SCAN_PGUP:
	case SCAN_KEYPAD_PGUP:

	  if (offset < 1000) {
	    offset += speed;
	    if (offset > 1000) offset = 1000;
	    redraw = TRUE;
	   }

	  break;
	case SCAN_PGDN:
	case SCAN_KEYPAD_PGDN:

	  if (offset > 0) {
	    offset -= speed;
	    if (offset < 0) offset = 0;
	    redraw = TRUE;
	   }

	  break;
       };
     }
    else if (keyboard.last_key) {
      switch (keyboard.last_key) {
	case 's':
	case 'S':
	  pcx.convert_from_lbm ((unsigned char *)(pic + offset - 2), width, height, pal_ptr);
	  pcx.save_pcx ("genpic.pcx");
	  delay (KEY_DELAY);
	  break;
	case 'l':
	case 'L':
	  pcx.load_pcx ("icons.pcx", NULL);
	  delete pic;
	  pcx.convert_to_lbm (&pic, width, height);
	  offset = 2;
	  save_dagpic ("icon1.img");
	  delay (KEY_DELAY);
	  redraw = TRUE;
	  break;
	case '1':
	  speed = 1;
	  break;
	case '2':
	  speed = 2;
	  break;
	case '3':
	  speed = 3;
	  break;
	case '4':
	  speed = 4;
	  break;
	case '5':
	  speed = 5;
	  break;
	case '6':
	  speed = 6;
	  break;
	case '7':
	  speed = 7;
	  break;
	case '8':
	  speed = 8;
	  break;
	case '9':
	  speed = 9;
	  break;
	case '0':
	  speed = 10;
	  break;
       };
     }


   }


	/* Return to text mode */
  x_text_mode();

	/* Delete things */
  DESTROY(pic);
  DESTROY(font_ptr);
  DESTROY(pal_ptr);

  return (TRUE);
 }
/*========== End of program ===============================================*/
