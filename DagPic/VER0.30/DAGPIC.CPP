/* DAGPIC v0.30 - Copyright (c) 1996-1997 by Dave Humphrey - aj589@freenet.carleton.ca
 * Code is supplied to show the IMG file format and is freely available
 * for modification etc...  Although I've tried to comment things
 * read at your own risk.  May require some modifications in the
 * graphics routines as I used a 3rd party software library XLIB v6.12
 */

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
#include "genutil.h"
#include "fileutil.h"

	/* Of course, include file for this program */
#include "dagpic.h"

	/* Are we doing debugging currently? */
#define DEBUG FALSE

	/* Global variables...pathnames */
char pal_path[80] = "";
char img_path[80] = "";
char pcx_path[80] = "";
char bak_path[80] = "";
char dagpic_err_header[] = "DAGPIC.C v%s, April 1997, Dave Humphrey\n";
char dagpic_err_version[] = VERSION;

boolean delay = FALSE, bitmap = FALSE, display = TRUE;  /* Toggles */
boolean convert_pcx = FALSE, pic_loaded = FALSE, pal_loaded = FALSE;
boolean img_file = FALSE, is_cif = FALSE, is_texture = FALSE;

int modex = X_MODE_360x240;	/* Which graphics mode to use */
int texty = 220;	/* Starting y-pixel to draw text at */
int c;	/* Best display color */

	/* List the valid header types pointing to pictures...must end with a 0
	 * 1st Number is texture->header->type1, 2nd is header->type2 */
unsigned int texture_header_types[MAX_THT][2];
int texture_groups[MAX_GROUPS][MAX_SUBGROUPS];



//========= CLASS DAG_PICTURE PROCEDURES/FUNCTIONS ==========================
void DAG_PICTURE::init (void) {
  buffer = NULL;
  x_offset = 0;
  y_offset = 0;
  width = 0;
  height = 0;
  rle = FALSE;
 }


void DAG_PICTURE::destroy (void) {
  if (buffer) farfree (buffer);
  buffer = NULL;
  x_offset = 0;
  y_offset = 0;
  width = 0;
  height = 0;
  rle = FALSE;
 }

void DAG_PICTURE::save_pcx (const char *filename, char *pal) {
  /* I got some of this from some pcx source code file but alot from
   * actually looking at a pcx file.  There are a few numbers near the
   * beginning which I don't know what they are, and a few other things
   * which I don't understand, but for the most part it works...
   */
  int x, y = 1, num;
  char buffer1[8], pre, temp_file[80], far *temp_ptr;
  FILE *f;

  strcpy(temp_file, filename);

	//Does fileexist, and does user want to overwrite>
  if (!overwrite(&temp_file[0]))
    return;

  if ((f = fopen(temp_file, "wb")) == NULL)
    bug(FILE_ERR_MSG, "wb - '%s'", filename);

  fputc(10, f);
  fputc(5, f);
  fputc(1, f);  //Encoding?
  fputc(8, f);

  fputc(0, f);
  fputc(0, f);
  fputc(0, f);
  fputc(0, f);


  int_to_hex(width - 1, &buffer1[0]);   //Write Width, height
  int_to_hex(height - 1, &buffer1[2]);
  fwrite(buffer1, sizeof(char), 4, f);

  int_to_hex(360, &buffer1[0]);
  fwrite(buffer1, sizeof(char), 2, f);
  fwrite(buffer1, sizeof(char), 2, f);

  for (x = 0; x < 48; x++)
    fputc (0, f); //EGA Palette

  fputc(0, f);
  fputc(1, f); //Color planes?

  int_to_hex(width, &buffer1[0]); //Write Width - Bytes/line
  fwrite(buffer1, sizeof(char), 2, f);

  fputc (1, f);  //256 color
  fputc (0, f);
  fputc (0, f);

  fputc(4, f);
  fputc(0, f);
  fputc(3, f);

  for (x = 0; x < 54; x++)
    fputc(0, f);  //Padding

  pre = *(buffer);
  temp_ptr = buffer;
  temp_ptr++;

  for (x = 0; x < image_size; x++) {
    num = 1;

    while (*temp_ptr == pre && num < 63 && y != width) {
      temp_ptr++;
      num++;
      x++;
      y++;
     }

    if ((unsigned char)pre < 192 && num == 1)
      fputc(pre, f);
    else if (num == 1) {
      fputc(193, f);
      fputc(pre, f);
     }
    else { //RLE...
      fputc((char)(192+num), f);
      fputc(pre, f);
     }

    if (y == width) y = 0;
    pre = *temp_ptr;
    temp_ptr++;
    y++;
   }

  fputc(0x0C, f);  //?

    //Write palette data
  for (x = 0; x < 256*3; x++) {
    fputc((*pal)*4, f);
    pal++;  }

  fclose(f);
}
//========= END CLASS DAG_PICTURE PROCEDURE/FUNCTIONS =======================


//========= CLASS TEXTURE_TYPE PROCEDURE/FUNCTIONS ==========================
void TEXTURE_TYPE::init (void) {
  int i;

  for (i = 0; i < MAX_IMAGES; i++) {
    header[i] = NULL;
    pics[i] = NULL;
   }

  extra_file = NULL;
 }


void TEXTURE_TYPE::destroy (void) {
  int i;

  for (i = 0; i < MAX_IMAGES; i++) {
    if (pics[i]) {
      pics[i]->destroy ();
      farfree (pics[i]);
     }

    if (header[i]) farfree (header[i]);
    header[i] = NULL;
    pics[i] = NULL;
   }

  if (extra_file) farfree (extra_file);
  extra_file = NULL;
 }
//========= END CLASS TEXTURE_TYPE PROCEDURE/FUNCTIONS ======================


//========= CLASS TEXTURE_PIC PROCEDURE/FUNCTIONS ===========================
void TEXTURE_PIC::TEXTURE_PIC (void) {
  init();
 }

void TEXTURE_PIC::~TEXTURE_PIC (void) {
  destroy ();
 }
//========= END CLASS TEXTURE_PIC PROCEDURE/FUNCTIONS =======================


//========= Displays a simple help screen ===================================
void display_help(void) {
  printf ("\n");
  printf ("DAGPIC v%s Copyright (c) 1996-1997 - Dave Humphrey\n", VERSION);
  printf ("Displays Daggerfall IMG/CIF files to the screen and converts them to/from PCX.\n\n");
  printf ("  DAGPIC filename /B:PCX_filename [options]\n");
  printf ("     filename..........The IMG/CIF file\n");
  printf ("     /B:PCX_filename...The PCX image file\n\n");
  printf ("  OPTIONS:\n");
  printf ("     [/S[:#]].......Save PCX as IMG/CIF file. Optional number is for\n");
  printf ("                    saving in position # in CIF file (Default = 1)\n");
  printf ("                    If '/S:e' is used, PCX is appended to end of CIF file\n");
  printf ("     [/D]...........Pause before displaying image\n");
  printf ("     [/P:palette]...Uses the specified palette file\n");
  printf ("     [/N]...........Don't display image, no output to screen\n\n");

  printf ("  EXAMPLES:\n");
  printf ("     Save IMG/CIF to PCX....'dagpic file.img /B:file1.pcx'\n");
  printf ("     Save PCX to IMG........'dagpic file.img /B:file1.pcx /S'\n");
  printf ("     Save PCX to CIF........'dagpic file.cif /B:file1.pcx /S:1'\n\n");

  printf ("Run DAGPIC with no command line parameters for prompting. Default palette\n");
  printf ("is MAP.PAL. Bugs/comments etc... are welcome and can be E-Mailed to: \n");
  printf ("aj589@freenet.carleton.ca\n\n");
 }
//========= End of procedure display_help() =================================


