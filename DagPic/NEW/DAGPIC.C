/* DAGPIC v0.1 - Copyright (c) 1996 by Dave Humphrey - aj589@freenet.carleton.ca
 * Code is supplied to show the IMG file format and is freely available
 * for modification etc...  Although I've tried to comment things
 * read at your own risk.  May require some modifications in the
 * graphics routines as I used a 3rd party software library XLIB v6.12
 */


//Some conversion routines I got from a BMP conversion source code
#define HexToLong(Start) (unsigned long)(*(Start) & 0xff) + \
			 (unsigned long)((*(Start + 1) << 8) & 0xffff) + \
			 (unsigned long)(((unsigned long)*(Start+3) << 24) & 0xffffffffl) + \
			 (unsigned long)(((unsigned long)*(Start+2) << 16) & 0xffffffl)

#define HexToInt(Start) (unsigned int)(*(Start) & 0xff) + \
			(unsigned int)((*(Start + 1) << 8) & 0xffff)

//General include files
#include <sys\stat.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <malloc.h>
#include <ctype.h>
#include <graph.h>
//Include files for the graphics routines, can remove if your using/creating
//your own
#include "c:\temp\temp2\inc\xlib.h"
#include "c:\temp\temp2\inc\xpal.h"
#include "c:\temp\temp2\inc\xpoint.h"
#include "c:\temp\temp2\inc\xrect.h"
#include "c:\temp\temp2\inc\xtext.h"

#define VERSION 0.22
#define MAX_IMAGES 25   //Number of images which can loaded at one time
#define CR 0x0A

//Standard boolean type
typedef enum {TRUE = 1, FALSE = 0} boolean;

//STRUCTURES
typedef struct pcx_header_typ {
        char manufacturer;
        char version;
        char encoding;
        char bits_per_pixel;
        int x,y;
        int width,height;
        int horz_res;
        int vert_res;
        char ega_palette[48];
        char reserved;
        char num_color_planes;
        int bytes_per_line;
        int palette_type;
        char padding[58];
  } pcx_header, *pcx_header_ptr;


typedef struct pcx_picture_typ {
	pcx_header header;
	char far *buffer;
  } pcx_picture, *pcx_picture_ptr;


typedef struct dag_picture_typ {
	int x_offset, y_offset;
	unsigned int width, height;
	unsigned int image_size;
	char far *buffer;
  } dag_picture, *dag_picture_ptr;


//Procedure/function prototypes
    void bug           (char *string, char *filename);
    void chrcat        (char *buf, const char ch);
    void display_help  (void);
unsigned get_filesize  (char *filename);
     int find_color    (char *pal);
 boolean if_exist      (char *filename);
    void int_to_hex    (unsigned int i, char *buf);
 boolean load_config   (char *filename);
 boolean load_pcx      (char *filename, pcx_picture_ptr image);
    void load_dagpal   (char *pal, char *file);
 boolean load_dagpic   (dag_picture_ptr picture_ptr, char *file_ptr, int *num_pic, const boolean convert_pcx, char *pal, char *pal_file);
 boolean load_font     (char *userfont);
    void long_to_hex   (unsigned long i, char *buf);
    void mod_savefile  (char *buffer, int i);
    void put_dagpic    (int x, int y, unsigned width, unsigned height, char far *pic);
 boolean save_img      (char *filename, dag_picture_ptr pic, pcx_picture_ptr pcx_pic, int num_pic, int image_pos);
    void save_pcx      (dag_picture_ptr pic, char *pal, char *filename, const int c, const int mode);

//Global variables...pathnames
char pal_path[80] = "";
char img_path[80] = "";
char pcx_path[80] = "";
char bak_path[80] = "";


//========= Notifies User of a bug/error ====================================
void bug(char *string, char *filename) {
  /* General error message routine notifying user of an error */

  x_text_mode(); //Switch to text mode

  _clearscreen(_GWINDOW);
  printf ("DAGPIC.C v%3.2f, 5 January 1997, Dave Humphrey\n", VERSION);
  printf ("%s\n", string);
  if (filename != NULL) printf ("Filename = %s\n", filename);
  printf ("\n\n");
  _exit(1); //Exit program
 }
//========= End of procedure bug() ==========================================


//========= Appends a character to end of string ============================
void chrcat(char *buf, const char ch) {
  int i = strlen(buf);

  *(buf + i + 1) = '\0';
  *(buf + i) = ch; }
