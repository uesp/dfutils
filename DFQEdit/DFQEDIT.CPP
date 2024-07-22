/* DFQEDIT.C - Copyright (c) 1996 - Dave Humphrey - aj589@freenet.carleton.ca
 * v0.10 Beta (12 December 1996)
 * Editor of the QRC and QBN files...hopefully
 */

	/* Standard 'C' Includes */
#include <stdlib.h>
#include <conio.h>
#include <stdio.h>
#include <string.h>
#include <io.h>
#include <dos.h>
#include <direct.h>
#include <dir.h>
#include <ctype.h>
#include <process.h>
#include <alloc.h>
#include <time.h>

	/* User Defined Includes */
#include "genutil.h"
#include "fileutil.h"
#include "mouse.h"
#include "keyasm.h"
#include "keybrd.h"
#include "buttons.h"
#include "util.h"

	/* DFQEdit Specific Includes */
#include "edit.h"
#include "scrlbar.h"
#include "listbox.h"
#include "filelist.h"
#include "textfld.h"
#include "dragbox.h"
#include "dfqedit.h"

	/* String Descriptions of Program */
char dfqedit_version[] = VERSION;
char dfqedit_title[] = "DFQEDIT.C v%s - Dave Humphrey\n";

	/* Location of various files...from CFG file... */
char quest_path[MAX_STRING_LENGTH + 1] = "";
char backup_path[MAX_STRING_LENGTH + 1] = "";

	/* For the initial available memory */
long int initial_far_heapsize;

	/* Define a drag-type box for selecting guilds */
LIST_BOX guild_list (GUILD_X, GUILD_Y, GUILD_WIDTH, 8, DRAGBOX_TEXT_COLOR, DRAGBOX_BACK_COLOR, DRAGBOX_BOLDTEXT_COLOR, DRAGBOX_BOLDBACK_COLOR );

	/* Define drag-type box for selecting minimum rank for quest */
LIST_BOX rank_list (RANK_X, RANK_Y, RANK_WIDTH, 10, DRAGBOX_TEXT_COLOR, DRAGBOX_BACK_COLOR, DRAGBOX_BOLDTEXT_COLOR, DRAGBOX_BOLDBACK_COLOR );

	/* Define the drag-box for selecting the pc-status required for quest */
LIST_BOX pc_status_list (PC_STATUS_X, PC_STATUS_Y, PC_STATUS_WIDTH, 4, DRAGBOX_TEXT_COLOR, DRAGBOX_BACK_COLOR, DRAGBOX_BOLDTEXT_COLOR, DRAGBOX_BOLDBACK_COLOR );

	/* Define the drag-box for selecting how the quest is given to PC */
LIST_BOX qgiven_list (QGIVEN_X, QGIVEN_Y, QGIVEN_WIDTH, 4, DRAGBOX_TEXT_COLOR, DRAGBOX_BACK_COLOR, DRAGBOX_BOLDTEXT_COLOR, DRAGBOX_BOLDBACK_COLOR );

	/* Define the drag-boxs for qbn-editing mode */
LIST_BOX reward_type_list (REWARD_TYPE_X, REWARD_TYPE_Y, REWARD_TYPE_WIDTH, 4, DRAGBOX_TEXT_COLOR, DRAGBOX_BACK_COLOR, DRAGBOX_BOLDTEXT_COLOR, DRAGBOX_BOLDBACK_COLOR );
LIST_BOX reward_list (REWARD_X, REWARD_Y, REWARD_WIDTH, 10, DRAGBOX_TEXT_COLOR, DRAGBOX_BACK_COLOR, DRAGBOX_BOLDTEXT_COLOR, DRAGBOX_BOLDBACK_COLOR );
LIST_BOX msg_type_list (MSG_TYPE_X, MSG_TYPE_Y, MSG_TYPE_WIDTH, 10, DRAGBOX_TEXT_COLOR, DRAGBOX_BACK_COLOR, DRAGBOX_BOLDTEXT_COLOR, DRAGBOX_BOLDBACK_COLOR );
LIST_BOX find_msg_list (FIND_MSG_X, FIND_MSG_Y, FIND_MSG_WIDTH, FIND_MSG_HEIGHT, DRAGBOX_TEXT_COLOR, DRAGBOX_BACK_COLOR, BLACK, LIGHTGREEN );
LIST_BOX gender_list (GENDER_X, GENDER_Y, 16, 3, DRAGBOX_TEXT_COLOR, DRAGBOX_BACK_COLOR, DRAGBOX_BOLDTEXT_COLOR, DRAGBOX_BOLDBACK_COLOR );
BUTTON_ARRAY find_msg_buttons;

	/* Many lists for the section[4] qbn data */
LIST_BOX gen_loc_list (LOC_X, LOC_Y, 14, 3, DRAGBOX_TEXT_COLOR, DRAGBOX_BACK_COLOR, DRAGBOX_BOLDTEXT_COLOR, DRAGBOX_BOLDBACK_COLOR );
LIST_BOX loc_list (LOC_X, LOC_Y + 2, 18, 8, DRAGBOX_TEXT_COLOR, DRAGBOX_BACK_COLOR, DRAGBOX_BOLDTEXT_COLOR, DRAGBOX_BOLDBACK_COLOR );
LIST_BOX sec41_list (LOC_X, LOC_Y + 16, 18, 8, DRAGBOX_TEXT_COLOR, DRAGBOX_BACK_COLOR, DRAGBOX_BOLDTEXT_COLOR, DRAGBOX_BOLDBACK_COLOR );
LIST_BOX sec42_list (LOC_X, LOC_Y + 18, 18, 3, DRAGBOX_TEXT_COLOR, DRAGBOX_BACK_COLOR, DRAGBOX_BOLDTEXT_COLOR, DRAGBOX_BOLDBACK_COLOR );
LIST_BOX sec43_list (LOC_X, LOC_Y + 20, 16, 3, DRAGBOX_TEXT_COLOR, DRAGBOX_BACK_COLOR, DRAGBOX_BOLDTEXT_COLOR, DRAGBOX_BOLDBACK_COLOR );

	/* Section 7 List */
LIST_BOX mob_list (MOB_X, MOB_Y, 18, 8, DRAGBOX_TEXT_COLOR, DRAGBOX_BACK_COLOR, DRAGBOX_BOLDTEXT_COLOR, DRAGBOX_BOLDBACK_COLOR );
LIST_BOX sec71_list (MOB_X, MOB_Y + 6, 16, 8, DRAGBOX_TEXT_COLOR, DRAGBOX_BACK_COLOR, DRAGBOX_BOLDTEXT_COLOR, DRAGBOX_BOLDBACK_COLOR );

	/* Data for all buttons/fields/lists on the screen */
DRAG_ARRAY drag_buttons;
DRAG_ARRAY section_drag[11];
BUTTON_ARRAY section_button[11];
TEXT_ARRAY section_field[11];

	/* Define the main screen buttons, load, save, edit */
BUTTON_ARRAY main_buttons;

	/* Option Buttons for editting QText */
BUTTON_ARRAY qtext_buttons;

	/* Buttons for the qbn edit screen */
BUTTON_ARRAY qbn_buttons;

	/* Holds all the data loaded from QBN file */
QBN_DATA qbn_data;

	/* Define all the quest text types for displaying/editting */
QUEST_TEXT_ARRAY quest_texts;

	/* Holds all the QRC data */
QRC_DATA qrc_data;

	/* Description of quest text types */
QTEXT_DESC_TYPE qtext_desc[] = {
  { "Description", 	0x3E8 },
  { "Unaccepted", 	0x3E9 },
  { "Accepted", 	0x3EA },
  { "Failed Quest", 	0x3EB },
  { "Successful", 	0x3EC },
  { "Rumours During",	0x3ED },
  { "Rumours Success",	0x3EE },
  { "Rumours Failed", 	0x3EF },
  { "Success Greeting",	0x3F0 },
  { "Failed Greeting", 	0x3F1 },
  { "Log Entry", 	0x3F2 },
  { "End", 		0xFFFF},
  { "", 		0x000 }
 };


	/* Quest Filenames and current save/load status */
char quest_filename[MAX_STRING_LENGTH];
char new_quest_filename[MAX_STRING_LENGTH];
boolean quest_loaded = FALSE;
boolean quest_saved = FALSE;

	/* Editting modes */
boolean qrc_screen = TRUE;	/* Which screen mode are we in? */
int qbn_mode = 0;		/* Which screen mode in the qbn screen are we in? */

	/* Array indices */
int PC_STATUS_INDEX = 0;
int GUILD_INDEX = 0;
int QGIVEN_INDEX = 0;
int RANK_INDEX = 0;
int REWARD_TYPE_INDEX = 0;
int REWARD_INDEX = 0;
int MSG_TYPE_INDEX[11];
int MSGID1_INDEX[11];
int MSGID2_INDEX[11];
int NUMBER1_INDEX[11];
int NUMBER2_INDEX[11];
int NUMBER3_INDEX[11];
int NUMBER4_INDEX[11];
int GENDER_INDEX;
int GEN_LOC_INDEX;
int LOC_INDEX;
int MOB_INDEX;

	/* What QBN object in each section is being editted/displayed currently */
int current_section[11];

	/* Maximum sizes of the QBN section arrays */
const int MAX_QBN_ARRAY[11] = {
  MAX_QBN0_ARRAY,
  MAX_QBN1_ARRAY,
  MAX_QBN2_ARRAY,
  MAX_QBN3_ARRAY,
  MAX_QBN4_ARRAY,
  MAX_QBN5_ARRAY,
  MAX_QBN6_ARRAY,
  MAX_QBN7_ARRAY,
  MAX_QBN8_ARRAY,
  MAX_QBN9_ARRAY,
  MAX_QBN10_ARRAY
 };


/*========== Displays information about Q-Edit and Environment ============*/
void click_about(void) {
  int x = (80 - ABOUT_WIDTH)/2 + 3;	/* Where to print stuff */
  int y = (50 - ABOUT_HEIGHT)/2 + 3;
  struct date current_date;
  struct time current_time;

  char months[12][10] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" };

  textcolor (BIGWIN_BORD_COLOR);
  textbackground (BIGWIN_BACK_COLOR);

  draw_textbox_center (ABOUT_WIDTH, ABOUT_HEIGHT, "About DFQEdit", BIGWIN_TEXT_COLOR);

  gotoxy (x, y);
  textcolor (BLUE);
  gotoxy (x, y + 1);
  cprintf ("DFQEdit v%s - Copyright (c) 1996 - by Dave Humphrey", VERSION);

  getdate (&current_date);
  gettime (&current_time);

  textcolor (BIGWIN_TEXT_COLOR);
  gotoxy (ABOUT_WIDTH + x - 30, y + 3);
  cprintf ("%2d:%02d - %2d %s %4d", (int) current_time.ti_hour, (int)current_time.ti_min,
	   (int) current_date.da_day, months[current_date.da_mon - 1], current_date.da_year);

  gotoxy (x, y + 5);
  cprintf ("TES: Daggerfall Quest Editor.  Can currently load and save QRC");
  gotoxy (x, y + 6);
  cprintf ("files which contain all quest texts.");

  gotoxy (x, y + 9);
  textcolor (BLUE);
  cprintf ("CREDITS");
  gotoxy (x, y + 10);
  textcolor (BIGWIN_TEXT_COLOR);
  cprintf ("    Peggy S Hanks (df4@juno.com)");
  gotoxy (x, y + 11);
  cprintf ("    Lord Phoenix (gozer@esoterica.pt)");
  gotoxy (x, y + 12);
  cprintf ("    INT9 (IRQ1) keyboard handler #9 by Patch (hamell@cs.pdx.edu)");

  gotoxy (x, y + 15);
  textcolor (BLUE);
  cprintf ("SYSTEM");
  gotoxy (x, y + 16);
  textcolor (BIGWIN_TEXT_COLOR);
  cprintf ("        Initial Memory: %ld bytes", initial_far_heapsize);
  gotoxy (x, y + 17);
  cprintf ("     Current Available: %ld bytes", farcoreleft());
  gotoxy (x, y + 18);
  cprintf ("     Current Status of Heap: ");

  switch (farheapcheck()){
    case _HEAPCORRUPT: cprintf ("Corrupt Heap - Quit and Reboot!\n"); break;
    case _HEAPOK: cprintf ("Heap is OK\n"); break;
    default: cprintf ("No Heap\n"); break;
   };

  gotoxy (x, y + 19);

  if (MouseInstalled) {	/* Get Mouse information */
    cprintf ("     Mouse detected with %d buttons", MouseButtonCount);
   }
  else
    cprintf ("     No Mouse present!");

  get_ok_button (y + ABOUT_HEIGHT - 7); /* Get input key/mouse */
 }
/*========== End of procedure click_about() ===============================*/


/*========== When exit button is pressed - Display Exit Window ============*/
void click_exit(void) {
  if (get_yn_choice(EXIT_WIDTH, EXIT_HEIGHT, "Exit DFQEdit", "Really Exit from DFQEdit"))
    exit(TRUE);	/* If 'Y'...exit program */
 }
