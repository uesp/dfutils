/* DAGPICG.CPP - Simple DF Image Editor */

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
#include "keybrd.h"
#include "pcx.h"
#include "dagputil.h"
#include "gentime.h"

	/* Colors */
#define TEXT_COLOR 80
#define BG_COLOR 92

	/* Hmm... */
#define DAGPICG_VERSION "0.36g - January 1998"

	/* Sizes for editting area */
#define XI 5
#define YI 14
#define MAX_X 244
#define MAX_Y 188
#define BX1 295
#define BX2 255

#define PAL_XI 5
#define PAL_YI (MAX_Y + 5 )
#define MAX_PAL_X 244
#define MAX_PAL_Y 235
#define PAL_BOX_SIZE 5
#define PAL_GAP_SIZE 1

int BOX_SIZE = 1;
int GAP_SIZE = 0;
int START_X = 0;
int START_Y = 0;

	/* Global Variables */
BUTTON_ARRAY main_buttons;		/* Main Display Buttons */
BUTTON_ARRAY edit_buttons;
MENU_TYPE menu (0, 0, 60);
PCX_TYPE pcx;
DAGPIC_ARRAY pictures;			/* The Main Picture Data */
unsigned char *pal = NULL, *font = NULL;/* Palette and font data */
long initial_mem = farcoreleft();    	/* For debugging */
char dagpal_filename[81] = "dagpic.pal";   /* What palette is loaded in mem */
char pic_filename[81] = "temp.img";	/* What PBM is current loaded */
char pcx_filename[81] = "temp.pcx";	/* What PBM is current loaded */
int pre_color = 0, color = 0;		/* What Colors are being used */
int current_pic = 0;			/* Current Picture we are editing */
int file_menu, edit_menu, help_menu, view_menu;	/* Menu indexes */
boolean done = FALSE;			/* Main Loop Counter */
boolean redraw = TRUE, redraw_all = TRUE;
boolean picture_modified = FALSE;	/* Has picture been modified and not saved? */
	/* End of Globals */


void put_pix (const int x1, const int y1, const int data) {
  x_put_pix (x1, y1, VisiblePageOffs, data);
 }

/*========== Draws in the Specified Palette Colour Choice =================*/
void draw_palette_choice (const int c, const int color) {
  int x, y;

  x = (c % ((MAX_PAL_X - PAL_XI + 1)/(PAL_BOX_SIZE + PAL_GAP_SIZE))) * (PAL_BOX_SIZE + PAL_GAP_SIZE) + PAL_XI;
  y = (c / ((MAX_PAL_X - PAL_XI + 1)/(PAL_BOX_SIZE + PAL_GAP_SIZE))) * (PAL_BOX_SIZE + PAL_GAP_SIZE) + PAL_YI;

  x_hide_mouse();
  x_rect (x - 1, y - 1, x + PAL_BOX_SIZE, y + PAL_BOX_SIZE, VisiblePageOffs, color);
  x_rect (x - 1, y - 1, x + PAL_BOX_SIZE, y + PAL_BOX_SIZE, HiddenPageOffs, color);
  x_show_mouse();
 }
/*========== End of Procedure draw_palette_choice() =======================*/


/*========== Finds the Color Pressed By a Mouse Click =====================*/
int find_color_press (const int mx, const int my, const int current_color) {
  int c;

  if (mx > PAL_XI && mx < MAX_PAL_X && my > PAL_YI && my < MAX_PAL_Y) {
    c = (my - PAL_YI) / (PAL_BOX_SIZE + PAL_GAP_SIZE) * (MAX_PAL_X - PAL_XI + 1) / (PAL_BOX_SIZE + PAL_GAP_SIZE) + (mx - PAL_XI) / (PAL_BOX_SIZE + PAL_GAP_SIZE);

    if (c < 0)
      c = 0;
    else if (c > 255)
      c = 255;
   }
  else
    c = -1;

	/* No need to change if nothing happened */
  if (c == current_color) c = -1;
  return (c);
 }
/*========== End of Function find_color_press() ===========================*/


/*========== Gets the Current Color at Mouse Position =====================*/
int get_pixel (const int mx, const int my) {
  int i;

	/* Is the mouse click in the proper area? */
  if (mx > XI && mx < MAX_X && my > YI && my < MAX_Y) {
    x_hide_mouse();
    i = x_get_pix (mx, my, VisiblePageOffs);
    x_show_mouse();
    return (i);
   }

  return (-1);
 }
/*========== End of function get_pixel() ==================================*/


