	/* Standard 'C' Includes */
#include <dos.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <alloc.h>
#include <ctype.h>
#include <dos.h>
#include <dir.h>

	/* Some User Defined Includes */
#include "genutil.h"
#include "fileutil.h"
#include "keyasm.h"
#include "keybrd.h"
#include "mouseasm.h"
#include "buttons.h"
#include "util.h"

	/* Program Specific Includes */
#include "dagbutil.h"
#include "scrlbar.h"
#include "listbox.h"
#include "filelist.h"
#include "menu.h"
#include "editbook.h"


	/* The main text edit window */
EDIT_LIST editor (1, 1, 80, 25);

	/* For saving/loading the book text */
DAGBOOK_TYPE dagbook;

	/* Default Path to where text files are... */
char text_path [MAX_STRING_LENGTH + 1] = "";

	/* Save the initial available memory for debugging */
long initial_far_heapsize;

	/* For restoring the initial drive+dir on exit */
int orig_disk;
char orig_dir[MAX_STRING_LENGTH];

	/* Main edit loop counter */
boolean quit = FALSE;

	/* Are we in the 80x50 text mode? */
boolean high_textmode = FALSE;

	/* String Descriptions of Program */
char bookedit_version[] = BOOKEDIT_VERSION;
char bookedit_title[] = "BOOKEDIT v%s - Dave Humphrey\n";


/*========== Is ch an alphanumeric character ==============================*/
boolean edit_isalpha (char ch) {

  if (ch < ' ' || ch > '~')
    return (FALSE);
  else
    return (TRUE);

 }
/*========== End of function edit_isalpha() ===============================*/


/*========== Special String Search Function ===============================*/
int edit_strrchr (char *string, int start) {

	/* Make sure there is something there... */
  if (!string) return (-1);

	/* Find the Next Space in Reverse Direction */
  while (start >= 0) {
    if (string[start] == ' ') break;
    start--;
   }

  return (start);
 }
/*========== End of Function edit_strrchr() ===============================*/


/*========== Class EDIT_TYPE Constructor ==================================*/
void EDIT_TYPE::EDIT_TYPE (void) {
  data[0] = 0;
  next = NULL;
  prev = NULL;
 }
/*========== End of Class EDIT_TYPE Constructor ===========================*/


/*========== Class EDIT_TYPE Destructor ===================================*/
void EDIT_TYPE::~EDIT_TYPE (void) {
  if (next) next->prev = prev;
  if (prev) prev->next = next;
 }
/*========== End of Class EDIT_TYPE Destructor ============================*/


/*========== Destroys the Linked List =====================================*/
void EDIT_TYPE::destroy (void) {
  long i = 0;	/* Safety backup */

	/* Assume this one is the start */
  while (next && i < 100000l) {
    delete next;
    i++;
   }

  if (i >= 100000l)
    write_log_entry ("ERROR: Maximum Linked List Destroy Counter Exceeded (100,000)!");

 }
/*========== End of Procedure EDIT_TYPE::destroy() ========================*/


/*========== Class EDIT_LIST Constructor ==================================*/
void EDIT_LIST::EDIT_LIST (const int x1, const int y1, const int w, const int h) {
  edit_view = edit_head = edit_tail = edit_current = NULL;
  num_lines = view = current_char = current_line = 0;
  edit_filename = NULL;
  file_loaded = FALSE;
  pressed = FALSE;
  start_fixed = FALSE;
  modified = FALSE;
  shift_pressed = FALSE;
  x = x1;
  y = y1;
  width = w;
  height = h;
  start_select_char = start_select_line = -1;
  end_select_char = end_select_line = -1;
  end_clip = &clipboard;

  file_menu.init (x + 2, y, 17, "File", BLACK, LIGHTGRAY, LIGHTGRAY, BLACK);
  edit_menu.init (x + 8, y, 21, "Edit", BLACK, LIGHTGRAY, LIGHTGRAY, BLACK);
  help_menu.init (x + width - 12, y, 9, "Help", BLACK, LIGHTGRAY, LIGHTGRAY, BLACK);

	/* Initialize some menus */
  file_menu.add_menu ("New", editmenu_new);
  file_menu.add_menu ("Load Book  F3", editmenu_loadbook);
  file_menu.add_menu ("Save Book  F2", editmenu_savebook);
  file_menu.add_menu ("-", NULL);
  file_menu.add_menu ("Load Text", editmenu_load);
  file_menu.add_menu ("Save Text", editmenu_save);
  file_menu.add_menu ("-", NULL);
  file_menu.add_menu ("Quit    Alt+Q", editmenu_quit);

  edit_menu.add_menu ("Cut       Ctrl+K", editmenu_cut);
  edit_menu.add_menu ("Copy      Ctrl+C", editmenu_copy);
  edit_menu.add_menu ("Paste  Shift+Ins", editmenu_paste);
  edit_menu.add_menu ("-", NULL);
  edit_menu.add_menu ("Change Title", editmenu_title);
  edit_menu.add_menu ("Change Author", editmenu_author);
  edit_menu.add_menu ("-", NULL);
  edit_menu.add_menu ("Font Fancy  Ctrl+F", editmenu_fontfancy);
  edit_menu.add_menu ("Font Normal Ctrl+N", editmenu_fontnormal);
  edit_menu.add_menu ("Center Line", editmenu_center);
  edit_menu.add_menu ("Page Break", editmenu_endpage);

  help_menu.add_menu ("About", editmenu_help);

  scroll_bar.init(x + width - 1, y + 1, y + 1, height - 2, 0, 0);
 }
/*========== End of Class EDIT_LIST Constructor ===========================*/


/*========== Adds a character to The Current Line =========================*/
void EDIT_LIST::add_char (const char ch) {
  int len, i;

	/* Make sure there is something to add to... */
  if (!edit_current) return;
  len = strlen(edit_current->data);

	/* Simply add to a short string */
  if (len < MAX_EDIT_LENGTH) {
    chradd (edit_current->data, current_char, ch);
    modified = TRUE;
    draw_current_line ();
   }
	/* Add a character to middle of long line? */
  else {
		/* Find the first possible space */
     i = edit_strrchr(edit_current->data, MAX_EDIT_LENGTH);

	/* Found a space */
    if (i >= 0) {
      edit_current->data[i] = 0;

	/* Add to the next line or create a new end? */
      if (edit_current->next) {
	len = strlen(edit_current->data + i + 1) + 1;

		/* Will the string fit in the next line? */
	if (strlen(edit_current->next->data) + len < MAX_EDIT_LENGTH) {
	  memmove (edit_current->next->data + len, edit_current->next->data, strlen(edit_current->next->data) + 1);
	  memcpy (edit_current->next->data, edit_current->data + i + 1, len - 1);
	  edit_current->next->data[len - 1] = ' ';
	 }
	else {	/* Create a new line */
	  edit_current = edit_current->next;
	  add_current_line (edit_current->prev->data + i + 1);
	  edit_current = edit_current->prev;
	 }

       }
      else
	add_tail (edit_current->data + i + 1);

      chradd (edit_current->data, current_char, ch);
     }
    else {	/* Just chop the string */

		/* Add to next line or end? */
      if (edit_current->next) {

		/* Will the string fit in the next line? */
	if (strlen(edit_current->next->data) + 1 < MAX_EDIT_LENGTH) {
	  chradd (edit_current->next->data, 0, edit_current->data[MAX_EDIT_LENGTH - 1]);
	  edit_current->data[MAX_EDIT_LENGTH - 1] = 0;
	 }
	else {	/* Create a new line */
	  edit_current = edit_current->next;
	  add_current_line (edit_current->prev->data + MAX_EDIT_LENGTH - 1);
	  edit_current = edit_current->prev;
	  edit_current->data[MAX_EDIT_LENGTH - 1] = 0;
	 }

       }
      else {
	add_tail (edit_current->data + MAX_EDIT_LENGTH);
	edit_current->data[MAX_EDIT_LENGTH] = 0;
       }

      chradd (edit_current->data, current_char, ch);
     }

    modified = TRUE;
    draw_lines (DRAW_ALL);
   } /* End of if long line */
 }
/*========== End of Procedure EDIT_LIST::add_char() =======================*/


/*========== Adds a Tail to the Clipboard =================================*/
void EDIT_LIST::add_cliptail (const char *string) {

	/* Make sure the list exists */
  if (!end_clip) return;

	/* Move to end if not already at end, just in case */
  while (end_clip->next)
    end_clip = end_clip->next;

	/* Attempt to create a new end */
  if(!(end_clip->next = new EDIT_TYPE))
    bug (MEM_ERR_MSG, "*add_cliptail() - %d", sizeof(EDIT_TYPE));

	/* Setup the pointers */
  end_clip->next->prev = end_clip;
  end_clip = end_clip->next;

	/* Copy the string if any into line */
  if (string) {
    strncpy (end_clip->data, string, MAX_EDIT_LENGTH + 1);
    end_clip->data[MAX_EDIT_LENGTH] = 0;
   }
  else
    end_clip->data[0] = 0;

 }
/*========== End of Procedure EDIT_LIST::add_cliptail() ===================*/


/*========== Adds a New Node at the Current Line ==========================*/
void EDIT_LIST::add_current_line (const char *string) {
  EDIT_TYPE *temp_line;		/* Temp Pointer */

	/* No current line, add to tail, if any */
  if (!edit_current) edit_current = edit_tail;

	/* Does List Exist? */
  if (edit_current) {
    temp_line = create_line (string);

	/* Mix up pointers properly... */
    if (edit_current->prev)
      edit_current->prev->next = temp_line;
    else 	/* Must be start of list */
      edit_head = temp_line;

    temp_line->prev = edit_current->prev;
    temp_line->next = edit_current;
    edit_current->prev = temp_line;
    edit_current = temp_line;
   }
  else {
    init (string);
   }

  modified = TRUE;
 }