/*========== End of procedure click_exit() ================================*/


/*========== Clears all contents of Current Quest =========================*/
void click_new(void) {
  if (get_yn_choice(EXIT_WIDTH, EXIT_HEIGHT, "New Quest", "Clear Current Quest"))
    new_quest();	/* If 'Y'...clear all variables */
 }
/*========== End of procedure click_new() =================================*/


/*========== Attempts to Load a QRC/QBN file set ==========================*/
void click_load(void) {
  new_quest_filename[0] = 0;	/* Reset string to nothing */

	/* Input filename */
  if (file_dialogue(&new_quest_filename[0], &quest_path[0], "Load QRC", "Load", "*.qrc")) {
    parse_quest_filename(&new_quest_filename[0]);	/* Make sure no extension... */
    new_quest();    /* Make sure we clear everything out first */

    if (qbn_data.load (new_quest_filename) && qrc_data.load (new_quest_filename)) {
      quest_loaded = TRUE;
      strcpy (quest_filename, new_quest_filename);
     } /* End of if loaded qbn... */
    else {
      error_box ("ERROR: Load Quest", "Could not Load file '%s' (QBN/QRC)!", new_quest_filename);
     }
   }
 }
/*========== End of procedure click_load() ================================*/


/*========== Attempts to Save a QRC/QBN file set ==========================*/
void click_save(void) {

  if (!quest_loaded) quest_filename[0] = 0;	/* Reset string to nothing */

		/* Input filename */
  if (file_dialogue(&new_quest_filename[0], &quest_path[0], "Save QRC", "Save", "*.qrc")) {
    parse_quest_filename(&new_quest_filename[0]);	//Make sure no extension...

    if (qrc_data.save (new_quest_filename)) {
      qbn_data.save (new_quest_filename);
      _fstrcpy (quest_filename, new_quest_filename);
      quest_loaded = TRUE;
     }
    else {
      error_box ("ERROR: Save Quest", "Could not Save file '%s'!", new_quest_filename);
     }
   }
 }
/*========== End of procedure click_save() ================================*/


/*========== Converts the edit text into a char * =========================*/
boolean convert_edit_list (char far **text) {
  EDIT_TYPE far *line;

  if (*text) delete *text;

  if (get_editsize() <= 1) { //No strings...
    *text = NULL;
    return (FALSE);
   }

  if (get_editsize() > 64000l || !(*text = new far char[(unsigned int) get_editsize() + 24]))
    bug (MEM_ERR_MSG, "convert_edit_list() - **text (%d)", get_editsize() + 24);

  line = edit_start;

  **text = 0;	/* Make sure string is initially empty */

  while (line) {	/* Go through edit dllist */
    _fstrcat (*text, line->data);

	/* Figure out what type of line, center or left justified */
    if (_fstrlen(*text) > 0) {
      if (*(*text + _fstrlen(*text) - 1) == TEXT_CR_CENTER)
	*(*text + _fstrlen(*text) - 1) = CR_CENTER;
      else if (*(*text + _fstrlen(*text) - 1) == TEXT_CR_LEFT)
	*(*text + _fstrlen(*text) - 1) = CR_LEFT;
      else if (*(*text + _fstrlen(*text) - 1) == '~') {
		/* Do Nothing */
	}
      else
	chrcat (*text, CR_LEFT);
     }

    line = line->next;
   } /* End of while loop */

  chrcat (*text, 0);	/* Terminate the string */
  return(TRUE);
 }
/*========== End of Procedure convert_edit_list() =========================*/


/*========== Converts a char * data of QText into an Edit Line LList ======*/
void convert_qtext (char far *text) {
  char buffer[MAX_STRING_LENGTH + 1];

  delete_all_editlines();	/* Make sure we clear the edit file */

  buffer[0] = 0;	/* Terminate string, 0 length */

  while (*text != 0) { 	/* Continue until end of string */
    if ((unsigned char) *text == CR_CENTER) {		/* A CR signalling end of centered text line */
      chrcat (&buffer[0], TEXT_CR_CENTER);
      create_new_end (buffer);	/* Add line to end of llist */
      buffer[0] = 0;	/* Reset string */
     }
    else if ((unsigned char) *text == CR_LEFT) {	/* A CR signalling end of left justified text line */
      chrcat (&buffer[0], TEXT_CR_LEFT);
      create_new_end (buffer);	//Add line to end of llist
      buffer[0] = 0;	//Reset string
     }
    else if ((unsigned char) *text == '~') {		/* End of one text section */
      chrcat (&buffer[0], *text);
      create_new_end (buffer);
      buffer[0] = 0;
     }
    else { /* Just add to buffer */
      chrcat(&buffer[0], *text);

      if (_fstrlen(buffer) >= max_length) {	/* Make sure we don't exceed line length */
	create_new_end(buffer);	/*  Add line to end of llist */
	buffer[0] = 0;	/* Reset string */
       }
      else if (_fstrlen(buffer) >= max_length - 7 && *text == ' ') {
	create_new_end(buffer);	/* Add line to end of llist */
	buffer[0] = 0;	/* Reset string */
       }
     }

    text++;
   } /* End of while loop */

  if (_fstrlen(buffer) > 0)  	/* Make sure we add last line if it exists */
    create_new_end(buffer);

  edit_line = edit_start;	/* Move to beginning of edit text */
 }
/*========== End of procedure convert_qtext() =============================*/


/*========== Computes the Base of a New Quest Filename based on Types =====*/
void determine_new_quest_filename (char far *string) {
  *string = (char) drag_buttons.buttons[GUILD_INDEX].number;
  string++;

  *string = '0';
  string++;

  *string = (char) drag_buttons.buttons[PC_STATUS_INDEX].number;
  string++;

  *string = (char) drag_buttons.buttons[RANK_INDEX].number + '0';
  string++;

  *string = '0';
  string++;

  *string = (char) drag_buttons.buttons[QGIVEN_INDEX].number;
  string++;

  *string = 0;	/* Terminate string */
 }
/*========== End of procedure determine_new_quest_filename() ==============*/


/*========== Displays the Filenames at top of Screen ======================*/
void draw_filenames (void) {
  int orig_textattr;
  char far *temp_ptr;

  orig_textattr = get_textattr ();

	/* Set proper colors and clear the screen */
  textbackground (BACK_COLOR);
  textcolor (BORDER_COLOR);

	/* Based on current quest settings...compute what the filename would be */
  determine_new_quest_filename (&new_quest_filename[0]);

  if (!quest_loaded) _fstrcpy (quest_filename, "<none>");

	/* Strip the path, if any off of filename... (for length) */
  if ((temp_ptr = _fstrrchr(quest_filename, '\\')) == NULL)
    temp_ptr = &quest_filename[0];

  gotoxy ((80 - _fstrlen(temp_ptr) - _fstrlen(new_quest_filename) - 8)/2, 1);
  cprintf ("| %s / ", temp_ptr);

	/* Redraw the new filename in another color */
  textcolor (LIGHTGREEN);
  cprintf ("%s ", new_quest_filename);
  textcolor (WHITE);
  cprintf ("|");

  textattr (orig_textattr);
 }
/*========== End of procedure draw_filenames() ============================*/


/*========== Draws the Quest Edit Text window on Screen ===================*/
int draw_qtext_window (const char *string) {
  char far *save_screen;	/* For saving the screen background */
  boolean done = FALSE;	/* Loop Counter */
  int ret;	/* Return Value */

  if (!(save_screen = new far char[(QTEXT_WIDTH + 2) * (QTEXT_HEIGHT + 2) * 2 + 2]))
    bug (MEM_ERR_MSG, "draw_qtext_window() - *save_screen (%d)", (QTEXT_WIDTH + 2) * (QTEXT_HEIGHT  +2) * 2 + 2);

  hide_mouse ();

	/* Save background */
  gettext (40 - QTEXT_WIDTH/2, 25 - QTEXT_HEIGHT/2,
	   40 + QTEXT_WIDTH/2, 25 + QTEXT_HEIGHT/2, save_screen);

  textcolor (BIGWIN_BORD_COLOR);
  textbackground (BIGWIN_BACK_COLOR);
  draw_textbox_center (QTEXT_WIDTH, QTEXT_HEIGHT, string, BIGWIN_TEXT_COLOR);

  textcolor (BIGWIN_TEXT_COLOR);
  cprintf_cent (-2, "You may Edit or Delete this Quest Text [E/D]?");
  textcolor (BUTTON_BORD_COLOR);
  textbackground (BUTTON_BACK_COLOR);

	/* Draw the buttons */
  qtext_buttons.draw ();

  show_mouse();
  delay(KEY_DELAY);

  do {
	/* Update mouse status */
    switch (qtext_buttons.check (MOUSEX, MOUSEY, get_mouse_event (LEFT_BUTTON)) ) {
      case 0: ret = 0;	/* Edit */
	      done = TRUE;
	      break;
      case 1: ret = 1;	/* Delete */
	      done = TRUE;
	      break;
      case 2: ret = -1; /* Esc */
	      done = TRUE;
	      break;
     } /* End of switch which button pressed */

    GetKey();	/* Get keyboard status */

    if (key_press[SCAN_E]) {
      delay (KEY_DELAY);
      ret = 0;
      done = TRUE;
     }
    else if (key_press[SCAN_D]) {
      delay (KEY_DELAY);
      ret = 1;
      done = TRUE;
     }
    else if (get_esc_state()) {
      ret = -1;
      done = TRUE;
     }

  } while (!done);

  hide_mouse ();

	/* Restore background */
  puttext (40 - QTEXT_WIDTH/2, 25 - QTEXT_HEIGHT/2,
	   40 + QTEXT_WIDTH/2, 25 + QTEXT_HEIGHT/2, save_screen);

  show_mouse ();
  delete save_screen;
  return (ret);
 }
/*========== End of procedure draw_qtext_window() =========================*/


/*========== Draws the entire screen ======================================*/
void draw_screen(void) {
  _setcursortype (_NOCURSOR);	/* Don't want the cursor appearing */
  textcolor (BORDER_COLOR);
  textbackground (BACK_COLOR);

	/* Draw the main window box */
  draw_textbox (1, 1, 80, 50, NULL, TEXT_COLOR);

	/* Display the quest filenames */
  draw_filenames ();

	/* Display the main buttons */
  main_buttons.draw ();

  if (qrc_screen) {
	/* Display what drag buttons titles */
    textcolor (TITLE_COLOR);
    textbackground (BACK_COLOR);
    drag_buttons.draw_titles();

	/* Display the current values of the filename lists */
    drag_buttons.draw_texts();

	/* Draw all qtext fields */
    quest_texts.draw ();

	/* Draws all the little nice arrows for the drawboxes */
    drag_buttons.draw_arrows ();
   } /* End of if qrc_screen */
  else {
	/* Draw the QBN edit single-border box */
    textcolor (LIGHTGRAY);
    draw_sing_textbox (QBN_BOX_X, QBN_BOX_Y, QBN_BOX_X + QBN_BOX_WIDTH, QBN_BOX_Y + QBN_BOX_HEIGHT, NULL, LIGHTGRAY);

    qbn_buttons.draw ();
    section_button[qbn_mode].draw ();
    section_field[qbn_mode].draw_titles();
    draw_qbn_screen();
   } /* End of if qbn screen */

	/* Delete the temp string buffer and reshow the mouse cursor */
  show_mouse ();
 }
/*========== End of procedure draw_screen() ===============================*/


