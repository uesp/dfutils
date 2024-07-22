	/* Standard 'C' Includes */
#include <dos.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <alloc.h>
#include <ctype.h>

	/* Some User Defined Includes */
#include "genutil.h"
#include "fileutil.h"
#include "keyasm.h"
#include "keybrd.h"
#include "mouse.h"
#include "buttons.h"
#include "util.h"

	/* Program Specific Includes */
#include "edit.h"
#include "scrlbar.h"
#include "listbox.h"
#include "filelist.h"
#include "dfqedit.h"



MENU_TYPE file_menu =
  { 3, 1, 17, 6, "File",
   { "New", "Load     F3", "Save     F2", "-", "Quit   ALT-Q", "#NULL", "#NULL" },
   { editmenu_new,  editmenu_load, editmenu_save, NULL, editmenu_quit, NULL, NULL }
  };


EDIT_TYPE far *edit_line, far *edit_start, far *edit_end;


//Coordinates of load/save windows...
int load_x = 5;
int load_x1 = 75;
int load_y = 10;
int load_y1 = 14;

//Coordinates of note/idea windows....
int note_x = 10;
int note_x1 = 70;
int note_y = 9;
int note_y1 = 15;

int current_line = 0;    //Where are we in the screen?
int display_start = 0;   //Start printing at what line?
int current_char = 0;    //What cursor position are we?
int max_lines = 23;      //Display how many lines at once...
int lines_y = 2;         //Start printing at what line?
int max_length = 78;     //How long to display strings...
int delay_length = 100;  //Delay how long after reading an important char
int chosen_menu = 0;     //What menu item are we looking at
int send_speed = 200;    //Delay in milliseconds between lines...

char header[81] = "F1 = Help";
char save_temp[500];    //Used to save parts of screen behind menus etc...
char *loaded_filename;   //Contains name of current file...
char text_path [MAX_STRING_LENGTH+1] = "";	//Default Path to where text files are...

boolean quit = FALSE;    //Main edit loop
boolean loaded_file = FALSE; //Do we have a file loaded at present?


//========== Combines two adjacent lines as best as possible ================
void combine_lines (EDIT_TYPE far *line) {

  if ((strlen(line->data) + strlen(line->next->data)) <= 78) { //Combine em
    current_char = strlen(line->data);
    strcat (line->data, line->next->data);
    delete_edit(line->next);
    edit_line = line;
   }
  else {
    char *buffer;

    if (!(buffer = new char[max_length + 1]))
      bug (MEM_ERR_MSG, "combine_lines() - *buffer (%d)", max_length + 1);

    current_char = strlen(line->data);
    edit_line = line;

    do {
      sscanf(line->next->data, "%s", &buffer[0]);  //Get the 1st word

      if (strlen(buffer) + strlen(line->data) < max_length) {
	strcat (line->data, buffer);
	strcat (line->data, " ");
	strcpy (buffer, &line->next->data[strlen(buffer) + 1]);
	strcpy (&line->next->data[0], buffer);
       }
      else
	break;

    } while (strlen(line->next->data) > 0);

    delete buffer;
   }
 }
//========== End of procedure combine_lines() ===============================


//========== Creates a new line at the end dynamically ======================
void create_new_end (char *string) {

  if (edit_end != NULL) { //List already exists
    if (!(edit_end->next = new far EDIT_TYPE))
      bug (MEM_ERR_MSG, "create_new_end() *edit_end->next (%d)", sizeof (EDIT_TYPE));
    edit_line = edit_end;
    edit_end = edit_end->next;
    edit_end->prev = edit_line;
    edit_end->next = NULL;
   }
  else { //Must create new list
    init_edit();
   }

  strcpy (edit_end->data, string); }
//========== End of procedure create_new_end() ==============================


//========== Creates a new line in the middle dynamically ===================
void create_new_line (char *string) {
  EDIT_TYPE far *temp_line;

  if (!(temp_line = new far EDIT_TYPE))
    bug (MEM_ERR_MSG, "create_new_line() - *temp_line (%d)", sizeof (EDIT_TYPE));

  temp_line->prev = edit_line;
  temp_line->next = edit_line->next;
  (edit_line->next)->prev = temp_line;
  edit_line->next = temp_line;
  edit_line = temp_line;
  strcpy (edit_line->data, string); }
//========== End of procedure create_new_end() ==============================


//========== Frees all links in the node ====================================
void delete_all_editlines (void) {

  while (edit_end) {
    delete_edit (edit_end);
   }
}
//========== End of procedure delete_all_editlines() ========================


//========== Frees a link in the node =======================================
void delete_edit (EDIT_TYPE far *line) {

  if (line->next)
    (line->next)->prev = line->prev;
  else
    if (!line->prev)
      edit_end = edit_start = NULL;
    else
      edit_end = edit_end->prev;

  if (line->prev)
    (line->prev)->next = line->next;
  else
    if (!line->next)
      edit_end = edit_start = NULL;
    else
      edit_start = edit_start->next;

  delete line; }
//========== End of procedure delete_edit() =================================