//========== Reads in a string ==============================================
boolean edit_scanf (int x, int y, int length, char *string) {
  boolean redraw = TRUE, done = FALSE;
  char ch;

    //Get current coordinates if specified
  if (x == -1) x = wherex();
  if (y == -1) y = wherey();
  *string = 0;	//String is initial empty

  while (!done) {

    if (redraw) {	//Redraw the inputted string
      redraw = FALSE;
      _setcursortype (_NOCURSOR);
      gotoxy (x, y);
      cprintf ("%-*s", length, string);
      gotoxy (x + strlen(string), y);
      _setcursortype (_NORMALCURSOR);
     }
		//Get key
    ch = getch();

				//Backspace...delete a character
    if (ch == 8 && strlen(string) > 0) {
      string[strlen(string) - 1] = '\0';
      redraw = TRUE;
     }
    else if (ch == 13) {	//Enter
      done = TRUE;
     }
    else if (ch == 27) {	//Esc
      return (FALSE);
     } 				//Add to buffer
    else if (isprint(ch) && strlen(string) < length) {
      string[strlen(string) + 1] = '\0';
      string[strlen(string)] = ch;
      redraw = TRUE;
     }
   }; //End of while loop

  return (TRUE); }
//========== End of function edit_scanf() ===================================



//========= Finds the 'whitest' in palette Color for Box+Text ===============
int find_color(char *pal) {
  /* Looks for the color with the highest average in the current palette.
   * This color is returned and used for printing text etc...*/

  int y, r, b, g, avg;
  int best_c = 255, max_avg = 0;

  for (y = 0; y < 255; y++) {  //Look through all colours in palette
	//Read the 3 color values, red, green, and blue
    r = (int) *pal;
    pal++;
    g = (int) *pal;
    pal++;
    b = (int) *pal;
    pal++;

    avg = (r + g + b) / 3; //Compute the average

    if (avg > max_avg) { //Compare with maximum average
      max_avg = avg;
      best_c = y;
     }
   }

  return(best_c); }
//========= End of function find_color() ====================================


//========= Inputs any Parameters not on command line =======================
boolean input (char *pcx_filename, char *img_filename, char *bak_filename, int *image_pos) {
  char buffer[20], ch;	//Temp character buffers

  printf ("\n");

  if (!img_file) {
    printf ("\nEnter IMG/CIF Filename: ");
    if (!edit_scanf (-1, -1, 15, &buffer[0])) return (FALSE);

    if (strlen(buffer) > 0) {
      img_file = TRUE;
      _fstrupr (buffer);
      if (strstr (buffer, ".CIF") != NULL) is_cif = TRUE;
      _fstrcpy (bak_filename, bak_path);
      _fstrcat (bak_filename, buffer);
      _fstrcpy (img_filename, img_path);
      _fstrcat (img_filename, buffer);
      _fstrupr (bak_filename);
      _fstrupr (img_filename);
     }
    else {
      img_file = FALSE;
      strcpy (img_filename, "");
      strcpy (bak_filename, "");
     }

   }

  if (!bitmap) {
    printf ("\nEnter PCX Filename (press <Enter> for none): ");
    if (!edit_scanf (-1, -1, 15, &buffer[0])) return (FALSE);
   }
  else {
    printf ("\nUsing PCX Filename: %s", pcx_filename);
   }

  if (strlen(buffer) > 0) {
    _fstrcpy (pcx_filename, pcx_path);
    _fstrcat (pcx_filename, buffer);
    _fstrupr (pcx_filename);
    bitmap = TRUE;
    convert_pcx = TRUE;

    if (strlen (img_filename) > 0) { //Are we converting or just viewing?
      printf ("\nConvert PCX to IMG/CIF [y/n]?");

      do {
	ch = toupper(getch());
      } while (ch != 'Y' && ch != 'N' && ch != 27 && ch != 13);

      if (ch == 27)	//Esc
	return(FALSE);
      else if (ch == 'Y')
	convert_pcx = TRUE;
      else
	convert_pcx = FALSE;

	//Are we converting to a CIF...if so, what image position
      if (convert_pcx && is_cif) {

	do {
	  printf ("\nSave PCX in which CIF position: ");
	  if (!edit_scanf (-1, -1, 4, &buffer[0])) return (FALSE);
	  if ((*image_pos = atoi(buffer)) == 0)
	    printf ("\nError: Image Position must be greater than 0!\n");
	} while (*image_pos <= 0);
       }
      else
	*image_pos = 1;	//Always 1st position in IMG file

     } //End of if img_filename
   } //End of if valid pcx filename
  else {
    strcpy (pcx_filename, "");
   }

  return (TRUE); }
//========== End of function input() ========================================


//========== Returns TRUE if i is not an invalid Texture Type Indentifier ===
boolean istexture (const unsigned int i) {
  if (i == 0x0080) return (FALSE);
  return (TRUE);
 }
//========== End of function istexture() ====================================


//========== Attempts to Load and Parse a Config file =======================
boolean load_config(char *filename) {
  FILE *f;
  char ch, buffer[MAX_STRING_LENGTH + 1], *temp_ptr, buffer1[MAX_STRING_LENGTH + 1], *temp_ptr1;
  char command[21], arg[81];
  boolean done = FALSE;

  if ((f = fopen(filename, "rt")) == NULL) return(FALSE);

  do {
    buffer[0] = '\0';

    do {
     ch = fgetc(f);
     chrcat(&buffer[0], ch);
    } while (ch != CR && strlen(buffer) < MAX_STRING_LENGTH && ch != EOF);

    if (ch == EOF) {
      done = TRUE;
      buffer[strlen(buffer) - 1] = CR;
     }
    else if (ch == CR)
      buffer[strlen(buffer) - 1] = 0;

    temp_ptr = &buffer[0];

    if (*temp_ptr != '#') {
      while (*temp_ptr == 32 && *temp_ptr != CR)  /* Remove leading spaces */
	temp_ptr++;

      if (*temp_ptr != CR) {
	strcpy(buffer1, temp_ptr);
	temp_ptr1 = &buffer[0];

	while (*temp_ptr1 != 32 && *temp_ptr1 != CR)
	  temp_ptr1++;

	if (*temp_ptr1 != CR) {
	  *temp_ptr1 = 0;
	  strcpy(command, buffer);
	  temp_ptr1++;

	  while (*temp_ptr1 == 32 && *temp_ptr1 != CR)  /* Remove mid spaces */
	    temp_ptr1++;

	  if (*temp_ptr1 != CR) {
	    strcpy(buffer, temp_ptr1);

	    while (*temp_ptr1 != 32 && *temp_ptr1 != CR)
	      temp_ptr1++;

	     *temp_ptr1 = 0;
	     strcpy(arg, buffer);

	     if (strcmp(arg, ".") == 0) arg[0] = 0;

	     if (strcmpi(command, "pal") == 0)
	       strcpy(pal_path, arg);
	     else if (strcmpi(command, "img") == 0)
	       strcpy(img_path, arg);
	     else if (strcmpi(command, "pcx") == 0)
	       strcpy(pcx_path, arg);
	     else if (strcmpi(command, "bak") == 0)
	       strcpy(bak_path, arg);
	     else if (strcmpi(command, "end") == 0)
	       done = TRUE;
	   }
	 }
       }
     } /* End of if != '#' */
  } while (!done);

  fclose(f);
  return(TRUE); }
/*========== End of function load_config ==================================*/


/*========== Loads the game pallette into memory ==========================*/
void load_dagpal (const char *filename, char *pal) {
  int count;
  FILE *f;

	/* Only want to load one palette */
  if (pal_loaded) return;

	/* Attempt and open the file */
  if ((f = fopen (filename, "rb")) == NULL) bug(FILE_ERR_MSG, "rb - '%s'", filename);

	/* If palette is a COL file skip header information */
  if (get_filesize (f) == 776) fseek (f, 8, SEEK_SET);

	/* Read in the colour values */
  for (count = 0; count < 768; count++) {
    if ((*pal = fgetc(f)) == EOF) break;
    pal++;
   }

  pal_loaded = TRUE;
  fclose(f); }
/*========= End of procedure load_pal() ===================================*/