/*========== Draws only the QBN data screen ===============================*/
void draw_qbn_screen (void) {
  int i;

	/* Make sure current_section[] is set correctly */
  if (qbn_data.section_size[qbn_mode] > 0 && current_section[qbn_mode] == 0)
    current_section[qbn_mode] = 1;
  else if (qbn_data.section_size[qbn_mode] == 0)
    current_section[qbn_mode] = 0;

	/* Get the data from the qbn data structure */
  parse_qbn_data();

  textcolor (TITLE_COLOR);
  textbackground (BACK_COLOR);
  section_drag[qbn_mode].draw_all();
  section_field[qbn_mode].draw();

  i = current_section[qbn_mode];
  gotoxy (30, 6);
  textcolor (LIGHTCYAN);
  cprintf (" Record %2d of %2d (Maximum = %2d) ", i, qbn_data.section_size[qbn_mode], MAX_QBN_ARRAY[qbn_mode]);

  gotoxy (QBN_BOX_X + 12, 3);
  textcolor (LIGHTGRAY);
  cprintf (" Editing QBN Data: ");
  i--;

  if (qbn_mode == QBN_ITEMS) {
    cprintf ("Items      ");
    textcolor (CYAN);
    gotoxy (QBN_BOX_X + 8, 28);
    cprintf ("    Index: %3u (0x%02x) ", qbn_data.section0[i].section_index, qbn_data.section0[i].section_index);
    gotoxy (QBN_BOX_X + 4, 29);
    cprintf (" Reward Type: %5u (0x%04x) ", qbn_data.section0[i].reward_type, qbn_data.section0[i].reward_type);
    gotoxy (QBN_BOX_X + 4, 30);
    cprintf ("      Reward: %10lu (0x%08lx) ", qbn_data.section0[i].reward, qbn_data.section0[i].reward);
    gotoxy (QBN_BOX_X + 4, 31);
    cprintf ("           Sub-Type 1: %5u (0x%04x) ", qbn_data.section0[i].rew1, qbn_data.section0[i].rew1);
    gotoxy (QBN_BOX_X + 4, 32);
    cprintf ("           Sub-Type 2: %5u (0x%04x) ", qbn_data.section0[i].rew2, qbn_data.section0[i].rew2);
    gotoxy (QBN_BOX_X + 4, 33);
    cprintf ("Message Type: %10lu (0x%08lx) ", qbn_data.section0[i].message_type, qbn_data.section0[i].message_type);
   } /* End of items mode */
  else if (qbn_mode == QBN_NPCS) {
    cprintf ("NPC's      ");
    textcolor (CYAN);
    gotoxy (QBN_BOX_X + 8, 32);
    cprintf ("    Index: %3u (0x%02x) ", qbn_data.section3[i].section_index, qbn_data.section3[i].section_index);
    gotoxy (QBN_BOX_X + 8, 33);
    cprintf (" Msg Type: %10lu (0x%08lx) ", qbn_data.section3[i].message_type, qbn_data.section3[i].message_type);
   }
  else if (qbn_mode == QBN_LOC) {
    cprintf ("Location    ");
    textcolor (CYAN);
    gotoxy (QBN_BOX_X + 8, 36);
    cprintf ("    Index: %3u (0x%02x) ", qbn_data.section4[i].section_index, qbn_data.section4[i].section_index);
    gotoxy (QBN_BOX_X + 8, 37);
    cprintf (" Msg Type: %10lu (0x%08lx) ", qbn_data.section4[i].message_type, qbn_data.section4[i].message_type);
   }
  else if (qbn_mode == QBN_MOB) {
    cprintf ("Monsters    ");
    gotoxy (QBN_BOX_X + 8, 31);
    cprintf ("   Index: %3u (0x%02x) ", qbn_data.section7[i].section_index, qbn_data.section7[i].section_index);
    gotoxy (QBN_BOX_X + 8, 32);
    cprintf (" Monster: %3u (0x%02x) ", qbn_data.section7[i].mob_index, qbn_data.section7[i].mob_index);
    gotoxy (QBN_BOX_X + 8, 33);
    cprintf (" Message Type: %10lu (0x%08lx) ", qbn_data.section7[i].message_type, qbn_data.section7[i].message_type);
    gotoxy (QBN_BOX_X + 8, 34);
    cprintf ("   NULL1: %5u (0x%04x) ", qbn_data.section7[i].null1, qbn_data.section7[i].null1);
    gotoxy (QBN_BOX_X + 8, 35);
    cprintf ("   NULL2: %10u (0x%08x) ", qbn_data.section7[i].null2, qbn_data.section7[i].null2);
   }

 }
/*========== End of procedure draw_qbn_screen() ===========================*/


/*========== Edits a Quest Text With a Given Message ID# ==================*/
void edit_msgid (const unsigned id) {
  int i;	/* Loop counter */
  char buffer[41];	/* Temp string buffer */

  if (id == 0) return;

  for (i = 0; i < qrc_data.num_entries; i++) {
    if (qrc_data.header[i].type == id) {	/* Did we find it? */
		/* Edit the text */
      if (quest_texts.texts[i].active)
	convert_qtext (qrc_data.text[i]);
      else {
	delete_all_editlines();
	init_edit();
       }

      sprintf (buffer, "Edit %s (%d)", get_qtext_desc (qrc_data.header[i].tag), qrc_data.header[i].type);

      text_editor (buffer);
      quest_texts.texts[i].active = convert_edit_list(&qrc_data.text[i]);
      draw_screen();
      return;
     }
   } /* End of for loop */

  get_message_box ("Text Not Found", "Quest Text with Type = %u not found!", id);
 }
/*========== End of procedure edit_msgid() ================================*/


/*========== Routines run when program is stopped =========================*/
void exit_dfqedit(void) {
  int i;	/* Loop counter */

  textmode(C80);	/* Normal text mode */
  printf ("Returned to textmode (80x25)\n");

	/* Reset to original drive and directory */
  setdisk (orig_drive);

  if (chdir (orig_directory) == -1)
    printf ("Failed to restore original path!\n");

	/* Replace our own keyboard interrupt routine with the original */
  delete_keyboard ();
  printf ("Replaced original keyboard driver\n");

  printf ("Freeing allocated memory\n");

  new_quest ();
  editmenu_new ("!");
  guild_list.delete_list ();
  rank_list.delete_list ();
  pc_status_list.delete_list ();
  qgiven_list.delete_list ();
  reward_type_list.delete_list();
  reward_list.delete_list();
  msg_type_list.delete_list();
  find_msg_list.delete_list();
  gender_list.delete_list();

	/* Delete all section 4 lists */
  gen_loc_list.delete_list();
  loc_list.delete_list();
  sec41_list.delete_list();
  sec42_list.delete_list();
  sec43_list.delete_list();

	/* Delete all section 7 lists */
  mob_list.delete_list();
  sec71_list.delete_list();

  find_msg_buttons.destroy();

  for (i = 0; i < 11;i ++)
    section_button[i].destroy();

  main_buttons.destroy();
  qtext_buttons.destroy();
  qbn_buttons.destroy();

  printf ("     Freed all allocated memory\n");

  printf ("Initial/Final Available Memory Check: %ld / %ld bytes\n", initial_far_heapsize, farcoreleft());

  if (initial_far_heapsize - farcoreleft() != 0)
    printf ("     WARNING: Memory may not be fully freed?\n");

  printf ("farheapcheck() returns ");

  switch (farheapcheck()) {
    case _HEAPCORRUPT: printf ("Heap has been Corrupted - You Should Reboot!\n"); break;
    case _HEAPOK: printf ("Heap OK\n"); break;
    default: printf ("No heap\n"); break;
   };

  printf ("Finished clean-up of DFQEDIT v%s\n\n", VERSION);

 }
/*========== End of procedure exit_dfqedit() ==============================*/


/*========== Searches QRC Texts for a particular message type =============*/
void find_msg (const char far *message) {
  int event, i;	/* Loop counter */
  char buffer[81], buffer1[6], far *temp_ptr;	/* Temp string buffers */
  boolean do_edit = FALSE, main_redraw = TRUE, redraw = TRUE, done = FALSE;
  CAPTURE_TEXT cap_text;	/* For saving the screen background */
  int double_click_item = 0;
  clock_t double_click_time = 0;

  find_msg_list.delete_list();	/* Clear the list */

	/* Search for the message in all non-null quest texts */
  for (i = 0; i < qrc_data.num_entries; i++) {
    if (qrc_data.text[i]) {
		/* Did we find a match? Add to end of list if we did */
      if ((temp_ptr = _fstrstr(qrc_data.text[i], message)) != NULL ) {
	if ((temp_ptr[-1] == '_' || temp_ptr[-1] == '=') &&
	     temp_ptr[_fstrlen(message)] == '_') {
	  sprintf (buffer, "Text: %-20s (%4u)", get_qtext_desc(qrc_data.header[i].tag), qrc_data.header[i].type);
	  sprintf (buffer1, "%4u", i);
	  find_msg_list.add_dual_item (buffer, buffer1);
	 }
       }
     }
   } /* End of for loop */

  find_msg_list.view = find_msg_list.start;

  if (find_msg_list.view) {
    find_msg_list.active = TRUE;
    find_msg_list.current = 0;
   }
  else {
    find_msg_list.active = FALSE;
    find_msg_list.current = -1;
   }

  do {
    if (main_redraw) {
      hide_mouse();
      get_screen (FIND_MSG_WIDTH + 8, FIND_MSG_HEIGHT + 10, &cap_text);

      textcolor (BIGWIN_BORD_COLOR);
      textbackground (BIGWIN_BACK_COLOR);

      draw_textbox_center (FIND_MSG_WIDTH + 8, FIND_MSG_HEIGHT + 10, "Find Message", BIGWIN_TEXT_COLOR);
      find_msg_buttons.draw ();
      textcolor (BLUE);
      cprintf_cent (FIND_MSG_Y - get_screenheight()/2 - 2, "Finding message '%s'", message);
      show_mouse();
     }

    if (redraw || main_redraw) {
      redraw = FALSE;
      main_redraw = FALSE;
      find_msg_list.draw();
      _setcursortype (_NOCURSOR);
     }
	/* Check for any mouse events */
    event = get_mouse_event (LEFT_BUTTON) | get_mouse_event (RIGHT_BUTTON);

    if ((i = find_msg_buttons.check (MOUSEX, MOUSEY, event) ) != -1) {

      switch (i) {
	case 0: /* Edit Button Was Pressed */
		do_edit = TRUE;
		break;
	case 1: /*  The Cancel button Pressed */
		done = TRUE;
		break;
       } /* End of switch */
     } /*  End of if buttons pressed */
    else if (event == BUTTON_RELEASE && (i = find_msg_list.check (MOUSEX, MOUSEY, event)) != NO_ITEM && i != SCROLL_BAR) {
      if (   find_msg_list.active && find_msg_list.current + i == double_click_item
	  && clock() - double_click_time <= DOUBLE_CLICK_TIME) {
	do_edit = TRUE;
	double_click_time = 0;
       }
      else {
	double_click_time = clock();
	double_click_item = find_msg_list.current + i;
       }

      find_msg_list.move (i);
      redraw = TRUE;
     } /* End of if button click in list box */

	/* Get Keyboard input */
    GetKey();

    if (get_esc_state()) { /* ESC */
      done = TRUE;
     }
    else if (get_enter_state()) { /* Enter */
      do_edit = TRUE;
     }
    else if (get_up_state()) { /* Up */
      if (find_msg_list.move(-1)) redraw = TRUE;
      delay (2*KEY_DELAY);
     }
    else if (get_down_state()) { /* Down */
      if (find_msg_list.move(1)) redraw = TRUE;
      delay (2*KEY_DELAY);
     }
    else if (get_pgup_state()) { /* PgUp */
      if (find_msg_list.move(find_msg_list.height)) redraw = TRUE;
      delay (2*KEY_DELAY);
     }
    else if (get_pgdn_state()) { /* PgDn */
      if (find_msg_list.move(find_msg_list.height)) redraw = TRUE;
      delay (2*KEY_DELAY);
     }
    else if (get_home_state()) { /* Home */
      find_msg_list.view = find_msg_list.start;
      find_msg_list.current = 0;
      redraw = TRUE;
      delay (KEY_DELAY);
     }
    else if (get_end_state()) { /* End */
      find_msg_list.view = find_msg_list.end;
      find_msg_list.current = 0;
      redraw = TRUE;
      delay (KEY_DELAY);
     }

    if (do_edit && find_msg_list.start) {
      do_edit = FALSE;
      delete cap_text.save_screen;
      find_msg_list.get_name (NULL);
      i = get_ext_int (find_msg_list.list);

      if (quest_texts.texts[i].active)
	convert_qtext (qrc_data.text[i]);
      else {
	delete_all_editlines();
	init_edit();
       }

      sprintf (buffer, "Edit %s (%d)", get_qtext_desc (qrc_data.header[i].tag), qrc_data.header[i].type);

      text_editor (buffer);
      quest_texts.texts[i].active = convert_edit_list(&qrc_data.text[i]);
      draw_screen();
      main_redraw = TRUE;
     }

  } while (!done);

  hide_mouse();
  put_screen (&cap_text);
  show_mouse();
  find_msg_list.delete_list();
 }
/*========== End of procedure find_msg() ==================================*/


/*========== Returns the Quest text's Description =========================*/
char *get_qtext_desc (unsigned int type) {
  int i = 0;	/* loop counter */

  while (strlen(qtext_desc[i].desc) && i < 100) {
    if (qtext_desc[i].type == type) return (qtext_desc[i].desc);
    i++;
   }

  return ("Unknown");
 }
/*========== End of Function get_qtext_desc() =============================*/