/*========== End of Procedure EDIT_LIST::add_current_line() ===============*/


/*========== Adds a String at the Current Location ========================*/
void EDIT_LIST::add_string (const char *string, const boolean draw) {
  char buffer[MAX_EDIT_LENGTH * 2 + 4];
  int len, len1, i;

	/* Make sure there is something to add */
  if (!string || !edit_current || !(len = strlen(string)) || len > MAX_EDIT_LENGTH) return;
  len1 = strlen(edit_current->data);

	/* Will the string fit into the line? */
  if (len + len1 < MAX_EDIT_LENGTH) {
    memmove (edit_current->data + current_char + len, edit_current->data + current_char, len1 - current_char + 1);
    memcpy (edit_current->data + current_char, string, len);
   }
	/* Must create a new line so it will fit */
  else if (edit_current->next) {
    strcpy (buffer, edit_current->data);
    memmove (buffer + current_char + len, buffer + current_char, strlen(buffer) - current_char + 1);
    memcpy (buffer + current_char, string, len);

	 /* Find a space to break it at */
    i = edit_strrchr(buffer, MAX_EDIT_LENGTH);

	/* Will the left over pieces fit into next line? */
    if (i >= 0 && strlen(edit_current->next->data) + strlen(buffer + i) < MAX_EDIT_LENGTH) {
      chrcat (buffer, ' ');
      strcat (buffer, edit_current->next->data);
      buffer[i] = 0;
      strcpy (edit_current->data, buffer);
      strcpy (edit_current->next->data, buffer + i + 1);
     }
	/* Create a new line nicely */
    else if (i >= 0 && strlen(buffer + i) < MAX_EDIT_LENGTH) {
      buffer[i] = 0;
      strcpy (edit_current->data, buffer);
      edit_current = edit_current->next;
      add_current_line (buffer + i + 1);
      edit_current = edit_current->prev;
     }
	/* Chop and add to the next line */
    else if (len1 + strlen(buffer)  < MAX_EDIT_LENGTH*2) {
      chrcat (buffer, ' ');
      strcat (buffer, edit_current->next->data);
      strncpy (edit_current->data, buffer, MAX_EDIT_LENGTH);
      edit_current->data[MAX_EDIT_LENGTH] = 0;
      strcpy (edit_current->next->data, buffer + MAX_EDIT_LENGTH);
     }
	/* Chop it off and create a new line */
    else {
      strncpy (edit_current->data, buffer, MAX_EDIT_LENGTH);
      edit_current->data[MAX_EDIT_LENGTH] = 0;
      edit_current = edit_current->next;
      add_current_line (buffer + MAX_EDIT_LENGTH);
      edit_current = edit_current->prev;
     }
   }
	/* Just add to the tail of list */
  else {
    strcpy (buffer, edit_current->data);
    memmove (buffer + current_char + len, buffer + current_char, strlen(buffer) - current_char + 1);
    memcpy (buffer + current_char, string, len);

	/* Find a space to break it at */
    i = edit_strrchr(buffer, MAX_EDIT_LENGTH);

	/* Found a space... otherwise chop off */
    if (i >= 0 && strlen(buffer + i) < MAX_EDIT_LENGTH) {
      buffer[i] = 0;
      strcpy (edit_current->data, buffer);
      add_tail (buffer + i + 1);
     }
    else {
      strncpy (edit_current->data, buffer, MAX_EDIT_LENGTH);
      edit_current->data[MAX_EDIT_LENGTH] = 0;
      add_tail (buffer + MAX_EDIT_LENGTH);
     }
   }

  movechar_rel(0, draw);
  if (draw) draw_lines (DRAW_ALL);
 }
/*========== End of Procedure EDIT_LIST::add_string() =====================*/


/*========== Adds a New Node at the Tail ==================================*/
void EDIT_LIST::add_tail (const char *string) {

	/* Does the list exist? */
  if (edit_tail) {
    edit_tail->next = create_line (string);
    edit_tail->next->prev = edit_tail;
    edit_tail = edit_tail->next;
   }
  else
    init(string);

  modified = TRUE;
 }
/*========== End of Procedure EDIT_LIST::add_tail() =======================*/


/*========== Adds a Line Break at the Current Character ===================*/
void EDIT_LIST::break_line (const boolean draw) {
  char buffer[MAX_EDIT_LENGTH + 1];

	/* Make sure there is something there */
  if (!edit_current) return;

	/* Character at end of line? */
  if (current_char >= strlen(edit_current->data)) {
    if (edit_current->next) {
      edit_current = edit_current->next;
      add_current_line ();
      edit_current = edit_current->prev;
     }
    else
      add_tail ();

    current_char = 0;
    moveline_rel(1, draw);
    if (draw) draw_lines (DRAW_PREVIOUS);
   }
  else {
    strcpy (buffer, edit_current->data + current_char);
    edit_current->data[current_char] = 0;

    if (edit_current->next) {
      edit_current = edit_current->next;
      add_current_line (buffer);
      edit_current = edit_current->prev;
     }
    else
      add_tail (buffer);

    current_char = 0;
    moveline_rel(1, draw);
    if (draw) draw_lines (DRAW_PREVIOUS);
   }

 }
/*========== End of Procedure EDIT_LIST::break_line() =====================*/


/*========== Special Variation of the clreol() function ===================*/
void EDIT_LIST::clear_eol (void) {
  int x1 = wherex(), i;

  for (i = x1; i < x + width - 1; i++)
    putch (' ');

  gotoxy (x1, wherey());
 }
/*========== End of Procedure clear_eol() =================================*/


/*========== Destroys the Clipboard =======================================*/
void EDIT_LIST::clip_destroy (void) {
  clipboard.destroy();
  clipboard.data[0] = 0;
  end_clip = &clipboard;
 }
/*========== End of Procedure EDIT_LIST::clip_destroy() ===================*/


/*========== Attempts to combine to adjacent lines ========================*/
void EDIT_LIST::combine_lines (const int combine_type) {
  EDIT_TYPE *next_line;
  int i;

	/* Fix the current line depending on the combine type */
  if (!edit_current)
    return;
  else if (combine_type == COMBINE_PREV && edit_current->prev)
    edit_current = edit_current->prev;
  else if (combine_type == COMBINE_PREV)
    return;
  else if (combine_type == COMBINE_NEXT && !edit_current->next)
    return;

	/* Set a shortcut to the next line to be combined */
  next_line = edit_current->next;

	/* Will the combined lines fit in the text array? */
  if (strlen(edit_current->data) + strlen(next_line->data) <= MAX_EDIT_LENGTH) {
    chrcat (edit_current->data, ' ');
    strcat (edit_current->data, next_line->data);
    next_line = edit_current;

	/* Delete one of the combined lines */
    edit_current = edit_current->next;
    delete_node();

    edit_current = next_line;
   }
  else {	/* Must carefully combine them */
    i = edit_strrchr (next_line->data, MAX_EDIT_LENGTH - strlen(edit_current->data));

	/* Found a space... otherwise do nothing */
    if (i >= 0) {
      next_line->data[i] = 0;
      chrcat (edit_current->data, ' ');
      strcat (edit_current->data, next_line->data);
      memccpy (next_line->data, next_line->data + i + 1, 0, strlen(next_line->data + i + 1));
     }
   }

 }
/*========== End of Procedure EDIT_LIST::combine_lines() ==================*/


/*========== Copies any Selected Lines to the Clipboard ===================*/
void EDIT_LIST::copy (void) {
  char buffer[MAX_EDIT_LENGTH + 1];	/* Temp buffer */
  EDIT_TYPE *temp_line;			/* Temp pointer */
  int i;	/* Loop counter */

	/* Make sure there is something to copy from */
  if (start_select_line == -1 || end_select_line == -1 || start_select_char == -1 || end_select_char == -1 ||
      start_select_line > end_select_line || (start_select_line == end_select_line && start_select_char > end_select_char)) return;

	/* Clear the clipboard */
  clip_destroy();

	/* Move to the first selected line */
  temp_line = edit_head;

  for (i = 0; i < start_select_line - 1 && temp_line; i++)
    temp_line = temp_line->next;

  if (!temp_line) return;

	/* Start and end on same line? */
  if (end_select_line == start_select_line) {
    strcpy (clipboard.data, temp_line->data + start_select_char);
    clipboard.data[end_select_char - start_select_char + 1] = 0;
   }
  else {	/* More Complicated Case */

	/* Copy the First Line */
    strcpy (clipboard.data, temp_line->data + start_select_char);

	/* Copy the middle lines */
    temp_line = temp_line->next;
    i = start_select_line;

    while (temp_line && i < end_select_line - 1) {
      add_cliptail (temp_line->data);
      temp_line = temp_line->next;
      i++;
     }

	/* And the end line */
    if (i == end_select_line - 1 && temp_line) {
      strcpy (buffer, temp_line->data);
      buffer[end_select_char + 1] = 0;
      add_cliptail (buffer);
     }
   }

	/* No more selected text? */
//  end_select_line = start_select_line = -1;
//  end_select_char = start_select_char = -1;
//  draw_lines (DRAW_ALL);
 }
/*========== End of Procedure EDIT_LIST::copy() ===========================*/