/*========= Loads a picture named *file_ptr into picture_ptr ==============*/
boolean load_dagpic(const char *filename, DAG_PICTURE *pic, int *num_pic, char *pal) {
  unsigned long count, file_size;
  char buffer[4], *temp_pic;
  unsigned int ch, i;
  unsigned long int current_fpos;
  FILE *f;

	/* Attempt to open an IMG file */
  if ((f = fopen (filename, "rb")) == NULL) return(FALSE);
  file_size = get_filesize (f);  /* Get file size */

	/* Special file format for image size of 64kb */
  if (file_size == 64000l && _fstrstr(filename, ".IMG")) {
    pic->y_offset = 0;
    pic->x_offset = 0;
    pic->width = 320;
    pic->height = 200;
    pic->image_size = 64000l;

    if ((pic->buffer = (char far *) farmalloc (pic->image_size + 1)) == NULL) {
      if (!convert_pcx)
	bug (MEM_ERR_MSG, "load_dagpic(64kb) - pic->buffer (%d)", pic->image_size);
      else
	return(FALSE);
     }

    temp_pic = pic->buffer;

			/* Read in image data */
    for (count = 1; count <= pic->image_size && count <= file_size; count++)
      *temp_pic = fgetc(f);

    (*num_pic)++;  /* Increase number of pictures loaded */
    pic++;
   } /* End of loading file_size = 64768b */
  else if (file_size == 64768l && _fstrstr(filename, ".IMG"))	{ //320x200 image with attached palette
    pic->y_offset = 0;
    pic->x_offset = 0;
    pic->width = 320;
    pic->height = 200;
    pic->image_size = 64000l;

    if ((pic->buffer = (char far *) farmalloc (pic->image_size + 1)) == NULL) {
      if (!convert_pcx)
	bug (MEM_ERR_MSG, "load_dagpic(64768b) - pic->buffer (%ld)", pic->image_size);
      else
	return(FALSE);
     }

    temp_pic = pic->buffer;

		/* Read in image data */
    for (count = 1; count <= pic->image_size; count++, temp_pic++) /* && count <= file_size; count++) */
      *temp_pic = fgetc(f);

    (*num_pic)++;  /* Increase number of pictures loaded */
    pic++;

		/* Load palette contained in file */
    for (count = 1; count < 256*3; count++) {
      if ((*pal = fgetc(f)) == EOF) break;
      pal = pal + 1;
     }

    pal_loaded = TRUE;
   } /* End of load 320x200 image with attached palette */
  else {	/* Load regular IMG/CIF */

    do { /* Load multiple images in CIF/IMG file */
      current_fpos = ftell(f);
      if (fread(&buffer[0], sizeof(char), 2, f) != 2) return(FALSE);
      pic->x_offset = HexToInt(buffer);
      if (fread(&buffer[0], sizeof(char), 2, f) != 2) return(FALSE);
      pic->y_offset = HexToInt(buffer);

		/* Read 5th + 6th byte...image width, and convert from Hex to INT */
      if (fread(&buffer[0], sizeof(char), 2, f) != 2) return(FALSE);
      pic->width = HexToInt(buffer);

		/* Read 7th and 8th bytes...image height, and convert from Hex to INT */
      if (fread(&buffer[0], sizeof(char), 2, f) != 2) return(FALSE);
      pic->height = HexToInt(buffer);

      fgetc(f);     /* Skip 2 bytes of 0's */
      fgetc(f);

		/* Read 11th and 12th bytes...image size */
      if (fread(&buffer[0], sizeof(char), 2, f) != 2) return(FALSE);
      pic->image_size = HexToInt(buffer);

		/* Check for weapon/alternate type RLE CIF file */
      if (is_cif && pic->height*pic->width != pic->image_size) {
	pic->image_size = pic->height * pic->width;	//Set proper size
	pic->rle = TRUE;
       }
      else
	pic->rle = FALSE;

		/* Check image size....make sure its a valid one */
      if (pic->image_size > 65000l || pic->width > 340 || pic->height > 220) {
	if (!convert_pcx)
	  bug ("Image is too large to be displayed...I think", "%s - (%d x %d)", filename, pic->width, pic->height);
	else
	  return(FALSE);
       }

      if ((pic->buffer = (char far *) farmalloc (pic->image_size + 1)) == NULL) {
	if (!convert_pcx)
	  bug (MEM_ERR_MSG, "load_dagpic() - pic->buffer (%ld)", pic->image_size);
	else
	  return(FALSE);
       }

      temp_pic = pic->buffer;

      if (!pic->rle) {	/* Non RLE encoded picture */

		/* Read in image data */
	for (count = 1; count <= pic->image_size && count <= current_fpos + file_size; temp_pic++, count++)
	  *temp_pic = fgetc(f);
       }
      else {	/* Must decode picture data */
	fseek (f, 64, SEEK_CUR);	/* Skip header_info? */

	for (count = 1; count <= pic->image_size && count <= current_fpos + file_size; temp_pic++, count++) {
	  ch = fgetc(f);

	  if (ch >= RLE_BYTE) { /* Is encoded bit */
	    for (i = 2*RLE_BYTE - ch, ch = fgetc(f); i < RLE_BYTE; i++, temp_pic++, count++)
	      *temp_pic = ch;
	   }
	  else 	/* Regular byte */
	    *temp_pic = ch;
	 }
       }

      (*num_pic)++;  /* Increase number of pictures loaded */
      pic++;
    } while (ftell(f) + 10 < file_size && *num_pic < MAX_IMAGES);  /* More images to load? */
   }

  fclose(f);
  return (TRUE); }
/*========== End of load_pic() ============================================*/


/*========== Loads a user defined font into memory ========================*/
boolean load_font (char *userfont) {
  FILE *f;
  char font_file[16] = "smalthin.fnt", ch;

  if ((f = fopen (font_file, "rb")) == NULL) return (FALSE);

  do {
    ch = fgetc (f);
    *userfont = ch;
    userfont++;
  } while (ch != EOF);

  fclose (f);
  return (TRUE); }
//========== End of procedure load_font() ===================================


//========= Loads a PCX into memory and converts it to IMG structure ========
boolean load_pcx(const char *filename, PCX_PICTURE *image) {
// this function loads a pcx file into a picture structure, the actual image
// data for the pcx file is decompressed and expanded into a secondary buffer
// within the picture structure, the separate images can be grabbed from this
// buffer later.  also the header and palette are loaded

  FILE *fp;
  int num_bytes, i, row_diff, row, bytes;
  unsigned int count, image_size;
  unsigned char data;
  char far *temp_buffer;

	//open the file
  if ((fp = fopen(filename, "rb")) == NULL)
    bug(FILE_ERR_MSG, "rb - '%s'", filename);

	//load the header
  temp_buffer = (char far *)image;

  for (i = 0; i < 128; i++) {
    temp_buffer[i] = (char)getc(fp);
   } // end for index

  image->header.width++;
  image->header.height++;

	// load the data and decompress into buffer
  count = 0;
  i = 0;

  if ((image->buffer = (char far *) farmalloc(image->header.width * image->header.height + 1)) == NULL)
    bug(MEM_ERR_MSG, "load_pcx() - image->buffer (%ld)", image->header.width * image->header.height + 1);

  row_diff = image->header.bytes_per_line - image->header.width;
  image_size = image->header.width * image->header.height;
  row = 0;
  bytes = 0;

  while(count <= image_size) {

	// is this a rle?
    if (row_diff > 0 && i >= image->header.width) {  //Remove row padding
      if (i == image->header.bytes_per_line) {
	i = 0;
	row++;
	count--;
       }
      else if (i >= image->header.width) {
	i = 0;
	row++;

	while (i != row_diff) {
	  data = (unsigned char) getc(fp);
	  bytes++;

	  if (data >= 192) {
	    data = (unsigned char)getc(fp);
	    bytes++;
	   }

	  i++; }

	i = 0;
       }

     }
    else {
	// get the first piece of data
      data = (unsigned char) getc(fp);
      bytes++;

      if (data >= 192) {

		// how many bytes in run?
	num_bytes = data - 192;

		// get the actual data for the run
	data  = (unsigned char)getc(fp);
	bytes++;

	// replicate data in buffer num_bytes times
	while(num_bytes-- > 0) {
	  *(image->buffer + count) = data;
	  count++;
	  i++;
	 } // end while

       } // end if rle
      else { // actual data, just copy it into buffer at next location
	*(image->buffer + count) = data;
	count++;
	i++;
       } // end else not rle
     }//End of else not row padding
   } // end while

  return(TRUE); }