//========== Main menu routine, scrolling etc... ============================
void do_editmenu (MENU_TYPE *menu) {
  boolean done = FALSE, redraw = TRUE;
  int prev_chosen = 0, i;

  chosen_menu = 0;
  hide_mouse();
  _setcursortype (_NOCURSOR);
  gettext (menu->x + 1, menu->y + lines_y - 1, menu->x + menu->width + 1, menu->y + menu->height + lines_y - 1, &save_temp[0]);
  show_mouse();
  draw_editmenu (menu);

  do {
    if (redraw) {
      redraw = FALSE;
      textcolor (BLACK);
      textbackground (LIGHTGRAY);
      draw_editmenu_item (menu, prev_chosen);
      textcolor (LIGHTGRAY);
      textbackground (BLACK);
      draw_editmenu_item (menu, chosen_menu);
      prev_chosen = chosen_menu;
     }

    if (get_mouse_event(LEFT_BUTTON) == BUTTON_PRESS) {
      if ((i = find_mouse_menu(MOUSEX, MOUSEY, menu)) != -1) {
	chosen_menu = i;
	hide_mouse();
	puttext (menu->x + 1, menu->y + lines_y - 1, menu->x + menu->width + 1, menu->y + menu->height + lines_y - 1, &save_temp[0]);
	show_mouse();
	menu->do_menu[chosen_menu] (NULL);
	done = TRUE;
	redraw = TRUE;

       } //End of switch
     }

    GetKey();

    if (get_up_state()) { /* Move up */
      chosen_menu--;
      if (chosen_menu < 0) chosen_menu = menu->height - 2;

      while (*menu->name[chosen_menu] == '-')
	chosen_menu--;

      redraw = TRUE;
      delay(delay_length*2);
     }
    else if (get_down_state()) { /* Move down */
      chosen_menu++;
      if (chosen_menu > menu->height - 2) chosen_menu = 0;

      while (*menu->name[chosen_menu] == '-')
	chosen_menu++;

      redraw = TRUE;
      delay(delay_length*2);
     }
    else if (get_enter_state()) { /* Do a menu item */
      hide_mouse();
      puttext (menu->x + 1, menu->y + lines_y - 1, menu->x + menu->width + 1, menu->y + menu->height + lines_y - 1, &save_temp[0]);
      show_mouse();
      done = TRUE;
      menu->do_menu[chosen_menu] (NULL);
     }
    else if (get_esc_state()) {  /* Get out of menu */
      hide_mouse();
      puttext (menu->x + 1, menu->y + lines_y - 1, menu->x + menu->width + 1, menu->y + menu->height + lines_y - 1, &save_temp[0]);
      show_mouse();
      done = TRUE;
     }
    else if (isprintable(keylast)) {
      i = 0;

      while (menu->name[i][0] != '#') {	//Look at all menu entries
	while (menu->name[i][0] == '-')	//Skips bars
	  i++;

	if (toupper(keylast) == toupper(menu->name[i][0])) {
	  hide_mouse();
	  puttext (menu->x + 1, menu->y + lines_y - 1, menu->x + menu->width + 1, menu->y + menu->height + lines_y - 1, &save_temp[0]);
	  show_mouse();
	  menu->do_menu[i] (NULL);
	  done = TRUE;
	  break;
	 }

	i++;
       }
     }

   } while (!done);

 }
//========== End of procedure do_editmenu() =================================


//========== Redraws the entire screen ======================================
void draw_all_lines (EDIT_TYPE far *line, int type) {
  int count = 0;

  hide_mouse();
  _setcursortype (_NOCURSOR);

  if (type == END) { //Move to start of screen display..
    while (count < 22 && line->prev != NULL) {
      count++;
      line = line->prev;
    };  //End of while loop

    count = 0;
   }
  else if (type == START)
    count = 0;
  else if (type == MIDDLE)
    count = current_line;

  do {
    gotoxy (1, count + lines_y - 1);
    parse_edit_line (line->data);
    if (strlen(line->data) < 78) clreol();
    line = line->next;
    count++;
  } while (count < 23 && line != NULL);

  while (count < 23) {  //Clear rest of lines
    gotoxy (1, count + lines_y - 1);
    clreol ();
    count++;
   } //End of while loop

  gotoxy (current_char + 1, current_line + 1);
  _setcursortype (_NORMALCURSOR);
  show_mouse();
 }
//========== End of procedure draw_all_lines() ==============================


//========== Draws one edit line ============================================
void draw_edit_line (EDIT_TYPE far *line, int y) {

  if (current_line > 22) {
    current_line = 22;
    draw_all_lines (line, END);
   }
  else if (current_line < 0) {
    current_line = 0;
    draw_all_lines (line, START);
   }
  else {
    hide_mouse ();
    _setcursortype (_NOCURSOR);
    gotoxy (1, y + lines_y - 1);
    parse_edit_line (line->data);
    if (strlen (line->data) < 78) clreol();
    gotoxy (current_char + 1, current_line + 1);
    _setcursortype (_NORMALCURSOR);
    show_mouse ();
   }
 }
//========== End of procedure draw_edit_line() ==============================


//========== Draws a menu ===================================================
void draw_editmenu (MENU_TYPE *menu) {
  int count = 0;

  hide_mouse();
  _setcursortype (_NOCURSOR);
  gotoxy (menu->x, menu->y);
  textcolor (BLACK);
  textbackground (LIGHTGRAY);
  draw_textbox (menu->x, menu->y, menu->x + menu->width, menu->y + menu->height, NULL, BLACK);

  while (*menu->name[count] != '#') {
    draw_editmenu_item (menu, count);
    count++;
   }

  textcolor (LIGHTGRAY);
  textbackground (BLACK);
  show_mouse();
 }
//========== End of procedure draw_editmenu() ===============================


//========== Draws one line of a menu =======================================
void draw_editmenu_item (MENU_TYPE *menu, int item) {
  int orig_text_color;

  if (*menu->name[item] == '-') { //Print a bar...
    int count1;
    gotoxy (menu->x, menu->y + 1 + item);
    cprintf ("%c", T_RIGHT);

    for (count1 = menu->x + 1; count1 < menu->x + menu->width; count1++)
      cprintf ("%c", SING_HORIZONTAL);

    cprintf ("%c", T_LEFT);
   }
  else { //Print menu name
    orig_text_color = get_text_color();
    gotoxy (menu->x + 1, menu->y + 1 + item);
    textcolor (RED);
    cprintf (" %c", *menu->name[item]);
    textcolor (orig_text_color);
    cprintf ("%-*s", menu->width - 3, menu->name[item] + 1);
   }
 }