/*========== Takes values in Drag-Lists and tranfers them to the QBN_DATA =*/
void get_qbn_data (void) {
  int i = current_section[qbn_mode] - 1;

  if (qbn_mode == QBN_ITEMS) {	/* Item mode */
    if (i >= 0) {
      qbn_data.section0[i].reward_type = (unsigned int) section_drag[0].buttons[REWARD_TYPE_INDEX].number;
      qbn_data.section0[i].reward = section_drag[0].buttons[REWARD_INDEX].number;
      qbn_data.section0[i].message_type = section_drag[0].buttons[MSG_TYPE_INDEX[0]].number;
      qbn_data.section0[i].message_id1 = (unsigned) strtol(section_field[0].fields[MSGID1_INDEX[0]].value, NULL, 0);
      qbn_data.section0[i].message_id2 = (unsigned) strtol(section_field[0].fields[MSGID2_INDEX[0]].value, NULL, 0);
     }

   } /* End of items mode */
  else if (qbn_mode == QBN_NPCS) {	/* NPC mode */
    if (i >= 0) {
      qbn_data.section3[i].gender = (unsigned int) section_drag[3].buttons[GENDER_INDEX].number;
      qbn_data.section3[i].message_type = section_drag[3].buttons[MSG_TYPE_INDEX[3]].number;
      qbn_data.section3[i].message_id1 = (unsigned) strtol(section_field[3].fields[MSGID1_INDEX[3]].value, NULL, 0);
      qbn_data.section3[i].message_id2 = (unsigned) strtol(section_field[3].fields[MSGID2_INDEX[3]].value, NULL, 0);
      qbn_data.section3[i].val1 = (unsigned char) strtol(section_field[3].fields[NUMBER1_INDEX[3]].value, NULL, 0);
      qbn_data.section3[i].val2 = (unsigned) strtol(section_field[3].fields[NUMBER2_INDEX[3]].value, NULL, 0);
      qbn_data.section3[i].val3 = (unsigned) strtol(section_field[3].fields[NUMBER3_INDEX[3]].value, NULL, 0);
     }
   } /* End of items mode */
  else if (qbn_mode == QBN_LOC) { 	/* Location mode */
    if (i >= 0) {
      qbn_data.section4[i].gen_location = (unsigned char) section_drag[4].buttons[GEN_LOC_INDEX].number;
      qbn_data.section4[i].location = (unsigned) section_drag[4].buttons[LOC_INDEX].number;

      qbn_data.section4[i].val1 = (unsigned) section_drag[4].buttons[NUMBER1_INDEX[4]].number;
      qbn_data.section4[i].val2 = (unsigned) section_drag[4].buttons[NUMBER2_INDEX[4]].number;
      qbn_data.section4[i].val3 = (unsigned char) section_drag[4].buttons[NUMBER3_INDEX[4]].number;
      qbn_data.section4[i].val4 = (unsigned char) strtol(section_field[4].fields[NUMBER4_INDEX[4]].value, NULL, 0);

      qbn_data.section4[i].message_type = (unsigned) section_drag[4].buttons[MSG_TYPE_INDEX[4]].number;
      qbn_data.section4[i].message_id1 = (unsigned) strtol(section_field[4].fields[MSGID1_INDEX[4]].value, NULL, 0);
      qbn_data.section4[i].message_id2 = (unsigned) strtol(section_field[4].fields[MSGID2_INDEX[4]].value, NULL, 0);
     }
   } /* End of location mode */
  else if (qbn_mode == QBN_MOB) {
    if (i >= 0) {
      qbn_data.section7[i].mob_index = (unsigned char) section_drag[7].buttons[MOB_INDEX].number;
      qbn_data.section7[i].val1 = (unsigned) section_drag[7].buttons[NUMBER1_INDEX[7]].number;
      qbn_data.section7[i].message_type = (unsigned long) section_drag[7].buttons[MSG_TYPE_INDEX[7]].number;
     }
   } /* End of mob mode */
 }
/*========== End of procedure get_qbn_data() ==============================*/


/*========== Initializes everything =======================================*/
void init_dfqedit(void) {
  char ch;   	/* Temp character buffer */
  int i;      	/* Loop counter */

  _wscroll = 0;		/* No scrolling of text on screen */
  qrc_screen = TRUE;
  qbn_mode = 0;
  initial_far_heapsize = farcoreleft();	/* Save initial available memory */
  atexit (exit_dfqedit);	/* Define exit procedure */
  max_string_length = MAX_STRING_LENGTH;	/* Set the maximum string length */
  init_buttons ();	/* Intialize the Default Buttons in UTIL.CPP */
  err_header_string = &dfqedit_title[0];
  err_header_version = &dfqedit_version[0];

	/* Initialize the QBN section selection */
  for (i = 0 ; i < 10; i++) {
    current_section[i] = 0;
   }

	/* Print opening title.... */
  printf ("DFQEDIT v%s - Copyright (c) 1996 - by Dave Humphrey\n\n", VERSION);

	/* Check to see if program has been run before... */
  if (fileexists("file_id.diz")) {
    printf ("Since this is the first time you have run this version of DFQEdit, you may wish\n");
    printf ("to read the DFQEdit Manual First. It contains a lot of important information\n");
    printf ("about this program.    This message will not be displayed again.\n");
    printf ("Do you wish to read DFQEDIT.TXT now? [y/n]?");

    do {
      ch = toupper(getch());
    } while (ch != 'Y' && ch != 'N');

    unlink ("file_id.diz");	/* Delete the temp file */

    if (ch == 'Y') { //Read DFQEDIT.TXT
      if (system ("edit dfqedit.txt") != 0)
	bug ("Couldn't Spawn EDIT dfqedit.txt!", NULL);
     }

    printf ("\n\n");
   }

	/* Save the current directory and drive...for reseting at end and
	 * goto intial directory */
  if (getcwd (&orig_directory[0], MAX_STRING_LENGTH) == NULL)
    printf ("ERROR: Path too Long, Path length cannot exceed 80 bytes!\n\n");

  orig_drive = getdisk();

	/* Replace some DOS error handling routines with our own */
  harderr (dos_err_handler);
  ctrlbrk (ctrl_brk_handler);
  setcbrk(1);
  printf ("Replaced DOS error handler and CTRL-BRK routines\r\n");

	/* Initialize the Mouse Driver */
  printf ("Initialize Mouse Driver\r\n");
  if (!mouse_init()) bug ("Could not intialize mouse driver!", NULL);
  printf ("    Mouse Driver Initialized\r\n");

	/* Replace keyboard interrupt with our own routine */
  printf ("Replace default keyboard interrupt\r\n");
  install_keyboard ();
  ClearKeys();
  printf ("     Keyboard interrupt replaced\n");

	/* Initialize some buttons */
  section_button[0].new_button (REWARD_X + 24, REWARD_Y + 2, REWARD_Y/2 + 2, 10, 1, "Find Msg", BLACK, BLACK);
  section_button[0].new_button (MSG_X + 14, MSG_Y, MSG_Y/2, 10, 1, "View Msg", BLACK, BLACK);
  section_button[0].new_button (MSG_X + 14, MSG_Y + 3, MSG_Y/2 + 4, 10, 1, "View Msg", BLACK, BLACK);

  section_button[3].new_button (GENDER_X + 24, GENDER_Y + 2, GENDER_Y/2 + 2, 10, 1, "Find Msg", BLACK, BLACK);
  section_button[3].new_button (MSG_X + 14, MSG_Y, MSG_Y/2 + 2, 10, 1, "View Msg", BLACK, BLACK);
  section_button[3].new_button (MSG_X + 14, MSG_Y + 3, MSG_Y/2 + 2, 10, 1, "View Msg", BLACK, BLACK);

  section_button[4].new_button (LOC_X + 24, LOC_Y + 4, GENDER_Y/2 + 2, 10, 1, "Find Msg", BLACK, BLACK);
  section_button[4].new_button (MSG_X + 14, MSG_Y, MSG_Y/2 + 2, 10, 1, "View Msg", BLACK, BLACK);
  section_button[4].new_button (MSG_X + 14, MSG_Y + 3, MSG_Y/2 + 2, 10, 1, "View Msg", BLACK, BLACK);

  section_button[7].new_button (REWARD_X + 24, REWARD_Y + 2, REWARD_Y/2 + 2, 10, 1, "Find Msg", BLACK, BLACK);

  find_msg_buttons.new_button (FIND_MSG_X + 3, FIND_MSG_Y + FIND_MSG_HEIGHT + 1, 5, 8, 3, "Edit");
  find_msg_buttons.new_button (FIND_MSG_X + 22, FIND_MSG_Y + FIND_MSG_HEIGHT + 1, 5, 8, 3, "Cancel");
  qtext_buttons.new_button (40-QTEXT_WIDTH/2 +  3, 25-QTEXT_HEIGHT/2 + 4, (25-QTEXT_HEIGHT)/2 + 4, 13, 3, "Edit Text");
  qtext_buttons.new_button (40-QTEXT_WIDTH/2 + 20, 25-QTEXT_HEIGHT/2 + 4, (25-QTEXT_HEIGHT)/2 + 4, 15, 3, "Delete Text");
  qtext_buttons.new_button (40-QTEXT_WIDTH/2 + 41, 25-QTEXT_HEIGHT/2 + 4, (25-QTEXT_HEIGHT)/2 + 4, 10, 3, "Cancel");
  qbn_buttons.new_button (QBN_BOX_X + 15, QBN_BOX_Y + QBN_BOX_HEIGHT - 4, 20, 8, 3, "Prev");
  qbn_buttons.new_button (QBN_BOX_X + 27, QBN_BOX_Y + QBN_BOX_HEIGHT - 4, 20, 8, 3, "Add");
  qbn_buttons.new_button (QBN_BOX_X + 39, QBN_BOX_Y + QBN_BOX_HEIGHT - 4, 20, 8, 3, "Next");
  qbn_buttons.new_button (4, 10, 5, 10, 3, "Items");
  qbn_buttons.new_button (4, 15, 5, 10, 3, "NPC's");
  qbn_buttons.new_button (4, 20, 5, 10, 3, "Location");
  qbn_buttons.new_button (4, 25, 5, 10, 3, "Monsters");
  main_buttons.new_button ( 3, 46, 22,  8, 3, "Load" );
  main_buttons.new_button (15, 46, 22,  8, 3, "Save" );
  main_buttons.new_button (43, 46, 22,  8, 3, "New" );
  main_buttons.new_button (55, 46, 22,  9, 3, "About" );
  main_buttons.new_button (68, 46, 22,  8, 3, "Exit" );
  main_buttons.new_button (27, 46, 22, 12, 3, "QBN Data" );

	/* Create some lists of values */
  qgiven_list.add_dual_end ("in Person", "Y");
  qgiven_list.add_dual_end ("by Letter", "L");
  qgiven_list.add_dual_end ("Unknown", "0");

  gender_list.add_dual_end ("Random? - 0xFF00", "0xFF00");
  gender_list.add_dual_end ("Male? - 0x0000", "0x0000");
  gender_list.add_dual_end ("Female? - 0x0100", "0x0100");

  pc_status_list.add_dual_end ("is Member", "B");
  pc_status_list.add_dual_end ("is Not Member", "C");
  pc_status_list.add_dual_end ("is Joining", "A");
  pc_status_list.add_dual_end ("Unknown", "0");

	/* Define the rank list */
  rank_list.add_dual_end ("0", "\0");
  rank_list.add_dual_end ("1", "\1");
  rank_list.add_dual_end ("2", "\2");
  rank_list.add_dual_end ("3", "\3");
  rank_list.add_dual_end ("4", "\4");
  rank_list.add_dual_end ("5", "\5");
  rank_list.add_dual_end ("6", "\6");
  rank_list.add_dual_end ("7", "\7");
  rank_list.add_dual_end ("8", "\8");
  rank_list.add_dual_end ("9", "\9");

	/* Section 0 Lists */
  reward_type_list.add_dual_end ("Item?", "00");
  reward_type_list.add_dual_end ("Item", "256");
  reward_type_list.add_dual_end ("Gold", "512");
  reward_type_list.add_dual_end ("Unknown", "768");

	/* Some Section 4 Lists */
  gen_loc_list.add_dual_end ("Unknown", "00");
  gen_loc_list.add_dual_end ("Current Town", "01");
  gen_loc_list.add_dual_end ("Outside Town", "02");

  if (load_dual_list ("loc.dat", &loc_list)) {
    printf ("Location data types successfully loaded\n");
   }
  else {
    printf ("Warning: Could not load Location data types!\n");
    getch ();
   }

  if (load_dual_list ("val41.dat", &sec41_list)) {
    printf ("Section 4, Val1 data types successfully loaded\n");
   }
  else {
    printf ("Warning: Could not load Section 4, Val1 data types!\n");
    getch ();
   }

  if (load_dual_list ("val42.dat", &sec42_list)) {
    printf ("Section 4, Val2 data types successfully loaded\n");
   }
  else {
    printf ("Warning: Could not load Section 4, Val2 data types!\n");
    getch ();
   }

  if (load_dual_list ("val43.dat", &sec43_list)) {
    printf ("Section 4, Val3 data types successfully loaded\n");
   }
  else {
    printf ("Warning: Could not load Section 4, Val3 data types!\n");
    getch ();
   }

	/* Section 7 Lists */
  if (load_dual_list ("mobs.dat", &mob_list)) {
    printf ("Mob data types successfully loaded\n");
   }
  else {
    printf ("Warning: Could not load Mob data types!\n");
    getch ();
   }

  if (load_dual_list ("val71.dat", &sec71_list)) {
    printf ("Section 7, Val1 data types successfully loaded\n");
   }
  else {
    printf ("Warning: Could not load Section 7, Val1 data types!\n");
    getch ();
   }

	/* Load the guild names data file */
  if (load_guilds ("guilds.dat", &guild_list)) {
    printf ("Guild data types succesfully loaded\n");
   }
  else {
    printf ("Warning: Could not load Guild data types!\n");
    getch();
   }

  guild_list.add_dual_end ("Unknown", "~");	/* Make sure there's at least one element in list */

	/* Load the item names data file */
  if (load_dual_list("items.dat", &reward_list)) {
    printf ("Item data file successfully loaded\n");
   }
  else {
    printf ("Warning: Could not load Item data file 'items.dat'!\n");
    getch();
   }

  reward_list.add_dual_end ("Unknown", "0");

	/* Load the msg_type data file */
  if (load_msg("msg.dat")) {
    printf ("Message name data file successfully loaded\n");
   }
  else {
    printf ("Warning: Could not load Message name data file 'msg.dat'!\n");
    getch();
   }

  msg_type_list.add_dual_end ("Unknown", "0");

	/* Create the Drag Button Array for the QRC screen */
  GUILD_INDEX = drag_buttons.add_button (&guild_list, "Quest Guild:");
  RANK_INDEX = drag_buttons.add_button (&rank_list, "Minimum Rank:");
  PC_STATUS_INDEX = drag_buttons.add_button (&pc_status_list, "PC Status:");
  QGIVEN_INDEX = drag_buttons.add_button (&qgiven_list, "Quest is Given:");

	/* Section 0 Drag Buttons */
  REWARD_TYPE_INDEX = section_drag[0].add_button (&reward_type_list, "Item Type:");
  REWARD_INDEX = section_drag[0].add_button (&reward_list, "Item:");
  MSG_TYPE_INDEX[0] = section_drag[0].add_button (&msg_type_list, "Message Type:");
  MSGID1_INDEX[0] = section_field[0].add_text (MSG_X, MSG_Y, MSG_WIDTH, "Message ID (int):");
  MSGID2_INDEX[0] = section_field[0].add_text (MSG_X, MSG_Y + 3, MSG_WIDTH, "Letter ID (int):");

	/* Section 3 Drag Buttons */
  MSGID1_INDEX[3] = section_field[3].add_text (MSG_X, MSG_Y, MSG_WIDTH, "Message ID1 (int):");
  MSGID2_INDEX[3] = section_field[3].add_text (MSG_X, MSG_Y + 3, MSG_WIDTH, "Message ID2 (int):");
  NUMBER1_INDEX[3] = section_field[3].add_text (MSG_X, MSG_Y + 6, 5, "Unknown #1 (char):");
  NUMBER2_INDEX[3] = section_field[3].add_text (MSG_X, MSG_Y + 8, 8, "Unknown #2 (int):");
  NUMBER3_INDEX[3] = section_field[3].add_text (MSG_X, MSG_Y + 10, 8, "Unknown #3 (int):");
  GENDER_INDEX = section_drag[3].add_button (&gender_list, "Gender?:");
  MSG_TYPE_INDEX[3] = section_drag[3].add_button (&msg_type_list, "Message Type:");

	/* Section 4 Drag Buttons */
  GEN_LOC_INDEX = section_drag[4].add_button (&gen_loc_list, "Location #1:");
  LOC_INDEX = section_drag[4].add_button (&loc_list, "Location #2:");
  NUMBER1_INDEX[4] = section_drag[4].add_button (&sec41_list, "Unknown #1 (int):");
  NUMBER2_INDEX[4] = section_drag[4].add_button (&sec42_list, "Unknown #2 (int):");
  NUMBER3_INDEX[4] = section_drag[4].add_button (&sec43_list, "Unknown #3 (int):");
  NUMBER4_INDEX[4] = section_field[4].add_text (LOC_X, LOC_Y + 21, 6, "Unknown #4 (char):");
  MSG_TYPE_INDEX[4] = section_drag[4].add_button (&msg_type_list, "Message Type:");
  MSGID1_INDEX[4] = section_field[4].add_text (MSG_X, MSG_Y, MSG_WIDTH, "Message ID1 (int):");
  MSGID2_INDEX[4] = section_field[4].add_text (MSG_X, MSG_Y + 3, MSG_WIDTH, "Message ID2 (int):");

	/* Section 7 Drag Buttons */
  NUMBER1_INDEX[7] = section_drag[7].add_button (&sec71_list, "Unknown #1 (int):");
  MOB_INDEX = section_drag[7].add_button (&mob_list, "Monster:");
  MSG_TYPE_INDEX[7] = section_drag[7].add_button (&msg_type_list, "Message Type:");

	/* Initialize the guild values */
  drag_buttons.init_values();
  section_drag[0].init_values();
  section_drag[3].init_values();
  section_drag[4].init_values();
  section_drag[7].init_values();

	/* Goto 80x50 and Show mouse cursor */
  printf ("Going to textmode (80x50) and showing mouse cursor\r\n");
  textmode(64);
  show_mouse ();
 }