//========== End of function load_pcx() =====================================


//========== Attempts to Load a DF Texture into Memory ======================
boolean load_texture (const char *filename, TEXTURE_TYPE *texture, int *num_pic) {
  FILE *f, *ft;	//File pointers
  unsigned long file_size, offset;	//Size of file, offset to something in file
  boolean done = FALSE;	//Loop counter
  TEXTURE_HEADER far *temp_header;	//Temp pointers
  TEXTURE_PIC far *temp_pic;
  unsigned i, j, k, l;		//Loop counters

	//Attempt to open a texture file
  if (!(f = fopen (filename, "rb"))) return(FALSE);
  file_size = get_filesize (f);  //Get file size

  ft = fopen ("texerr.txt", "wt");	//Open error file for writing

  if (!istexture ((texture->type = read_int (f))) ) {
    fprintf (ft, "Invalid Texture File Type = 0x%04X (%5u)\n", texture->type, texture->type);
    err_code = ERR_INVALID;
    fclose (ft);
    fclose (f);
    return (FALSE);
   }

	//Read Text header
  if (fread(&texture->text[0], sizeof(char), 24, f) != 24) {
    err_code = ERR_OVERREAD;
    fclose (ft);
    fclose (f);
    return(FALSE);
   }

  texture->text[24] = 0;	//Make sure string is terminated

	//Read all the picture header information
  while (!done && (*num_pic) < MAX_IMAGES) {
    if (!(temp_header = texture->header[*num_pic] = (TEXTURE_HEADER far *) farmalloc (sizeof (TEXTURE_HEADER))))
       bug (MEM_ERR_MSG, "load_texture() - texture->header (%d)", sizeof(TEXTURE_HEADER));

    temp_header->type1 = read_int (f);
    temp_header->offset = read_long (f);
    temp_header->type2 = read_int (f);

    fprintf (ft, "  FOUND THT = 0x%04X (%5u),  0x%04X (%5u)\n", temp_header->type1, temp_header->type1, temp_header->type2, temp_header->type2);

    #if DEBUG
      printf ("  FOUND THT = 0x%04X (%5u),  0x%04X (%5u)\n", temp_header->type1, temp_header->type1, temp_header->type2, temp_header->type2);
    #endif

    if (fread (temp_header->extra_header, sizeof (char), 12, f) != 12) {
      err_code = ERR_OVERREAD;
      fclose (ft);
      fclose (f);
      return (FALSE);
     }

    (*num_pic)++;

	//Make sure we haven't run into the start of the PIC headers
    if (texture->header[0] && ftell(f) >= texture->header[0]->offset)
      done = TRUE;
    else if (ftell(f) > file_size) {
      fclose (ft);
      fclose (f);
      err_code = ERR_OVERREAD;
      return (FALSE);
     }
   } //End of while loop

	//Create groupings
  for (i = 0; i < MAX_GROUPS; i++) {
    texture_groups[i][0] = -1;
   }

  for (i = 0; i < *num_pic; i++) {
    j = 0;
    done = FALSE;

	//Find proper group to add, or create new
    while (!done) {
      if (texture_groups[j][0] == -1)	//End of groups and no match...create new
	done = TRUE;
      else { //Search group for a offset match
	k = 0;

	while (texture_groups[j][k] != -1 && k < MAX_SUBGROUPS) {
	  offset = labs (texture->header[texture_groups[j][k]]->offset - texture->header[i]->offset);

	  if (offset % 28 == 0 && offset / 28 <= 10) {
	    done = TRUE;
	    break;	//A match...
	   }

	  k++;
	 }	//End of while searching groups for offset match

	if (done != TRUE) {
	  j++;
	  if (j == MAX_GROUPS) done = TRUE;
	 }
       } //End of else search group for offset match
     } //End of while loop

	//Add to group
    if (j < MAX_GROUPS) {
      if (texture_groups[j][0] == -1) { //Create new
	texture_groups[j][0] = i;
	texture_groups[j][1] = -1;
       }
      else { 	//Add to existing
	while (texture_groups[j][k] != -1 && k < MAX_SUBGROUPS)
	  k++;	//Find end of list

	if (k < MAX_SUBGROUPS) {
	  texture_groups[j][k] = i;
	  if (k + 1 != MAX_SUBGROUPS) texture_groups[j][k + 1] = -1;
	 }
	else {	//Error overflow
	  fclose (ft);
	  fclose (f);
	  err_code = ERR_ARRAY_OVERFLOW;
	  return (FALSE);
	 }
       }
     }
    else {
      fclose (ft);
      fclose (f);
      err_code = ERR_ARRAY_OVERFLOW;
      return (FALSE);
     }

   } //End of for loop  (create groupings)

  if ((offset = ftell (f)) == -1L) {
    fclose (ft);
    fclose (f);
    err_code = ERR_OVERREAD;
    return (FALSE);
   }

	//Read in texture picture headers
  for (i = 0; i < *num_pic; i++) {
    fseek (f, texture->header[i]->offset, SEEK_SET);	//Jump to header info

    if (!(temp_pic = texture->pics[i] = (TEXTURE_PIC far *) farmalloc (sizeof (TEXTURE_PIC))))
      bug (MEM_ERR_MSG, "load_texture() - texture->pics[%d] (%d)", i, sizeof (TEXTURE_PIC));

    temp_pic->x_offset = read_int (f);
    temp_pic->y_offset = read_int (f);
    temp_pic->width    = read_int (f) * 4;
    temp_pic->height   = read_int (f);
    temp_pic->image_size = temp_pic->width * temp_pic->height;

    fprintf (ft, "Reading Texture Picture Header:\n");
    fprintf (ft, "   x_offset = %4d,   y_offset = %4d\n", temp_pic->x_offset, temp_pic->y_offset);
    fprintf (ft, "      width = %4d,     height = %4d\n\n", temp_pic->width, temp_pic->height);

    #if DEBUG
    printf ("Reading Texture Picture Header:\n");
    printf ("   x_offset = %4d,   y_offset = %4d\n", temp_pic->x_offset, temp_pic->y_offset);
    printf ("      width = %4d,     height = %4d\n\n", temp_pic->width, temp_pic->height);
    #endif

    if (fread (temp_pic->extra_header, sizeof (char), 20, f) != 20) {
      fclose (ft);
      fclose (f);
      err_code = ERR_OVERREAD;
      return (FALSE);
     }
   }

	//Read in picture data according to texture grouping
  for (j = 0, offset = 0; j < MAX_GROUPS && texture_groups[j][0] != -1; j++) {

    fprintf (ft, "\nImage Group %d: ", j);

	//Find last pic header in group
    for (k = 0, i = 0; k < MAX_SUBGROUPS && texture_groups[j][k] != -1; k++) {
      fprintf (ft, "%d ", texture_groups[j][k]);

      if (texture->header[i]->offset < texture->header[texture_groups[j][k]]->offset)
	i = texture_groups[j][k];
     }

	//Jump to start of pic data
    if (fseek (f, texture->header[i]->offset + 28, SEEK_SET)) {
      fclose (ft);
      fclose (f);
      err_code = ERR_OVERREAD;
      return (FALSE);
     }

	//Sub-Grouping loop
    for (l = k, k = 0; k < MAX_SUBGROUPS && texture_groups[j][k] != -1; k++) {
      i = texture_groups[j][k];
      temp_pic = texture->pics[i];

	//Adjust size according to groupings
      if (l == 1) {
	temp_pic->height /= 1;
	temp_pic->image_size /= 1;
       }
      else {
	temp_pic->height /= 4;
	temp_pic->image_size /= 4;
       }

	//Attempt to allocate memory for picture data
      if (!(temp_pic->buffer = (char far *) farmalloc (temp_pic->image_size + 1)))
	bug (MEM_ERR_MSG, "load_texture() - texture->pics[%d]->buffer (%d)", i, temp_pic->image_size + 1);

	//Attempt to read in image data
      if (fread (temp_pic->buffer, sizeof(char), (unsigned)temp_pic->image_size, f) != temp_pic->image_size) {
	fclose (ft);
	fclose (f);
	err_code = ERR_OVERREAD;
	return (FALSE);
       }
     } //End of for k loop (sub-groups)

    if (ftell (f) > offset) offset = ftell (f);	//Save max file offset
   } //End of for j loop (groups)

  if (offset < file_size) {
    texture->extra_file_length = file_size - offset;

    if (!(texture->extra_file = (char far *) farmalloc (texture->extra_file_length)))
      bug (MEM_ERR_MSG, "load_texture() - texture->extra_file (%d)", texture->extra_file_length);

    fseek (f, offset, SEEK_SET);
    fread (texture->extra_file, sizeof (char), (unsigned)texture->extra_file_length, f);
   }
  else {
    texture->extra_file_length = 0;
    texture->extra_file = NULL;
   }


	//Close the files
  fclose (f);
  fclose (ft);

  return (TRUE); }