//========== End of procedure draw_editmenu_item() ==========================


//========== Draws the border etc... of the edit screen =====================
void draw_edit_screen (const char *title) {
  int count;

  hide_mouse();
  clrscr();
  window(1, 1, 80, 25);
		 //Print header at top of screen
  textcolor(BLACK);
  textbackground(LIGHTGRAY);
  cprintf ("%79s ", header);

  gotoxy (file_menu.x + 1, 1);
  textcolor(RED);
  cprintf ("%c", file_menu.title[0]);
  textcolor(BLACK);
  cprintf ("%s", &file_menu.title[1]);

  gotoxy (14, 1);
  textcolor (BLUE);
  cprintf ("DFQEdit");
  textcolor (BLACK);
  cprintf (" - ");
  textcolor (DARKGRAY);
  cprintf ("QRC Text");
  textcolor (BLACK);
  cprintf (" - %s", title);

  textcolor(LIGHTGRAY);
  textbackground(BLACK);

		//Print right and left sides
  for (count = 2; count < 25; count++) {
    gotoxy (1, count);
    cprintf ("%c", VERTICAL);
    gotoxy (80, count);
    cprintf ("%c", VERTICAL);
   }

	       //Print the two bottom corners
  gotoxy (80, 25);
  cprintf ("%c", BOTRIGHT_CORNER);
  gotoxy (1, 25);
  cprintf ("%c", BOTLEFT_CORNER);

	      //Print the bottom line...
  for (count = 1; count < 79; count++)
    cprintf ("%c", HORIZONTAL);

  window (2, lines_y, 1 + max_length, max_lines + 1);
  show_mouse();
 }
//========== End of procedure draw_edit_screen() ============================


//========== Adds a character to edit->data =================================
void edit_addchar (EDIT_TYPE far *line, char ch) {
  char buffer[MAX_STRING_LENGTH], *temp_ptr;	//Temp string buffers
  EDIT_TYPE far *temp_line;	//Temp pointer to save value
  int orig_char, orig_line;	//Save position

		 //Add to end, short string
  if (strlen(line->data) <= current_char && strlen(line->data) < max_length - 1) {
    current_char++;
    line->data[strlen(line->data) + 1] = '\0';
    line->data[strlen(line->data)] = ch;
    draw_edit_line (edit_line, current_line);
   } //End of add to end, short
  else if (strlen(line->data) < max_length - 1) { //Add in middle, short
    strcpy (buffer, &line->data[current_char]);
    line->data[current_char] = ch;
    strcpy (&line->data[current_char + 1], buffer);
    current_char++;
    draw_edit_line (edit_line, current_line);
   }
  else { //Add a character to middle of long line?
		//Find the first space from end of line to break...
    if ((temp_ptr = strrchr (line->data, ' ')) == NULL)
      temp_ptr = &line->data[strlen(line->data) - 2];	//No spaces in line? take only last character

    strcpy (buffer, temp_ptr + 1);	//Don't keep spaces
    *temp_ptr = 0; //Terminate string at proper position
    edit_addchar (line, ch);	//Add character to shortened line
    temp_line = line;

    if (line->next == NULL) {
      create_new_end (buffer);
     }
    else { //Try to combine lines if possible
      chrcat (&buffer[0], ' ');	//Insert a padding space
      create_new_line (buffer);

      orig_char = current_char;	//Save values to reset later
      orig_line = current_line;

      combine_lines (edit_line);
      current_char = orig_char;	//Reset variables
      current_line = orig_line;
     }

    edit_line = temp_line;
    draw_all_lines (edit_line, MIDDLE);
   }

 }
//========== End of procedure edit_addchar() ================================


//========== Deletes a character from edit->data ============================
void edit_delchar (EDIT_TYPE far *line, int type) {
  char buffer[80];

  if (type == DELETE) {
		  //Delete char
    if (strlen(line->data) > current_char) {
      strcpy (buffer, &line->data[current_char + 1]);  //Save end string
      strcpy (&line->data[current_char], buffer);
      line->data[strlen(line->data) + 1] = '\0';
      draw_edit_line (edit_line, current_line);
     }
    else if (strlen(line->data) == current_char && line->next != NULL) {
      combine_lines (line);
      draw_all_lines (edit_line, MIDDLE);
     }
   } //End of delete
  else { //backspace

    if (strlen(line->data) > current_char - 1 && current_char > 0) {
      strcpy (buffer, &line->data[current_char]);  //Save end string
      strcpy (&line->data[current_char - 1], buffer);
      current_char--;
      draw_edit_line (edit_line, current_line);
     }
    else if (strlen(line->data) <= current_char - 1 && current_char > 0) {
      line->data[strlen(line->data) - 1] = '\0';  //Just truncate
      current_char--;
      draw_edit_line (edit_line, current_line);
     }
    else if (current_char == 0 && edit_line->prev != NULL) {
      combine_lines (edit_line->prev);
      current_line--;
      draw_all_lines (edit_line, MIDDLE);
     }
   } //End of backspace

 }
/*========== End of procedure edit_delchar() ==============================*/


/*========== Is ch an alphanumeric character ==============================*/
boolean edit_isalpha(char ch) {
  if (ch >= ' ' && ch <= '~') return(TRUE);
  return (FALSE); }
/*========== End of function edit_isalpha() ===============================*/