/*========== End of procedure init_dfqedit() ==============================*/


/*========== Loads a Data file for a Dual List Values =====================*/
boolean load_dual_list (const char *filename, LIST_BOX *list) {
  FILE *f;	/* File pointer */
  boolean done = FALSE;	/* Loop counter */
  char l[81], *buf, *temp_ptr;	/* Temp input buffers */

	/* Try and open file... */
  if (!(f = fopen (filename, "rt")) ) {
    printf ("\n    Error: Couldn't open file '%s'!\n", filename);
    return(FALSE);
   }

	/* Attempt to allocate temp input buffer */
  buf = create_ptr (MAX_STRING_LENGTH + 1);

  while (!done) {

	/* Read number */
    if (read_word (f, buf) == READ_EOF)
      done = TRUE;

    sprintf (l, "0x%lx", strtoul (buf, NULL, 0));

    if (!done) {	/* Read description */
      if (read_eol (f, buf, MAX_STRING_LENGTH) == READ_EOF)
	done = TRUE;

      if (strlen(buf)) {
	temp_ptr = lstrip (buf);
	rstrip (temp_ptr);
	list->add_dual_sort (temp_ptr, l);
       }
     }
   }; /* End of while loop */

  fclose (f);	/* Close file */
  delete buf;
  list->view = list->start;
  return (TRUE);
 }
/*========== End of function load_dual_list() =============================*/


/*========== Loads the Guild Data file for a Dual List Values =============*/
boolean load_guilds (const char *filename, LIST_BOX *list) {
  FILE *f;	/* File pointer */
  boolean done = FALSE;	/* Loop counter */
  char l[81], *buf, *temp_ptr;	/* Temp input buffers */

	/* Try and open file... */
  if (!(f = fopen (filename, "rt")) ) {
    printf ("\n    Error: Couldn't open file '%s'!\n", filename);
    return(FALSE);
   }

	/* Attempt to allocate temp input buffer */
  buf = create_ptr (MAX_STRING_LENGTH + 1);

  while (!done) {

	/* Read number */
    if (read_word (f, buf) == READ_EOF)
      done = TRUE;

    sprintf (l, "%d", *buf);

    if (!done) {	/* Read description */
      if (read_eol (f, buf, MAX_STRING_LENGTH) == READ_EOF)
	done = TRUE;

      if (strlen(buf)) {
	temp_ptr = lstrip (buf);
	rstrip (temp_ptr);
	list->add_dual_sort (temp_ptr, l);
       }
     }
   }; /* End of while loop */

  fclose (f);	/* Close file */
  delete buf;
  list->view = list->start;
  return (TRUE);
 }
/*========== End of function load_guilds() ================================*/


/*========== Loads the Message Data file ==================================*/
boolean load_msg (const char *filename) {
  FILE *f;	/* File pointer */
  boolean done = FALSE;	/* Loop counter */
  char l[12], *buf, *temp_ptr;	/* Temp input buffers */

	/* Try and open file... */
  if (!(f = fopen (filename, "rt")) ) return(FALSE);

	/* Attempt to allocate temp input buffer */
  buf = create_ptr (MAX_STRING_LENGTH + 1);

  while (!done) {
	/* Read line */
    done = (boolean) !read_eol (f, buf);

	/* Remove trailing/leading spaces...if any */
    temp_ptr = lstrip(buf);
    rstrip(temp_ptr);

	/* Create the hash numerical value */
    sprintf (l, "0x%lx", msg_hash(temp_ptr));

	/* Add to list */
    msg_type_list.add_dual_sort (temp_ptr, l);
   }; /* End of while loop */

  fclose (f);	/* Close file */
  delete buf;
  return (TRUE);
 }
/*========== End of function load_msg() ===================================*/


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


/*========== Deletes all current quest texts ==============================*/
void new_quest(void) {
  int i = 0;

  quest_loaded = FALSE;
  qrc_data.num_entries = 0;
  qrc_data.delete_texts ();
  qbn_data.delete_data ();
  drag_buttons.init_values();

  for (i = 0; i < MAX_QRC_ARRAY; i++) {
    quest_texts.texts[i].visible = quest_texts.texts[i].active = FALSE;
   }

 }
/*========== End of procedure new_quest() =================================*/


/*========== Translates the QBN data into list values =====================*/
void parse_qbn_data (void) {
  int i;

  i = current_section[qbn_mode] - 1;

  if (qbn_mode == QBN_ITEMS) {	/* Item mode */
    if (i >= 0) {
      section_drag[0].buttons[REWARD_TYPE_INDEX].find_ext_int (qbn_data.section0[i].reward_type);
      section_drag[0].buttons[REWARD_INDEX].find_ext_long (qbn_data.section0[i].reward);
      section_drag[0].buttons[MSG_TYPE_INDEX[0]].find_ext_long (qbn_data.section0[i].message_type);
      section_field[0].fields[MSGID1_INDEX[0]].number = qbn_data.section0[i].message_id1;
      sprintf (section_field[0].fields[MSGID1_INDEX[0]].value, "%u", qbn_data.section0[i].message_id1);
      section_field[0].fields[MSGID2_INDEX[0]].number = qbn_data.section0[i].message_id2;
      sprintf (section_field[0].fields[MSGID2_INDEX[0]].value, "%u", qbn_data.section0[i].message_id2);
     }
   }
  else if (qbn_mode == QBN_NPCS) {	/* NPC mode */
    if (i >= 0) {
      section_drag[3].buttons[GENDER_INDEX].find_ext_int (qbn_data.section3[i].gender);
      section_drag[3].buttons[MSG_TYPE_INDEX[3]].find_ext_long (qbn_data.section3[i].message_type);
      section_field[3].fields[MSGID1_INDEX[3]].number = qbn_data.section3[i].message_id1;
      sprintf (section_field[3].fields[MSGID1_INDEX[3]].value, "%u", qbn_data.section3[i].message_id1);
      section_field[3].fields[MSGID2_INDEX[3]].number = qbn_data.section3[i].message_id2;
      sprintf (section_field[3].fields[MSGID2_INDEX[3]].value, "%u", qbn_data.section3[i].message_id2);

      section_field[3].fields[NUMBER1_INDEX[3]].number = qbn_data.section3[i].val1;
      sprintf (section_field[3].fields[NUMBER1_INDEX[3]].value, "%u", qbn_data.section3[i].val1);
      section_field[3].fields[NUMBER2_INDEX[3]].number = qbn_data.section3[i].val2;
      sprintf (section_field[3].fields[NUMBER2_INDEX[3]].value, "%u", qbn_data.section3[i].val2);
      section_field[3].fields[NUMBER3_INDEX[3]].number = qbn_data.section3[i].val3;
      sprintf (section_field[3].fields[NUMBER3_INDEX[3]].value, "%u", qbn_data.section3[i].val3);
     }
   }
  else if (qbn_mode == QBN_LOC) { 	/* Location mode */
    if (i >= 0) {
      section_drag[4].buttons[GEN_LOC_INDEX].find_ext_int (qbn_data.section4[i].gen_location);
      section_drag[4].buttons[LOC_INDEX].find_ext_int (qbn_data.section4[i].location);
      section_drag[4].buttons[NUMBER1_INDEX[4]].find_ext_int (qbn_data.section4[i].val1);
      section_drag[4].buttons[NUMBER2_INDEX[4]].find_ext_int (qbn_data.section4[i].val2);
      section_drag[4].buttons[NUMBER3_INDEX[4]].find_ext_int (qbn_data.section4[i].val3);
      section_drag[4].buttons[MSG_TYPE_INDEX[4]].find_ext_long (qbn_data.section4[i].message_type);

      section_field[4].fields[NUMBER4_INDEX[4]].number = qbn_data.section4[i].val4;
      sprintf (section_field[4].fields[NUMBER4_INDEX[4]].value, "%u", qbn_data.section4[i].val4);

      section_field[4].fields[MSGID1_INDEX[4]].number = qbn_data.section4[i].message_id1;
      sprintf (section_field[4].fields[MSGID1_INDEX[4]].value, "%u", qbn_data.section4[i].message_id1);
      section_field[4].fields[MSGID2_INDEX[4]].number = qbn_data.section4[i].message_id2;
      sprintf (section_field[4].fields[MSGID2_INDEX[4]].value, "%u", qbn_data.section4[i].message_id2);
     }
   }
  else if (qbn_mode == QBN_MOB) {	/* Monster mode */
    if (i >= 0) {
      section_drag[7].buttons[MOB_INDEX].find_ext_int (qbn_data.section7[i].mob_index);
      section_drag[7].buttons[NUMBER1_INDEX[7]].find_ext_int (qbn_data.section7[i].val1);
      section_drag[7].buttons[MSG_TYPE_INDEX[7]].find_ext_long (qbn_data.section7[i].message_type);
     }
   }
 }