/*========== Cuts any Selected Lines to the Clipboard =====================*/
void EDIT_LIST::cut (void) {
  EDIT_TYPE *temp_line, *end_line;		/* Temp pointers */
  boolean last_char = FALSE;
  int i;			/* Loop counter */

	/* Make sure there is something to copy from */
  if (start_select_line == -1 || end_select_line == -1 || start_select_char == -1 || end_select_char == -1 ||
      start_select_line > end_select_line || (start_select_line == end_select_line && start_select_char > end_select_char)) return;

	/* Clear the clipboard */
  clip_destroy();

	/* Move to the first selected line */
  temp_line = edit_head;

  for (i = 0; i < start_select_line - 1 && temp_line; i++)
    temp_line = temp_line->next;

  if (!temp_line) return;

	/* Start and end on same line? */
  if (end_select_line == start_select_line) {

		/* Copy text to clipboard */
    strcpy (clipboard.data, temp_line->data + start_select_char);
    clipboard.data[end_select_char - start_select_char + 1] = 0;

		/* Clear text */
    memmove (temp_line->data + start_select_char, temp_line->data + end_select_char + 1, strlen(temp_line->data + end_select_char) + 1);
   }
  else { /* More Complicated Case */

		/* Copy First Line to Clip */
    strcpy (clipboard.data, temp_line->data + start_select_char);
    temp_line->data[start_select_char] = 0;

		/* Find the end selected line */
    end_line = temp_line;

    for (i = start_select_line; i < end_select_line - 1 && end_line && end_line->next; i++)
      end_line = end_line->next;

    if (!end_line || !end_line->next) return;

		/* Now reset pointers for the middle lines */
    if (end_select_line - start_select_line >= 2) {
      clipboard.next = temp_line->next;
      end_clip = end_line;
     }
    else
      clipboard.next = NULL;

		/* Check different conditions */
    if (view > start_select_line - 1 && view < end_select_line - 1) {
      edit_view = temp_line;
      view = start_select_line - 1;
     }
    else if (view >= end_select_line - 1)
      view -= end_select_line - start_select_line - 1;


    if (current_line > start_select_line - 1 && current_line <= end_select_line - 1) {
      current_line = start_select_line;
      current_char = 0;
     }
    else if (current_line > end_select_line) {
      current_line -= end_select_line - start_select_line - 1;
      if ((current_line - view) >= height - 2) current_line = view + height - 3;
     }

    num_lines -= end_select_line - start_select_line - 1;

		/* Reset the rest of the pointers */
    if (clipboard.next) clipboard.next->prev = &clipboard;
    end_line = end_line->next;
    temp_line->next = end_line;
    end_line->prev = temp_line;
    end_clip->next = NULL;

		/* Copy Last Line to Clip */
    add_cliptail (end_line->data);
    if (end_select_char - 1 >= strlen(end_line->data)) last_char = TRUE;
    end_clip->data[end_select_char + 1] = 0;
    memmove (end_line->data, end_line->data + end_select_char + 1, strlen(end_line->data + end_select_char) + 1);

	/* Mix around with the points some more */
    if (start_select_char == 0) {

		/* Various cases */
      if (start_select_line == 1 && edit_head->next) {
	edit_head = edit_head->next;
	delete edit_head->prev;

	if (view == 0)
	  edit_view = edit_head;
	else
	  view--;

	if (num_lines) num_lines--;
	if (current_line) current_line--;
	if (current_line == 0) edit_current = edit_head;
       }
      else if (start_select_line - 1 == view && edit_view->prev) {
	edit_view = edit_view->prev;
	delete edit_view->next;
	view--;
	num_lines--;
	current_line--;
       }
		/* Regular line... */
      else {
	delete temp_line;
	num_lines--;
	if (current_line > start_select_line) current_line--;
	if (view > start_select_line) view--;
       }
     } /* End of if start_select_char == 0 */

    if (last_char) {

      if (start_select_char == 0 && start_select_line == 1 && edit_head->next) {
	edit_head = edit_head->next;
	delete edit_head->prev;

	if (view == 0)
	  edit_view = edit_head;
	else
	  view--;

	if (num_lines) num_lines--;
	if (current_line) current_line--;
	if (current_line == 0) edit_current = edit_head;
       }
      else if (start_select_char == 0 && start_select_line - 1 == view && edit_view->prev) {
	edit_view = edit_view->prev;
	delete edit_view->next;
	view--;
	num_lines--;
	current_line--;
       }

      else if (end_select_line >= num_lines && edit_tail->prev) {
	edit_tail = edit_tail->prev;
	delete edit_tail->next;

	if (view == end_select_line - 1) edit_view = edit_tail;
	if (num_lines) num_lines--;

	if (current_line > num_lines - 1) {
	  edit_current = edit_tail;
	  current_line = num_lines - 1;
	 }
       }
      else if (end_select_line - 1 == view && edit_view->prev) {
	edit_view = edit_view->prev;
	delete edit_view->next;
	view--;
	num_lines--;
	current_line--;

	if (current_line > num_lines - 1) {
	  edit_current = edit_tail;
	  current_line = num_lines - 1;
	 }
       }
      else {
	delete end_line;
	num_lines--;
	if (current_line > end_select_line) current_line--;
	if (view > end_select_line) view--;

	if (current_line > num_lines - 1) {
	  edit_current = edit_tail;
	  current_line = num_lines - 1;
	 }
       }

     } /* end of if last_char */

   }

	/* No more selected text? */
  end_select_line = start_select_line = -1;
  end_select_char = start_select_char = -1;
  modified = TRUE;
  draw_lines (DRAW_ALL);
 }
/*========== End of Procedure EDIT_LIST::cut() ============================*/



/*========== Creates a New EDIT_TYPE node =================================*/
EDIT_TYPE *EDIT_LIST::create_line (const char *string) {
  EDIT_TYPE *temp_ptr;

	/* Attempt to Create a new line */
  if(!(temp_ptr = new EDIT_TYPE))
    bug (MEM_ERR_MSG, "*create_line() - %d", sizeof(EDIT_TYPE));

	/* Copy the string if any into line */
  if (string)
    strncpy (temp_ptr->data, string, MAX_EDIT_LENGTH + 1);
  else
    temp_ptr->data[0] = 0;

	/* Increase the number of lines... */
  num_lines++;
  return (temp_ptr);
 }
/*========== End of Function EDIT_LIST::create_line() =====================*/


/*========== Deletes a character from the current position ================*/
void EDIT_LIST::del_char (const int del_type) {
  int char_pos = 0, len;

	/* Make sure there is something to delete */
  if (!edit_current) return;
  len = strlen (edit_current->data);

	/* Determine what type of delete to do */
  switch (del_type) {
    case DEL_DELETE:
      char_pos = current_char;
      break;
    default:
      char_pos = current_char - 1;
      break;
   }

	/* Regular delete in string */
  if (char_pos < len && char_pos >= 0 && len >= 0) {
    chrdel (edit_current->data, char_pos);

    if (del_type == DEL_BACKSPACE)
      movechar_rel(-1);
    else
      movechar_rel(0);

    draw_current_line();
    modified = TRUE;
   }
	/* Must be a more complicated delete case */
  else if (char_pos < 0) {
    current_char = strlen(edit_current->prev->data);
    combine_lines (COMBINE_PREV);
    moveline_rel (-1);
    draw_lines (DRAW_CURRENT);
    modified = TRUE;
   }
  else {
    combine_lines (COMBINE_NEXT);
    draw_lines (DRAW_CURRENT);
    modified = TRUE;
   }

 }
/*========== End of procedure EDIT_LIST::del_char() =======================*/


/*========== Properly Deletes a Node in the List, list_current ============*/
void EDIT_LIST::delete_node (void) {

	/* Don't delete if nothing there */
  if (!edit_current) return;

	/* Is the current line the start of the display screen? */
  if (edit_current == edit_view) {
    if (edit_current->next)
      edit_view = edit_current->next;
    else {
      edit_view = edit_current->prev;
      if (view > 0) view--;
     }
   }

	/* Is the list the last/first line of the text? */
  if (edit_current == edit_tail) edit_tail = edit_tail->prev;
  if (edit_current == edit_head) edit_head = edit_head->next;

	/* Delete the node */
  if (edit_current->next) {
    edit_current = edit_current->next;
    delete edit_current->prev;
    num_lines--;
    modified = TRUE;
   }
  else if (edit_current->prev) {
    edit_current = edit_current->prev;
    delete edit_current->next;
    num_lines--;
    modified = TRUE;
   }
  else {	/* Only node in list, just delete text */
    if (edit_current->data[0]) modified = TRUE;
    edit_current->data[0] = 0;
    current_char = 0;
   }

 }
/*========== End of Procedure EDIT_LIST::delete_node() ====================*/


/*========== Deletes the Entire List ======================================*/
void EDIT_LIST::destroy (void) {

	/* Delete the list if there is any */
  if (edit_head) {
    edit_head->destroy();
    delete edit_head;
   }

  if (edit_filename) {
    delete edit_filename;
    edit_filename = NULL;
   }

  file_loaded = FALSE;
  shift_pressed = FALSE;
  edit_head = edit_tail = edit_current = edit_view = NULL;
  num_lines = view = current_char = current_line = 0;
  start_select_char = start_select_line = -1;
  end_select_char = end_select_line = -1;
  modified = FALSE;
 }
/*========== End of Procedure EDIT_LIST::destroy() ========================*/