/*========== The Help window ==============================================*/
void editmenu_help(char *argument) {
  _setcursortype (_NOCURSOR);
  window (1, 1, 80, 25);	/* We need to use entire screen */
  textcolor (BIGWIN_BORD_COLOR);
  textbackground (BIGWIN_BACK_COLOR);

	/* Draw the window */
  draw_textbox (1, 1, 80, 25, "HELP: DFQEdit - Text Editor", BIGWIN_TEXT_COLOR);

  hide_mouse();

  textcolor (BLUE);
  cprintf_cent (-10, "DFQEdit v%s - Text Editor - by Dave Humphrey, 1996", VERSION);

  textcolor (BIGWIN_TEXT_COLOR);
  gotoxy (4, 4);
  cprintf ("The text editor used in DFQEdit is a basic one, much like the DOS editor");
  gotoxy (4, 5);
  cprintf ("Edit. Mouse and key combinations are supported and special QRC text codes");
  gotoxy (4, 6);
  cprintf ("are displayed in different colors. Read DFQEDIT.TXT for more information.");

  gotoxy (3, 8);
  cprintf ("Editor Key and Mouse Commands");
  gotoxy (5, 9);
  cprintf ("%c%c%c%c / PgUP / PgDN / Home / End / Mouse - Move around text area", 27, 24, 25, 26);
  gotoxy (5, 10);
  cprintf ("ALT+F - Activate File Menu    ALT+Q - Quit Text Editor");
  gotoxy (5, 11);
  cprintf ("   F1 - This help Screen         F2 - Save Text to TXT file (not to QRC!)");
  gotoxy (5, 12);
  cprintf ("   F3 - Load TXT file        CTRL+Y - Delete Current Line");

  gotoxy (3, 13);
  cprintf ("Special QRC Text Codes");

  gotoxy (5, 14);
  textcolor (_STRING_TEXT_COLOR);
  textbackground (_STRING_BACK_COLOR);
  cprintf ("_string_");
  textcolor (BIGWIN_TEXT_COLOR);
  textbackground (BIGWIN_BACK_COLOR);
  cprintf (" - Name/Location          ");
  textcolor (__STRING_TEXT_COLOR);
  textbackground (__STRING_BACK_COLOR);
  cprintf ("__string_");
  textcolor (BIGWIN_TEXT_COLOR);
  textbackground (BIGWIN_BACK_COLOR);
  cprintf (" - City Name");

  gotoxy (5, 15);
  textcolor (___STRING_TEXT_COLOR);
  textbackground (___STRING_BACK_COLOR);
  cprintf ("___string_");
  textcolor (BIGWIN_TEXT_COLOR);
  textbackground (BIGWIN_BACK_COLOR);
  cprintf (" - Location of Person or Dungeon Name");

  gotoxy (5, 16);
  textcolor (____STRING_TEXT_COLOR);
  textbackground (____STRING_BACK_COLOR);
  cprintf ("____string_");
  textcolor (BIGWIN_TEXT_COLOR);
  textbackground (BIGWIN_BACK_COLOR);
  cprintf (" - Province            ");
  textcolor (PER_STRING_TEXT_COLOR);
  textbackground (PER_STRING_BACK_COLOR);
  cprintf ("%%string");
  textcolor (BIGWIN_TEXT_COLOR);
  textbackground (BIGWIN_BACK_COLOR);
  cprintf (" - Variable, like players name");

  gotoxy (5, 17);
  textcolor (EQ_STRING_TEXT_COLOR);
  textbackground (EQ_STRING_BACK_COLOR);
  cprintf ("=string_");
  textcolor (BIGWIN_TEXT_COLOR);
  textbackground (BIGWIN_BACK_COLOR);
  cprintf (" - Variable type          ");
  textcolor (END_PART_TEXT_COLOR);
  textbackground (END_PART_BACK_COLOR);
  cprintf ("~");
  textcolor (BIGWIN_TEXT_COLOR);
  textbackground (BIGWIN_BACK_COLOR);
  cprintf (" - End part of text section");

  gotoxy (5, 18);
  textcolor (CR_TEXT_COLOR);
  textbackground (CR_BACK_COLOR);
  cprintf ("|");
  textcolor (BIGWIN_TEXT_COLOR);
  textbackground (BIGWIN_BACK_COLOR);
  cprintf (" - End line character representing a centered text line");

  gotoxy (5, 19);
  textcolor (CR_TEXT_COLOR);
  textbackground (CR_BACK_COLOR);
  cprintf ("<");
  textcolor (BIGWIN_TEXT_COLOR);
  textbackground (BIGWIN_BACK_COLOR);
  cprintf (" - End line character representing a left justified text line");

  gotoxy (5, 21);
  cprintf ("Initial / Current Available Memory: %ld / %ld bytes", initial_far_heapsize, farcoreleft());
  gotoxy (5, 22);
  cprintf ("Status of Far Heap: ");

  switch (farheapcheck()){
    case _HEAPCORRUPT: cprintf ("Heap has been Corrupted - You Should Reboot!"); break;
    case _HEAPOK: cprintf ("Heap OK"); break;
    default: cprintf ("No heap"); break;
   };

  cprintf_cent (14, "Press Any Key to Continue...");

  show_mouse();
  wait_for_key();
 }
/*========== End of procedure void editmenu_help() ========================*/


/*========== Does load command ============================================*/
void editmenu_load (char *argument) {
  char far *buffer, far *temp_buf;

  window (1, 1, 80, 25);

  if (!(buffer = new far char[max_length + 1]))
    bug (MEM_ERR_MSG, "void editmenu_load() - *buffer (%d)", max_length + 1);

  buffer[0] = '\0';	/* Set filename to intially nothing */

		/* Input filename to load */
  if (file_dialogue(buffer, &text_path[0], "Load Text", "Load", "*.txt")) {
    if (load_editfile (buffer)) {  /* If successful, try and load the file... */
      loaded_file = TRUE;

      if (loaded_filename) delete loaded_filename;

      if (!(loaded_filename = new far char[_fstrlen(buffer) + 1]))
	bug(MEM_ERR_MSG, "editmenu_load() - *loaded_filename (%d)", _fstrlen(buffer) + 1);

      strcpy (loaded_filename, buffer);
      current_line = 0; //Succesful load, set cursor to start
      current_char = 0;
     }
    else {
      if (!(temp_buf = new far char[_fstrlen(buffer) + 30]))
	bug (MEM_ERR_MSG, "editmenu_load() - *temp_buf (%d)", _fstrlen(buffer) + 30);

      sprintf (temp_buf, "Could not Load file '%s'!", buffer);
      error_box ("ERROR: Load Text", temp_buf);
      delete temp_buf;
     }
   }

  delete buffer;
  window (2, lines_y, 1 + max_length, max_lines + 1);
  draw_all_lines (edit_start, START); //Redraw Screen...
 }