//========== End of Function load_texture () ================================


//========== Modifies a File Dependant on the Image Pos =====================
void mod_savefile (char *buffer, int i) {
  char buffer1[80], buffer2[4], *temp_ptr1;

  sprintf(&buffer2[0], "%d", i);

  if ((temp_ptr1 = strrchr(buffer, '.')) != NULL) {
    strcpy(buffer1, temp_ptr1);  //Save extension

    if ((strlen(buffer) - strlen(temp_ptr1)) <= 8 - strlen(buffer2)) {
      *temp_ptr1 = '\0';	    //Terminate pcx file
      strcat(buffer, buffer2);   //Add image # to filename
      strcat(buffer, buffer1);  //Add extension to filename
     }
    else {
      *(temp_ptr1 - strlen(buffer)) = '\0';	    //Terminate pcx file
      strcat(buffer, buffer2);   //Add image # to filename
      strcat(buffer, buffer1);  //Add extension to filename
     }
   }
  else if (strlen(buffer) <= 8 - strlen(buffer2))
    strcat(buffer, buffer2);
  else
    strxfrm(buffer + 8 - strlen(buffer2), buffer2, strlen(buffer2));
 }
//========== End of procedure mod_savefile() ================================


//========== Sees if File Exists and Asked if User wants to Overwrite =======
boolean overwrite (char *filename) {
  FILE *f;	//File pointer
  char buffer[25], ch;	//Temp string buffers

		//Attempt to open file for writing to
  while ((f = fopen(filename, "rb")) != NULL) {
    fclose(f);

    if (display) {
      x_rect_fill(0, texty + 10, 360, 240, VisiblePageOffs, 0);  //Clear area
      x_printf (1, texty + 10, VisiblePageOffs, c, "File %s already exists. Overwrite? [y/n]?", filename);
     }
    else { //Text mode
      printf ("\nFile %s already exists. Overwrite? [y/n]?", filename);
     }

    do {
       ch = toupper(getch());
    } while (ch != 'Y' && ch != 'N' && ch != 27);

    if (ch == 27)
      return (FALSE);
    else if (ch == 'Y')
      break;
    else if (ch == 'N') {

      if (display) {
	boolean done = FALSE;

	buffer[0] = '\0';
	x_rect_fill(0, texty + 10, 360, 240, VisiblePageOffs, 0);  //Clear area
	x_printf (1, texty + 10, VisiblePageOffs, c, "Save Image as: ");

	do {
	  ch = getch();

	  if (isprint(ch) && strlen(buffer) < 15) {
	    buffer[strlen(buffer) + 1] = '\0';
	    buffer[strlen(buffer)] = ch;
	   }
	  else if (ch == 27) //Esc
	    return (FALSE);
	  else if (ch == 13) //enter
	    done = TRUE;
	  else if (ch == 8 && strlen(buffer) > 0)  //Del
	    buffer[strlen(buffer) - 1] = '\0';

	  x_rect_fill(100, texty + 10, 360, 240, VisiblePageOffs, 0);  //Clear area
	  x_printf (100, texty + 10, VisiblePageOffs, c, "%s", buffer);
	} while(!done);

	strcpy(filename, buffer);
       }
      else { //Text mode
	printf ("\nSave Image as: ");
	scanf ("%13s", &buffer[0]);
	strcpy(filename, buffer);
       }
     } //End of if (ch == 'N')
   } //End of while loop

  return (TRUE); }
//========== End of function overwrite() ====================================


//========== Puts a Daggerfall Image onto the Screen ========================
void put_dagpic(int x, int y, unsigned int width, unsigned int height, char far *pic) {
  int x1, y1, i = 0;
			//Print out the image
  for (y1 = y; y1 < y + height; y1++)
    for (x1 = x; x1 < x + width; x1++, i++, pic++)
      x_put_pix(x1, y1, VisiblePageOffs, *pic);
  //Replace with put_pixel routine if not using XLIB library
 }
//========== End of procedure put_dagpic() ==================================


//========== Saves an IMG or CIF file =======================================
boolean save_img(const char *filename, DAG_PICTURE *pic, PCX_PICTURE *pcx_pic, int num_pic, const int image_pos) {
  FILE *f;
  char buffer[4];
  unsigned int image_size, i;

			       //Overwrite file for writing new data
  if ((f = fopen(filename, "wb")) == NULL) return(FALSE);
  if (num_pic == 0) num_pic = 1;

  for (i = 1; i <= num_pic; i++, pic++) {
    int_to_hex(pic->x_offset, &buffer[0]);   //Write 4 bytes of header info
    fwrite(&buffer[0], sizeof(char), 2, f);
    int_to_hex(pic->y_offset, &buffer[0]);
    fwrite(&buffer[0], sizeof(char), 2, f);

    if (i != image_pos) { //Regular put...
      int_to_hex(pic->width, &buffer[0]); //Write width+height
      fwrite(buffer, sizeof(char), 2, f);
      int_to_hex(pic->height, &buffer[0]);
      fwrite(buffer, sizeof(char), 2, f);

      fputc('\0', f);		//Padding Zeros?
      fputc('\0', f);

		//This will mess up if image_size > 64k
      int_to_hex((unsigned int) pic->image_size, &buffer[0]);
      fwrite(buffer, sizeof(char), 2, f);
      fwrite(pic->buffer, sizeof(char), (unsigned int) pic->image_size, f);
     }
    else {	//Convert PCX to IMG...
      int_to_hex(pcx_pic->header.width, &buffer[0]); //Write width+height
      fwrite(buffer, sizeof(char), 2, f);
      int_to_hex(pcx_pic->header.height, &buffer[0]);
      fwrite(buffer, sizeof(char), 2, f);

      fputc('\0', f);		//Padding Zeros?
      fputc('\0', f);

      image_size = pcx_pic->header.width * pcx_pic->header.height;
      int_to_hex(image_size, &buffer[0]);
      fwrite(buffer, sizeof(char), 2, f);
      fwrite(pcx_pic->buffer, sizeof(char), image_size, f);
     } //End of else put new data
   } //End of for loop

  if (image_pos == num_pic + 1 && num_pic > 1) { //Insert PCX at end of CIF
    int_to_hex(pcx_pic->header.width/2, &buffer[0]);   //Write 4 bytes of header info
    fwrite(&buffer[0], sizeof(char), 2, f);
    int_to_hex(pcx_pic->header.height, &buffer[0]);
    fwrite(&buffer[0], sizeof(char), 2, f);

    int_to_hex(pcx_pic->header.width, &buffer[0]); //Write width+height
    fwrite(buffer, sizeof(char), 2, f);
    int_to_hex(pcx_pic->header.height, &buffer[0]);
    fwrite(buffer, sizeof(char), 2, f);

    fputc('\0', f);		//Padding Zeros?
    fputc('\0', f);

    image_size = pcx_pic->header.width * pcx_pic->header.height;
    int_to_hex(image_size, &buffer[0]);
    fwrite(buffer, sizeof(char), 2, f);
    fwrite(pcx_pic->buffer, sizeof(char), image_size, f);
   }

  fclose(f);
  return(TRUE); }