/*========== Redraws Some or All the text in the window ===================*/
void EDIT_LIST::draw_lines (const int draw_type) {
  EDIT_TYPE *temp_line;
  int y1;

  if (!edit_current) return;

	/* Choose the Starting Position */
  switch (draw_type) {
    case DRAW_CURRENT:
      temp_line = edit_current;
      y1 = y + 1 + current_line - view;
      break;
    case DRAW_PREVIOUS:

      if (edit_current->prev) {
	temp_line = edit_current->prev;
	y1 = y + current_line - view;
       }
      else {
	temp_line = edit_current;
	y1 = y + 1 + current_line - view;
       }

      break;
    case DRAW_ALL:
      editor.draw_filename();
      editor.draw_status_line();
    default:
      temp_line = edit_view;
      y1 = y + 1;
      break;
   }

  hide_mouse();
  _setcursortype (_NOCURSOR);

	/* Draw the Lines */
  while (temp_line && y1 < y + height - 1) {
    gotoxy (x + 1, y1);
    parse_edit_line (temp_line->data, y1 - y + view);
    clear_eol();
    temp_line = temp_line->next;
    y1++;
   }

	/* Clear rest of lines on screen */
  while (y1 < y + height - 1) {
    gotoxy (x + 1, y1);
    clear_eol ();
    y1++;
   }

	/* Return to current cursor position */
  GOTO_CURSOR;
  _setcursortype (_NORMALCURSOR);
  show_mouse();
 }
/*========== End of procedure EDIT_LIST::draw_lines() =====================*/


/*========== Draws The Current Edit Line ==================================*/
void EDIT_LIST::draw_current_line (void) {

	/* Make Sure We have somethere there */
  if (!edit_current) return;

  hide_mouse ();
  _setcursortype (_NOCURSOR);
  gotoxy (x + 1, y + 1 + current_line - view);
  parse_edit_line (edit_current->data, current_line);
  clear_eol();

	/* Return to cursor position */
  GOTO_CURSOR;
  _setcursortype (_NORMALCURSOR);
  show_mouse ();
 }
/*========== End of procedure EDIT_LIST::draw_current_line() ==============*/


/*========== Draws the border etc... of the edit Window ===================*/
void EDIT_LIST::draw_edit_window (const char *title) {
  int count;

  hide_mouse();
  textcolor (LIGHTGRAY);
  textbackground (BLACK);
  clrscr();

	/* Draw the Header */
  gotoxy (x, y);
  textcolor (BLACK);
  textbackground (LIGHTGRAY);
  cprintf ("%*s", width, " ");

  file_menu.display();
  edit_menu.display();
  help_menu.display();

  textcolor (BLUE);
  cprintf_centx (y, "%s", title);

  textcolor(LIGHTGRAY);
  textbackground(BLACK);

	/* Print right and left sides */
  for (count = y + 1; count < y + height - 1; count++) {
    gotoxy (x, count);
    cprintf ("%c", VERTICAL);
    gotoxy (x + width - 1, count);
    cprintf ("%c", VERTICAL);
   }

	       /* Print the two bottom corners */
  gotoxy (x + width - 1, y + height - 1);
  cprintf ("%c", BOTRIGHT_CORNER);
  gotoxy (x, y + height - 1);
  cprintf ("%c", BOTLEFT_CORNER);

	      /* Print the bottom line... */
  for (count = x + 1; count < x + width - 1; count++)
    cprintf ("%c", HORIZONTAL);

  scroll_bar.draw();

  show_mouse();
  draw_status_line();
  draw_filename();
 }
/*========== End of procedure EDIT_LIST::draw_edit_window() ===============*/


/*========== Updates the Position Status at the Bottom of the Screen ======*/
void EDIT_LIST::draw_status_line (void) {

  hide_mouse();
   _setcursortype(_NOCURSOR);

  gotoxy (x + 5, y + height - 1);
  cprintf (" %3d:%3d ", current_line + 1, current_char + 1);

  if (modified)
    cprintf ("* ");
  else
    cprintf ("  ");

  _setcursortype(_NORMALCURSOR);
  show_mouse();
  scroll_bar.change_max(num_lines - 1 - scroll_bar.max);
  scroll_bar.change_value (current_line);
  GOTO_CURSOR;
 }
/*========== End of Procedure EDIT_LIST::draw_status_line() ===============*/


/*========== Draws in the Filename At the Bottom of the Screen ============*/
void EDIT_LIST::draw_filename (void) {

  hide_mouse();
   _setcursortype(_NOCURSOR);

  gotoxy (x + 20, y + height - 1);

  if (edit_filename)
    cprintf (" %013s ", edit_filename);
  else
    cprintf (" %013s ", "<none>");

  _setcursortype(_NORMALCURSOR);
  show_mouse();
  GOTO_CURSOR;
 }
/*========== End of Procedure EDIT_LIST::draw_filename() ==================*/


/*========== Returns String Length of All Strings in Edit List ============*/
unsigned long EDIT_LIST::get_editsize (void) {
  EDIT_TYPE *line;
  unsigned long size = 0;

  line = edit_head;

  while (line) {
    size += strlen(line->data) + 1;
    line = line->next;
   }

  return (size);
 }
/*========== End of function EDIT_LIST::get_editsize() ====================*/


/*========== We Will Always Need At Least One Line Allocated Initially ====*/
void EDIT_LIST::init (const char *string) {

	/* If list doesn't exist, create the head */
  if (!edit_head) {
    edit_head = create_line (string);
    edit_tail = edit_current = edit_view = edit_head;
    current_line = current_char = view = 0;
    start_select_char = start_select_line = -1;
    end_select_char = end_select_line = -1;
    pressed = FALSE;
    modified = FALSE;
    shift_pressed = FALSE;
   }

 }
/*========== End of Procedure EDIT_LIST::init() ===========================*/


/*========== Attempts to Move to a New Character ==========================*/
boolean EDIT_LIST::movechar_abs (const int new_char, const boolean draw) {
  int len;

	/* Make sure there is a proper pointer */
  if (!edit_current) return (FALSE);

	/* Is this a normal move? */
  if (new_char >= 0 && new_char <= strlen(edit_current->data)) {
    if (current_char == new_char) return (FALSE);
    current_char = new_char;

	/* Need to update selected text? */
    if (get_shift_state() && shift_pressed)
      select_text (current_char + x + 1, current_line - view + y + 1);

    if (draw) draw_status_line();
    return (TRUE);
   }
	/* Move to the previous line */
  else if (new_char < 0) {

	/* Attempt to move to the line */
    if (moveline_rel (-1)) {
      current_char = strlen(edit_current->data) - new_char - current_char;
      if (current_char < 0) current_char = 0;
      if (draw) draw_status_line();

		/* Need to update selected text? */
      if (get_shift_state() && shift_pressed)
	select_text (current_char + x + 1, current_line - view + y + 1);

      return (TRUE);
     }
   }
  else {
    len = strlen(edit_current->data);

	/* Attempt to move to the line */
    if (moveline_rel (1)) {
      current_char = new_char - len;
      len = strlen(edit_current->data);
      if (current_char > len) current_char = len;
      if (draw) draw_status_line();

		/* Need to update selected text? */
      if (get_shift_state() && shift_pressed)
	select_text (current_char + x + 1, current_line - view + y + 1);

      return (TRUE);
     }
   }

  return (FALSE);
 }
/*========== End of Function EDIT_LIST::movechar_abs() ====================*/


/*========== Moves the Cursor to a Given Line =============================*/
boolean EDIT_LIST::moveline_abs (const int line, const boolean draw) {
  int new_line = line, i;

	/* Make sure the number is a valid one... */
  if (new_line < 0)
    new_line = 0;
  else if (line >= num_lines)
    new_line = num_lines - 1;

  if (new_line == current_line) return (FALSE);
  current_line = new_line;
  edit_current = edit_head;

	/* Move the line pointer */
  for (i = 0; i < current_line && edit_current; i++)
    edit_current = edit_current->next;

  if (!edit_current) edit_current = edit_tail;
  i = strlen(edit_current->data);
  if (current_char > i) current_char = i;

	/* Check the view */
  if (current_line < view) {
    edit_view = edit_current;
    view = current_line;

	/* Need to update selected text? */
    if (get_shift_state() && shift_pressed)
      select_text (current_char + x + 1, current_line - view + y + 1);
    else if (draw)
      draw_lines (DRAW_ALL);
   }
  else if (current_line > view + height - 3) {

    for (i = 0; i < current_line - view - height + 3 && edit_view; i++)
      edit_view = edit_view->next;

    if (!edit_view) {
      edit_view = edit_current;
      view = current_line;
     }

    view = current_line - height + 3;
    moveline_abs (view + height - 3, draw);

	/* Need to update selected text? */
    if (get_shift_state() && shift_pressed)
      select_text (current_char + x + 1, current_line - view + y + 1);
    else if (draw)
      draw_lines (DRAW_ALL);
   }
  else {
    if (get_shift_state() && shift_pressed)
      select_text (current_char + x + 1, current_line - view + y + 1);

    if (draw) draw_status_line();
   }

  return (TRUE);
 }
/*========== End of Function EDIT_LIST::moveline_abs() ====================*/


/*========== Moves the Cursor to a New Position relative to the screen ====*/
void EDIT_LIST::move_cursor (int mx, int my, const boolean draw) {

	/* Click in text window */
  if (mx > x && mx < x + width - 1 && my > y && my < y + height - 1) {

	/* Click on the current line */
    if (my == y + 1 + current_line - view) {

      if (mx - x - 1 > strlen(edit_current->data))
	movechar_abs (strlen(edit_current->data), draw);
      else
	movechar_abs (mx - x - 1, draw);
     }

	/* Click on new line */
    else {
      moveline_abs (my - y - 1 + view, draw);

      if (mx - x - 1 > strlen(edit_current->data))
	movechar_abs (strlen(edit_current->data), draw);
      else
	movechar_abs (mx - x - 1, draw);
     }

   } /* End of click in text window */
 }