//========= End of procedure chrcat() =======================================


//========= Displays a simple help screen ===================================
void display_help(void) {
  printf ("\n");
  printf ("DAGPIC v%3.2f  Copyright (c) 1996 - Dave Humphrey\n", VERSION);
  printf ("                      Modified by - Jeff Hanson\n");
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

  printf ("Default palette is MAP.PAL. Bugs/comments etc... are welcome and can be\n");
  printf ("E-Mailed to: aj589@freenet.carleton.ca\n");
 }
//========= End of procedure display_help() =================================


//========= Finds the 'whitest' in palette Color for Box+Text ===============
int find_color(char *pal) {
  /* Looks for the color with the highest average in the current palette.
   * This color is returned and used for printing text etc...*/

  int y, r, b, g, avg;
  int best_c = 255, max_avg = 0;

  for (y = 0; y < 255; y++) {  //Look through all colours in paleete
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


//========= Returns the size of the file ====================================
unsigned int get_filesize(char *filename) {
  FILE *f;
  struct stat file_stat;

  if (!if_exist(filename)) return(0);
  f = fopen(filename, "r");
  fstat(fileno(f), &file_stat);
  fclose(f);
  return((unsigned int)file_stat.st_size); }
//========= End of function get_filesize() ==================================


//========= Returns TRUE if file exists =====================================
boolean if_exist(char *filename) {
  FILE *f;
		       //Attempt to open file
  if ((f = fopen(filename, "r")) == NULL) return(FALSE);
  fclose(f);	   //File was opened and therefore exists, close it
  return(TRUE); }
//========= End of function if_exist() ======================================


//========== Converts a Int to Hexadecimal String ===========================
void int_to_hex (unsigned int i, char *buf) {
  unsigned int rem, div;

  rem = i/256;
  div = i - rem * 256;
  *(buf + 1) = (unsigned char) rem;
  *(buf) = (unsigned char) div;
  *(buf + 2) = '\0';
 }
//========== End of procedure int_to_hex() ==================================


//========== Attempts to Load and Parse a Config file =======================
boolean load_config(char *filename) {
  FILE *f;
  char ch, buffer[101], *temp_ptr, buffer1[101], *temp_ptr1;
  char command[21], arg[81];
  boolean done = FALSE;

  if ((f = fopen(filename, "rt")) == NULL) return(FALSE);

  do {
    buffer[0] = '\0';

    do {
     ch = fgetc(f);
     chrcat(&buffer[0], ch);
    } while (ch != CR && strlen(buffer) < 100 && ch != EOF);

    if (ch == EOF) {
      done = TRUE;
      buffer[strlen(buffer) - 1] = CR;
     }
    else if (ch == CR)
      buffer[strlen(buffer) - 1] = 0;

    temp_ptr = &buffer[0];

    if (*temp_ptr != '#') {
      while (*temp_ptr == 32 && *temp_ptr != CR)  //Remove leading spaces
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

	  while (*temp_ptr1 == 32 && *temp_ptr1 != CR)  //Remove mid spaces
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
     } //End of if != '#'
  } while (!done);

  fclose(f);
  return(TRUE); }
//========== End of function load_config ====================================


//========== Loads the game pallette into memory ============================
void load_dagpal (char *pal, char *file) {
  int count;
  FILE *f;
		//Attempt and open the file
  if ((f = fopen (file, "rb")) == NULL) bug("Error: Could not open Palette!", file);

		//Read in the colour values
  for (count = 1; count < 256*3; count++) {
    if ((*pal = fgetc(f)) == EOF) break;
     pal = pal + 1; }

  fclose(f); }
//========= End of procedure load_pal() =====================================


//========= Loads a picture named *file_ptr into picture_ptr ================
boolean load_dagpic(dag_picture_ptr pic, char *filename, int *num_pic, const boolean convert_pcx, char *pal, char *pal_file) {
  unsigned int count, file_size;
  char buffer[4];
  FILE *f;
			//Attempt to open an IMG file
  if ((f = fopen (filename, "rb")) == NULL) return(FALSE);
  file_size = get_filesize(filename);  //Get file size
  
  if (file_size == 64000)
  	{	pic->y_offset = 0;
  		pic->x_offset = 0;
  		pic->width = 320;
  		pic->height = 200;
  		pic->image_size = 64000;

		if ((pic->buffer = (char far *)_fmalloc(pic->image_size + 1)) == NULL && !convert_pcx)
	        bug("ERROR: Could not allocate sizeof(picture)!", gcvt(pic->image_size, 10, NULL));
    	else if((pic->buffer = (char far *)_fmalloc(pic->image_size + 1)) == NULL)
      		return(FALSE);

  			//Read in image data
    	for (count = 1; count <= pic->image_size && count <= file_size; count++)
      		*(pic->buffer + count - 1) = fgetc(f);

    	(*num_pic)++;  //Increase number of pictures loaded
    	pic++;
  	    fclose(f);
  	    load_dagpal(pal, pal_file);   //Load the palette
  	}
  
  else if (file_size == 64768)
  	{	pic->y_offset = 0;
  		pic->x_offset = 0;
  		pic->width = 320;
  		pic->height = 200;
  		pic->image_size = 64000;
  		
  		if ((pic->buffer = (char far *)_fmalloc(pic->image_size + 1)) == NULL && !convert_pcx)
      		bug("ERROR: Could not allocate sizeof(picture)!", gcvt(pic->image_size, 10, NULL));
    	else if((pic->buffer = (char far *)_fmalloc(pic->image_size + 1)) == NULL)
      		return(FALSE);

  			//Read in image data
    	for (count = 1; count <= pic->image_size; count++) //&& count <= file_size; count++)
      		*(pic->buffer + count - 1) = fgetc(f);

    	(*num_pic)++;  //Increase number of pictures loaded
    	pic++;
	    for (count = 1; count < 256*3; count++) {
    		if ((*pal = fgetc(f)) == EOF) break;
     		pal = pal + 1; }
		fclose(f);
  	}
  
  else {
  do { //Load multiple images in CIF/IMG file
    if (fread(&buffer[0], sizeof(char), 2, f) != 2) return(FALSE);
    pic->x_offset = HexToInt(buffer);
    if (fread(&buffer[0], sizeof(char), 2, f) != 2) return(FALSE);
    pic->y_offset = HexToInt(buffer);

		//Read 5th + 6th byte...image width, and convert from Hex to INT
    if (fread(&buffer[0], sizeof(char), 2, f) != 2) return(FALSE);
    pic->width = HexToInt(buffer);

		//Read 7th and 8th bytes...image height, and convert from Hex to INT
    if (fread(&buffer[0], sizeof(char), 2, f) != 2) return(FALSE);
    pic->height = HexToInt(buffer);
    pic->image_size = pic->width * pic->height;

			//Check image size....make sure its a valid one
    if ((pic->image_size > 64000l || pic->width > 320 || pic->height > 200) && !convert_pcx)
      bug("Error: Image is too large to be displayed...I think", filename);
    else if (pic->image_size > 64000l || pic->width > 320 || pic->height > 200)
      return(FALSE);

    if ((pic->buffer = (char far *)_fmalloc(pic->image_size + 1)) == NULL && !convert_pcx)
      bug("ERROR: Could not allocate sizeof(picture)!", gcvt(pic->image_size, 10, NULL));
    else if((pic->buffer = (char far *)_fmalloc(pic->image_size + 1)) == NULL)
      return(FALSE);

    fgetc(f);     //Skip 2 bytes of 0's
    fgetc(f);
    fgetc(f);     //Skip 2 bytes of Image Size
    fgetc(f);
		//Read in image data
    for (count = 1; count <= pic->image_size && count <= file_size; count++)
      *(pic->buffer + count - 1) = fgetc(f);

    (*num_pic)++;  //Increase number of pictures loaded
    pic++;
  } while (ftell(f) + 10 < file_size && *num_pic < MAX_IMAGES);  //More images to load?
  }
  if (file_size != 64000 && file_size != 64768)
  	{  	fclose(f);
    	load_dagpal(pal, pal_file);   //Load the palette
  	}
  return (TRUE); }
//========== End of load_pic() ==============================================


//========== Loads a user defined font into memory ==========================
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
boolean load_pcx(char *filename, pcx_picture_ptr image) {
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
    bug("Error: Could not open PCX file!", filename);

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

  if ((image->buffer = (char far *) _fmalloc(image->header.width * image->header.height + 1)) == NULL)
    bug("Error: Could not allocate PCX buffer!", NULL);

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



//========== Converts a Long Int to Hexadecimal String ======================
void long_to_hex (unsigned long i, char *buf) {
  unsigned long rem, div;

  rem = i/256/256/256;
  div = i - rem * 256 * 256 * 256;
  *(buf + 3) = (unsigned char) rem;

  rem = div/256/256;
  div = div - rem * 256 * 256;
  *(buf + 2) = (unsigned char) rem;

  rem = div/256;
  div = div - rem * 256;
  *(buf + 1) = (unsigned char) rem;
  *(buf) = (unsigned char) div;
  *(buf + 4) = '\0';
 }
//========== End of procedure long_to_hex() =================================


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
//========== End of procedure mod_savefile();


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
boolean save_img(char *filename, dag_picture_ptr pic, pcx_picture_ptr pcx_pic, int num_pic, int image_pos) {
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

      int_to_hex(pic->image_size, &buffer[0]);
      fwrite(buffer, sizeof(char), 2, f);
      fwrite(pic->buffer, sizeof(char), pic->image_size, f);
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


//========== Saves pcx to disk ==============================================
void save_pcx (dag_picture_ptr pic, char *pal, char *filename, const int c, const int mode) {
  /* I got some of this from some pcx source code file but alot from
   * actually looking at a pcx file.  There are a few numbers near the
   * beginning which I don't know what they are, and a few other things
   * which I don't understand, but for the most part it works...
   */
  int x, y = 1, num;
  char buffer[80], ch, pre, temp_file[80], *temp_ptr;
  FILE *f;

  strcpy(temp_file, filename);

		//Attempt to open file for writing to
  while ((f = fopen(temp_file, "rb")) != NULL) {
    fclose(f);

    if (mode == 1) {
      x_rect_fill(0, 190, 320, 200, VisiblePageOffs, 0);  //Clear area
      x_printf (1, 190, VisiblePageOffs, c, "File %s already exists. Overwrite? [y/n]?", temp_file);
     }
    else {
      printf ("\nFile %s already exists. Overwrite? [y/n]?", temp_file);
     }

    do {
       ch = getch();
    } while (toupper(ch) != 'Y' && toupper(ch) != 'N' && ch != 27);

    if (ch == 27)
      return;
    else if (toupper(ch) == 'Y')
      break;
    else if (toupper(ch) == 'N') {

      if (mode == 1) {
	boolean done = FALSE;

	buffer[0] = '\0';
	x_rect_fill(0, 190, 320, 200, VisiblePageOffs, 0);  //Clear area
	x_printf (1, 190, VisiblePageOffs, c, "Save Image as: ");

	do {
	  ch = getch();

	  if (isprint(ch) && strlen(buffer) < 15) {
	    buffer[strlen(buffer) + 1] = '\0';
	    buffer[strlen(buffer)] = ch;
	   }
	  else if (ch == 27) //Esc
	    return;
	  else if (ch == 13) //enter
	    done = TRUE;
	  else if (ch == 8 && strlen(buffer) > 0)  //Del
	    buffer[strlen(buffer) - 1] = '\0';

	  x_rect_fill(100, 190, 320, 200, VisiblePageOffs, 0);  //Clear area
	  x_printf (100, 190, VisiblePageOffs, c, "%s", buffer);
	} while(!done);

	strcpy(temp_file, buffer);
       }
      else {
	printf ("\nSave Image as: ");
	scanf ("%13s", &buffer[0]);
	strcpy(temp_file, buffer);
       }
     } //End of if (ch == 'N')
   } //End of while loop

  if ((f = fopen(temp_file, "wb")) == NULL) bug("Error: Could not create file!", filename);
  fputc(10, f);
  fputc(5, f);
  fputc(1, f);  //Encoding?
  fputc(8, f);

  fputc(0, f);
  fputc(0, f);
  fputc(0, f);
  fputc(0, f);


  int_to_hex(pic->width - 1, &buffer[0]);   //Write Width, height
  int_to_hex(pic->height - 1, &buffer[2]);
  fwrite(buffer, sizeof(char), 4, f);

  int_to_hex(360, &buffer[0]);
  fwrite(buffer, sizeof(char), 2, f);
  fwrite(buffer, sizeof(char), 2, f);

  for (x = 0; x < 48; x++)
    fputc (0, f); //EGA Palette

  fputc(0, f);
  fputc(1, f); //Color planes?

  int_to_hex(pic->width, &buffer[0]); //Write Width - Bytes/line
  fwrite(buffer, sizeof(char), 2, f);

  fputc (1, f);  //256 color
  fputc (0, f);
  fputc (0, f);

  fputc(4, f);
  fputc(0, f);
  fputc(3, f);

  for (x = 0; x < 54; x++)
    fputc(0, f);  //Padding

  pre = *(pic->buffer);
  temp_ptr = pic->buffer;
  temp_ptr++;

  for (x = 0; x < pic->image_size - 1; x++) {
    num = 1;

    while (*temp_ptr == pre && num < 63 && y != pic->width) {
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

    if (y == pic->width) y = 0;
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
//========== End of procedure save_pcx() ====================================


//========== Begin main program =============================================
int main (int argv, char *argc[]) {
  dag_picture picture[100];	//Pointers to hold picture data
  pcx_picture pcx_pic;
  char *temp_ptr, ch;   //Temp variables
  char *pal, pal_file[80];  //Palette pointer and filename
  char *smalthin; 	   //Hold font data
  char filename[80], pcx_file[80];  //pcx filename and file to load
  char buffer[80], bak_file[80];	  //Temp string, backup filename
  int c, i;	          //Loop counters
  int num_pictures = 0;   //Number of pictures loaded (used for face images)
  unsigned int image_pos = 1;      //Save pcx at position in CIF file
  boolean delay = FALSE, bitmap = FALSE, display = TRUE;  //Toggles
  boolean convert_pcx = FALSE, pic_loaded = FALSE;

  load_config("dagpic.cfg");
  strcpy(pal_file, pal_path);  //Set up initial conditions
  strcat(pal_file, "map.pal");
  strcpy(filename, "NULL");
  strcpy(pcx_file, "NULL");

  if (argv > 1) { //Make sure there are arguments
			      //Check ARGs for commands
    for (i = 1; i < argv;i ++) {
				       //Palette
      if ((temp_ptr = strstr(strupr(argc[i]), "/P:")) != NULL) {
	if (strlen(argc[i]) > 3) {
	  strcpy(pal_file, pal_path);
	  strcat(pal_file, temp_ptr + 3);  //Move to start of palette filename
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
	    strcpy(pcx_file, pcx_path);
	    strcat(pcx_file, temp_ptr + 3);
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
	strcpy(bak_file, bak_path);
	strcat(bak_file, argc[i]);
	strcpy(filename, img_path);
	strcat(filename, argc[i]);
       }
     } //End of for arg loop

    if (bitmap && strcmp(pcx_file, "NULL") == 0) {  //If default pcx file, chose filename
      strcpy (pcx_file, filename);

      if ((temp_ptr = strrchr(pcx_file, '.')) != NULL) {
	strcpy(temp_ptr + 1, "pcx");
       }
      else {
	*(pcx_file + strlen(pcx_file)) = '.';
	strcpy(strrchr(pcx_file, '.'), ".pcx\0");
       }
     }

    if (convert_pcx && bitmap) { //Load pcx and convert to IMG/CIF
      if (!load_pcx(pcx_file, (pcx_picture_ptr) &pcx_pic)) bug("Error: Failed to read PCX file!", pcx_file);
     }
    else if (convert_pcx)
      bug("Error: Must specify pcx file to convert!", NULL);

		       //Allocate palette and image pointers
    if ((pal = (char *) malloc (800)) == NULL) bug("ERROR: Could not allocate 800 Bytes!", NULL);
    if ((smalthin = (char *) malloc(800)) == NULL) bug("ERROR: Could not allocate 800 Bytes!", NULL);

    if (if_exist(filename)) {
      if (!(pic_loaded = load_dagpic(picture, filename, &num_pictures, convert_pcx, pal, pal_file)) && !convert_pcx)   //Load the IMG/CIF
	bug("Error: Could not load image!", filename);
      if (image_pos == 0 && num_pictures > 1) image_pos = num_pictures + 1;
     }
    else if (!convert_pcx)
      bug("Error: Image file not found!", filename); //Does file exist?

    //load_dagpal(pal, pal_file);   //Load the palette
    if (!load_font(smalthin)) bug("Error: Could not load font file!", NULL);
    c = find_color(pal);

    if (delay && display && !convert_pcx) {
      printf ("\n\n");
      printf ("Display image %s [y/n]?", filename);

      do {
	ch = getch();
      } while (toupper(ch) != 'Y' && toupper(ch) != 'N' && ch != 27);

      if (toupper(ch) != 'Y') exit(0);
     }

    if (convert_pcx) { //Displays bitmap
      if (x_set_mode (0, 320) < 0) bug("Error: Could not initialize graphics mode!", NULL);
      x_text_init();      //Setup the text functions
      x_register_userfont(smalthin);
      x_set_font(2);
      x_put_pal_raw ((unsigned char *) pal, 256, 0);
      temp_ptr = &pcx_pic.buffer[0];
      //x_rect_fill(0, 0, pcx_pic.header.width + 2, pcx_pic.header.height + 2, VisiblePageOffs, c);   //Display image bounding box
      //put_dagpic(1, 1, pcx_pic.header.width, pcx_pic.header.height, temp_ptr);
      put_dagpic(0, 0, pcx_pic.header.width, pcx_pic.header.height, temp_ptr);
      
      x_printf(1, 180, VisiblePageOffs, c, "%s - %d x %d", pcx_file,
	       pcx_pic.header.width, pcx_pic.header.height);

      x_printf (1, 190, VisiblePageOffs, c, "Press any key to Continue....");

      if (getch() == 27)
	goto END;

      x_text_mode();
      printf ("\nAssuming pcx %s (%d x %d) was converted correctly.\n",
	      pcx_file, pcx_pic.header.width, pcx_pic.header.height);

      if (num_pictures > 1 && image_pos >= 1 && (image_pos <= num_pictures || (image_pos == num_pictures + 1 && num_pictures > 1)) && pic_loaded)
	if (image_pos == num_pictures + 1)
	  printf ("Convert image %s to %s (At End) [y/n]?", pcx_file, filename);
	else
	  printf ("Convert image %s to %s (Position #%d) [y/n]?", pcx_file, filename, image_pos);
      else if ((image_pos > num_pictures || image_pos < 1) && pic_loaded) {
	printf ("Cannot insert %s in position %d.\nImage position must be between 1 and %d for file %s.\n\n", pcx_file, image_pos, num_pictures, filename);
	goto END;
       }
      else
	printf ("Convert image %s to %s [y/n]?", pcx_file, filename);

      //do {
	//ch = toupper(getch());
      //} while (ch != 'Y' && ch != 'N' && ch != 27);

      do {
	ch = getch();
      } while (toupper(ch) != 'Y' && toupper(ch) != 'N' && ch != 27);

      if (ch == 27 || toupper(ch) == 'N') goto END;

      if (pic_loaded || if_exist(filename)) {  //Does the file already exist
	printf("\nWARNING: File %s already exists. Make Backup? [y/n]?", filename);

	do {
	  ch = getch();
	} while (toupper(ch) != 'Y' && toupper(ch) != 'N' && ch != 27);

	if (ch == 27)
	  goto END;
	else if (toupper(ch) == 'N') {
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

	  if (if_exist(buffer))
	    printf ("\nBackup %s already exists!", buffer);
	  else {

	    if (!save_img(buffer, picture, NULL, num_pictures, -1)) {
	      printf ("\nError: Could not create backup %s. Continue [y/n]?", buffer);

	      do {
		ch = getch();
	      } while (toupper(ch) != 'Y' && toupper(ch) != 'N' && ch != 27);

	      if (ch == 27 || toupper(ch) == 'N') goto END;
	      printf ("\nBackup %s was not created!", buffer);
	     }
	    else
	      printf ("\nBackup %s successfully written.", buffer);
	   } //End of backup doesn't exist
	 } //End of create a backup
       } //End of exist pcx_file?

      if (num_pictures <= 1) { //Simply overwrite
	if (picture[image_pos].width != pcx_pic.header.width && picture[image_pos].height != pcx_pic.header.height && pic_loaded) { //Images are not same size!
	  printf("\n\nWarning: %s(%dx%d) and %s(%dx%d) are not the same\n",
		 pcx_file, pcx_pic.header.width, pcx_pic.header.height, filename, picture[image_pos].width, picture[image_pos].height);
	  printf("size.  This _may_ cause undefined results in Daggerfall. Continue [y/n]?");

	  do {
	    ch = getch();
	  } while (toupper(ch) != 'Y' && toupper(ch) != 'N' && ch != 27);

	  if (toupper(ch) != 'Y') goto END;
	 } //End of if not same size

	if (save_img(filename, picture, &pcx_pic, num_pictures, image_pos)) printf("\nFile %s successfully written!\n", filename);
       }
      else { //Move to image pos and overwrite in random access fashion

	if (if_exist(filename) && image_pos != num_pictures+1 && picture[image_pos - 1].width != pcx_pic.header.width && picture[image_pos - 1].height != pcx_pic.header.height) { //Images are not same size!
	  printf("\n\nWarning: %s(%d)[%dx%d] and %s(%d)[%dx%d] are not the\n",
		 pcx_file, image_pos, pcx_pic.header.width, pcx_pic.header.height, filename, image_pos, picture[image_pos - 1].width, picture[image_pos - 1].height);
	  printf("same size.  This _may_ cause undefined results in Daggerfall. Continue [y/n]?");

	  do {
	    ch = getch();
	  } while (toupper(ch) != 'Y' && toupper(ch) != 'N' && ch != 27);

	  if (toupper(ch) != 'Y') goto END;
	 } //End of if not same size

	if (save_img(filename, picture, &pcx_pic, num_pictures, image_pos)) printf("\nFile %s successfully written!\n", filename);
       } //End of write certain image number
     } //End of convert pcx
    else if (display) {    //Display the image (IMG/CIF)
      if (x_set_mode (0, 320) < 0) bug("Error: Could not initialize graphics mode!", NULL);
      x_text_init();      //Setup the text functions
      x_register_userfont(smalthin);
      x_set_font(2);
      x_put_pal_raw ((unsigned char *) pal, 256, 0);

      for (i = 0; i < num_pictures; i++) { //Display all pictures one by one
	//x_rect_fill(0, 0, 2 + picture[i].width, 2 + picture[i].height, VisiblePageOffs, c);   //Display image bounding box
	//put_dagpic (1, 1, picture[i].width, picture[i].height, picture[i].buffer);  //Display the picture
    put_dagpic (0, 0, picture[i].width, picture[i].height, picture[i].buffer);  //Display the picture

	if (num_pictures > 1) //Display which image # in file if > 1 image
	  x_printf(1, 180, VisiblePageOffs, c, "%s #%d of %d - %d x %d", filename, i + 1, num_pictures,
		   picture[i].width, picture[i].height);
	else
	  x_printf(1, 180, VisiblePageOffs, c, "%s - %d x %d", filename,
		   picture[i].width, picture[i].height);

	if (bitmap) {   //Save IMG/CIF to pcx file
	  strcpy(buffer, pcx_file);

	  if (num_pictures > 1)  //Modify pcx filename to reflect image position
	    mod_savefile(&buffer[0], i + 1);

	  x_printf (1, 190, VisiblePageOffs, c, "Convert image %s to %s [y/n]?", filename, buffer);

	  do {
	    ch = getch();
	  } while (toupper(ch) != 'Y' && toupper(ch) != 'N' && ch != 27);

	  if (toupper(ch) == 'Y')
	    save_pcx(&picture[i], pal, buffer, c, 1);  //Save pcx if specified
	  else if (ch == 27)
	    break;  //Quit displaying
	 } //End of if bitmap
	else { //Just get a key
	  x_printf (1, 190, VisiblePageOffs, c, "Press any key to Continue...");
	  if (getch() == 27) break;  //Quit displaying
	 } //End of getkey

	x_rect_fill(0, 0, 320, 200, VisiblePageOffs, 0);  //Clear screen
       } //End of for loop

      x_text_mode();   //Clean things up
      _clearscreen(_GWINDOW);
     }
    else if (bitmap) {	//Save pcx if specified
      printf ("\nFor now, saving as a bitmap without displaying only saves the\nfirst image in CIF's.\n\n");
      save_pcx(picture, pal, pcx_file, c, 0);
     }

END:  //Yes, a goto statement...

    _ffree(picture);
    if (convert_pcx) _ffree(pcx_pic.buffer);
    free (pal);
    free (smalthin); }
  else { //No arguments
    display_help();
   }

  return (TRUE); }
//========== End of program =================================================