//========== End of procedure editmenu_load() ===============================


//========== Does new command ===============================================
void editmenu_new (char *argument) {

  while (edit_end)
    delete_edit (edit_end);

  if (*argument != '!') {
    if (!init_edit())
      bug ("Couldn't initialize edit list!", "editmenu_new()");
   }

  current_line = 0;
  current_char = 0;

  if (loaded_filename) {
    delete loaded_filename;
    loaded_filename = NULL;
   }

  loaded_file = FALSE;
  if (*argument != '!') draw_all_lines (edit_line, START);
  edit_end = NULL;
  return;
 }
//========== End of procedure editmenu_new() ================================


//========== Does quit command ==============================================
void editmenu_quit (char *argument) {
  quit = TRUE;
 }
//========== End of procedure editmenu_quit() ===============================


//========== Does save command ==============================================
void editmenu_save (char *argument) {
  char far *buffer, far *temp_buf;

  window (1, 1, 80, 25);

  if (!(buffer = new far char[max_length + 1]))
    bug (MEM_ERR_MSG, "editmenu_save() - *buffer (%d)", max_length + 1);

  if (loaded_file && loaded_filename) //If file is loaded in memory, set filename
    strcpy(buffer, loaded_filename);
  else
    *buffer = 0;	//Set filename to nothing if nothing loaded

	//Get filename to save text as...
  if (file_dialogue(buffer, &text_path[0], "Save Text", "Save", "*.txt")) {
    if (save_editfile(buffer)) {   //If input is sucessful, save file...

      if (loaded_filename) delete loaded_filename;

      if (!(loaded_filename = new far char[_fstrlen(buffer) + 1]))
	bug (MEM_ERR_MSG, "editmenu_save() - *loaded_filename (%d)", _fstrlen(buffer) + 1);

      loaded_file = TRUE;      //Save filename if save was succesful
      _fstrcpy(loaded_filename, buffer);
     }
    else {
      if (!(temp_buf = new far char[_fstrlen(buffer) + 30]))
	bug (MEM_ERR_MSG, "editmenu_save() - *temp_buf (%d)", _fstrlen(buffer) + 30);

      sprintf (temp_buf, "Could not Save file '%s'!", buffer);
      error_box ("ERROR: Save Text", temp_buf);
      delete temp_buf;
     }
   }

  delete buffer;
  window(2, lines_y, 1 + max_length, max_lines + 1);
  draw_all_lines (edit_start, START); //Redraw Screen...
 }
//========== End of procedure editmenu_save() ===============================


//========== Reads in a string ==============================================
boolean edit_scanf (int x, int y, int length, char *string) {
  boolean redraw = TRUE, done = FALSE;
  char ch;
  string[0] = '\0';

  while (!done) {

    if (redraw) {
      redraw = FALSE;
      hide_mouse ();
      _setcursortype (_NOCURSOR);
      gotoxy (x, y);
      cprintf ("%-*s", length, string);
      gotoxy (x + strlen(string), y);
      _setcursortype (_NORMALCURSOR);
      show_mouse ();
     }

    GetKey();
    ch = (char) keylast;

    if (edit_isalpha(ch) && strlen(string) < length) {
      string[strlen(string) + 1] = '\0';
      string[strlen(string)] = ch;
      redraw = TRUE;
      delay (KEY_DELAY/2);
     }
    else if (key_press[SCAN_BKSP] != 0 && strlen(string) > 0) {
      string[strlen(string) - 1] = '\0';
      redraw = TRUE;
      delay (delay_length);
     }
    else if (get_enter_state()) {
      done = TRUE;
      delay(KEY_DELAY);
     }
    else if (get_esc_state()) {
      delay(KEY_DELAY);
      return (FALSE);
     }
   }; //End of while loop

  return (TRUE); }
//========== End of function edit_scanf() ===================================


//========== Returns Which Menu Item the Mouse is On ========================
int find_mouse_menu (const int x, const int y, MENU_TYPE *menu) {
  int i = 0;

  while (menu->name[i][0] != '#') {
    while (menu->name[i][0] == '-')	//Skip horizontal bars...
      i++;

    if (x > menu->x && x < menu->x + menu->width - 1 &&
	y == menu->y + i + 2)
      return(i);

    i++;
   }

  return (-1); }
//========== End of function find_mouse_menu() ==============================


//========== Returns String Length of All Strings in Edit List ==============
long int get_editsize(void) {
  EDIT_TYPE far *line;
  long int size = 0;

  line = edit_start;

  while (line != NULL) {
    size += strlen(line->data) + 1;
    line = line->next;
   }

  return (size); }
//========== End of function get_editsize() =================================


//========== Inits the edit doubly linked list ==============================
boolean init_edit (void) {

  if (edit_start == NULL) {
    if (!(edit_start = new far EDIT_TYPE))
      bug (MEM_ERR_MSG, "init_edit() - *edit_start (%d)", sizeof (EDIT_TYPE));

    edit_start->data[0] = 0;  /* Nothing in string... */
    edit_start->next = NULL;
    edit_start->prev = NULL;
    edit_end = edit_start;
    edit_line = edit_start;
    return (TRUE);
   }

  return(FALSE); }