/*========== Modifies A Pixel at the Mouse Position, If Any ===============*/
boolean change_pixel (const int mx, const int my, const int color, DAG_PICTURE *pic) {
  int x, y;	/* Pixel Location of the Mouse Click */

	/* Is the mouse click in the proper area? */
  if (pic && pic->data && mx > XI && mx < MAX_X && my > YI && my < MAX_Y) {
    x = (mx - XI)/(BOX_SIZE + GAP_SIZE) + START_X;
    y = (my - YI)/(BOX_SIZE + GAP_SIZE) + START_Y;

	/* Is this pixel inside the picture? */
    if (x >= 0 && x < pic->width && y >= 0 && y < pic->height) {
      if (pic->data[x + y*pic->width] == color) return (FALSE);
      pic->data[x + y*pic->width] = color;

		/* Update the pixel on the screen */
      x = (x - START_X) * (BOX_SIZE + GAP_SIZE) + XI;
      y = (y - START_Y) * (BOX_SIZE + GAP_SIZE) + YI;
      x_hide_mouse();
      x_rect_fill_clipped (x, y, x + BOX_SIZE - 1, y + BOX_SIZE - 1, VisiblePageOffs, color);
      x_rect_fill_clipped (x, y, x + BOX_SIZE - 1, y + BOX_SIZE - 1, HiddenPageOffs, color);
      x_show_mouse();
      return (TRUE);
     }

   }

  return (FALSE);
 }
/*========== End of procedure change_pixel() ==============================*/


/*========== Draws the Picture on the Screen ==============================*/
void draw_picture (DAG_PICTURE *pic) {
  int x, y, i, j, x1, y1;	/* Loop counters */
  int w, h;

  x_hide_mouse();

		/* Clear the area */
  x_rect_fill (XI - 1, YI - 1, MAX_X + 1, MAX_Y + 1, HiddenPageOffs, 0);

  if (!pic || !pic->data) {
    x_page_flip (0, 0);
    x_show_mouse();
    return;
   }

  w = pic->width;
  h = pic->height;
  i = XI + (w - START_X) * (BOX_SIZE + GAP_SIZE) - GAP_SIZE;
  j = YI + (h - START_Y) * (BOX_SIZE + GAP_SIZE) - GAP_SIZE;
  x = i - 2;
  y = j - 2;

	/* Edge Boxes */
  x_rect_fill_clipped (x, y, i, j, HiddenPageOffs, TEXT_COLOR);
  x_rect_fill_clipped (XI - START_X - 1, y, XI - START_X + 1, j, HiddenPageOffs, TEXT_COLOR);
  x_rect_fill_clipped (XI - START_X - 1, YI - START_Y - 1, XI - START_X + 1, YI - START_Y + 1, HiddenPageOffs, TEXT_COLOR);
  x_rect_fill_clipped (x, YI - START_Y - 1, i, YI - START_Y + 1, HiddenPageOffs, TEXT_COLOR);

  for (y = YI, j = START_Y*w, y1 = START_Y; y1 < h && y < MAX_Y; y += GAP_SIZE + BOX_SIZE, j += w, y1++) {
    for (x = XI, x1 = START_X, i = j + START_X;  x1 < w && x < MAX_X; x += BOX_SIZE + GAP_SIZE, i++, x1++) {
      x_rect_fill_clipped (x, y, x + BOX_SIZE - 1, y + BOX_SIZE - 1, HiddenPageOffs, pic->data[i]);
     }
   }

  x_page_flip (0, 0);
  x_show_mouse();
 }
/*========== End of Procedure draw_picture() ==============================*/


/*========== Input New Picture Width and Height ===========================*/
boolean get_picture_size (DAG_PICTURE *picture) {
  char buffer[20] = "";	/* Temp input Buffer */

  if (!picture) return (FALSE);
  sprintf (buffer, "%d", picture->width);

  if (do_input_window (TEXT_COLOR, 5, "Image Width", "Enter Width for New Image", buffer)) {
    picture->width = atoi(buffer);

    if (picture->width > 320)
      picture->width = 320;
    else if (picture->width < 1)
      picture->width = 1;

    sprintf (buffer, "%d", picture->height);

    if (do_input_window (TEXT_COLOR, 5, "Image Height", "Enter Height for New Image", buffer)) {
      picture->height = atoi(buffer);

      if (picture->height > 200)
        picture->height = 200;
      else if (picture->height < 1)
	picture->height = 1;

       return (TRUE);
     }
   }

  return (FALSE);
 }
/*========== End of Procedure get_picture_size() ==========================*/


/*========== Change the Size of the Current Image =========================*/
boolean change_picture_size (DAG_PICTURE *picture) {
  int w, h, x, y, i, j;
  unsigned char *temp_ptr;

  if (!picture) return (FALSE);
  w = picture->width;
  h = picture->height;

	/* Get the new size */
  if (!get_picture_size (picture)) return (FALSE);

	/* Has Image Size Changed? */
  if (w == picture->width && h == picture->height) return (FALSE);

	/* Create the new data buffer for the image */
  temp_ptr = (unsigned char *) create_ptr (picture->height * picture->width + 3);
  memset (temp_ptr, 0, picture->height * picture->width + 3);

	/* Copy the values over */
  if (picture->data) {
    for (y = 0, i = 0, j = 0; y < h && y < picture->height; y++, i = 2 + y*picture->width, j = 2 + y*w) {
      for (x = 0; x < w && x < picture->width; x++, i++, j++) {
	temp_ptr[i] = picture->data[j];
       }
     }

    delete picture->data;
   }

  picture->data = temp_ptr;
  draw_picture (picture);
  return (TRUE);
 }