/*========== End of Procedure EDIT_LIST::move_cursor() ====================*/


/*========== Parses An Edit Line, Printing in Color? ======================*/
void EDIT_LIST::parse_edit_line(const char *string, const int line) {
  boolean select = FALSE;
  int select_line = SELECT_NONE;
  int orig_text_color, orig_back_color;
  int text_color2, back_color2;
  int text_color4, back_color4;
  int i = 0;

  if (!string) return;
  text_color2 = orig_text_color = get_text_color();
  back_color2 = orig_back_color = get_back_color();

	/* Determine selected status */
  if (line == start_select_line && line == end_select_line)
    select_line = SELECT_BOTH;
  else if (line == start_select_line)
    select_line = SELECT_START;
  else if (line == end_select_line) {
    select_line = SELECT_END;
    select = TRUE;
    textcolor (BLACK);
    textbackground (LIGHTGRAY);
   }
  else if (line > start_select_line && line < end_select_line) {
    select_line = SELECT_MIDDLE;
    select = TRUE;
    textcolor (BLACK);
    textbackground (LIGHTGRAY);
   }

	/* Print to end of line */
  while (string[i] && i < editor.width - 2) {

	/* Determine selected status */
    if (select_line == SELECT_BOTH && !select && i >= start_select_char && i < end_select_char) {
      select = TRUE;
      textcolor (SELECT_TEXT_COLOR);
      textbackground (SELECT_BACK_COLOR);
     }
    else if (select_line == SELECT_START && !select && i >= start_select_char) {
      select = TRUE;
      textcolor (SELECT_TEXT_COLOR);
      textbackground (SELECT_BACK_COLOR);
     }
    else if ((select_line == SELECT_END || select_line == SELECT_BOTH) && select && i > end_select_char) {
      textcolor (LIGHTGRAY);
      textbackground (BLACK);
      select = FALSE;
     }

    if (select)
      cprintf ("%c", string[i]);
    else {

      switch (string[i]) {
	case '}':		/* If Font Selected Char */
	  cprintf ("}");
	  textcolor (text_color2);
	  textbackground (back_color2);
	  break;
	case '{':
	  text_color2 = get_text_color();
	  back_color2 = get_back_color();
	  textcolor (FONT_TEXT_COLOR);
	  textbackground (FONT_BACK_COLOR);
	  cprintf ("{");
	  break;
	case END_PAGE_CHAR:	/* End of Page Section */
	  text_color4 = get_text_color();
	  back_color4 = get_back_color();
	  textcolor (END_PAGE_TEXT_COLOR);
	  textbackground (END_PAGE_BACK_COLOR);
	  cprintf ("~");
	  textcolor (text_color4);
	  textbackground (back_color4);
	  break;
	case CENTER_CHAR:		/* Center Text Section */
	  text_color4 = get_text_color();
	  back_color4 = get_back_color();
	  textcolor (CR_TEXT_COLOR);
	  textbackground (CR_BACK_COLOR);
	  cprintf ("%c", string[i]);
	  textcolor (text_color4);
	  textbackground (back_color4);
	  break;
	case HEX_CHAR:			/* Possible Hex String? */
	  if (isxdigit(string[i + 1]) && isxdigit(string[i + 2])) {
	    text_color4 = get_text_color();
	    back_color4 = get_back_color();
	    textcolor (HEX_TEXT_COLOR);
	    textbackground (HEX_BACK_COLOR);
	    cprintf ("#%c%c", string[i + 1], string[i + 2]);
	    i += 2;
	    textcolor (text_color4);
	    textbackground (back_color4);
	   }
	  else
	    cprintf ("%c", string[i]);

	  break;
	default:			/* Just put a character */
	  cprintf ("%c", string[i]);
	  break;
       } /* End of switch */
     }

    i++;
   } /* End of while loop */

  textcolor (orig_text_color);
  textbackground (orig_back_color);
 }
/*========== End of procedure EDIT_LIST::parse_edit_line() ================*/


/*========== Pastes the Clipboard Contents at the Current Location ========*/
void EDIT_LIST::paste (void) {
  EDIT_TYPE *temp_line;		/* Temp pointer */
  int pre_char = current_char;

	/* Make sure there is something to paste and a place to paste to */
  if (!strlen(clipboard.data) || !edit_current) return;

	/* Only One Line of Text to paste? */
  if (!clipboard.next) {
    add_string (clipboard.data);
   }
	/* More Complicated */
  else {

	/* Break the Line at the current point */
    break_line (FALSE);

	/* Start with the last line in the clipboard and add a new line */
    temp_line = end_clip;

    while (temp_line->prev) {
      add_current_line (temp_line->data);
      temp_line = temp_line->prev;
     }

	/* Add the first line */
    if (edit_current->prev) {
      edit_current = edit_current->prev;
      current_char = pre_char;
      add_string (temp_line->data, FALSE);
      current_line--;
     }
    else
      add_string (temp_line->data, FALSE);

    draw_lines (DRAW_ALL);
   }
 }
/*========== End of Procedure EDIT_LIST::paste() ==========================*/


/*========== Procedure For Selecting Text Lines ===========================*/
void EDIT_LIST::select_text (const int mx, const int my) {
  int i, j;	/* Temp variables */

	/* Is the new screen position valid? */
  if (mx > x && mx < x + width - 1 && my > y && my < y + height - 1) {
    i = mx - x - 1;
    j = my - y + view;

    if (start_fixed && (j < start_select_line || (j == start_select_line && i < start_select_char))) {
      swap (start_select_line, end_select_line);
      swap (start_select_char, end_select_char);
      start_fixed = FALSE;
     }
    else if (!start_fixed && (j > end_select_line || (j == start_select_line && i > end_select_char) || start_fixed)) {
      swap (start_select_line, end_select_line);
      swap (start_select_char, end_select_char);
      start_fixed = TRUE;
     }

    if (start_fixed && (i != end_select_char || j != end_select_line)) {
      if (j > num_lines) j = num_lines;
      end_select_line = j;
      end_select_char = i;
      draw_lines (DRAW_ALL);
     }
    else if (!start_fixed && (i != start_select_char || j != start_select_line)) {
      start_select_line = j;
      start_select_char = i;
      draw_lines (DRAW_ALL);
     }

   }

 }
/*========== End of Procedure EDIT_LIST::select_text() ====================*/