/*========== End of procedure parse_qbn_data() ============================*/


/*========== Translates a Quest filename into Screen List Values ==========*/
void parse_filename (const char far *filename) {
  drag_buttons.buttons[GUILD_INDEX].find_ext_char (toupper(*filename));

		/* These quests don't conform to known standards... */
  if (!filename || *filename == 'S' || *filename == '$' || *filename == '_' || strlen(filename) < 3) {
    drag_buttons.buttons[RANK_INDEX].number = '\0';
    strcpy (drag_buttons.buttons[RANK_INDEX].value, "0");
    drag_buttons.buttons[PC_STATUS_INDEX].number = '0';
    strcpy (drag_buttons.buttons[PC_STATUS_INDEX].value, "<none>");
    drag_buttons.buttons[QGIVEN_INDEX].number = '0';
    strcpy (drag_buttons.buttons[QGIVEN_INDEX].value, "<none>");
    return;
   }

  filename += 2;	/* Move to 3rd character */
  drag_buttons.buttons[PC_STATUS_INDEX].find_ext_char (toupper(*filename));

  if (_fstrlen(filename) > 3) {
    filename++;		/* Move to 4th character */
    drag_buttons.buttons[RANK_INDEX].find_ext_char (*filename - '0');

    if (_fstrlen(filename) > 5) {
      filename += 2;	/* Move to 6th character */
      drag_buttons.buttons[QGIVEN_INDEX].find_ext_char (toupper(*filename));
     }
   }
 }
/*========== End of procedure parse_filename() ============================*/


/*========== Removes any QRC/QBN extension on the end of inputed Filenames */
void parse_quest_filename(char far *filename) {
  char far *temp_ptr;

  _fstrupr(filename);

  if ((temp_ptr = _fstrstr(filename, ".QRC")) != NULL) {
    *temp_ptr = 0;  /* Simply terminate string at found location */
   }
  else if ((temp_ptr = _fstrstr(filename, ".QBN")) != NULL) {
    *temp_ptr = 0;  /* Simply terminate string at found location */
   }
}
/*========== End of procedure parse_quest_filename() ======================*/


/*========== Class QBN_DATA Constructor ===================================*/
void QBN_DATA::QBN_DATA (void) {
  int i;    	/* Loop counter */

  for (i = 0; i < 10; i++) {
    section_size[i] = 0;
    section_offset[i] = -1;
    header[i] = 0;
   }

  for (; i < 16; i++)
    header[i] = 0;

  for (i = 0; i < MAX_QBN1_ARRAY; i++)
    section1[i].data = NULL;

  for (i = 0; i < MAX_QBN2_ARRAY; i++)
    section2[i].data = NULL;

  for (i = 0; i < MAX_QBN5_ARRAY; i++)
    section5[i].data = NULL;

  for (i = 0; i < MAX_QBN6_ARRAY; i++)
    section6[i].data = NULL;

  for (i = 0; i < MAX_QBN8_ARRAY; i++)
    section8[i].data = NULL;

  for (i = 0; i < MAX_QBN9_ARRAY; i++)
    section9[i].data = NULL;

  for (i = 0; i < MAX_QBN10_ARRAY; i++)
    section10[i].data = NULL;

 }
//========== End of Class QBN_DATA Constructor ==============================


//========== Class QBN_DATA Destructor ======================================
void QBN_DATA::~QBN_DATA (void) {
  delete_data ();
 }
//========== End of Class QBN_DATA Destructor ===============================


//========== Deletes All Data in the QBN_DATA Class =========================
void QBN_DATA::delete_data (void) {
  int i;	/* Loop counter */

	/* Delete the Section 1 Data */
  for (i = 0; i < MAX_QBN1_ARRAY; i++) {
    if (section1[i].data) {
      delete section1[i].data;
      section1[i].data = NULL;
     }
   }

	/* Delete the Section 2 Data */
  for (i = 0; i < MAX_QBN2_ARRAY; i++) {
    if (section2[i].data) {
      delete section2[i].data;
      section2[i].data = NULL;
     }
   }

	/* Delete the Section 5 Data */
  for (i = 0; i < MAX_QBN5_ARRAY; i++) {
    if (section5[i].data) {
      delete section5[i].data;
      section5[i].data = NULL;
     }
   }

	/* Delete the Section 6 Data */
  for (i = 0; i < MAX_QBN6_ARRAY; i++) {
    if (section6[i].data) {
      delete section6[i].data;
      section6[i].data = NULL;
     }
   }

	/* Delete the Section 8 Data */
  for (i = 0; i < MAX_QBN8_ARRAY; i++) {
    if (section8[i].data) {
      delete section8[i].data;
      section8[i].data = NULL;
     }
   }

	/* Delete the Section 9 Data */
  for (i = 0; i < MAX_QBN9_ARRAY; i++) {
    if (section9[i].data) {
      delete section9[i].data;
      section9[i].data = NULL;
     }
   }

	/* Delete the Section 10 Data */
  for (i = 0; i < MAX_QBN10_ARRAY; i++) {
    if (section10[i].data) {
      delete section10[i].data;
      section10[i].data = NULL;
     }
   }

 }
/*========== End of Procedure QBN_DATA::delete_data () ====================*/


/*========== Attempts to Load the QBN =====================================*/
boolean QBN_DATA::load (const char far *filename) {
  FILE *f; 	/* File Pointer */
  char *temp_file;	/* For creating the proper filename, QBN */
  long filesize;
  int i;	/* Loop counter */

	/* Allocate Temp String Buffer */
  if (!(temp_file = new char[_fstrlen(quest_path) + _fstrlen(filename) + 7]))
    bug (MEM_ERR_MSG, "load_qrc() - *temp_file (%d)", _fstrlen(quest_path) + _fstrlen(filename) + 7);

  _fstrcpy (temp_file, quest_path);

	/* Make sure there is a proper slash at end */
  if (_fstrlen(temp_file) > 0 && *(temp_file + _fstrlen(temp_file) - 1) != '\\')
    chrcat (temp_file, '\\');

  _fstrcat (temp_file, filename);
  _fstrcat (temp_file, ".QBN");

	/* Make sure the file exists before opening it */
  if (!fileexists(temp_file)) return(FALSE);

	/* Open QBN file */
  f = openfile (temp_file, "rb");
  filesize = get_filesize (f);

	/* Read the 15-Byte Header */
  if (fread (header, sizeof(char), 15, f) != 15) return (FALSE);

	/* Read the 10th Section Byte */
  section10_status = fgetc (f);

	/* Read the Section Size Info */
  for (i = 0; i < 10; i++)
    section_size[i] = read_int (f);

	/* Make sure there are no array-size limit violations */
  for (i = 0; i < 10; i++) {
    if (section_size[i] >= MAX_QBN_ARRAY[i])
      bug ("Error Loading QBN File. Array %d Size Exceeded.", "Filename = '%s' - %d of %d", i, temp_file, section_size[i], MAX_QBN_ARRAY[i]);
   }

	/* Read the Section Offset Info */
  for (i = 0; i < 11; i++)
    section_offset[i] = read_int (f);

	/* Read the Section 0 Data */
  if (fseek (f, section_offset[0] , SEEK_SET)) return (FALSE);

  for (i = 0; i < section_size[0] && i < MAX_QBN0_ARRAY; i++) {
    section0[i].section_index = fgetc (f);
    section0[i].reward_type = read_int (f);
    section0[i].rew1 = read_int (f);
    section0[i].rew2 = read_int (f);
    section0[i].reward = section0[i].rew1 + ((long)section0[i].rew2 << 16);
    section0[i].message_type = read_long (f);
    section0[i].null_bytes = read_long (f);
    section0[i].message_id1 = read_int (f);
    section0[i].message_id2 = read_int (f);
   }

	/* Read the Section 1 Data */
  if (fseek (f, section_offset[1] , SEEK_SET)) return (FALSE);

  for (i = 0; i < section_size[1] && i < MAX_QBN1_ARRAY; i++) {
    bug ("Error: No Size Defined For QBN Section #1", "Filename = '%s'   Section Size = %d", temp_file, section_size[1]);
   }

	/* Read the Section 2 Data */
  if (fseek (f, section_offset[2] , SEEK_SET)) return (FALSE);

  for (i = 0; i < section_size[2] && i < MAX_QBN2_ARRAY; i++) {
    bug ("Error: No Size Defined For QBN Section #2", "Filename = '%s'   Section Size = %d", temp_file, section_size[2]);
   }

	/* Read the Section 3 Data */
  if (fseek (f, section_offset[3] , SEEK_SET)) return (FALSE);

  for (i = 0; i < section_size[3] && i < MAX_QBN3_ARRAY; i++) {
    section3[i].section_index = fgetc(f);
    section3[i].gender = read_int(f);
    section3[i].val1 = fgetc(f);
    section3[i].val2 = read_int (f);
    section3[i].val3 = read_int (f);
    section3[i].message_type = read_long(f);
    section3[i].null_bytes = read_long(f);
    section3[i].message_id1 = read_int(f);
    section3[i].message_id2 = read_int(f);
   }

	/* Read the Section 4 Data */
  if (fseek (f, section_offset[4] , SEEK_SET)) return (FALSE);

  for (i = 0; i < section_size[4] && i < MAX_QBN4_ARRAY; i++) {
    section4[i].section_index = fgetc(f);
    section4[i].null1 = read_int(f);
    section4[i].gen_location = fgetc(f);
    section4[i].location = read_int (f);
    section4[i].val1 = read_int (f);
    section4[i].val2 = read_int (f);
    section4[i].val3 = fgetc (f);
    section4[i].val4 = fgetc (f);
    section4[i].message_type = read_long (f);
    section4[i].null2 = read_long (f);
    section4[i].message_id1 = read_int (f);
    section4[i].message_id2 = read_int (f);
   }

	/* Read the Section 5 Data */
  if (fseek (f, section_offset[5] , SEEK_SET)) return (FALSE);

  for (i = 0; i < section_size[5] && i < MAX_QBN5_ARRAY; i++) {
    bug ("Error: No Size Defined For QBN Section #5", "Filename = '%s'   Section Size = %d", temp_file, section_size[5]);
   }

	/* Read the Section 6 Data */
  if (fseek (f, section_offset[6] , SEEK_SET)) return (FALSE);

  for (i = 0; i < section_size[6] && i < MAX_QBN6_ARRAY; i++) {
    if (!(section6[i].data = new far char[33]))
      bug (MEM_ERR_MSG, "QBN_DATA::load() - section6[i].data (33)");

    if (fread (section6[i].data, sizeof (char), 33, f) != 33) return (FALSE);
   }

	/* Read the Section 7 Data */
  if (fseek (f, section_offset[7] , SEEK_SET)) return (FALSE);

  for (i = 0; i < section_size[7] && i < MAX_QBN7_ARRAY; i++) {
    section7[i].section_index = fgetc(f);
    section7[i].null1 = read_int(f);
    section7[i].mob_index = fgetc(f);
    section7[i].val1 = read_int(f);
    section7[i].message_type = read_long(f);
    section7[i].null2 = read_long (f);
   }

	/* Read the Section 8 Data */
  if (fseek (f, section_offset[8] , SEEK_SET)) return (FALSE);

  for (i = 0; i < section_size[8] && i < MAX_QBN8_ARRAY; i++) {
    if (!(section8[i].data = new far char[87]))
      bug (MEM_ERR_MSG, "QBN_DATA::load() - section8[i].data (87)");

    if (fread (section8[i].data, sizeof (char), 87, f) != 87) return (FALSE);
   }

	/* Read the Section 9 Data */
  if (fseek (f, section_offset[9] , SEEK_SET)) return (FALSE);

  for (i = 0; i < section_size[9] && i < MAX_QBN9_ARRAY; i++) {
    if (!(section9[i].data = new far char[8]))
      bug (MEM_ERR_MSG, "QBN_DATA::load() - section9[i].data (8)");

    if (fread (section9[i].data, sizeof (char), 8, f) != 8) return (FALSE);
   }

	/* Read the section 10 Data */
  if (section10_status && section_offset[10]) {
    if (fseek (f, section_offset[10], SEEK_SET)) return (FALSE);
    i = 0;

	/* Read until end of file */
    while (ftell(f) < filesize && i < MAX_QBN10_ARRAY) {
      if (!(section10[i].data = new far char[27]))
	bug (MEM_ERR_MSG, "QBN_DATA::load() - section10[i].data (27)");

      if (fread (section10[i].data, sizeof (char), 27, f) != 27) return (FALSE);
      i++;
     }

    if (i >= MAX_QBN10_ARRAY) bug ("Error Loading QBN File. Array Size Exceeded.", "Filename = '%s' - %d of %d", temp_file, i, MAX_QBN10_ARRAY);
   } /* End of if Section 10 Read */

  fclose (f);

	/* Use this data to get the current type of quest we just loaded */
  parse_filename (filename);

  delete temp_file;	/* Delete temp string buffer */
  return (TRUE);
 }