//========== End of function save_img() =====================================



//========== Saves a TEXTURE FILE ===========================================
boolean save_texture(const char *filename, TEXTURE_TYPE *texture, PCX_PICTURE *pcx_pic, int num_pic, const int image_pos) {
  FILE *f;	//File pointer
  unsigned i, j, k;	//loop counters
  long offset;

			       //Overwrite file for writing new data
  if (!(f = fopen(filename, "wb"))) return(FALSE);
  if (num_pic == 0) num_pic = 1;

	//Write texture type and text header
  write_int (f, texture->type);
  fwrite (texture->text, sizeof (char), 24, f);

	//Write texture headers
  for (i = 0; i < num_pic; i++) {
    write_int (f, texture->header[i]->type1);
    write_long(f, texture->header[i]->offset);
    write_int (f, texture->header[i]->type2);
    fwrite ("\0\0\0\0\0\0\0\0\0\0\0\0", sizeof (char), 12, f);
   }

	//Write picture headers
  for (i = 0; i < num_pic; i++) {
    fseek (f, texture->header[i]->offset, SEEK_SET);	//Jump to write position

    write_int (f, texture->pics[i]->x_offset);
    write_int (f, texture->pics[i]->y_offset);

	//Find number in sub-group
    for (j = 0; j < MAX_GROUPS && texture_groups[j][0] != -1; j++) {
      for (k = 0; k < MAX_SUBGROUPS && texture_groups[j][k] != -1 && texture_groups[j][k] != i; k++) {
       }

      if (texture_groups[j][k] == i) break;
     }

	//Count number of images in group
    for (k = 0; k < MAX_SUBGROUPS && texture_groups[j][k] != -1; k++) {
     }

    write_int (f, texture->pics[i]->width / 4);

    if (k == 1)
      write_int (f, texture->pics[i]->height);
    else
      write_int (f, texture->pics[i]->height * 4);

    fwrite (texture->pics[i]->extra_header, sizeof(char), 20, f);
   }

	//Write picture data according to texture grouping
  for (j = 0, offset = 0; j < MAX_GROUPS && texture_groups[j][0] != -1; j++) {

	//Find last pic header in group
    for (k = 0, i = 0; k < MAX_SUBGROUPS && texture_groups[j][k] != -1; k++) {
      if (texture->header[i]->offset < texture->header[texture_groups[j][k]]->offset)
	i = texture_groups[j][k];
     }

	//Jump to start of pic data
    fseek (f, texture->header[i]->offset + 28, SEEK_SET);

	//Sub-Grouping loop
    for (k = 0; k < MAX_SUBGROUPS && texture_groups[j][k] != -1; k++) {
      i = texture_groups[j][k];

	//Write image data
      if (i != image_pos - 1)
	fwrite (texture->pics[i]->buffer, sizeof(char), (unsigned)texture->pics[i]->image_size, f);
      else	//Write pcx image data
	fwrite (pcx_pic->buffer, sizeof (char), (unsigned)texture->pics[i]->image_size, f);
     } //End of for k loop (sub-groups)

	//Save max offset
    if (ftell (f) > offset) offset = ftell (f);
   } //End of for j loop (groups)

	//Write any extra file data
  if (texture->extra_file) {
    fseek (f, offset, SEEK_SET);
    fwrite (texture->extra_file, sizeof (char), (unsigned) texture->extra_file_length, f);
   }

  fclose(f);	//Close file
  return(TRUE); }
//========== End of function save_texture() =================================