//========== End of procedure init_edit() ===================================


//========== Attempts to load a text file ===================================
boolean load_editfile (char far *filename) {
  FILE *f;	/* File pointer */
  char buffer[81], ch, far *temp_file;	/* Temp string buffers */

	/* Allocate data for full filename */
  if (!(temp_file = new far char[_fstrlen(filename) + strlen(text_path) + 2]))
    bug (MEM_ERR_MSG, "load_editfile() - *temp_file (%d)", _fstrlen(filename) + _fstrlen(text_path) + 2);

  _fstrcpy (temp_file, text_path);

	/* Make sure path has ending */
  if (_fstrlen(temp_file) > 0 && *(temp_file + _fstrlen(temp_file) - 1) != '\\')
    chrcat (temp_file, '\\');

  _fstrcat (temp_file, filename);

  f = openfile (temp_file, "rt");

  editmenu_new("!");  //Clear the current file...
  buffer[0] = '\0';

  do {
    ch = fgetc(f);	//Read a character

    if (ch == EOF) {	//End of file...
      if (strlen(buffer) > 0) create_new_end (buffer);
      edit_line = edit_start;
      break;
     }
    else if (ch == 9) { //Tab Character
      if (strlen(buffer) + 5 >= max_length) { //Make sure we don't exceed the maximum length
	create_new_end (buffer);
	strcpy (buffer, "     ");
       }
      else { //Simply add onto end
	strcat (buffer, "     ");
       }
     }
    else if (ch == 10) { //CR
      create_new_end (buffer);
      buffer[0] = 0;
     }
    else if (strlen (buffer) < max_length) { //Append to end of buffer
      chrcat(&buffer[0], ch);
     }
    else { //End of line
      create_new_end (buffer);
      buffer[1] = 0;
      buffer[0] = ch;
     }
  } while(TRUE);	//Read file until EOF is read

  fclose (f);
  delete temp_file;	//Delete allocated data
  return (TRUE); }
//========== End of procedure load_editfile() ===============================


//========== Parses An Edit Line, Printing in Color? ========================
void parse_edit_line(char *string) {
  int orig_text_color, orig_back_color, text_color1, back_color1;
  int text_color2, back_color2, text_color3 = 7, back_color3 = 0;
  int text_color4, back_color4;
  boolean var1 = FALSE, var2 = FALSE, var3 = FALSE;

  orig_text_color = get_text_color();
  orig_back_color = get_back_color();

  while (*string != 0) { //Print to end of line
    if (*string == '_') {  //If _string_
      if (*(string + 1) == '_') {	//If __string_
	if (*(string + 2) == '_') {	//If ___string_
	  if (*(string + 3) == '_') {	//If ____string_
	    var2 = TRUE;
	    text_color2 = get_text_color();
	    back_color2 = get_back_color();
	    textcolor (____STRING_TEXT_COLOR);
	    textbackground (____STRING_BACK_COLOR);
	    cprintf ("____");
	    string += 3;
	   } //End of if '____'
	  else {
	    var2 = TRUE;
	    text_color2 = get_text_color();
	    back_color2 = get_back_color();
	    textcolor (___STRING_TEXT_COLOR);
	    textbackground (___STRING_BACK_COLOR);
	    cprintf ("___");
	    string += 2;
	   }
	 } //End of if '___'
	else {
	  var2 = TRUE;
	  text_color2 = get_text_color();
	  back_color2 = get_back_color();
	  textcolor (__STRING_TEXT_COLOR);
	  textbackground (__STRING_BACK_COLOR);
	  cprintf ("__");
	  string++;
	 }
       } //End of if '__'
      else if (var3) {
	cprintf ("_");
	var3 = FALSE;
	textcolor (text_color3);
	textbackground (back_color3);
       }
      else if (!var2) {
	var2 = TRUE;
	text_color2 = get_text_color();
	back_color2 = get_back_color();
	textcolor (_STRING_TEXT_COLOR);
	textbackground (_STRING_BACK_COLOR);
	cprintf ("_");
       }
      else if (var2) {
	var2 = FALSE;
	cprintf ("_");
	textcolor (text_color2);
	textbackground (back_color2);
       }
     } //End of if '_'
    else if (*string == '=') {
      var3 = TRUE;
      text_color3 = get_text_color();
      back_color3 = get_back_color();
      textcolor (EQ_STRING_TEXT_COLOR);
      textbackground (EQ_STRING_BACK_COLOR);
      cprintf ("=");
     }
    else if (*string == '%') {
      var1 = TRUE;
      text_color1 = get_text_color();
      back_color1 = get_back_color();
      textcolor (PER_STRING_TEXT_COLOR);
      textbackground (PER_STRING_BACK_COLOR);
      cprintf ("%");
     }
    else if (ispunctuation(*string) && var1) {
      var1 = FALSE;
      textcolor (text_color1);
      textbackground (back_color1);
      cprintf ("%c", *string);
     }
    else if (*string == '~') { 	//End of text section
      text_color4 = get_text_color();
      back_color4 = get_back_color();
      textcolor (END_PART_TEXT_COLOR);
      textbackground (END_PART_BACK_COLOR);
      cprintf ("~");
      textcolor (text_color4);
      textbackground (back_color4);
     }
    else if (*string == TEXT_CR_CENTER || *string == TEXT_CR_LEFT) { //CR signalling end of text line
      text_color4 = get_text_color();
      back_color4 = get_back_color();
      textcolor (CR_TEXT_COLOR);
      textbackground (CR_BACK_COLOR);
      cprintf ("%c", *string);
      textcolor (text_color4);
      textbackground (back_color4);
     }
    else {	//Just put a character
      cprintf ("%c", *string);
     }

    string++;
   } //End of while loop

  textcolor (orig_text_color);
  textbackground (orig_back_color);
 }