/*========== End of Function change_picture_size() ========================*/


/*========== Gets Picture Data from the Screen ============================*/
void get_picture (DAG_PICTURE *pic) {
  int x, y, y1, x1, i, j;	/* Loop counters */

  if (!pic || !pic->data) return;
  x_hide_mouse();

  for (y = YI, j = 0, y1 = 0; y1 < pic->height && y < MAX_Y; y++, j += pic->width, y1++) {
    for (x = XI, x1 = 0, i = j;  x1 < pic->width && x < MAX_X; x++, i++, x1++) {
      pic->data[i] = x_get_pix (x, y, HiddenPageOffs);
     }
   }

  x_show_mouse();
 }
/*========== End of Procedure get_picture() ===============================*/


/*========== Does a Flood Fill ============================================*/
void do_flood_fill (int mx, int my, const int color, DAG_PICTURE *pic) {
  int y, x, y1, x1, i, j;	/* Loop counters */

	/* Ignore any NULL pointers */
  if (!pic || !pic->data) return;

	/* Convert Mouse Position to Real position */
  mx = (mx - XI)/(BOX_SIZE + GAP_SIZE) + START_X + XI;
  my = (my - YI)/(BOX_SIZE + GAP_SIZE) + START_Y + YI;

	/* IS the mouse click out of bounds? */
  if (mx < XI || mx >= pic->width + XI || mx >= MAX_X || my < YI || my >= MAX_Y || my >= pic->height + YI)
    return;

	/* Draw the normal image on the hidden page */
  x_hide_mouse();
  x_rect_fill_clipped (XI - 1, YI - 1, MAX_X + 1, MAX_Y + 1, HiddenPageOffs, TEXT_COLOR);

  for (y = YI, j = 0, y1 = 0; y1 < pic->height && y < MAX_Y; y++, j += pic->width, y1++) {
    for (x = XI, x1 = 0, i = j;  x1 < pic->width && x < MAX_X; x++, i++, x1++) {
      x_put_pix (x, y, HiddenPageOffs, pic->data[i]);
     }
   }

	/* Do the fill */
  x_flood_fill (mx, my, HiddenPageOffs, color);
  x_show_mouse();

	/* Get the data */
  get_picture (pic);

	/* Update the display */
  draw_picture (pic);
 }
/*========== End of procedure do_flood_fill() =============================*/


/*========== Creates a Random Pattern With the Given Color ================*/
void draw_random (const int color, DAG_PICTURE *pic) {
  unsigned int i;	/* Loop counters */
  unsigned int j, k;

	/* Ignore NULL pointers */
  if (!pic || !pic->data) return;

  j = pic->width * pic->height + 1;

	/* To big for random to handle */
  if (j > 32000) j = 32000;
  k = j >> 3;

  for (i = 0; i < k; i++)
    pic->data[random(j)] = color;
 }
/*========== End of draw_random() =========================================*/


/*========== Procedures Called by Buttons =================================*/
void button_exit (__CPPARGS) {
  if (get_choice ("Do you really wish to exit program", "Exit DagPicG"))
    done = TRUE;
 }

void button_mergepic (__CPPARGS) {

  if (do_file_list (pic_filename, "*.img;*.cif;texture.*", "Load", "Load Pic")) {

    if (pictures.load_pic(pic_filename, pal, current_pic)) {
      START_Y = START_X = 0;
      redraw_all = TRUE;
     }
    else
      do_error_message ("File Error", "Error Loading Pic File '%s'!\n%s", pic_filename, dag_error_msgs[dag_error_code]);
   }
 }

void button_loadpic (__CPPARGS) {
  pictures.destroy();
  current_pic = 0;
  button_mergepic();
 }

void button_savepic (__CPPARGS) {

  if (do_file_list (pic_filename, "*.img;*.cif", "Save", "Save All Pictures")) {

    if (!pictures.save_pic (pic_filename, current_pic))
      do_error_message ("File Error", "Error Saving Pic File '%s'!\n%s", pic_filename, dag_error_msgs[dag_error_code]);

   }
 }


void button_save1pic (__CPPARGS) {

  if (do_file_list (pic_filename, "*.img", "Save", "Save Current Picture")) {

    if (!pictures.pics[current_pic]->save_img(pic_filename))
      do_error_message ("File Error", "Error Saving Pic File '%s'!\n%s", pic_filename, dag_error_msgs[dag_error_code]);

   }
 }