//========== Begin main program =============================================
int main (int argv, char *argc[]) {
  DAG_PICTURE picture[MAX_IMAGES];	//Pointers to hold picture data
  PCX_PICTURE pcx_pic;
  TEXTURE_TYPE texture;
  char far *temp_ptr, ch;   	//Temp variables
  char far *pal = NULL, pal_file[80];  	//Palette pointer and filename
  char far *smalthin = NULL; 	   	//Hold font data
  char filename[80], pcx_file[80];  	//PCX filename and file to load
  char buffer[80], bak_file[80];	//Temp string, backup filename
  int i;	       	//Loop counters
  long orig_coreleft;	//How much memory initially?
  int num_pictures = 0; //Number of pictures loaded (used for face images)
  unsigned int image_pos = 1;  	//Save pcx at position in CIF file

//Begin Program Initializations
  err_header_string = &dagpic_err_header[0]; 	//Need these lines for error handling
  err_header_version = &dagpic_err_version[0];
  max_string_length = MAX_STRING_LENGTH;
  orig_coreleft = farcoreleft();	//Save initial avaiable memory
  load_config ("dagpic.cfg");		//Load paths etc... in config file
  _fstrcpy (pal_file, pal_path);  	//Set up initial filenames
  _fstrcat (pal_file, "map.pal");
  _fstrcpy (filename, "");
  _fstrcpy (pcx_file, "");

	//Initialize all dag picture elements
  for (i = 0; i < MAX_IMAGES; i++)
    picture[i].init();

  texture.init ();
  pcx_pic.buffer = NULL;
//End of program initializations


  if (argv > 1) { //Make sure there are arguments
			      //Check ARGs for commands
    for (i = 1; i < argv;i ++) {
				       //Palette
      if ((temp_ptr = strstr(strupr(argc[i]), "/P:")) != NULL) {
	if (_fstrlen (argc[i]) > 3) {
	  _fstrcpy (pal_file, pal_path);
	  _fstrcat (pal_file, temp_ptr + 3);  //Move to start of palette filename
	  _fstrupr (pal_file);
	 }
       }
					//Delay before displaying
      else if ((temp_ptr = strstr(strupr(argc[i]), "/D")) != NULL && !delay) {
	delay = TRUE;
       }
					//Don't display image
      else if ((temp_ptr = strstr(strupr(argc[i]), "/N")) != NULL && display) {
	display = FALSE;
       }
					//Convert to IMG/CIF
      else if ((temp_ptr = strstr(strupr(argc[i]), "/S")) != NULL && !convert_pcx) {
	convert_pcx = TRUE;

	if ((temp_ptr = strstr(strupr(argc[i]), "/S:")) != NULL) {
	  if (strlen(argc[i]) > 3) {
	    if (toupper(*(temp_ptr + 3)) == 'E') //Append to CIF end>
	      image_pos = 0;
	    else 	//Get command-line number where to insert PCX in CIF
	      image_pos = atoi(temp_ptr + 3);
	   }
	 }
	else { //Default save image at pos
	  image_pos = 1;
	 }
       } //End of convert pcx
					//Save as PCX
      else if ((temp_ptr = strstr(strupr(argc[i]), "/B")) != NULL && !bitmap) {
				//Save specified pcx filename
	if ((temp_ptr = strstr(strupr(argc[i]), "/B:")) != NULL) {
	  if (strlen(argc[i]) > 3) {
	    _fstrcpy (pcx_file, pcx_path);
	    _fstrcat (pcx_file, temp_ptr + 3);
	    _fstrupr (pcx_file);
	    bitmap = TRUE;
	   }
	 }
	else { //Default pcx filename
	  bitmap = TRUE;
	 }
       } //end of if /B
				//Help screen
      else if (strchr(argc[i], '?') != NULL) {
	display_help();
	exit(0);
       }
      else { //Must be image filename
	_fstrupr (argc[i]);
	if (strstr (argc[i], ".CIF") != NULL) is_cif = TRUE;
	_fstrcpy (bak_file, bak_path);
	_fstrcat (bak_file, argc[i]);
	_fstrcpy (filename, img_path);
	_fstrcat (filename, argc[i]);
	_fstrupr (filename);
	_fstrupr (bak_file);
	img_file = TRUE;
       }
     } //End of for arg loop
   } //End of at least one argument
  else {  //Get inputs not on command line
    if (!input (&pcx_file[0], &filename[0], &bak_file[0], (int *)&image_pos)) goto END;

    if (!img_file && !bitmap) { //Nothing to do
      printf ("\nNothing to do!\n");
      display_help ();
      goto END;
     }
   }

	//Check to see if image file is a texture type
  if ((temp_ptr = _fstrrchr (filename, '.')) != NULL) {
    temp_ptr++;	//Skip the . character

    while (isdigit (*temp_ptr) && *temp_ptr != 0) {
      temp_ptr++;
     }

    if (*temp_ptr == 0)
      is_texture = TRUE;
    else
      is_texture = FALSE;
   }
  else
    is_texture = FALSE;

  if (bitmap && strlen(pcx_file) == 0) {  //If default pcx file, chose filename
    strcpy (pcx_file, filename);

    if ((temp_ptr = strrchr(pcx_file, '.')) != NULL)
      strcpy(temp_ptr + 1, "pcx");
    else {
      *(pcx_file + strlen(pcx_file)) = '.';
      strcpy(strrchr(pcx_file, '.'), ".pcx\0");
     }
   }

  if (convert_pcx && bitmap) {  //Load pcx and convert to IMG/CIF
    if (!load_pcx(pcx_file, &pcx_pic))
      bug("Failed to read PCX file!", "File = '%s'", pcx_file);
   }
  else if (convert_pcx && !bitmap)
    bug("Must specify pcx file to convert!", NULL);

		       //Allocate palette and image pointers
  if ((pal = (char far *) farmalloc (800)) == NULL) bug(MEM_ERR_MSG, "main() - *pal (800)");
  if ((smalthin = (char far *) farmalloc(800)) == NULL) bug(MEM_ERR_MSG, "main() - *smalthin (800)");

  if (fileexists(filename)) {
    if (is_texture) {	//Load texture
      if (!(pic_loaded = load_texture (filename, &texture, &num_pictures)) && !convert_pcx)
	bug("Could not load texture!", "%s\n   File = '%s'", err_desc[err_code], filename);
     }
    else {	//Load the IMG/CIF
      if (!(pic_loaded = load_dagpic(filename, &picture[0], &num_pictures, pal)) && !convert_pcx)
	bug("Could not load image!", "File = '%s'", filename);
     }
    if (image_pos == 0 && num_pictures > 1) image_pos = num_pictures + 1;
   }
  else if (!convert_pcx)
    bug("Image file not found!", "File = '%s'", filename); //Does file exist?

  load_dagpal (pal_file, pal);   //Load the palette
  if (!load_font (smalthin)) bug("Could not load font file!", NULL);
  c = find_color(pal);

  if (delay && display && !convert_pcx) {
    printf ("\n\n");
    printf ("Display image %s [y/n]?", filename);

    do {
      ch = toupper(getch());
    } while (ch != 'Y' && ch != 'N' && ch != 27);

    if (ch != 'Y') exit(0);
   }

  if (convert_pcx) { //Convert PCX to IMG/CIF
    if (display) {
      if (x_set_mode (modex, 0) < 0) bug("Could not initialize graphics mode!", "Mode = %d", modex);
      x_text_init();      //Setup the text functions
      x_register_userfont(smalthin);
      x_set_font(2);
      x_put_pal_raw ((unsigned char *) pal, 256, 0);
      temp_ptr = &pcx_pic.buffer[0];
      x_rect_fill(0, 0, pcx_pic.header.width + 2, pcx_pic.header.height + 2, VisiblePageOffs, c);   //Display image bounding box
      put_dagpic(1, 1, pcx_pic.header.width, pcx_pic.header.height, temp_ptr);
      x_printf(1, texty, VisiblePageOffs, c, "%s - %d x %d", pcx_file,
		 pcx_pic.header.width, pcx_pic.header.height);

      x_printf (1, texty + 10, VisiblePageOffs, c, "Press any key to Continue....");

      if (getch() == 27)
	goto END;

      x_text_mode();
     } //End of if (display)

	//Just displaying PCX image?
    if (strlen (filename) == 0) goto END;

    printf ("\nAssuming pcx %s (%d x %d) was converted correctly.\n",
	      pcx_file, pcx_pic.header.width, pcx_pic.header.height);


    if (num_pictures > 1 && image_pos >= 1 && (image_pos <= num_pictures || (image_pos == num_pictures + 1 && num_pictures > 1)) && pic_loaded) {
      if (image_pos == num_pictures + 1 && !is_texture)
	printf ("Convert image %s to %s (At End) [y/n]?", pcx_file, filename);
      else if (image_pos == num_pictures + 1 && is_texture) {
	printf ("Cannot Convert image %s to End of Texture File %s!", pcx_file, filename);
	goto END;
       }
      else
	printf ("Convert image %s to %s (Position #%d) [y/n]?", pcx_file, filename, image_pos);
     }
    else if ((image_pos > num_pictures || image_pos < 1) && pic_loaded) {
      printf ("Cannot insert %s in position %d.\nImage position must be between 1 and %d for file %s.\n\n", pcx_file, image_pos, num_pictures, filename);
      goto END;
     }
    else	//Only one image
      printf ("Convert image %s to %s [y/n]?", pcx_file, filename);

    do {
      ch = toupper(getch());
    } while (ch != 'Y' && ch != 'N' && ch != 27);

    if (ch == 27 || ch == 'N') goto END;

    if (pic_loaded || fileexists(filename)) {  //Does the file already exist
      printf("\nWARNING: File %s already exists. Make Backup? [y/n]?", filename);

      do {
	ch = toupper(getch());
      } while (ch != 'Y' && ch != 'N' && ch != 27);

      if (ch == 27)
	goto END;
      else if (ch == 'N') {
	printf("\nOverwriting file %s...", filename);
       }
      else { //Make backup
	strcpy(buffer, bak_file);

	if ((temp_ptr = strrchr(buffer, '.')) != NULL) {
	  temp_ptr++;
	  strxfrm(temp_ptr, "BAK\0", 4);
	 }
	else
	  strcat(buffer, ".BAK");

	if (fileexists(buffer))
	  printf ("\nBackup %s already exists!", buffer);
	else {

	  if (!save_img(buffer, picture, NULL, num_pictures, -1)) {
	    printf ("\nError: Could not create backup %s. Continue [y/n]?", buffer);

	    do {
	      ch = toupper(getch());
	    } while (ch != 'Y' && ch != 'N' && ch != 27);

	    if (ch == 27 || ch == 'N') goto END;
	    printf ("\nBackup %s was not created!", buffer);
	   }
	  else
	    printf ("\nBackup %s successfully written.", buffer);
	 } //End of backup doesn't exist
       } //End of create a backup
     } //End of exist pcx_file?

    if (num_pictures <= 1) { //Simply overwrite
      if (is_texture) {
	if (texture.pics[image_pos - 1]->width != pcx_pic.header.width && texture.pics[image_pos - 1]->height != pcx_pic.header.height && pic_loaded) { //Images are not same size!
	  printf("\n\nWarning: %s(%dx%d) and %s(%dx%d) are not the same\n",
		 pcx_file, pcx_pic.header.width, pcx_pic.header.height,
		 filename, texture.pics[image_pos - 1]->width, texture.pics[image_pos - 1]->height);
	  printf("size.  Cannot yet change size of images in Texture files.\n\n");
	  goto END;
	 } //End of not same size

       } //End of if texture file
      else { //Normal image/cif
	if (picture[image_pos - 1].width != pcx_pic.header.width && picture[image_pos - 1].height != pcx_pic.header.height && pic_loaded) { //Images are not same size!
	  printf("\n\nWarning: %s(%dx%d) and %s(%dx%d) are not the same\n",
		 pcx_file, pcx_pic.header.width, pcx_pic.header.height, filename, picture[image_pos - 1].width, picture[image_pos - 1].height);
	  printf("size.  This _may_ cause undefined results in Daggerfall. Continue [y/n]?");

	  do {
	    ch = toupper(getch());
	  } while (ch != 'Y' && ch != 'N' && ch != 27);

	  if (ch != 'Y') goto END;
	 } //End of if not same size
       } //End of if not texture file

      if (is_texture) {
	if (save_texture(filename, &texture, &pcx_pic, num_pictures, image_pos))
	  printf("\nTexture File %s successfully written!\n", filename);
	else
	  printf("\nTexture File %s WAS NOT successfully written!\n", filename);
       }
      else {
	if (save_img(filename, picture, &pcx_pic, num_pictures, image_pos))
	  printf("\nImage File %s successfully written!\n", filename);
	else
	  printf("\nImage File %s WAS NOT successfully written!\n", filename);
       }
     }
    else { //Move to image pos and overwrite
      if (is_texture) {
	if (texture.pics[image_pos - 1]->width != pcx_pic.header.width && texture.pics[image_pos - 1]->height != pcx_pic.header.height && pic_loaded) { //Images are not same size!
	  printf("\n\nWarning: %s(%d)[%dx%d] and %s(%d)[%dx%d] are not the same\n",
		 pcx_file, image_pos, pcx_pic.header.width, pcx_pic.header.height,
		 filename, image_pos, texture.pics[image_pos - 1]->width, texture.pics[image_pos - 1]->height);
	  printf("size.  Cannot yet change size of images in Texture files.\n\n");
	  goto END;
	 } //End of not same size

       } //End of if texture file
      else { //Normal image/cif
	if (fileexists(filename) && image_pos != num_pictures+1 && picture[image_pos - 1].width != pcx_pic.header.width && picture[image_pos - 1].height != pcx_pic.header.height) { //Images are not same size!
	  printf("\n\nWarning: %s(%d)[%dx%d] and %s(%d)[%dx%d] are not the\n",
		 pcx_file, image_pos, pcx_pic.header.width, pcx_pic.header.height,
		 filename, image_pos, picture[image_pos - 1].width, picture[image_pos - 1].height);
	  printf("same size.  This _may_ cause undefined results in Daggerfall. Continue [y/n]?");

	  do {
	    ch = toupper(getch());
	  } while (ch != 'Y' && ch != 'N' && ch != 27);

	  if (ch != 'Y') goto END;
	 } //End of if not same size
       } //End of else IMG/CIF

      if (is_texture) {
	if (save_texture(filename, &texture, &pcx_pic, num_pictures, image_pos))
	  printf("\nTexture File %s successfully written!\n", filename);
	else
	  printf("\nTexture File %s WAS NOT successfully written!\n", filename);

       }
      else {
	if (save_img(filename, picture, &pcx_pic, num_pictures, image_pos))
	  printf("\nImage File %s successfully written!\n", filename);
	else
	  printf("\nImage File %s WAS NOT successfully written!\n", filename);
       }
     } //End of write certain image number
   } //End of convert pcx
  else if (!convert_pcx) {    //Convert from IMG to PCX
    if (display) {
      if (x_set_mode (modex, 0) < 0) bug("Could not initialize graphics mode!", "Mode = %d", modex);
      x_text_init();      //Setup the text functions
      x_register_userfont(smalthin);
      x_set_font(2);
      x_put_pal_raw ((unsigned char *) pal, 256, 0);
     } //End of if display

    for (i = 0; i < num_pictures; i++) { //Display all pictures one by one
      if (display) {
	if (is_texture) {
	  x_rect_fill(0, 0, 2 + texture.pics[i]->width, 2 + texture.pics[i]->height, VisiblePageOffs, c);   //Display image bounding box
	  put_dagpic (1, 1, texture.pics[i]->width, texture.pics[i]->height, texture.pics[i]->buffer);  //Display the picture
	 }
	else {
	  x_rect_fill(0, 0, 2 + picture[i].width, 2 + picture[i].height, VisiblePageOffs, c);   //Display image bounding box
	  put_dagpic (1, 1, picture[i].width, picture[i].height, picture[i].buffer);  //Display the picture
	 }
       }

      if (num_pictures > 1) //Display which image # in file if > 1 image
	if (display) {
	  if (is_texture) {
	    x_printf (1, texty - 10, VisiblePageOffs, c, "%s #%d of %d - %d x %d", filename, i + 1, num_pictures,
		     texture.pics[i]->width, texture.pics[i]->height);
	    x_printf (1, texty, VisiblePageOffs, c, "Texture Header Description: %s\n", texture.text);
	   }
	  else
	    x_printf (1, texty, VisiblePageOffs, c, "%s #%d of %d - %d x %d", filename, i + 1, num_pictures,
		     picture[i].width, picture[i].height);
	 }
	else { //Text mode
	  if (is_texture) {
	    printf ("\n\n%s #%d of %d - %d x %d\n", filename, i + 1, num_pictures,
		    texture.pics[i]->width, texture.pics[i]->height);
	    printf ("Texture Header Description: %s\n", texture.text);
	   }
	  else
	    printf ("\n\n%s #%d of %d - %d x %d\n", filename, i + 1, num_pictures,
		    picture[i].width, picture[i].height);
	 }
      else //Only one image
	if (display) {
	  if (is_texture)
	    x_printf (1, texty, VisiblePageOffs, c, "%s - %d x %d", filename,
		      texture.pics[i]->width, texture.pics[i]->height);
	  else
	    x_printf (1, texty, VisiblePageOffs, c, "%s - %d x %d", filename,
		      picture[i].width, picture[i].height);
	 }
	else { //Text mode
	  if (is_texture)
	    printf ("%s - %d x %d\n\n", filename, texture.pics[i]->width, texture.pics[i]->height);
	  else
	    printf ("%s - %d x %d\n\n", filename, picture[i].width, picture[i].height);
	 }

      if (bitmap) {   //Save IMG/CIF to pcx file
	strcpy(buffer, pcx_file);

	if (num_pictures > 1)  //Modify pcx filename to reflect image position
	  mod_savefile(&buffer[0], i + 1);

	if (display)
	  x_printf (1, texty + 10, VisiblePageOffs, c, "Convert image %s to %s [y/n]?", filename, buffer);
	else //Text mode
	  printf ("Convert image %s to %s [y/n]?", filename, buffer);

	do {
	  ch = toupper(getch());
	} while (ch != 'Y' && ch != 'N' && ch != 27);

	if (ch == 'Y') {
	  if (is_texture)
	    texture.pics[i]->save_pcx (buffer, pal);
	  else
	    picture[i].save_pcx(buffer, pal);  //Save pcx if specified
	 }
	else if (ch == 27)
	  break;  //Quit displaying
       } //End of if bitmap
      else { //Just get a key
	if (display)
	  x_printf (1, texty + 10, VisiblePageOffs, c, "Press any key to Continue...");
	else //Text mode
	  printf ("\n\nPress any key to Continue...\n");

	if (getch() == 27) break;  //Quit displaying
       } //End of getkey

      if (display) x_rect_fill(0, 0, 360, 240, VisiblePageOffs, 0);  //Clear screen
     } //End of for loop

    if (display) {
      x_text_mode();   //Clean things up
      clrscr();
     }
    else
      printf ("\n\n");
   } //End of if (!convert_pcx) IMG->PCX

END:  //Yes, a goto statement...

			//Free up allocated memory
  if (!is_texture) {
    for (i = 0; i < num_pictures; i++) {
      picture[i].destroy ();
     }
   }
  else {
    texture.destroy ();
   }

  if (smalthin) farfree (smalthin);
  if (pal) farfree (pal);
  if (pcx_pic.buffer) farfree (pcx_pic.buffer);

  printf ("\nComparing Initial/Final Available Memory: %ld / %ld bytes\n", orig_coreleft, farcoreleft());
  printf ("farheapcheck() returns ");

  switch (farheapcheck()) {
    case _HEAPEMPTY: printf ("Heap Empty\n\n"); break;
    case _HEAPOK: printf ("Heap OK\n\n"); break;
    case _HEAPCORRUPT: printf ("Heap Corrupt! - You should reboot.\n\n"); break;
   }

  return (TRUE); }
//========== End of program =================================================