/*========== Main loop for the text editor ================================*/
void EDIT_LIST::text_editor(const char *title) {
  boolean pre_shift = FALSE;
  int mx, my, event, i;
  char ch;

  quit = FALSE;
  _wscroll = 0;

	/* Initialize the list */
  init();

	/* Draw Some Things */
  draw_edit_window (title);
  draw_lines (DRAW_ALL);
  gotoxy (x + 1, y + 1);

  do {

	/* A mouse click somewhere? */
    event = get_mouse_event (LEFT_BUTTON);
    get_mouse_pos();
    mx = MOUSEX;
    my = MOUSEY;

    if ((i = scroll_bar.check (MOUSEX, MOUSEY, event)) != NO_SCROLL) {
      moveline_rel(i);
     }
		/* Check for a click on the 'file menu' */
    else if (file_menu.check (mx, my, event)) {
      GOTO_CURSOR;
      _setcursortype (_NORMALCURSOR);
     }
		/* Check the Edit Menu */
    else if (edit_menu.check (mx, my, event)) {
      GOTO_CURSOR;
      _setcursortype (_NORMALCURSOR);
     }
		/* Check the Help Menu */
    else if (help_menu.check (mx, my, event)) {
      draw_edit_window(title);
      draw_lines (DRAW_ALL);
      GOTO_CURSOR;
      _setcursortype (_NORMALCURSOR);
     }

    else if (event == BUTTON_PRESS) {

      if (mx > x && mx < x + width - 1 && my > y && my < y + height - 1) {
	end_select_char = start_select_char = mx - x - 1;
	end_select_line = start_select_line = my - y + view;
	draw_lines (DRAW_ALL);
	pressed = TRUE;
	move_cursor (mx, my);
       }
     }

	/* For selecting text */
    else if (pressed && event == NO_EVENT && MouseButtonStatus != 0) {
      mx = (MouseX>>2) + 1;
      my = (MouseY>>3) + 1;
      select_text (mx, my);
      move_cursor (mx, my);
      show_mouse();
     }

    else if (event == BUTTON_RELEASE) {
      move_cursor(mx, my);
      pressed = FALSE;
     } /* End of if BUTTON_RELEASE */

	/* Check Keyboard Input */
    GetKey();
    ch = (char) keylast;

	/* ALT-F = File Menu */
    if (get_alt_key(SCAN_F)) {
      file_menu.display_menu();
      gotoxy (current_char + 2, current_line + 2);
      _setcursortype (_NORMALCURSOR);
     }
	/* ALT-E = Edit Menu */
    else if (get_alt_key(SCAN_E)) {
      edit_menu.display_menu();
      gotoxy (current_char + 2, current_line + 2);
      _setcursortype (_NORMALCURSOR);
     }
	/* ALT-Q = Quit */
    else if (get_alt_key(SCAN_Q)) {
      editmenu_quit();
     }
	/* ALT-H/F1 = Help */
    else if ((key_press[SCAN_F1] != 0) || (get_alt_key(SCAN_H))) {
      editmenu_help (NULL);
      draw_edit_window (title);	/* Redraw screen */
      draw_lines (DRAW_ALL);
     }
	/* For Debugging... */
    else if (get_alt_key (SCAN_D)) {
      delay (KEY_DELAY);
     }
	/* F2, save book */
    else if (key_press[SCAN_F2] != 0) {
      editmenu_savebook(NULL);
      gotoxy (current_char + 2, current_line + 2);
      _setcursortype (_NORMALCURSOR);
     }
	/* F3, Load Book */
    else if (key_press[SCAN_F3] != 0) {
      editmenu_loadbook(NULL);
      gotoxy (current_char + 2, current_line + 2);
      _setcursortype (_NORMALCURSOR);
     }
	/* Ctrl+C, Copy */
    else if (get_ctrl_key(SCAN_C)) {
      editmenu_copy();
      delay(KEY_DELAY*2);
     }
	/* Ctrl+X, Cut */
    else if (get_ctrl_key(SCAN_X)) {
      editmenu_cut();
      delay(KEY_DELAY*2);
     }
	/* Ctrl+F, FancyFontt */
    else if (get_ctrl_key(SCAN_F)) {
      editmenu_fontfancy();
      delay(KEY_DELAY*2);
     }
	/* Ctrl+N, NormFont */
    else if (get_ctrl_key(SCAN_N)) {
      editmenu_fontnormal();
      delay(KEY_DELAY*2);
     }

	/* Shift+Ins, Paste */
    else if (get_shift_key(SCAN_KEYPAD_INS)) {
      shift_pressed = FALSE;
      start_select_char = end_select_char = -1;
      start_select_line = end_select_line = -1;
      editmenu_paste();
      delay(KEY_DELAY*2);
     }
	/* Tab character ... 5 spaces */
    else if (key_press[SCAN_TAB] != 0) {
      add_string ("     ");
      delay (KEY_DELAY);
     }
	/* CTRL-Y, delete line */
    else if (get_ctrl_key(SCAN_Y)) {
      delete_node();
      end_select_line = start_select_line = -1;
      end_select_char = start_select_char = -1;
      if (current_line >= num_lines) moveline_rel(-1);
      draw_lines (DRAW_ALL);
      delay(KEY_DELAY * 2);
      ClearKeys();
     }
	/* Delete something */
    else if (key_press[SCAN_BKSP] != 0) {
      del_char (DEL_BACKSPACE);
      delay(KEY_DELAY);
     }
	/* Insert line */
    else if (get_enter_state()) {
      break_line ();
      delay(KEY_DELAY);
     }
	/* Delete something */
    else if (get_del_state()) {
      del_char (DEL_DELETE);
      delay(KEY_DELAY);
     }
	/* Home */
    else if (get_home_state()) {
      movechar_abs(0);
     }
	/* End */
    else if (get_end_state ()) {
      movechar_abs (strlen(edit_current->data));
     }
	/* PGUP Move up faster */
    else if (get_pgup_state()) {
      moveline_rel (-height + 1);
      delay (KEY_DELAY);
     }
	/* PGDN, Move down faster */
    else if (get_pgdn_state()) {
      moveline_rel (height - 1);
      delay (KEY_DELAY);
     }
	/* Move up */
    else if (get_up_state()) {
      moveline_rel (-1);
      delay(KEY_DELAY);
     }
	/* Move down */
    else if (get_down_state()) {
      moveline_rel (1);
      delay(KEY_DELAY);
     }
	/* Move left */
    else if (get_left_state()) {
      movechar_rel (-1);
      delay(KEY_DELAY);
     }
	/* Move Right */
    else if (get_right_state()) {
      movechar_rel(1);
      delay(KEY_DELAY);
     }
	/* Normal character... */
    else if (edit_isalpha(ch)) {
      if (get_shift_state() && shift_pressed)
	shift_pressed = FALSE;

      add_char (ch);
      movechar_rel(1);
     }
	/* Shift Key */
    else if (get_shift_state() && !pre_shift && !shift_pressed && !MouseButtonStatus) {
      pre_shift = TRUE;
      shift_pressed = TRUE;
      start_select_char = end_select_char = current_char;
      start_select_line = end_select_line = current_line + 1;
     }


	/* Reset the text selection */
    if (!get_shift_state() && pre_shift) {
      shift_pressed = FALSE;
      pre_shift = FALSE;
     }

  } while (!quit);

  hide_mouse ();
  clrscr();
  show_mouse();
 }
/*========== End of procedure EDIT_LIST::text_editor() ====================*/


/*========== Attempts to Convert a Book File into Normal Text =============*/
boolean convert_book (DAGBOOK_TYPE *book) {
  int i, j;	/* Loop Counter */
  unsigned int size;
  unsigned char *temp_ptr;
  boolean done_line = FALSE;

  char buffer[101], buf[8];	/* Temp Input Buffer */

	/* Parse each page at a time */
  for (i = 0; i < book->num_pages; i++) {
    temp_ptr = (unsigned char *) book->pages[i];
    buffer[0] = 0;
    size = (unsigned int) (book->page_offsets[i + 1] - book->page_offsets[i]);
    j = 0;

	/* Parse characters at a time */
    while (temp_ptr && j < size) {

      switch (*temp_ptr) {
	case 0xFD:	/* Means next line is centered on screen */
	  chrcat (buffer, CENTER_CHAR);
	  break;
	case 0x01:	/* Unknown */
	  strcat (buffer, "#01");
	  break;
	case 0xFB:	/* Unknown */
	  strcat (buffer, "#FB");
	  temp_ptr++;
	  j++;
	  sprintf (buf, "#%02X", *temp_ptr);
	  temp_ptr++;
	  j++;
	  strcat (buffer, buf);
	  break;
	case 0xF9:	/* Font selection char */
	  temp_ptr++;
	  j++;

	  if (*temp_ptr == 0x02)
	    strcat (buffer, FANCY_FONT_STRING);
	  else
	    strcat (buffer,  NORMAL_FONT_STRING);

	  break;
	case 0xF6:	/* Pause Output, end of page */
	  chrcat (buffer, END_PAGE_CHAR);
	  done_line = TRUE;
	  break;
	case 0:		/* End of line */
	  done_line = TRUE;
	  break;
	default:	/* Normal Character */
	  chrcat (buffer, *temp_ptr);
	  break;
       } /* End of switch */

      if (strlen(buffer) >= MAX_EDIT_LENGTH - 1) {
	chradd (buffer, MAX_EDIT_LENGTH, 0);
	editor.add_tail (buffer);
	memccpy (&buffer[0], &buffer[MAX_EDIT_LENGTH], 0, strlen(&buffer[MAX_EDIT_LENGTH + 1]));
       }

      if (done_line || strlen(buffer) >= MAX_EDIT_LENGTH - 1) {
	editor.add_tail (buffer);
	buffer[0] = 0;
	done_line = FALSE;
       }

      temp_ptr++;
      j++;
     } /* End of while loop */

   } /* End of for loop */

  return (TRUE);
 }
/*========== End of Function convert_dagbook() ============================*/


/*========== Attempts to Convert some Text into a Book File ===============*/
boolean convert_text (DAGBOOK_TYPE *book) {
  EDIT_TYPE *temp_line;	/* Temp Pointer */
  int i;                /* Loop Counter */
  unsigned int buflen, pre_size = 0;
  unsigned long size;
  char *temp_ptr;
  boolean done_page = FALSE, done;
  char *buffer;	/* Temp Input Buffer */
  char buf[8];

	/* Allocate the temp input buffer */
  buffer = create_ptr (64000u);

	/* Clear the book */
  book->destroy();
  temp_line = editor.edit_head;
  buffer[0] = 0;
  size = 0;
  buflen = 0;

	/* Write until one page is found... */
  while (temp_line) {
    temp_ptr = temp_line->data;
    done = FALSE;

	/* Parse each line, a character at a time */
    while (temp_ptr && !done) {
      switch (*temp_ptr) {
	case 0:			/* EOL */
	  buffer[buflen] = 0;
	  done = TRUE;
	  break;
	case END_PAGE_CHAR:	/* Start New Page */
	  buffer[buflen] = 0xF6;

	  if (*(temp_ptr + 1) == 0) {
	    temp_ptr++;
	    done = TRUE;
	   }

	  done_page = TRUE;
	  break;
	case CENTER_CHAR:	/* Center line character */
	  buffer[buflen] = 0xFD;
	  break;
	case HEX_CHAR:		/* Undefined Hexdecimal string */

			/* Make sure its a valid hex... */
	  if (isxdigit(*(temp_ptr + 1)) && isxdigit(*(temp_ptr + 2))) {
	    buf[0] = '0';
	    buf[1] = 'x';
	    buf[2] = *(temp_ptr + 1);
	    buf[3] = *(temp_ptr + 2);
	    buf[4] = 0;
	    buffer[buflen] = (unsigned char) strtol (buf, NULL, 0);
	    temp_ptr += 2;
	   }
	  else
	    chrcat (buffer, *temp_ptr);

	  break;
	case '{':	/* Font Selected Character */
	  if (strnicmp (temp_ptr, FANCY_FONT_STRING, strlen(FANCY_FONT_STRING)) == 0) {
	    buffer[buflen++] = 0xF9;
	    buffer[buflen] = 0x02;
	    temp_ptr += strlen(FANCY_FONT_STRING) - 1;
	   }
	  else if (strnicmp (temp_ptr, NORMAL_FONT_STRING, strlen(NORMAL_FONT_STRING)) == 0) {
	    buffer[buflen++] = 0xF9;
	    buffer[buflen] = 0x04;
	    temp_ptr += strlen(NORMAL_FONT_STRING) - 1;
	   }
	  else
	    buffer[buflen] = *temp_ptr;

	  break;

	default:	/* Regular character */
	  buffer[buflen] = *temp_ptr;
	  break;
       } /* End of switch */

      temp_ptr++;
      buflen++;

		/* Create a new page */
      if (done_page || buflen > 64000u) {
	done_page = FALSE;
	book->pages[book->num_pages] = create_ptr(buflen + 1);
	memcpy (book->pages[book->num_pages], buffer, buflen + 1);
	book->page_offsets[book->num_pages] = (unsigned long) pre_size;
	buffer[0] = 0;
	pre_size = buflen;
	buflen = 0;
	book->num_pages++;
       }
     }

    temp_line = temp_line->next;
   } /* End of While Loop */

	/* Write the last Page, if any */
  if (buflen > 0) {
    book->pages[book->num_pages] = create_ptr(buflen + 1);
    memcpy (book->pages[book->num_pages], buffer, buflen + 1);
    book->page_offsets[book->num_pages] = (unsigned long) pre_size;
    buffer[0] = 0;
    pre_size = buflen;
    buflen = 0;
    book->num_pages++;
   }

	/* Fix the offsets */
  book->page_offsets[book->num_pages] = (unsigned long) pre_size;
  size = book->num_pages * 4 + 10 + 64 + 160 + 2;

  for (i = 0; i <= book->num_pages; i++) {
    size += book->page_offsets[i];
    book->page_offsets[i] = size;
   }

	/* Free Allocated Memory */
  delete buffer;

  return (TRUE);
 }