void button_mergepcx (__CPPARGS) {
  char *temp_ptr;
  PIC_TYPE picture;

  if (do_file_list (pcx_filename, "*.pcx", "Import", "Import PCX")) {

    if (pcx.load_pcx (pcx_filename, NULL)) {
      //x_put_pal_raw (pal, 256, 0);

		/* Convert the PCX to LBM */
      pcx.convert_to_lbm (&picture);

		/* Copy PBM to Dag Picture */
      if (!pictures.pics[current_pic]) {
	if (!(pictures.pics[current_pic] = new DAG_PICTURE))
	  bug (MEM_ERR_MSG, "loadpcx() - *pics[] (%d)", sizeof(DAG_PICTURE));
       }

      if (pictures.pics[current_pic]->data) delete pictures.pics[current_pic]->data;
      pictures.pics[current_pic]->data = (unsigned char *) create_ptr (picture.width * picture.height + 1);
      memcpy (pictures.pics[current_pic]->data, picture.data + 2, picture.width * picture.height);
      pictures.pics[current_pic]->width = picture.width;
      pictures.pics[current_pic]->height = picture.height;
      pictures.pics[current_pic]->image_size = picture.width * picture.height;
      pictures.pics[current_pic]->x_offset = 0;
      pictures.pics[current_pic]->y_offset = 0;
      if (pictures.num_pics == 0) pictures.num_pics = 1;

		/* Create the filename */
      strcpy (pic_filename, pcx_filename);
      temp_ptr = strrchr (pic_filename, '.');
      if (temp_ptr) *temp_ptr = 0;
      strcat (pic_filename, ".img");

      redraw_all = TRUE;
      START_Y = START_X = 0;
     }
    else
      do_error_message ("PCX Error", "Could not load PCX File '%s'!", pcx_filename);

	/* Clear the PCX */
    pcx.data = NULL;
   }
 }

void button_loadpcx (__CPPARGS) {
  pictures.destroy();
  current_pic = 0;
  button_mergepcx();
 }



void button_savepcx (__CPPARGS) {
  int i;

  if (!pictures.pics[current_pic] || !pictures.pics[current_pic]->data) return;

  if (do_file_list (pcx_filename, "*.pcx", "Save", "Save PCX")) {

	/* Create the picture */
    pcx.data = pictures.pics[current_pic]->data - 2;
    pcx.header.width = pictures.pics[current_pic]->width;
    pcx.header.height = pictures.pics[current_pic]->height;

	/* Assign the Palette */
    for (i = 0; i < 256; i++) {
      pcx.palette[i].red = pal[i*3] << 2;
      pcx.palette[i].green = pal[i*3 + 1] << 2;
      pcx.palette[i].blue = pal[i*3 + 2] << 2;
     }

	/* Save the PCX */
    if (!pcx.save_pcx (pcx_filename))
      do_error_message ("File Error", "Error Saving PCX File '%s'!", pcx_filename);

	/* Reset the PCX */
    pcx.data = NULL;
    redraw = TRUE;
   }
 }

void button_loadpal (__CPPARGS) {

  if (do_file_list (dagpal_filename, "*.pal;*.col", "Load", "Load DF PAL")) {
    if (load_dagpal (dagpal_filename, pal)) {
      x_put_pal_raw (pal, 256, 0);
      redraw = TRUE;
     }
    else
      do_error_message ("File Error", "Error Loading DF PAL File '%s'!\n%s", dagpal_filename, dag_error_msgs[dag_error_code]);
   }
 }

void button_savepal (__CPPARGS) {
  if (do_file_list (dagpal_filename, "*.pal", "Save", "Save DF PAL")) {
    if (!save_dagpal (dagpal_filename, pal))
      do_error_message ("File Error", "Error Saving DF PAL File '%s'!\n%s", dagpal_filename, dag_error_msgs[dag_error_code]);
   }
 }

void button_clearpic (__CPPARGS) {
  pictures.destroy();
  current_pic = 0;
  draw_picture(pictures.pics[current_pic]);
  strcpy (pic_filename, "temp.img");
  redraw = TRUE;
 }

void button_delpic (__CPPARGS) {
  int i;

  if (pictures.pics[current_pic]) {
    delete pictures.pics[current_pic];

    for (i = current_pic; i < pictures.num_pics - 1; i++)
      pictures.pics[i] = pictures.pics[i + 1];

    pictures.pics[pictures.num_pics - 1] = NULL;
    if (current_pic == pictures.num_pics - 1); pictures.num_pics--;
    draw_picture(pictures.pics[current_pic]);
    redraw = TRUE;
   }
 }


void button_addpic (__CPPARGS) {
  DAG_PICTURE temp_pic;
  int i;	/* Loop Counter */

  if (pictures.num_pics >= MAX_IMAGES - 1 ) {
    do_error_message ("Too Many Pictures", "Warning: Can't Add Picture, Maximum %d Exceed!", MAX_IMAGES);
    return;
   }

  temp_pic.width = 24;
  temp_pic.height = 24;
  if (!get_picture_size (&temp_pic)) return;

	/* Shift pictures to insert one... */
  for (i = pictures.num_pics; i > current_pic; i--) {
    pictures.pics[i] = pictures.pics[i - 1];
   }

  pictures.pics[current_pic] = NULL;

	/* Attempt to allocate a new picture */
  if (!(pictures.pics[current_pic] = new DAG_PICTURE))
    bug (MEM_ERR_MSG, "newpic() - *pics[] (%d)", sizeof(DAG_PICTURE));

  pictures.pics[current_pic]->width = temp_pic.width;
  pictures.pics[current_pic]->height = temp_pic.height;
  START_Y = START_X = 0;

	/* Create the data buffer for the image */
  pictures.pics[current_pic]->data = (unsigned char *) create_ptr (pictures.pics[current_pic]->height * pictures.pics[current_pic]->width + 1);
  memset (pictures.pics[current_pic]->data, 0, pictures.pics[current_pic]->height * pictures.pics[current_pic]->width + 1);

  draw_picture(pictures.pics[current_pic]);
  redraw = TRUE;
  pictures.num_pics++;
 }