//========== End of procedure parse_edit_line() =============================


//========== Attempts to save a text file ===================================
boolean save_editfile (char *filename) {
  FILE *f;	//File pointer
  EDIT_TYPE far *line;	//Temp pointer
  char far *buf, far *temp_file;	//Temp buffers

	//Allocate data for full filename
  if (!(temp_file = new far char[_fstrlen(filename) + _fstrlen(text_path) + 2]))
    bug (MEM_ERR_MSG, "load_editfile() - *temp_file (%d)", _fstrlen(filename) + _fstrlen(text_path) + 2);

  _fstrcpy (temp_file, text_path);

	/* Make sure path has ending \ */
  if (_fstrlen(temp_file) > 0 && *(temp_file + _fstrlen(temp_file) - 1) != '\\')
    chrcat (temp_file, '\\');

  _fstrcat (temp_file, filename);

  if (fileexists(temp_file)) { /* Does file exist? */
	/* Alloc temp string buffer for message */
    if (!(buf = new far char[_fstrlen(filename) + 26]))
      bug (MEM_ERR_MSG, "save_editfile() - *buf (%d)", _fstrlen(filename) + 26);

	/* Create warning message */
    sprintf (buf, "File '%s' exists, Overwrite", filename);

	/* Make sure user wants to overwrite file... */
    if (!get_yn_choice(_fstrlen(buf) + 26, 9, "Warning: File Exists", buf)) {
      delete buf;	//Delete allocated memory
      delete temp_file;
      return (FALSE);
     }

    delete buf;	/* Delete allocated memory */
   } //End of if file exists

	/* Attempt to open file for writing */
  f = openfile (filename, "wt");
  line = edit_start;	/*  Set temp pointer to beginning of text data */

  do {	//Save all the data
    fprintf (f, "%s\n", line->data);
    line = line->next;
  } while(line != NULL);

  fputc (EOF, f);	/* Write the End-Of-File marker */

  fclose (f);
  delete temp_file;	/* Delete allocated data */
  return(TRUE); }
//========== End of procedure save_editfile() ===============================