/*========== End of Function convert_text() ===============================*/


/*========== Cut Copy and Paste Menu Functions ============================*/
void editmenu_copy (__CPPARGS) {
  editor.copy();
 }

void editmenu_cut (__CPPARGS) {
  editor.cut();
 }

void editmenu_paste (__CPPARGS) {
  editor.paste();
 }
/*========== End of Menu Cut/Copy/Paste Functions =========================*/


/*========== Inserts the Font Fancy String ================================*/
void editmenu_fontfancy (__CPPARGS) {
  editor.add_string (FANCY_FONT_STRING);
 }
/*========== End of Procedure editmenu_fontfancy() ========================*/


/*========== Inserts the Font Normal String ===============================*/
void editmenu_fontnormal (__CPPARGS) {
  editor.add_string (NORMAL_FONT_STRING);
 }
/*========== End of Procedure editmenu_fontnormal() =======================*/


/*========== Misc Menu Functions ==========================================*/
void editmenu_center (__CPPARGS) {
  editor.current_char = 0;

  if (editor.edit_current && editor.edit_current->data[0] != CENTER_CHAR) {
    editor.add_char (CENTER_CHAR);
    editor.draw_lines (DRAW_CURRENT);
   }
 }

void editmenu_endpage (__CPPARGS) {
  editor.break_line (FALSE);
  editor.add_current_line (END_PAGE_STRING);
  editor.draw_status_line();
  editor.draw_lines (DRAW_PREVIOUS);
 }
/*========== End of Misc Menu Functions ===================================*/


/*========== The Help window ==============================================*/
void editmenu_help(__CPPARGS) {

  _setcursortype (_NOCURSOR);
  textcolor (BIGWIN_BORD_COLOR);
  textbackground (BIGWIN_BACK_COLOR);

	/* Draw the window */
  draw_textbox (1, 1, 80, 25, "HELP: BookEdit - DF Book Text Editor", BIGWIN_TEXT_COLOR);

  hide_mouse();

  textcolor (BLUE);
  cprintf_centx (3, "BookEdit v%s - Text Editor - by Dave Humphrey", BOOKEDIT_VERSION);

  textcolor (BIGWIN_TEXT_COLOR);
  gotoxy (4, 5);
  cprintf ("The text editor used in DF BookEdit is a basic one, much like the DOS");
  gotoxy (4, 6);
  cprintf ("editor Edit. Mouse and key combinations are supported and special text");
  gotoxy (4, 7);
  cprintf ("codes are displayed in different colors. Read the manual document");
  gotoxy (4, 8);
  cprintf ("BOOKEDIT.TXT for more information.");

  gotoxy (5, 10);
  cprintf ("Editor Key and Mouse Commands");
  gotoxy (8, 11);
  cprintf ("%c%c%c%c / PgUP / PgDN / Home / End / Mouse - Move around text area", 27, 24, 25, 26);
  gotoxy (8, 12);
  cprintf ("F1 - This Help Screen           ALT+Q - Quit Text Editor");
  gotoxy (8, 13);
  cprintf ("F2 - Save Book File             ALT+F - Activate File Menu");
  gotoxy (8, 14);
  cprintf ("F3 - Load Book File             CTRL+Y - Delete Current Line");

  gotoxy (5, 16);
  cprintf ("Special Color Text Codes");

  gotoxy (8, 17);
  textcolor (FONT_TEXT_COLOR);
  textbackground (FONT_BACK_COLOR);
  cprintf ("{FancyFont} {NormFont}");
  textcolor (BIGWIN_TEXT_COLOR);
  textbackground (BIGWIN_BACK_COLOR);
  cprintf (" - Font Type");

  gotoxy (48, 17);
  textcolor (END_PAGE_TEXT_COLOR);
  textbackground (END_PAGE_BACK_COLOR);
  cprintf ("%c", END_PAGE_CHAR);
  textcolor (BIGWIN_TEXT_COLOR);
  textbackground (BIGWIN_BACK_COLOR);
  cprintf (" - End of Page (Max 23 Lines)");

  gotoxy (8, 18);
  textcolor (CR_TEXT_COLOR);
  textbackground (CR_BACK_COLOR);
  cprintf ("%c", CENTER_CHAR);
  textcolor (BIGWIN_TEXT_COLOR);
  textbackground (BIGWIN_BACK_COLOR);
  cprintf (" - A Centered Text Line");

  gotoxy (48, 18);
  textcolor (HEX_TEXT_COLOR);
  textbackground (HEX_BACK_COLOR);
  cprintf ("%c00", HEX_CHAR);
  textcolor (BIGWIN_TEXT_COLOR);
  textbackground (BIGWIN_BACK_COLOR);
  cprintf (" - Undefined Text Code");

  gotoxy (5, 20);
  cprintf ("Initial / Current Available Memory: %ld / %ld bytes", initial_far_heapsize, farcoreleft());
  gotoxy (5, 21);
  cprintf ("Status of Far Heap: ");

  switch (farheapcheck()) {
    case _HEAPCORRUPT: cprintf ("Heap has been Corrupted - You Should Reboot!"); break;
    case _HEAPOK: cprintf ("Heap OK"); break;
    default: cprintf ("No heap"); break;
   };

  textcolor (RED);
  cprintf_centx (23, "Press Any Key to Continue...");
  textcolor (BIGWIN_TEXT_COLOR);
  textbackground (BIGWIN_BACK_COLOR);

  show_mouse();
  getch();
 }
/*========== End of procedure void editmenu_help() ========================*/


/*========== Does the Load Text Menu Command ==============================*/
void editmenu_load (__CPPARGS) {
  char *buffer;

	/* Make sure not to overwrite things */
  if (editor.modified && !get_yn_choice("File Not Saved", "Current File Not Saved! Continue"))
    return;

	/* Allocate temp buffer and set to nothing*/
  buffer = create_ptr (MAX_EDIT_LENGTH + 1);
  buffer[0] = 0;

	/* Input filename to load */
  if (file_dialogue(buffer, &text_path[0], "Load Text", "Load", "*.txt")) {

		/* If successful, try and load the file... */
    if (load_editfile (buffer)) {
      if (editor.edit_filename) delete editor.edit_filename;
      editor.edit_filename = create_ptr (buffer);
      editor.file_loaded = TRUE;
      editor.modified = FALSE;
     }
    else {
      error_box ("ERROR: Load Text", "Could not Load file '%s'!\n%s", buffer, dagbutil_errmsg[dagb_error]);
     }
   }

  delete buffer;
  editor.draw_lines (DRAW_ALL);
 }
/*========== End of procedure editmenu_load() =============================*/


/*========== Does load command for a DF Book File =========================*/
void editmenu_loadbook (__CPPARGS) {
  char *buffer;

	/* Make sure not to overwrite things */
  if (editor.modified && !get_yn_choice("File Not Saved", "Current File Not Saved! Continue"))
    return;

	/* Allocate temp buffer and set to nothing*/
  buffer = create_ptr (MAX_EDIT_LENGTH + 1);
  buffer[0] = 0;

	/* Input filename to load */
  if (file_dialogue(buffer, &text_path[0], "Load Book", "Load", "*.txt")) {

	/* Clear the current book structure */
    dagbook.destroy();

		/* If successful, try and load the file... */
    if (dagbook.load_book(buffer)) {
      if (editor.edit_filename) delete editor.edit_filename;
      editor.edit_filename = create_ptr (buffer);

		/* Need to clear the text area... */
      editor.destroy();

		/* Convert the book to a useable format */
      convert_book(&dagbook);

      editor.file_loaded = TRUE;
      editor.modified = FALSE;
     }
    else {
      error_box ("ERROR: Load Book Text", "Could not Load Book File '%s'!\n%s", buffer, dagbutil_errmsg[dagb_error]);
     }

	/* Destroy the book text now that we are done */
    dagbook.destroy();
   }

  delete buffer;
  editor.draw_lines (DRAW_ALL);
 }
/*========== End of procedure editmenu_loadbook() =========================*/