void change_size (__CPPARGS) {
  change_picture_size (pictures.pics[current_pic]);
  draw_picture (pictures.pics[current_pic]);
  redraw = TRUE;
 }


void zoom_100 (__CPPARGS) {
  BOX_SIZE = 1;
  draw_picture (pictures.pics[current_pic]);
 }
void zoom_200 (__CPPARGS) {
  BOX_SIZE = 2;
  draw_picture (pictures.pics[current_pic]);
 }
void zoom_300 (__CPPARGS) {
  BOX_SIZE = 3;
  draw_picture (pictures.pics[current_pic]);
 }
void zoom_400 (__CPPARGS) {
  BOX_SIZE = 4;
  draw_picture (pictures.pics[current_pic]);
 }
void zoom_500 (__CPPARGS) {
  BOX_SIZE = 5;
  draw_picture (pictures.pics[current_pic]);
 }

void center (__CPPARGS) {
  START_X = START_Y = 0;
  draw_picture (pictures.pics[current_pic]);
 }


void help_about (__CPPARGS) {
  boolean done = FALSE;

  ok_window.new_title ("About DagPicG");

	/* Resize the window */
  ok_window.new_size (200, TEXT_LINE_HEIGHT*12 + 25);

	/* Reset the button positions */
  ok_window.move_buttons();

	/* Draw the window and print string */
  ok_window.draw();
  ok_window.printf_cent ("DagPicG Version %s", DAGPICG_VERSION);
  ok_window.printf_cent ("Written By Dave Humphrey");
  ok_window.printf_cent ("aj589@freenet.carleton.ca");
  ok_window.printf_cent ("");
  ok_window.printf ("Visit the Unofficial Elder Scrolls Pages at");
  ok_window.printf_cent ("www.honesty.com/jackel");
  ok_window.printf ("for all the Elder Scrolls tips/cheats/hints");
  ok_window.printf ("and files you could ever need...");

  do {
    switch (ok_window.buttons->check(x_get_mouse_event(LEFT_BUTTON), FALSE)) {
      case -1: break;
      case 0: done = TRUE; break;
     } /* End switch */

    if (get_esc_state() || get_enter_state())
      done = TRUE;

  } while (!done);

  ok_window.erase();
 }

void help_status (__CPPARGS) {
  memory_window (initial_mem);
 }

void help_text (__CPPARGS) {
  display_text_file ("dagpicg.txt", "DagPicG Help File");
 }

/*========== End of Procedure button_...... ===============================*/