/*========== End of Function QBN_DATA::load() =============================*/


/*========== Saves The QRC-Hopefully ======================================*/
boolean QBN_DATA::save (const char far *filename) {
  FILE *f; 	/* File Pointer */
  char *temp_file;	/* For creating the proper filename, QBN */
  int i;	/* Loop counter */

	/* Allocate Temp String Buffer */
  if (!(temp_file = new char[_fstrlen(quest_path) + _fstrlen(filename) + 7]))
    bug (MEM_ERR_MSG, "load_qrc() - *temp_file (%d)", _fstrlen(quest_path) + _fstrlen(filename) + 5);

  _fstrcpy (temp_file, quest_path);

	/* Make sure there is a proper slash at end */
  if (_fstrlen(temp_file) > 0 && *(temp_file + _fstrlen(temp_file) - 1) != '\\')
    chrcat (temp_file, '\\');

  _fstrcat (temp_file, filename);
  _fstrcat (temp_file, ".QBN");

	/* Open QBN file */
  f = openfile (temp_file, "wb");

	/* Write the Header */
  fwrite (header, sizeof (char), 15, f);

	/* Write Section 10 Byte */
  fputc (section10_status, f);

	/* Write the Section Sizes */
  for (i = 0; i < 10; i++)
    write_int (f, section_size[i]);

	/* Write the Section Offsets */
  for (i = 0; i < 11; i++)
    write_int (f, section_offset[i]);

	/* Write Padding 0's */
  write_int (f, 0);

	/* Write the Section 0 Data */
  fseek (f, section_offset[0], SEEK_SET);

  for (i = 0; i < section_size[0]; i++) {
    fputc (section0[i].section_index, f);
    write_int (f, section0[i].reward_type);
    write_long (f, section0[i].reward);
    write_long (f, section0[i].message_type);
    write_long (f, section0[i].null_bytes);
    write_int (f, section0[i].message_id1);
    write_int (f, section0[i].message_id2);
   }

	/* Write the Section 1 Data */
  fseek (f, section_offset[1], SEEK_SET);

  for (i = 0; i < section_size[1]; i++) {
    fwrite (section1[i].data, sizeof (char), 1, f);
   }

	/* Write the Section 2 Data */
  fseek (f, section_offset[2], SEEK_SET);

  for (i = 0; i < section_size[2]; i++) {
    fwrite (section2[i].data, sizeof (char), 1, f);
   }

	/* Write the Section 3 Data */
  fseek (f, section_offset[3], SEEK_SET);

  for (i = 0; i < section_size[3]; i++) {
    fputc (section3[i].section_index, f);
    write_int (f, section3[i].gender);
    fputc (section3[i].val1, f);
    write_int (f, section3[i].val2);
    write_int (f, section3[i].val3);
    write_long (f, section3[i].message_type);
    write_long (f, section3[i].null_bytes);
    write_int (f, section3[i].message_id1);
    write_int (f, section3[i].message_id2);
   }

	/* Write the Section 4 Data */
  fseek (f, section_offset[4], SEEK_SET);

  for (i = 0; i < section_size[4]; i++) {
    fputc (section4[i].section_index, f);
    write_int (f, section4[i].null1);
    fputc (section4[i].gen_location , f);
    write_int (f, section4[i].location);
    write_int (f, section4[i].val1);
    write_int (f, section4[i].val2);
    fputc (section4[i].val3, f);
    fputc (section4[i].val4, f);
    write_long (f, section4[i].message_type);
    write_long (f, section4[i].null2);
    write_int (f, section4[i].message_id1);
    write_int (f, section4[i].message_id2);
   }

	/* Write the Section 5 Data */
  fseek (f, section_offset[5], SEEK_SET);

  for (i = 0; i < section_size[5]; i++) {
    fwrite (section5[i].data, sizeof (char), 1, f);
   }

	/* Write the Section 6 Data */
  fseek (f, section_offset[6], SEEK_SET);

  for (i = 0; i < section_size[6]; i++) {
    fwrite (section6[i].data, sizeof (char), 33, f);
   }

	/* Write the Section 7 Data */
  fseek (f, section_offset[7], SEEK_SET);

  for (i = 0; i < section_size[7]; i++) {
    fputc (section7[i].section_index, f);
    write_int (f, section7[i].null1);
    fputc (section7[i].mob_index, f);
    write_int (f, section7[i].val1);
    write_long (f, section7[i].message_type);
    write_long (f, section7[i].null2);
   }

	/* Write the Section 8 Data */
  fseek (f, section_offset[8], SEEK_SET);

  for (i = 0; i < section_size[8]; i++) {
    fwrite (section8[i].data, sizeof (char), 87, f);
   }

	/* Write the Section 9 Data */
  fseek (f, section_offset[9], SEEK_SET);

  for (i = 0; i < section_size[9]; i++) {
    fwrite (section9[i].data, sizeof (char), 8, f);
   }

	/* Write the section 10 Data */
  if (section10_status) {
    fseek (f, section_offset[10], SEEK_SET);
    i = 0;

	/* Read until end of file */
    while (section10[i].data) {
      fwrite (section10[i].data, sizeof (char), 27, f);
      i++;
     }
   } /* End of if Section 10 Read */

  fclose(f);	/* Close the QBN file */
  delete temp_file;
  return (TRUE);
 }
/*========== End of Function QBN_DATA::save() =============================*/


/*========== Class QRC_DATA Constructor ===================================*/
void QRC_DATA::QRC_DATA (void) {
  int i;	/* Loop Counter */

  for (i = 0; i < MAX_QRC_ARRAY; i++) {
    text[i] = NULL;
   }

  num_entries = 0;

 }
/*========== End Class QRC_DATA Constructor ===============================*/


/*========== Class QRC_DATA Destructor ====================================*/
void QRC_DATA::~QRC_DATA (void) {
  delete_texts();
 }
/*========== End Class QRC_DATA Destructor ================================*/


/*========== Deletes All Allocated Texts Loaded From QRC ==================*/
void QRC_DATA::delete_texts (void) {
  int i;	/* Loop counter */

  for (i = 0; i < MAX_QRC_ARRAY; i++) {
    if (text[i]) {
      delete text[i];
      text[i] = NULL;
     }
   }
 }
/*========== End of Procedure QRC_DATA::delete_texts() ====================*/


/*========== Attempts to load a QRC quest file into memory ================*/
boolean QRC_DATA::load (const char far *filename) {
  FILE *f;	/* File pointer */
  long int file_size;
  int header_size, i, string_length;
  char far *buf, *temp_file;	/* Temp string buffers */
  unsigned char ch;

		/* Allocate memory for temp_string */
  if (!(temp_file = new char[_fstrlen (quest_path) + _fstrlen(filename) + 7]))
    bug (MEM_ERR_MSG, "load_qrc() - *temp_file (%d)", _fstrlen (quest_path) + _fstrlen(filename) + 7);

  _fstrcpy (temp_file, quest_path);

  if (_fstrlen(temp_file) > 0 && *(temp_file + _fstrlen(temp_file) - 1) != '\\')
    chrcat (temp_file, '\\');	/* Make sure there is a proper slash at end */

  _fstrcat (temp_file, filename);
  _fstrcat (temp_file, ".QRC");

	/* Make sure the file exists before opening it */
  if (!fileexists(temp_file)) return(FALSE);

  f = openfile (temp_file, "rb");
  file_size = get_filesize(f);
  delete temp_file;

  header_size = read_int (f);
  i = 0;

	/* Read in header information */
  while (ftell(f) < header_size) { /* Make sure we don't read past the header */
		/* Read in type of text */
    header[i].tag = header[i].type = read_int (f);

		/* Read in offset to start of text from file beginning */
    header[i].offset = read_long (f);

    if (header[i].type != QTEXT_END) quest_texts.texts[i].visible = quest_texts.texts[i].active = TRUE;
    i++;
   } /* End of while reading header loop */

  num_entries = i - 1;

	/* Read in all texts in QRC File */
  for (i = 0; i < num_entries && ftell(f) < file_size; i++) {
    if (header[i].type == QTEXT_END) break;
    string_length = (unsigned int) (header[i + 1].offset - header[i].offset);

	/* Allocate the Text Data */
    if (!(text[i] = new far char[string_length + 10]))
      bug (MEM_ERR_MSG, "load_qrc() - qrc_data.texts[i] (%d)", string_length + 10);

    fseek (f, header[i].offset, SEEK_SET);	//Move to beginning of text data
    buf = text[i];		/* Makes things shorter... */
    *buf = 0;	/* Make sure string is initially nothing */

    while (ftell(f) < header[i+1].offset) { /* Read all of next text */
      ch = fgetc(f);

      if (ch < 0xFC) { /* Normal character? */
	chrcat (buf, (char)ch);
       }
      else if (ch == CR_CENTER) { /* CR, Center aligned type */
	chrcat(buf, CR_CENTER);
       }
      else if (ch == CR_LEFT) { /* CR, Left justified type */
	chrcat (buf, CR_LEFT);
       }
      else if (ch == 0xFE) { /* End of text section */
	break;
       }
      else if (ch == 0xFF) { /* Different text in section begins */
	chrcat((char *)buf, '~');
       }
     } /* End of While reading text section */
   } /* End of for loop */

  fclose(f);
  return(TRUE);
 }
/*========== End of function QRC_DATA::load() =============================*/


/*========== Attempts to save a QRC file ==================================*/
boolean QRC_DATA::save (const char far *filename) {
  FILE *f; 	/* File pointer */
  unsigned offset = 0, i, current_header = 0;
  unsigned header_size = num_entries*6 + 6;;
  char far *buf, *temp_ptr, far *temp_file;

  if (!(temp_file = new far char[_fstrlen (quest_path) + _fstrlen (filename) + 7]))
    bug (MEM_ERR_MSG, "save_qrc() - *temp_file (%d)", _fstrlen (quest_path) + _fstrlen (filename) + 7);

  _fstrcpy (temp_file, quest_path);

  if (_fstrlen(temp_file) > 0 && *(temp_file + _fstrlen(temp_file) - 1) != '\\')
    chrcat (temp_file, '\\');	/* Make sure there is a proper slash at end */

  _fstrcat (temp_file, filename);
  _fstrcat (temp_file, ".QRC");

	/* If file already exists...overwrite? */
  if (fileexists (temp_file)) {

    if (!(buf = new far char[_fstrlen (temp_file) + 20]))
      bug (MEM_ERR_MSG, "save_qrc() = *buf (%d)", _fstrlen (temp_file) + 20);

    sprintf (buf, "Overwrite file '%s'", temp_file);

    if (!get_yn_choice (_fstrlen(buf) + 17, 9, "File Exists", buf)) {
      delete buf;
      delete temp_file;
      return (FALSE);
     }

    delete buf;	/* Delete temp string buffer */
   }
	/* Attempt to open file for writing */
  f = openfile (temp_file, "wb");

	/* Write header size */
  write_int (f, header_size);
  offset = header_size + 2;

	/* Write the header and text information */
  for (i = 0; i < num_entries; i++, current_header++) {
    temp_ptr = text[i];

	/* Allocate data to hold parsed text data for QRC file */
    if (!(buf = new far char[_fstrlen(temp_ptr) + 20]))
      bug (MEM_ERR_MSG, "save_qrc() - *buf (%d)", _fstrlen(temp_ptr) + 20);

    *buf = 0;	/* Make sure string is initially empty */

	/* Parse quest text data into QRC format */
    while (*temp_ptr != 0) { /*  Parse all of quest text */
      if (*temp_ptr == '~')	/* End of part of text section */
	chrcat (buf, 0xFF);
      else	/* Normal character */
	chrcat (buf, *temp_ptr);

      temp_ptr++;
     } /* End of while loop */

    chrcat (buf, 0xFE);	/* End of text section */

	/* Write header - Goto Proper Header Location */
    fseek (f, 2 + current_header*6, SEEK_SET);
    write_int (f, header[i].type);	/* Write text type */
    write_int (f, offset);		/* Write Offset to text */
    fputc (0, f);
    fputc (0, f);

		/* Write Text - Goto Proper Header Location */
    if (header[i].type != QTEXT_END) {
      fseek (f, offset, SEEK_SET);
      fwrite (buf, sizeof(char), _fstrlen(buf), f);
      offset += _fstrlen(buf);	/* Update offset */
     }

    delete buf;
   }

	/* Write terminating header */
  fseek (f, 2 + current_header*6, SEEK_SET);
  write_int (f, 0xFFFF);
  write_int (f, offset);
  fputc (0, f);
  fputc (0, f);
  fclose(f);

  delete temp_file;
  return(TRUE);
 }