//========== Main loop for the text editor ==================================
void text_editor(const char *title) {
  int count, mx, my;
  char ch, buffer[80];

  hide_mouse ();
  textmode (C80);	/*  Regular 80x25 Text Mode */
  show_mouse ();
  quit = FALSE;
  _wscroll = 0;
  loaded_filename = NULL;
  edit_line = edit_start;
  current_line = 0;
  current_char = 0;
  draw_edit_screen(title);
  draw_all_lines(edit_start, START);
  gotoxy (1, 1);

  do {
	/* A mouse click somewhere? */
    if (get_mouse_event (LEFT_BUTTON) == BUTTON_RELEASE) {
      mx = MOUSEX;
      my = MOUSEY;
				//Check for a click on the 'file menu'
      if (mx >= file_menu.x-1 && mx < file_menu.x + 5 && my == 1) {
	do_editmenu (&file_menu);
	gotoxy (current_char + 1, current_line + 1);
	_setcursortype (_NORMALCURSOR);
       }
      else if (mx >= 70 && my == 1) { //Help
	editmenu_help (NULL);
	draw_edit_screen (title);	//Redraw screen
	draw_all_lines (edit_start, START);
       }
      else if (mx > 1 && mx < 79 && my > 1 && my < 25) { //Click in text window
	if (my - 2 == current_line) {
	  current_char = mx - 2;

	  if (current_char > strlen(edit_line->data))
	    current_char = strlen(edit_line->data);

	  draw_all_lines (edit_line, MIDDLE);
	 }
	else if (my - 2 < current_line) {    //Move up

	  while (edit_line->prev != NULL && current_line > my - 2) {
	    edit_line = edit_line->prev;
	    current_line--;
	   } //End of while loop

	  current_char = mx - 2;
	  if (current_char > strlen(edit_line->data))
	    current_char = strlen(edit_line->data);

	  if (current_line < 0)
	    draw_all_lines (edit_line, START);
	  else
	    draw_all_lines (edit_line, MIDDLE);
	 } //End click and move up
	else if (my - 2 > current_line) {    //Move down

	  while (edit_line->next != NULL && current_line < my - 2) {
	    edit_line = edit_line->next;
	    current_line++;
	   } //End of while loop

	  current_char = mx - 2;
	  if (current_char > strlen(edit_line->data))
	    current_char = strlen(edit_line->data);

	  if (current_line > 22)
	    draw_all_lines (edit_line, END);
	  else
	    draw_all_lines (edit_line, MIDDLE);
	 } //End of click and move down

       }
     }

    GetKey();
    ch = (char) keylast;

	/* ALT-F = File Menu */
    if (get_alt_key(SCAN_F)) {
      do_editmenu (&file_menu);
      gotoxy (current_char + 1, current_line + 1);
      _setcursortype (_NORMALCURSOR);
      delay (delay_length);
     } //End of ALT-F
    else if (get_alt_key(SCAN_Q)) {	/* ALT-Q = Quit */
       quit = TRUE;
     } //End of ALT-Q
    else if ((key_press[SCAN_F1] != 0) || (get_alt_key(SCAN_H))) {	/* ALT-H/F1 = Help */
     editmenu_help (NULL);
     draw_edit_screen (title);	/* Redraw screen */
     draw_all_lines (edit_start, START);
     } //End of F1 - Help
    else if (key_press[SCAN_F2] != 0) { /* F2, save */
      editmenu_save(NULL);
      gotoxy (current_char + 1, current_line + 1);
      _setcursortype (_NORMALCURSOR);
      delay (delay_length);
     } //End of F2
    else if (key_press[SCAN_F3] != 0) { /* F3, Load */
      editmenu_load(NULL);
      gotoxy (current_char + 1, current_line + 1);
      _setcursortype (_NORMALCURSOR);
      delay (delay_length);
     } //End of F3
    else if (key_press[SCAN_TAB] != 0) { /* Tab character ... 5 spaces */
     }
    else if (get_ctrl_key(SCAN_Y)) {	/* CTRL-Y */
      if (edit_line->next != NULL) { /* Normal delete in middle or at start */
	edit_line = edit_line->next;
	delete_edit (edit_line->prev);

	if (current_char > strlen(edit_line->data))
	  current_char = strlen(edit_line->data);

	draw_all_lines (edit_line, MIDDLE);
       }
      else if (edit_line->prev != 0) {	//Delete end line
	edit_line = edit_line->prev;
	delete_edit (edit_line->next);
	if (current_line != 0) current_line--;

	if (current_char > strlen(edit_line->data))
	  current_char = strlen(edit_line->data);

	draw_all_lines (edit_line, MIDDLE);
       }
      else { //Delete only text line
	edit_line->data[0] = 0;	//Just clear the string...
	current_char = 0;
	draw_all_lines (edit_start, START);
       }

      delay(delay_length * 2);
     } //End of CTRL-Y
    else if (edit_isalpha(ch)) {  //Normal character...
      edit_addchar (edit_line, ch);
     }
    else if (key_press[SCAN_BKSP] != 0) {  //Delete something
      edit_delchar (edit_line, BACKSPACE);
      delay(delay_length * 2);
     }
    else if (get_enter_state()) { //Insert line
      if (edit_line->next == NULL) { //Add to end

	if (strlen(edit_line->data) > current_char) {
	  strcpy(buffer, &edit_line->data[current_char]);
	  edit_line->data[current_char] = '\0';
	  draw_edit_line (edit_line, current_line);
	  create_new_end (buffer);
	 }
	else
	  create_new_end ("\0");

	current_line++;
	current_char = 0;
	edit_line = edit_end;
	draw_edit_line (edit_line, current_line);
       } //End of add to end
      else { //Add in middle
	if (strlen(edit_line->data) > current_char) {
	  strcpy(buffer, &edit_line->data[current_char]);
	  edit_line->data[current_char] = '\0';
	  draw_edit_line (edit_line, current_line);
	  create_new_line (buffer);
	 }
	else
	  create_new_line ("\0");

	current_line++;
	current_char = 0;
	draw_all_lines (edit_line, MIDDLE);
       } //End of add in middle

      delay(delay_length * 2);
     }
    else if (get_del_state()) {  //Delete something
      edit_delchar (edit_line, DELETE);
      delay(delay_length * 2);
     }
    else if (get_pgup_state()) {    //Move up faster
      count = 0;

      while (edit_line->prev != NULL && count < 20) {
	count++;
	edit_line = edit_line->prev;
	current_line--;
       } //End of while loop

      if (current_line < 0) current_line = 0;
      if (current_char > strlen(edit_line->data)) current_char = strlen(edit_line->data);

      if (current_line == 0)
	draw_all_lines (edit_line, START);
      else
	draw_all_lines (edit_line, MIDDLE);

      delay (delay_length);
     } //End of pageup
    else if (get_pgdn_state()) {    //Move down faster
      count = 0;

      while (edit_line->next != NULL && count < 20) {
	count++;
	edit_line = edit_line->next;
	current_line++;
       } //End of while loop

      if (current_line > 22) current_line = 22;
      if (current_char > strlen(edit_line->data)) current_char = strlen(edit_line->data);

      if (current_line == 22)
	draw_all_lines (edit_line, END);
      else
	draw_all_lines (edit_line, MIDDLE);

      delay (delay_length);
     } //End of pagedown
    else if (get_up_state()) {  //Move up
      if (edit_line->prev != NULL) {
	edit_line = edit_line->prev;
	current_line--;
	if (current_char > strlen(edit_line->data)) current_char = strlen(edit_line->data);
	draw_edit_line (edit_line, current_line);
       }

      delay(delay_length);
     } //End of move up
    else if (get_down_state()) {  //Move down
      if (edit_line->next != NULL) {
	edit_line = edit_line->next;
	current_line++;
	if (current_char > strlen(edit_line->data)) current_char = strlen(edit_line->data);
	draw_edit_line (edit_line, current_line);
       }

      delay(delay_length);
     } //End of move down
    else if (get_left_state()) {  //Move left
      current_char--;

      if (current_char < 0) {
	if (edit_line->prev == NULL)
	  current_char = 0;
	else {
	  edit_line = edit_line->prev;
	  current_char = strlen(edit_line->data);
	  current_line--;
	 }
       }

      gotoxy (current_char + 1, current_line + 1);
      delay(delay_length);
     }
    else if (get_right_state()) {  //Move Right
      current_char++;

      if (current_char > strlen(edit_line->data)) {
	if (edit_line->next == NULL)
	  current_char--;
	else {
	  edit_line = edit_line->next;
	  current_char = 0;
	  current_line++;
	 }
       }

      gotoxy (current_char + 1, current_line + 1);
      delay(delay_length);
     } //End of move right
    else if (get_home_state()) {	//Home
      current_char = 0;
      gotoxy (current_char + 1, current_line + 1);
      delay(delay_length);
     } //End of home
    else if (get_end_state ()) {	//End
      current_char = strlen(edit_line->data);
      gotoxy (current_char + 1, current_line + 1);
      delay(delay_length);
     } //End of end

  } while (!quit);

  hide_mouse ();
  window (1, 1, 80, 25);
  clrscr();
  textmode (64);	/* 80x50 text mode */
  window (1, 1, 80, 50);
  show_mouse();
 }
//========== End of procedure text_editor() =================================