/*========== Begin Main Program ===========================================*/
boolean main (void) {
  int event, i;				/* Records Mouse Events */
  int x, y;
  char orig_path[GENUTIL_STRING_LENGTH + 1];

	/* Open the program log file */
  open_log_file ("dagpicg.log");
  randomize();

	/* Get the current path */
  getcwd (orig_path, GENUTIL_STRING_LENGTH);

	/* Create the palette */
  pal = (unsigned char *) create_ptr (800);

	/* Attempt to load the palette, shade table, and font */
  load_dagpal (dagpal_filename, pal);
  create_shadetable (19, pal, &shade_table[0][0]);
  load_font ("smalthin.fnt", &font);

	/* Install the keyboard */
  install_keyboard();

	/* Replace the Timer Interrupt */
  set_timer_int (Timer);

	/* Init the graphics mode */
  x_set_mode (X_MODE_360x240, 360);
  x_put_pal_raw (pal, 256, 0);
  x_set_doublebuffer (240);
  x_set_cliprect (XI>>2, YI - 1, MAX_X>>2, MAX_Y);

	/* Init the ModeX text and Mouse */
  init_text (font);
  init_mouse();

	/* Init the default windows and values */
  init_windows();
  init_buttons();

	/* Load some pictures */
  misc_pics.load_pic_table ("dagpicg.dat");

	/* Set the mouse, if any */
  if (Mouse_Index != -1) init_mouse_cursor (misc_pics.pics[Mouse_Index]->data);

	/* Define the Menu */
  file_menu = menu.add_menu ("File");
  menu.add_menu_item (file_menu, "Load Picture", button_loadpic);
  menu.add_menu_item (file_menu, "Save All", button_savepic);
  menu.add_menu_item (file_menu, "Save Current", button_save1pic);
  menu.add_menu_item (file_menu, "-", NULL);
  menu.add_menu_item (file_menu, "Load Palette", button_loadpal);
  menu.add_menu_item (file_menu, "Save Palette", button_savepal);
  menu.add_menu_item (file_menu, "-", NULL);
  menu.add_menu_item (file_menu, "Load PCX", button_loadpcx);
  menu.add_menu_item (file_menu, "Save PCX", button_savepcx);
  menu.add_menu_item (file_menu, "-", NULL);
  menu.add_menu_item (file_menu, "Merge Picture", button_mergepic);
  menu.add_menu_item (file_menu, "Merge PCX",button_mergepcx);
  menu.add_menu_item (file_menu, "-", NULL);
  menu.add_menu_item (file_menu, "Exit", button_exit);

  edit_menu = menu.add_menu ("Edit");
  menu.add_menu_item (edit_menu, "Clear All", button_clearpic);
  menu.add_menu_item (edit_menu, "Add Picture", button_addpic);
  menu.add_menu_item (edit_menu, "Del Current", button_delpic);
  menu.add_menu_item (edit_menu, "-", NULL);
  menu.add_menu_item (edit_menu, "Change Size", change_size);

  view_menu = menu.add_menu ("View");
  menu.add_menu_item (view_menu, "Zoom 100%", zoom_100);
  menu.add_menu_item (view_menu, "Zoom 200%", zoom_200);
  menu.add_menu_item (view_menu, "Zoom 300%", zoom_300);
  menu.add_menu_item (view_menu, "Zoom 400%", zoom_400);
  menu.add_menu_item (view_menu, "Zoom 500%", zoom_500);
  menu.add_menu_item (view_menu, "-", NULL);
  menu.add_menu_item (view_menu, "Center", center);

  help_menu = menu.add_menu ("Help");
  menu.add_menu_item (help_menu, "DagPicG.Txt", help_text);
  menu.add_menu_item (help_menu, "About DagPicG", help_about);
  menu.add_menu_item (help_menu, "Program Status", help_status);

	/* Define some buttons */
  main_buttons.new_button (BX1,  5, 60, 12, "Clear All", NULL, NULL, FALSE, 2, TRUE, button_clearpic, BUTTON_BG_COLOR, BUTTON_TEXT_COLOR);
  main_buttons.new_button (BX1, 21, 60, 12, "Add Pic", NULL, NULL, FALSE, 2, TRUE, button_addpic, BUTTON_BG_COLOR, BUTTON_TEXT_COLOR);
  main_buttons.new_button (BX1, 37, 60, 12, "Del Pic", NULL, NULL, FALSE, 2, TRUE, button_delpic, BUTTON_BG_COLOR, BUTTON_TEXT_COLOR);
  main_buttons.new_button (BX1, 58, 60, 12, "Load", NULL, NULL, FALSE, 2, TRUE, button_loadpic, BUTTON_BG_COLOR, BUTTON_TEXT_COLOR);
  main_buttons.new_button (BX1, 74, 60, 12, "Save All", NULL, NULL, FALSE, 2, TRUE, button_savepic, BUTTON_BG_COLOR, BUTTON_TEXT_COLOR);
  main_buttons.new_button (BX1, 90, 60, 12, "Load PCX", NULL, NULL, FALSE, 2, TRUE, button_loadpcx, BUTTON_BG_COLOR, BUTTON_TEXT_COLOR);
  main_buttons.new_button (BX1,106, 60, 12, "Save PCX", NULL, NULL, FALSE, 2, TRUE, button_savepcx, BUTTON_BG_COLOR, BUTTON_TEXT_COLOR);

  main_buttons.new_button (BX1,127, 60, 12, "Exit", NULL, NULL, FALSE, 2, TRUE, button_exit, BUTTON_BG_COLOR, BUTTON_TEXT_COLOR);

  edit_buttons.new_button (BX2,  5,30,10,"100%", NULL, NULL, FALSE, 1, TRUE, zoom_100, BUTTON_BG_COLOR, BUTTON_TEXT_COLOR);
  edit_buttons.new_button (BX2, 19,30,10,"200%", NULL, NULL, FALSE, 1, TRUE, zoom_200, BUTTON_BG_COLOR, BUTTON_TEXT_COLOR);
  edit_buttons.new_button (BX2, 33,30,10,"300%", NULL, NULL, FALSE, 1, TRUE, zoom_300, BUTTON_BG_COLOR, BUTTON_TEXT_COLOR);
  edit_buttons.new_button (BX2, 47,30,10,"400%", NULL, NULL, FALSE, 1, TRUE, zoom_400, BUTTON_BG_COLOR, BUTTON_TEXT_COLOR);
  edit_buttons.new_button (BX2, 61,30,10,"500%", NULL, NULL, FALSE, 1, TRUE, zoom_500, BUTTON_BG_COLOR, BUTTON_TEXT_COLOR);
  edit_buttons.new_button (BX2, 75,30,10,"Cent", NULL, NULL, FALSE, 1, TRUE, center,   BUTTON_BG_COLOR, BUTTON_TEXT_COLOR);
  edit_buttons.new_button (BX2, 89,30,10,"Fill", NULL, NULL, FALSE, 1, TRUE, NULL,     BUTTON_BG_COLOR, BUTTON_TEXT_COLOR);
  edit_buttons.new_button (BX2,103,30,10,"Size", NULL, NULL, FALSE, 1, TRUE, change_size, BUTTON_BG_COLOR, BUTTON_TEXT_COLOR);
  edit_buttons.new_button (BX2,117,30,10,"Rand", NULL, NULL, FALSE, 1, TRUE, NULL,     BUTTON_BG_COLOR, BUTTON_TEXT_COLOR);
  edit_buttons.new_button (BX2,131,30,11,"NoGap", "Gap", NULL, FALSE, 1, TRUE, NULL,   BUTTON_BG_COLOR, BUTTON_TEXT_COLOR);
  edit_buttons.new_button (BX2,150,44,11,"Next Pic", NULL, NULL, FALSE, 1, TRUE, NULL, BUTTON_BG_COLOR, BUTTON_TEXT_COLOR);
  edit_buttons.new_button (BX2+50,150,44,11,"Prev Pic", NULL, NULL, FALSE, 1, TRUE, NULL, BUTTON_BG_COLOR, BUTTON_TEXT_COLOR);

  do_tasks = TRUE;

	/* Begin Main Loop */
  do {
    if (redraw_all) {
      x_hide_mouse();
      x_rect_fill (0, 0, 360, 240, VisiblePageOffs, BG_COLOR);
      x_rect_fill (XI - 1, YI - 1, MAX_X + 1, MAX_Y + 1, VisiblePageOffs, 0);
      x_rect_fill (PAL_XI - 1, PAL_YI - 1, MAX_PAL_X + 1, MAX_PAL_Y + 1, VisiblePageOffs, 0);
      x_rect_fill (0, 0, 360, 240, HiddenPageOffs, BG_COLOR);
      x_rect_fill (XI - 1, YI - 1, MAX_X + 1, MAX_Y + 1, HiddenPageOffs, 0);
      x_rect_fill (PAL_XI - 1, PAL_YI - 1, MAX_PAL_X + 1, MAX_PAL_Y + 1, HiddenPageOffs, 0);

		/* Print the colours */
      for (y = PAL_YI, i = 0; y < MAX_PAL_Y && i < 256; y += PAL_BOX_SIZE + PAL_GAP_SIZE) {
	for (x = PAL_XI; x < MAX_PAL_X && i < 256; x += PAL_BOX_SIZE + PAL_GAP_SIZE) {
	  x_rect_fill (x, y, x + PAL_BOX_SIZE - 1, y + PAL_BOX_SIZE - 1, VisiblePageOffs, i);
	  x_rect_fill (x, y, x + PAL_BOX_SIZE - 1, y + PAL_BOX_SIZE - 1, HiddenPageOffs, i);
	  i++;
	 }
       }

      x_show_mouse();
      redraw_all = FALSE;
      redraw = TRUE;
      draw_palette_choice (color, TEXT_COLOR);

      main_buttons.draw(TRUE);
      edit_buttons.draw(TRUE);
      draw_picture (pictures.pics[current_pic]);
      menu.draw_heads (TRUE);
     }

    if (redraw) {
      redraw = FALSE;

		/* Update the Palette */
      draw_palette_choice (pre_color, 0);
      draw_palette_choice (color, TEXT_COLOR);
      pre_color = color;

      x_hide_mouse();
      x_rect_fill (MAX_PAL_X + 3, 190, 360, 240, VisiblePageOffs, BG_COLOR);
      x_rect_fill (MAX_PAL_X + 3, 190, 360, 240, HiddenPageOffs, BG_COLOR);

      x_printf_shadowa (MAX_PAL_X + 6,190, TEXT_COLOR, "%-10s ", pic_filename);
      x_printf_shadowa (MAX_PAL_X + 6,200, TEXT_COLOR, "%-10s ", dagpal_filename);
      x_printf_shadowa (MAX_PAL_X + 6,210, TEXT_COLOR, "Pic %2d of %2d ", current_pic + 1, pictures.num_pics);
      x_printf_shadowa (MAX_PAL_X + 6,220, TEXT_COLOR, "Color=%-3d ", color);
      x_printf_shadowa (MAX_PAL_X + 6,230, TEXT_COLOR, "%d x %d ", pictures.pics[current_pic]->width, pictures.pics[current_pic]->height);
      x_show_mouse();
     }

	/* Check Mouse Input */
    event = x_get_mouse_event (LEFT_BUTTON);

    if (main_buttons.check (event) != NO_BUTTON) {
	/* Already taken care of */
     }
    else if ((i = edit_buttons.check(event)) != NO_BUTTON) {
      switch (i) {
	case 6:		/* Fill */
	  if (!pictures.pics[current_pic] || !pictures.pics[current_pic]->data) break;
	  x_printf_shadow (MAX_X + 3, MAX_Y - 10, VisiblePageOffs, TEXT_COLOR, "Click on Pixel to Start Fill");

	  do {
	    event = x_get_mouse_event(LEFT_BUTTON);
	  } while (event != BUTTON_RELEASE && !get_esc_state());

	  x_rect_fill (MAX_X + 3, MAX_Y - 10, 360, MAX_Y - 2, VisiblePageOffs, BG_COLOR);
	  if (event == BUTTON_RELEASE) do_flood_fill (ButtonX, ButtonY, color, pictures.pics[current_pic]);
	  redraw = TRUE;
	  break;
	case 8:	/* Random */
	  draw_random (color, pictures.pics[current_pic]);
	  draw_picture (pictures.pics[current_pic]);
	  break;
	case 9:	/* Gap */
	  GAP_SIZE = !GAP_SIZE;
	  draw_picture (pictures.pics[current_pic]);
	  break;
	case 10:/* Next */
	  if (current_pic < pictures.num_pics - 1) {
	    current_pic++;
	    draw_picture (pictures.pics[current_pic]);
	    redraw = TRUE;
	   }
	  break;
	case 11:/* Prev */
	  if (current_pic > 0) {
	    current_pic--;
	    draw_picture (pictures.pics[current_pic]);
	    redraw = TRUE;
	   }
	  break;
       }
     }
		/* Button click on the color palette? */
    else if (MouseButtonStatus == 1 && (i = find_color_press (MouseX, MouseY, color)) != -1) {
      color = i;
      redraw = TRUE;
     } /* End of palette button clicks */
		/* Left Button Hold Down to draw */
    else if (MouseButtonStatus == 1 && change_pixel(MouseX, MouseY, color, pictures.pics[current_pic]) ) {
     }
    else if (menu.do_menu(event, TRUE)) {
     }

	/* Check for Right Mouse Clicks */
    event = x_get_mouse_event (RIGHT_BUTTON);

	/* Get current color from mouse click */
    if (event == BUTTON_PRESS && (i = get_pixel(ButtonX, ButtonY)) != -1) {
      color = i;
      redraw = TRUE;
     }

    GetKey();

	/* Check keyboard input */
    if (get_esc_state()) {
      if (get_choice ("Do you really wish to exit program", "Exit DagPicG")) done = TRUE;
     }
    else if (get_down_state() && pictures.pics[current_pic]) {
      START_Y -= pictures.pics[current_pic]->height/10 + 1;
      if (START_Y < 0) START_Y = 0;
      draw_picture (pictures.pics[current_pic]);
      delay (KEY_DELAY);
     }
    else if (get_up_state() && pictures.pics[current_pic]) {
      START_Y += pictures.pics[current_pic]->height/10 + 1;;
      if (START_Y > pictures.pics[current_pic]->height - 2) START_Y = pictures.pics[current_pic]->height - 2;
      draw_picture (pictures.pics[current_pic]);
      delay (KEY_DELAY);
     }
    else if (get_left_state() && pictures.pics[current_pic]) {
      START_X += pictures.pics[current_pic]->width/10 + 1;
      if (START_X > pictures.pics[current_pic]->width - 2) START_X = pictures.pics[current_pic]->width - 2;
      draw_picture (pictures.pics[current_pic]);
      delay (KEY_DELAY);
     }
    else if (get_right_state() && pictures.pics[current_pic]) {
      START_X -= pictures.pics[current_pic]->width/10 + 1;
      if (START_X < 0) START_X = 0;
      draw_picture (pictures.pics[current_pic]);
      delay (KEY_DELAY);
     }
    else if (get_home_state()) {
      START_X = START_Y = 0;
      draw_picture (pictures.pics[current_pic]);
      delay (KEY_DELAY);
     }
    else if (get_end_state()) {
      START_X = pictures.pics[current_pic]->width - 2;
      START_Y = pictures.pics[current_pic]->height - 2;
      draw_picture (pictures.pics[current_pic]);
      delay (KEY_DELAY);
     }
    else if (get_pgup_state() && current_pic < pictures.num_pics - 1) {
      current_pic++;
      draw_picture (pictures.pics[current_pic]);
      redraw = TRUE;
      delay (KEY_DELAY);
     }
    else if (get_pgdn_state() && current_pic > 0) {
      current_pic--;
      draw_picture (pictures.pics[current_pic]);
      redraw = TRUE;
      delay (KEY_DELAY);
     }
    else if (get_alt_key (SCAN_D)) {	/* ALT+D */
      help_status();
     }

  } while (!done);	/* End Main Loop */

	/* Exit graphics mode */
  x_mouse_remove();
  x_text_mode();

	/* Delete the palette and other things */
  pictures.destroy();
  if (pal) delete pal;
  if (font) delete font;

	/* Return to orignal path */
  chdir (orig_path);

  printf ("\nInitial/Final Memory = %ld / %ld bytes\n", initial_mem, farcoreleft());
  printf ("FarHeapCheck() returns = %d\n\n", farheapcheck());


  return (TRUE);
 }
/*========== End Main Program =============================================*/