/*========== Does new command =============================================*/
void editmenu_new (__CPPARGS) {

	/* Clear the array */
  editor.destroy();

	/* Init the list */
  editor.init();

	/* Redraw Screen... */
  editor.draw_lines (DRAW_ALL);

	/* Will also need to destroy and reinit the dagbook structure */
  dagbook.destroy();
  dagbook.init();

  return;
 }
/*========== End of procedure editmenu_new() ==============================*/


/*========== Does quit command ============================================*/
void editmenu_quit (__CPPARGS) {
  if (editor.modified) {
    if (get_yn_choice("Exit BookEdit", "File Not Saved! Do you Really Wish to Exit"))
      quit = TRUE;
   }
  else
    quit = TRUE;

 }
/*========== End of procedure editmenu_quit() =============================*/


/*========== Does save command ============================================*/
void editmenu_save (__CPPARGS) {
  char *buffer;

	/* Allocate Temp Buffer */
  buffer = create_ptr (MAX_EDIT_LENGTH + 1);

	/* If file is loaded in memory, set filename */
  if (editor.file_loaded && editor.edit_filename)
    strcpy(buffer, editor.edit_filename);
  else
    *buffer = 0;	/* Set filename to nothing if nothing loaded */

	/* Get filename to save text as... */
  if (file_dialogue(buffer, &text_path[0], "Save Text", "Save", "*.txt")) {

	/* If input is sucessful, save file... */
    if (save_editfile(buffer)) {
      if (editor.edit_filename) delete editor.edit_filename;
      editor.edit_filename = create_ptr (buffer);
      editor.file_loaded = TRUE;
      editor.modified = FALSE;
     }
    else {
      error_box ("ERROR: Save Text", "Could not Save file '%s'\n%s", buffer, dagbutil_errmsg[dagb_error]);
     }
   }

  delete buffer;
 }
/*========== End of procedure editmenu_save() =============================*/


/*========== Saves a Book File ============================================*/
void editmenu_savebook (__CPPARGS) {
  char *buffer;

	/* Allocate Temp Buffer */
  buffer = create_ptr (MAX_EDIT_LENGTH + 1);

	/* If file is loaded in memory, set filename */
  if (editor.file_loaded && editor.edit_filename)
    strcpy(buffer, editor.edit_filename);
  else
    *buffer = 0;	/* Set filename to nothing if nothing loaded */

	/* Get filename to save text as... */
  if (file_dialogue(buffer, &text_path[0], "Save Book Text", "Save", "*.txt")) {

	/* First, convert the text into the DAGBOOK format */
    convert_text (&dagbook);

	/* If input is sucessful, save file... */
    if (dagbook.save_book(buffer)) {
      if (editor.edit_filename) delete editor.edit_filename;
      editor.edit_filename = create_ptr (buffer);
      editor.file_loaded = TRUE;
      editor.modified = FALSE;
     }
    else {
      error_box ("ERROR: Saving Book Text", "Could not Save Book File '%s'!\n%s", buffer, dagbutil_errmsg[dagb_error]);
     }

	/* Destroy the bookfile now that we are done with it */
     dagbook.destroy();
   }

  delete buffer;
 }
/*========== End of procedure editmenu_savebook() =========================*/


/*========== Change the Book Author =======================================*/
void editmenu_author (__CPPARGS) {
  char buffer[41];

  strncpy (buffer, dagbook.author, 40);
  buffer[40] = 0;

  if (get_input (buffer, 40, 50, 12, "Change Book Author", "Enter the New Author for the Book"))
    strcpy (dagbook.author, buffer);
 }
/*========== End of Procedure editmenu_author() ===========================*/


/*========== Change the Book Title ========================================*/
void editmenu_title (__CPPARGS) {
  char buffer[41];

  strncpy (buffer, dagbook.title, 40);
  buffer[40] = 0;

  if (get_input (buffer, 40, 50, 12, "Change Book Title", "Enter the New Title for the Book"))
    strcpy (dagbook.title, buffer);
 }
/*========== End of Procedure editmenu_title() ============================*/


/*========== Attempts to load a Text File =================================*/
boolean load_editfile (char far *filename) {
  FILE *f;	/* File pointer */
  int i;	/* Loop counter */
  char ch;	/* Temp string buffer */
  char buffer[MAX_EDIT_LENGTH + 1];

	/* Attempt to open the file for reading */
  if (!(f = openfile (filename, "rt")))
    return (FALSE);

	/* Clear the array */
  editor.destroy();
  buffer[0] = 0;
  i = 0;

  do {
    ch = fgetc(f);	/* Read a character */

    if (ch == EOF) {	/* End of file... */
      if (i > 0) editor.add_tail(buffer);
      break;
     }
    else if (ch == 9) { /* Tab Character */
      if (i + 5 >= MAX_EDIT_LENGTH) { /* Make sure we don't exceed the maximum length */
	editor.add_tail(buffer);
	strcpy (buffer, "     ");
	i = 5;
       }
      else { /* Simply add onto end */
	strcat (buffer, "     ");
	i += 5;
       }
     }
    else if (ch == 10) { /* CR */
      editor.add_tail(buffer);
      buffer[0] = 0;
      i = 0;
     }
    else if (i < MAX_EDIT_LENGTH) { /* Append to end of buffer */
      buffer[i] = ch;
      i++;
      buffer[i] = 0;
     }
    else { /* End of line */
      editor.add_tail(buffer);
      buffer[1] = 0;
      buffer[0] = ch;
      i = 1;
     }

  } while(TRUE);	/* Read file until EOF is read */

  fclose (f);
  return (TRUE);
 }
/*========== End of procedure load_editfile() =============================*/


/*========== Attempts to save a text file =================================*/
boolean save_editfile (char *filename) {
  FILE *f;		/* File pointer */
  EDIT_TYPE far *line;	/* Temp pointer */

	/* Does file exist? */
  if (fileexists(filename)) {

	/* Make sure user wants to overwrite file... */
    if (!get_yn_choice("Warning: File Exists", "File '%s' exists, Overwrite", filename))
      return (FALSE);

   }

	/* Attempt to open file for writing */
  if (!(f = openfile (filename, "wt")))
    return (FALSE);

  line = editor.edit_head;

  do {	/* Save all the data */
    fprintf (f, "%s\n", line->data);
    line = line->next;
  } while(line != NULL);

  fputc (EOF, f);	/* Write the End-Of-File marker */

  fclose (f);
  return(TRUE);
 }
/*========== End of procedure save_editfile() =============================*/


/*========== Routines run when program is stopped =========================*/
void exit_bookedit(void) {

  textmode(C80);	/* Normal text mode */
  printf ("Returned to textmode (80x25)\n");

	/* Reset to original drive and directory */
  setdisk (orig_disk);

  if (chdir (orig_dir) == -1)
    printf ("Failed to restore original path!\n");

	/* Replace our own keyboard interrupt routine with the original */
  delete_keyboard ();
  printf ("Replaced original keyboard driver\n");

  printf ("Freeing allocated memory\n");
  editor.destroy();
  editor.clip_destroy();
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

  printf ("Finished clean-up of BOOKEDIT v%s\n\n", BOOKEDIT_VERSION);
 }
/*========== End of procedure exit_bookedit() =============================*/


/*========== Initializes everything =======================================*/
void init_bookedit (void) {

	/* No scrolling of text on screen */
  _wscroll = 0;

	/* Save initial available memory */
  initial_far_heapsize = farcoreleft();

	/* Define exit procedure */
  atexit (exit_bookedit);

	/* Set the maximum string length */
  max_string_length = MAX_STRING_LENGTH;

	/* Intialize the Default Buttons in UTIL.CPP */
  init_buttons ();

	/* Define the strings used in case of a bug... */
  err_header_string = &bookedit_title[0];
  err_header_version = &bookedit_version[0];

	/* Print opening title.... */
  printf ("BOOKEDIT v%s - Copyright (c) 1997 - by Dave Humphrey\n\n", BOOKEDIT_VERSION);

	/* Save the current directory and drive...for reseting at end and
	 * goto intial directory */
  if (getcwd (&orig_dir[0], MAX_STRING_LENGTH) == NULL)
    printf ("ERROR: Path too Long, Path length cannot exceed 80 bytes!\n\n");

  orig_disk = getdisk();

	/* Replace some DOS error handling routines with our own */
  harderr (dos_err_handler);
  ctrlbrk (ctrl_brk_handler);
  setcbrk(1);
  printf ("Replaced DOS error handler and CTRL-BRK routines\r\n");

	/* Replace keyboard interrupt with our own routine */
  printf ("Replace default keyboard interrupt\r\n");
  install_keyboard ();
  ClearKeys();
  printf ("     Keyboard interrupt replaced\n");

	/* Initialize the Mouse Driver */
  printf ("Initialize Mouse Driver\r\n");
  if (!mouse_init()) bug ("Could not initialize mouse driver!", NULL);

	/* Some Mouse Cursor Stuff */
  asm {
    mov ax, 0x0A
    mov bx, 0
    mov cx, 0xFF00
    mov dx, 0x0108
    int 0x33
   };

  printf ("    Mouse Driver Initialized\r\n");

	/* Goto various text modes and Show mouse cursor */
  if (high_textmode) {
    textmode(64);
    mouse_window (0, 0, 319, 299);
   }
  else
    mouse_window (0, 0, 319, 199);

  show_mouse ();
 }
/*========== End of procedure init_bookedit() =============================*/


/*========== Begin Main Procedure =========================================*/
boolean main (void) {

  open_log_file ("bookedit.log");
  init_bookedit();

	/* Attempt to change directory */
//  chdir ("arena2\\books");


  editor.text_editor ("DF Book Editor");

  return (TRUE);
 }
/*========== End Main Program =============================================*/