/*========== End of function QRC_DATA::save() =============================*/


/*========== Class QRC_HEADER Constructor =================================*/
void QRC_HEADER::QRC_HEADER (void) {
  offset = 0;
  type = 0;
  tag = 0;
 }
/*========== End Class QRC_HEADER Constructor =============================*/


/*========== Class QUEST_TEXT Constructor =================================*/
void QUEST_TEXT::QUEST_TEXT (void) {
  x = 1;
  y = 1;
  active = FALSE;
  visible = FALSE;
 }
/*========== End Class QUEST_TEXT Constructor =============================*/


/*========== Draws One Quest Text Entry on Screen =========================*/
void QUEST_TEXT::draw (const int index) {
  int orig_textattr;

  if (!visible) return;

  orig_textattr = get_textattr ();
  hide_mouse ();

	/* Print title of field */
  gotoxy (x, y);
  textcolor (TEXT_COLOR);
  textbackground (BACK_COLOR);

	/* Display Type and hex_type of Quest Text */
  cprintf ("%*s (%04u) ", QTEXT_TITLE_WIDTH - 8, get_qtext_desc(qrc_data.header[index].tag), qrc_data.header[index].type);

	/* Print value of field, if any */
  textcolor (WIN_TEXT_COLOR);
  textbackground (WIN_BACK_COLOR);

  if (active)
    cprintf (" %-*s ", QTEXT_VALUE_WIDTH, "Yes");
  else
    cprintf (" %-*s ", QTEXT_VALUE_WIDTH, "No");

  textattr (orig_textattr);
  show_mouse ();
 }
//========== End of procedure QUEST_TEXT::draw () ===========================


//========== Class QUEST_TEXT_ARRAY Constructor =============================
void QUEST_TEXT_ARRAY::QUEST_TEXT_ARRAY (void) {
	/* Set all the x-y coordinates for the text fields */
  add_text (15,  8);
  add_text (15, 10);
  add_text (15, 12);
  add_text (15, 14);
  add_text (15, 16);
  add_text (15, 18);
  add_text (15, 20);
  add_text (15, 22);
  add_text (15, 24);
  add_text (15, 26);
  add_text (15, 28);
  add_text (15, 30);
  add_text (15, 32);
  add_text (15, 34);
  add_text (15, 36);
  add_text (15, 38);
  add_text (15, 40);
  add_text (15, 42);
  add_text (48,  8);
  add_text (48, 10);
  add_text (48, 12);
  add_text (48, 14);
  add_text (48, 16);
  add_text (48, 18);
  add_text (48, 20);
  add_text (48, 22);
  add_text (48, 24);
  add_text (48, 26);
  add_text (48, 28);
  add_text (48, 30);
  add_text (48, 32);
  add_text (48, 34);
  add_text (48, 36);
  add_text (48, 38);
  add_text (48, 40);
  add_text (48, 42);
  new_quest ();
 }
/*========== End of Class QUEST_TEXT_ARRAY Constructor ====================*/


/*========== Adds a Quest Text to End of Array ============================*/
boolean QUEST_TEXT_ARRAY::add_text (const int x, const int y, const boolean visible) {
  int i;	/* Loop counter */

  for (i = 0; i < MAX_QRC_ARRAY && texts[i].visible; i++) {
   }

  if (i == MAX_QRC_ARRAY) return (FALSE);
  texts[i].x = x;
  texts[i].y = y;
  texts[i].visible = visible;
  texts[i].active = TRUE;
  return (TRUE);
 }
/*========== End of Function QUEST_TEXT_ARRAY::add_text() =================*/


/*========== Checks the Quest Texts for a mouse click event ===============*/
boolean QUEST_TEXT_ARRAY::check (const int mx, const int my, const int event) {
  int i;		/* Loop Counter */
  char buffer[41];	/* Temp String Buffer */

  if (event != BUTTON_RELEASE) return (FALSE);

  for (i = 0; i < MAX_QRC_ARRAY && texts[i].visible; i++) {
    if (mx >= texts[i].x && mx < texts[i].x + QTEXT_TITLE_WIDTH + QTEXT_VALUE_WIDTH + 3 &&
	my == texts[i].y) {

      sprintf (buffer, "Edit %s (%d)", get_qtext_desc (qrc_data.header[i].tag), qrc_data.header[i].type);

		/* Run the general quest edit window */
      switch (draw_qtext_window (buffer)) {
	case 0:  /* Edit text */

	    if (texts[i].active)
	      convert_qtext (qrc_data.text[i]);
	    else {
	      delete_all_editlines();
	      init_edit();
	     }

	    text_editor (buffer);
	    texts[i].active = convert_edit_list(&qrc_data.text[i]);
	    return (TRUE);
	case 1: /* Delete */
	    texts[i].active = FALSE;

	    if (qrc_data.text[i]) delete qrc_data.text[i];
	    qrc_data.text[i] = NULL;
	    return (TRUE);
       } /* End of switch */

      return (FALSE);
     }
   } /* End of for loop */

  return (FALSE); }
/*========== End of function QUEST_TEXT_ARRAY::check() ====================*/


/*========== Draws Visible Portions of the Array ==========================*/
void QUEST_TEXT_ARRAY::draw (void) {
  int i;	/* Loop counter */

  for (i = 0; i < MAX_QRC_ARRAY && texts[i].visible; i++)
    texts[i].draw (i);

 }
/*========== End of procedure QUEST_TEXT_ARRAY::draw() ====================*/


/*========== Begin Main Program ===========================================*/
boolean main(void) {
  boolean done = FALSE;		/* Loop counter */
  boolean redraw = TRUE;	/* Do we draw the screen or not? */
  int event, i;

  init_dfqedit();		/* Initialize the program */

  do {

    if (redraw) {
      draw_screen();
      show_mouse();
      redraw = FALSE;
     }

	/* Update mouse status */
    event = get_mouse_event (LEFT_BUTTON);

    if ((i = main_buttons.check (MOUSEX, MOUSEY, event)) != -1) {
      switch (i) {
	case 0: click_load();	/* Load */
		redraw = TRUE;
		break;
	case 1: click_save();	/* Save */
		redraw = TRUE;
		break;
	case 2: click_new();	/* New */
		redraw = TRUE;
		break;
	case 3: click_about();	/* About */
		redraw = TRUE;
		break;
	case 4: click_exit();	/* Exit */
		redraw = TRUE;
		break;
	case 5: if (qrc_screen) {	/* Change to QRC/QBN Data screen */
		  qrc_screen = FALSE;
		  _fstrcpy (main_buttons.buttons[5].text, "QRC Data");
		 }
		else {
		  qrc_screen = TRUE;
		  _fstrcpy (main_buttons.buttons[5].text, "QBN Data");
		 }

		redraw = TRUE;
		break;
       } /* End of switch(i) */
     } /* End of if main buttons pressed */
    else if (qrc_screen) {
      if (quest_texts.check (MOUSEX, MOUSEY, event)) {
	redraw = TRUE;
       }
      else if (drag_buttons.check (MOUSEX, MOUSEY, event)) {
	drag_buttons.draw_texts();
	draw_filenames ();
       }
     } /* End of if qrc_screen */
    else {	/* else qbn_screen */
      if ((i = qbn_buttons.check (MOUSEX, MOUSEY, event)) != -1) {
	switch (i) {
	  case 0:	/* Prev Button */
		  if (qbn_data.section_size[qbn_mode] > 1) {
		    current_section[qbn_mode]--;

		    if (current_section[qbn_mode] < 1)
		      current_section[qbn_mode] = qbn_data.section_size[qbn_mode];

		    draw_qbn_screen();
		   }

		  break;

	  case 1: 	/* Add button */
		  break;

	  case 2:	/* Next button */
		  if (qbn_data.section_size[qbn_mode] > 1) {
		    current_section[qbn_mode]++;

		    if (current_section[qbn_mode] > qbn_data.section_size[qbn_mode])
		      current_section[qbn_mode] = 1;

		    draw_qbn_screen();
		   }
		  break;

	  case 3:	/* Item mode */
		  if (qbn_mode != QBN_ITEMS) {
		    qbn_mode = QBN_ITEMS;
		    redraw = TRUE;
		   }
		  break;
	  case 4:	/* NPC mode */
		  if (qbn_mode != QBN_NPCS) {
		    qbn_mode = QBN_NPCS;
		    redraw = TRUE;
		   }
		  break;
	  case 5:	/* Location mode */
		  if (qbn_mode != QBN_LOC) {
		    qbn_mode = QBN_LOC;
		    redraw = TRUE;
		   }
		  break;
	  case 6:	/* Monster mode */
		  if (qbn_mode != QBN_MOB) {
		    qbn_mode = QBN_MOB;
		    redraw = TRUE;
		   }
		  break;


	 } /* End of switch(i) */
       }
      else if (current_section[qbn_mode] > 0) {
	if (qbn_mode == QBN_ITEMS) {
	  if (section_drag[0].check (MOUSEX, MOUSEY, event)) {
	    section_drag[0].draw_texts();
	    get_qbn_data();
	   }
	  else if (section_field[0].check (MOUSEX, MOUSEY, event)) {
	    get_qbn_data();
	    parse_qbn_data();
	    section_field[0].draw();
	   }
	  else if ((i = section_button[0].check (MOUSEX, MOUSEY, event)) != NO_ITEM && i != SCROLL_BAR) {
	    switch (i) {
	      case 0: find_msg(section_drag[0].buttons[ MSG_TYPE_INDEX[0] ].value);
		      break;
	      case 1: edit_msgid ((unsigned) section_field[0].fields[ MSGID1_INDEX[0] ].number);
		      break;
	      case 2: edit_msgid ((unsigned) section_field[0].fields[ MSGID2_INDEX[0] ].number);
		      break;
	     } /* End of switch(i) */
	   }
	 } /* End of if qbn_mode == 0 */
	else if (qbn_mode == QBN_NPCS) {
	  if (section_drag[3].check (MOUSEX, MOUSEY, event)) {
	    section_drag[3].draw_texts();
	    get_qbn_data();
	   }
	  else if (section_field[3].check (MOUSEX, MOUSEY, event)) {
	    get_qbn_data();
	    parse_qbn_data();
	    section_field[3].draw();
	   }
	  else if ((i = section_button[3].check (MOUSEX, MOUSEY, event)) != NO_ITEM && i != SCROLL_BAR) {
	    switch (i) {
	      case 0: find_msg(section_drag[3].buttons[ MSG_TYPE_INDEX[3] ].value);
		      break;
	      case 1: edit_msgid ((unsigned) section_field[3].fields[ MSGID1_INDEX[3] ].number);
		      break;
	      case 2: edit_msgid ((unsigned) section_field[3].fields[ MSGID2_INDEX[3] ].number);
		      break;
	     } /* End of switch(i) */
	   }
	 } /* End of if qbn_mode == 3 */
	else if (qbn_mode == QBN_LOC) {
	  if (section_drag[4].check (MOUSEX, MOUSEY, event)) {
	    section_drag[4].draw_texts();
	    get_qbn_data();
	   }
	  else if (section_field[4].check (MOUSEX, MOUSEY, event)) {
	    get_qbn_data();
	    parse_qbn_data();
	    section_field[4].draw();
	   }
	  else if ((i = section_button[4].check (MOUSEX, MOUSEY, event)) != NO_ITEM && i != SCROLL_BAR) {
	    switch (i) {
	      case 0: find_msg(section_drag[4].buttons[ MSG_TYPE_INDEX[4] ].value);
		      break;
	      case 1: edit_msgid ((unsigned) section_field[4].fields[ MSGID1_INDEX[4] ].number);
		      break;
	      case 2: edit_msgid ((unsigned) section_field[4].fields[ MSGID2_INDEX[4] ].number);
		      break;
	     } /* End of switch(i) */
	   }
	 } /* End of if qbn_mode == 4 */
	else if (qbn_mode == QBN_MOB) {
	  if (section_drag[7].check (MOUSEX, MOUSEY, event)) {
	    section_drag[7].draw_texts();
	    get_qbn_data();
	   }
	  else if (section_button[7].check (MOUSEX, MOUSEY, event) == 0) {
	    find_msg(section_drag[7].buttons[ MSG_TYPE_INDEX[7] ].value);
	   }
	 } /* End of if qbn_mode == 7 */
       }

     } /* End of if qbn_screen */

    GetKey();

    if (get_esc_state()) {
      click_exit();
      redraw = TRUE;
     }

  } while (!done);

  return(TRUE);
 }
/*========== End Main Program =============================================*/
