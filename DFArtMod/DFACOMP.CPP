extern unsigned _stklen = 8000u;



	/* Standard C Includes */
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <time.h>
#include <alloc.h>
#include <string.h>
#include <process.h>
#include <dos.h>

	/* User Defined Includes */
#include "fileutil.h"
#include "guitext.h"
#include "daggen.h"
#include "dagfile.h"
#include "dagdata.h"
#include "dagfile1.h"
#include "dfacomp.h"
#include "dfart.h"

	/* For converting things */
#define ENCHANT_CON(e) (((unsigned long) ((signed char)(e & 0xff))) + (((unsigned long) (e >> 8)) << 16))

	/* Holds all the important data */
AMD_TYPE amdfile;

	/* The GUI Components */
WINDOW_TEXT art_window      ( 1,  1, 80, 50, DFACOMP_VERSION, def_colors);
WINDOW_TEXT text_window     ( 1,  1, 80, 50, DFACOMP_VERSION, def_colors);
WINDOW_TEXT addtext_window  (25, 15, 19, 17, "Add Text",      altdef_colors);
WINDOW_TEXT enchant_window  (10, 10, 56, 24, "Edit Enchantments", altdef_colors);

TEXTBOX_TEXT textbox (50, 15, text_colors);

	/* Some buttons for all the various windows */
BUTTON_TEXT addnew_button    (12, 3, "Add New",    NULL, NULL, button_colors),
	    addfile_button   (12, 3, "Add File",   NULL, NULL, button_colors),
	    addcancel_button ( 8, 1, "Cancel",     NULL, NULL, altbut_colors),
	    toggle_button    (16, 3, "Edit Texts",     NULL, menu_toggle, button_colors),
	    toggle1_button   (16, 3, "Edit Artifacts", NULL, menu_toggle, button_colors),
	    check_button     ( 8, 3, "Check",      NULL, menu_check,  button_colors),
	    check1_button    ( 8, 3, "Check",      NULL, menu_check,  button_colors),
	    load_button      ( 8, 3, "Load",       NULL, menu_load, button_colors),
	    load1_button     ( 8, 3, "Load",       NULL, menu_load, button_colors),
	    save_button      ( 8, 3, "Save", 	   NULL, menu_save, button_colors),
	    save1_button     ( 8, 3, "Save",       NULL, menu_save, button_colors),
	    exit_button      ( 8, 3, "Exit",       NULL, menu_exit, button_colors),
	    exit1_button     ( 8, 3, "Exit",       NULL, menu_exit, button_colors),
	    new_button       ( 8, 3, "New",        NULL, menu_new,  button_colors),
	    new1_button      ( 8, 3, "New",        NULL, menu_new,  button_colors),
	    next_button      ( 8, 3, "Next",       NULL, menu_next, button_colors),
	    prev_button      ( 8, 3, "Prev",       NULL, menu_prev, button_colors),
	    add_button       ( 8, 3, "Add",        NULL, menu_addart,  button_colors),
	    add1_button      ( 8, 3, "Add", 	   NULL, menu_addtext,  button_colors),
	    del_button       ( 8, 3, "Del",        NULL, menu_del,  button_colors),
	    del1_button      ( 8, 3, "Del",        NULL, menu_del,  button_colors),
	    ok_button	     ( 8, 3, "Ok",         NULL, NULL,      altbut_colors),
	    cancel_button    ( 8, 3, "Cancel",     NULL, NULL,      altbut_colors),
	    enchant_but[10];

	/* Various labels for the windows */
LABEL_TEXT text_label1   ("Texts in Current Project", label_colors),
	   enchant_label ("Enchantments",             label_colors),
	   index_label   ("Artifact   0 of   0 ",     altlabel_colors);

	/* Text input fields for the various windows */
TEXT_FIELD_TEXT editindex_text ( 5, "Text Index:", alttext_colors),
		name       (30, "Name:", text_colors),
		usage      ( 8, "Uses:", text_colors),
	    enchant_label2 (30, "Change From:", gray_colors),
		text_index ( 5, "Index:", text_colors),
		enchant[10];

	/* Various lists */
LIST_TEXT enchant_list(40, 16, list_colors),
	  index_list  (10, 15, list_colors);

DRAGLIST_TEXT type     (30, 10, "Type:",     FALSE, draglist_colors),
	      material (12, 10, "Material:", FALSE, draglist_colors);

	/* Holds the enchantment values for the current artifact being editted */
short enchant_values[10];

	/* Index number used for referencing GUI objects */
short enchant_index[10];
short item_group, item_subgroup, indexlist_index;
short addnew_index, addfile_index, addcancel_index;
short enchantlist_index, ok_index, cancel_index;

	/* Main loop counter and redraw variable */
boolean done = FALSE;
boolean redraw = FALSE;

	/* Has the current data been saved? */
boolean saved = TRUE;

	/* Which window display are we looking at? (artifact / text) */
boolean display_texts = FALSE;

	/* Which artifact are we currently looking at? */
short current_record = 0, current_text = 0;


/*========== Creates the Text Lists From Scratch ==========================*/
void create_textlists (void) {
  char buffer[21];
  short j, i;

  j = index_list.selected;
  index_list.clear_list();

  for (i = 0; i < amdfile.num_texts; i++) {
    sprintf (buffer, "%d", amdfile.text_index[i]);
    index_list.add_tail (buffer);
   }

  if (display_texts)
    index_list.move_abs(j);
  else {
    index_list.visible = FALSE;
    index_list.move_abs(j);
    index_list.visible = TRUE;
   }
 }
/*========== End of Procedure create_textlists() ==========================*/


/*========== Initializes the GUI Objects and Various other Things =========*/
void dfacomp_init (void) {
  short i;	/* Loop counter */
  long l;

	/* Load the data file */
  load_dag_data ("dagdata.dat", DFFL_ALL);

	/* Create the text mouse */
  create_text_mouse();

	/* Create the Daggerfall specific errors */
  post_df_errors();

	/* Create the add text window */
  addnew_index = addtext_window.open (3, 3, &addnew_button);
  addfile_index = addtext_window.open (3, 8, &addfile_button);
  addcancel_index = addtext_window.open (5, 13, &addcancel_button);

	/* Define the text editting window */
  text_window.open (34, 42, &check1_button);
  text_window.open (46, 42, &toggle1_button);
  text_window.open (68, 15, &new1_button);
  text_window.open (68, 20, &add1_button);
  text_window.open (68, 25, &del1_button);
  text_window.open (68, 32, &load1_button);
  text_window.open (68, 37, &save1_button);
  text_window.open (68, 42, &exit1_button);
  text_window.open ( 5, 16, &text_label1);
  indexlist_index = text_window.open (4, 18, &index_list);
  text_window.open (16, 18, &textbox);

	/* Define the artifact editting window */
  art_window.open (34, 42, &check_button);
  art_window.open (46, 42, &toggle_button);
  art_window.open (68,  3, &next_button);
  art_window.open (68,  8, &prev_button);
  art_window.open (68, 15, &new_button);
  art_window.open (68, 20, &add_button);
  art_window.open (68, 25, &del_button);
  art_window.open (68, 32, &load_button);
  art_window.open (68, 37, &save_button);
  art_window.open (68, 42, &exit_button);

  art_window.open (16,  4, &text_index);
  art_window.open (16,  6, &name);
  art_window.open (16,  8, &type);
  art_window.open (16, 10, &usage);
  art_window.open (16, 12, &material);
  art_window.open ( 5, 16, &enchant_label);
  art_window.open ( 3, 45, &index_label);

  for (i = 0; i < 10; i++) {
    enchant[i].init (1, 1, 40, "");
    enchant[i].enabled = FALSE;
    enchant_but[i].init (1, 1, 8, 1, FALSE, "Change", NULL, BLACK, BLACK, GREEN);
    art_window.open (10, 18 + i*2, &enchant[i]);
    enchant_index[i] = art_window.open (52, 18 + i*2, &enchant_but[i]);
   }

	/* Create the enchantment editting window */
  enchantlist_index = enchant_window.open (3, 3, &enchant_list);
  enchant_window.open (16, 20, &enchant_label2);
  ok_index = enchant_window.open (45, 5, &ok_button);
  cancel_index = enchant_window.open (45, 13, &cancel_button);

	/* Create the required lists */
  material.add_dual_tail ("Iron", 0);
  material.add_dual_tail ("Steel", 1);
  material.add_dual_tail ("Silver", 2);
  material.add_dual_tail ("Elven", 3);
  material.add_dual_tail ("Dwarven", 4);
  material.add_dual_tail ("Mithril", 5);
  material.add_dual_tail ("Adamantium", 6);
  material.add_dual_tail ("Ebony", 7);
  material.add_dual_tail ("Orcish", 8);
  material.add_dual_tail ("Daedric", 9);

  for (i = 0; i < num_enchants; i++) {
    l = ((unsigned long) enchants[i]->type) + ((((unsigned long) enchants[i]->subtype) << 16) & 0xFFFF0000l);
    enchant_list.add_dual_sort (enchants[i]->name, l);
   }

  enchant_list.add_dual_tail ("<none>", -1);


  for (i = 0; i < num_items; i++) {
    type.add_dual_sort (items[i]->name, i);
   }

  type.add_dual_tail ("? Unknown", -1);

	/* Set the screen mode */
  textmode (C4350);
  _setcursortype (_NOCURSOR);
 }
/*========== End of Procedure dfacomp_init() ==============================*/


/*========== Edits the Enchantment Type ===================================*/
long edit_enchantment (const long enchant) {
  GUI_RETURN ret;
  boolean done = FALSE;		/* Loop counter */
  boolean ok = TRUE;		/* Return success? */
  short j;
  long l;

  enchant_window.get_back();

	/* Set the initial list value */
  j = find_enchantment (enchant);

  if (j == -1) {
    enchant_list.move_abs(0);
    enchant_label2.change_text ("??? Unknown Enchantment ???");
   }
  else {
    enchant_list.move_abs(j);
    enchant_label2.change_text (enchants[j]->name);
   }

  enchant_window.draw();

  do {
	/* Check for input */
    keyboard.check();
    mouse->check();

	/* Check for events... */
    ret = enchant_window.event_loop();

    if (ret.code == GUIRET_EVENT) {
      if (ret.index == ok_index || ret.index == enchantlist_index) {
	done = TRUE;
       }
      else if (ret.index == cancel_index) {
	done = TRUE;
	ok = FALSE;
       }

     }

  } while (!done);

	/* Replace the background */
  enchant_window.put_back();

	/* Return the appropiate enchant code */
  if (ok) {
    l = enchant_list.get_dual_value();
    return (l);
   }
  else
    return (enchant);

 }
/*========== End of Function edit_enchantment() ===========================*/


/*========== Gets Values from the GUI Objects and saves to memory =========*/
void get_values (void) {
  AMD_RECORD *ptr = amdfile.records[current_record];
  char *buf;
  short i;

	/* There may be changes made */
  saved = FALSE;

	/* Are we in the text editting mode? */
  if (display_texts) {
    if (amdfile.num_texts == 0)
      return;

	/* Clear the contents of the AMD text record */
    i = current_text;
    buf = textbox.get_text();

    if (!buf)
      error_window ("Text Error", "Error retrieving text from window!");
    else {
      if (amdfile.texts[i]) delete amdfile.texts[i];
      amdfile.texts[i] = buf;
     }

    return;
   }

	/* Make sure the current record is valid */
  if (current_record >= amdfile.num_records || current_record < 0 || !ptr)
    return;

  strcpy (ptr->name, name.text);
  ptr->index = (short) strtoul (text_index.text, NULL, 0);
  ptr->magic_type = 2;
  ptr->u1 = 0xC350;
  ptr->uses = (short) strtoul(usage.text, NULL, 0);
  ptr->material = (char) material.get_value();

  for (i = 0; i < 10; i++)
    ptr->enchantments[i] = enchant_values[i];

  i = type.get_value();

  if (i == -1) {
    ptr->group = item_group;
    ptr->subgroup = item_subgroup;
   }
  else {
    ptr->group = items[i]->group;
    ptr->subgroup = items[i]->sub_group;
   }
 }
/*========== End of Procedure get_values() ================================*/


/*========== Posts values from memory to GUI objects ======================*/
void post_values (void) {
  AMD_RECORD *ptr;
  char buffer[41];
  short i, j;
  long l;

	/* Are we in the text editting mode? */
  if (display_texts) {
    current_text = index_list.selected;

    if (amdfile.num_texts == 0 || index_list.selected < 0) {
      textbox.enabled = FALSE;
      textbox.destroy();
      return;
     }

    textbox.enabled = TRUE;
    textbox.parse_text (amdfile.texts[index_list.selected]);
    return;
   }

  sprintf (buffer, "Artifact %3d of %3d ", current_record + 1, amdfile.num_records);
  index_label.change_text (buffer);

	/* Make sure the current record is valid */
  if (current_record >= amdfile.num_records || current_record < 0) {
    text_index.change_text ("");
    text_index.enabled = FALSE;
    name.change_text ("");
    name.enabled = FALSE;
    type.change_text ("");
    type.enabled = FALSE;
    usage.change_text ("");
    usage.enabled = FALSE;
    material.change_text ("");
    material.enabled = FALSE;
    next_button.enabled = FALSE;
    prev_button.enabled = FALSE;

    for (i = 0; i < 10; i++) {
      enchant[i].change_text ("");
      enchant_but[i].enabled = FALSE;
     }

    return;
   }

  text_index.enabled = TRUE;
  name.enabled = TRUE;
  type.enabled = TRUE;
  usage.enabled = TRUE;
  material.enabled = TRUE;
  next_button.enabled = TRUE;
  prev_button.enabled = TRUE;

	/* Assign the value to a pointer for speed and short code */
  ptr = amdfile.records[current_record];

	/* Copy the name to the text field */
  name.change_text (ptr->name);

	/* Copy the index of the artifact */
  sprintf (buffer, "%d", ptr->index);
  text_index.change_text (buffer);

	/* Number of uses */
  sprintf (buffer, "%d", ptr->uses);
  usage.change_text (buffer);

	/* Set the material list */
  material.change_selected(ptr->material);
  material.change_text (material.get_name());

	/* Attempt to find the item type in the known list */
  i = find_item ((short) ptr->group, (short) ptr->subgroup);
  item_group = ptr->group;
  item_subgroup = ptr->subgroup;

  if (i == -1) {
    sprintf (buffer, "? 0x%02X 0x%02X", ptr->group, ptr->subgroup);
    type.change_text (buffer);
    set_typelist (-1);
   }
  else {
    type.change_text (items[i]->name);
    set_typelist (i);
   }

	/* Set all the enchantments for the artifact */
  for (i = 0; i < 10; i++) {
    enchant_but[i].enabled = TRUE;

    if (ptr->enchantments[i] == -1) {
      enchant[i].change_text ("<none>");
     }
    else {
      l = ENCHANT_CON (ptr->enchantments[i]);
      j = find_enchantment (l);

      if (j == -1) {
	sprintf (buffer, "?? 0x%08lX", l);
	enchant[i].change_text (buffer);
       }
      else
	enchant[i].change_text (enchants[j]->name);

     }

    enchant_values[i] = ptr->enchantments[i];
   }

 }
/*========== End of Procedure post_values() ===============================*/


/*========== Sets the Item Type List according to the groups ==============*/
void set_typelist (const short index) {
  unsigned char *ptr;
  short i = 0, j, k;

  ptr = (unsigned char *) type.get_head();

  while (ptr) {
    j = strlen((char *) ptr) + 1;
    k = (short) ((unsigned short)ptr[j] + ((unsigned short)ptr[j + 1] << 8));

    if (index == k) {
      type.change_selected(i);
      type.change_start(i);
      return;
     }

    ptr = (unsigned char *) type.next();
    i++;
   }

  type.change_selected(0);
  type.change_start(0);
 }
/*========== End of Procedure set_typelist() ==============================*/


/*========== Begin Main Program ===========================================*/
 int main (void) {
  GUI_RETURN ret;
  unsigned long initial_mem = get_memleft();
  short i, j;
  long l;

  open_log_file ("dfacomp.log");
  write_log_entry ("Sizeof(Button_Text) = %d", sizeof(BUTTON_TEXT));

	/* Init the gui objects etc... */
  dfacomp_init();
  art_window.draw();

  current_record = 0;
  post_values();
  redraw = TRUE;

	/* Main event loop */
  do {
    if (redraw) {
      redraw = FALSE;

      if (display_texts) {
	index_list.draw();
	textbox.draw();
	text_window.update_focus();
       }
      else {
	index_label.draw();
	text_index.draw();
	name.draw();
	type.draw();
	usage.draw();
	material.draw();

	for (i = 0; i < 10; i++)
	  enchant[i].draw();

	art_window.update_focus();
       }
     }

	/* Get events */
    keyboard.check();
    mouse->check();

	/* Check GUI events */
    if (display_texts) {
      ret = text_window.event_loop();

      if (ret.index == indexlist_index && ret.code != GUIRET_FALSE) {

	if (ret.code == GUIRET_EVENT) {
	  menu_edittext();
	 }
	else if (ret.code == GUIRET_CHANGE) {
	  get_values();
	  current_text = index_list.selected;
	  post_values();
	  redraw = TRUE;
	 }

	index_list.draw();
       }

     } /* End of if displaying text window */
    else {
      ret = art_window.event_loop();

      if (ret.code == GUIRET_EVENT) {

	for (i = 0; i < 10; i++) {

	  if (enchant_index[i] == ret.index && current_record >= 0 && current_record < amdfile.num_records) {
	    l = ENCHANT_CON(enchant_values[i]);
	    l = edit_enchantment (l);
	    enchant_values[i] = ((unsigned char) (l & 0xFFFF)) + (((unsigned char) (l >> 16))<<8);
	    j = find_enchantment (l);

	    if (j == -1)
	      enchant[i].change_text ("??? Unknown Enchantment ???");
	    else
	      enchant[i].change_text (enchants[j]->name);

	    redraw = TRUE;
	    break;
	   }
	 }
       } /* End of if displaying artifact window */
      else if (ret.code) {
	j = 0;
       }

     }

	/* Check keyboard events */
    if (keyboard.get_alt_key (SCAN_X))
      menu_exit();
    else if (keyboard.get_alt_key (SCAN_S))
      menu_save();
    else if (keyboard.get_alt_key (SCAN_A)) {
      input_window (amdfile.author, 16, "Change Author", "Enter an author's name as you would wish it to appear");
     }
    else if (display_texts) {
     }
    else if (!ret.code) {

      if (keyboard.get_pgup_state())
	menu_next();
      else if (keyboard.get_pgdn_state())
	menu_prev();
      else if (keyboard.get_home_state() && amdfile.num_records > 0) {
	get_values();
	current_record = 0;
	post_values();
	redraw = TRUE;
       }
      else if (keyboard.get_end_state() && amdfile.num_records > 0) {
	get_values();
	current_record = amdfile.num_records - 1;
	post_values();
	redraw = TRUE;
       }
     }


  } while (!done);

  textmode (C80);

	/* Delete allocated memory */
  enchant_list.destroy();
  type.destroy();
  text_window.destroy();
  art_window.destroy();
  enchant_window.destroy();
  addtext_window.destroy();
  material.destroy();
  text_index.destroy();
  textbox.destroy();
  amdfile.destroy();

	/* Display finishing message */
  printf ("Finished %s: Created by Dave Humphrey - aj589@freenet.carleton.ca\n", DFACOMP_VERSION);
  printf ("   Initial / Final Memory = %ld / %ld bytes\n", initial_mem, get_memleft());
  printf ("   Heap is %s\n", get_heapstring());
  return (TRUE);
 }
/*========== End Main Program =============================================*/


/*========== Various Menu/Button Functions ================================*/
void menu_exit (const short, const pos_t, const pos_t) {

  if (saved || (!saved && prompt_window ("Exit DFAComp", "Exiting will lose any changes\nmade since last save, Exit [Y/N]?")))
    done = TRUE;
 }

void menu_new (const short, const pos_t, const pos_t) {
  get_values();

  if (saved || (!saved && prompt_window ("New Project", "This will erase any changes\nmade since last save, Continue [Y/N]?"))) {
    amdfile.destroy();
    create_textlists();
   }

  post_values();
  redraw = TRUE;
 }

void menu_load (const short, const pos_t, const pos_t) {
  char buffer[256];
  short i;

  get_values();

	/* Find the filename to load as */
  if (file_window(amdfile.filename, amdfile.path, "*.amd", "Load AMD", "Load")) {
    strcpy (buffer, amdfile.path);
    i = strlen(buffer) - 1;
    if (buffer[i] != '\\') chrcat (buffer, '\\');
    strcat (buffer, amdfile.filename);

    if (!amdfile.load (buffer))
      error_window ("File Error", "Couldn't load AMD file!\n   %s", get_errmsg(err_code));

    create_textlists();
    post_values();
    redraw = TRUE;
   }

 }

void menu_save (const short, const pos_t, const pos_t) {
  char buffer[256];
  short i;

  get_values();

	/* Find the filename to save as */
  if (file_window(amdfile.filename, amdfile.path, "*.amd", "Save AMD", "Save")) {
    strcpy (buffer, amdfile.path);
    i = strlen(buffer) - 1;
    if (buffer[i] != '\\') chrcat (buffer, '\\');
    strcat (buffer, amdfile.filename);

    if (!amdfile.save (buffer))
      error_window ("File Error", "Couldn't save AMD file!\n   %s", get_errmsg(err_code));
    else {
      msg_window ("File Saved", "File %s was successfully saved", amdfile.filename);
      saved = TRUE;
     }
   }

 }

void menu_next (const short, const pos_t, const pos_t) {
  get_values();
  if (amdfile.num_records == 0) return;
  current_record++;
  if (current_record >= amdfile.num_records) current_record = 0;
  post_values();
  redraw = TRUE;
 }

void menu_prev (const short, const pos_t, const pos_t) {
  get_values();
  if (amdfile.num_records == 0) return;
  current_record--;
  if (current_record < 0) current_record = amdfile.num_records - 1;
  post_values();
  redraw = TRUE;
 }

void menu_addart (const short, const pos_t, const pos_t) {
  get_values();

  if (current_record >= MAX_ARTIFACTS)
    error_window ("Record Error", "Cannot add more than %d artifacts!", MAX_ARTIFACTS);
  else if (!amdfile.add_art_record())
    error_window ("Record Error", "Couldn't add new record!\n   %s", get_errmsg(err_code));
  else
    current_record = amdfile.num_records - 1;

  post_values();
  redraw = TRUE;
 }


void menu_addtext (const short, const pos_t, const pos_t) {
  GUI_RETURN ret;
  boolean done = FALSE, ok = TRUE;
  char buffer[20] = "";
  char path[256] = "", *ptr;
  short i;

  if (amdfile.num_texts >= MAX_ARTIFACTS) {
    error_window ("Record Error", "Cannot add more than %d artifacts!", MAX_ARTIFACTS);
    return;
   }

  get_values();
  addtext_window.get_back();
  addtext_window.draw();

  do {
	/* Get any input */
    keyboard.check();
    mouse->check();

	/* Check giu objects */
    ret = addtext_window.event_loop();

    if (ret.code == GUIRET_EVENT) {
      done = TRUE;

      if (ret.index == addcancel_index)
	ok = FALSE;
      else if (ret.index == addfile_index) {

		/* Find the text file */
	if (file_window (buffer, path, "*.txt", "Choose File", "Ok")) {
	  i = strlen(path) - 1;
	  if (path[i] != '\\') chrcat (path, '\\');
	  strcat (path, buffer);

		/* Attempt to read the file */
	  ptr = read_textfile (path);

		/* Was there an error reading file? */
	  if (!ptr) error_window ("File Error", "Could not load text file '%s'", buffer);
	 }
	else
	  done = FALSE;

       }
      else
	ptr = NULL;
     }

    if (keyboard.get_esc_state()) {
      done = TRUE;
      ok = FALSE;
     }

  } while (!done);

	/* Clear the window */
  addtext_window.put_back();

  if (ok) {
    if (!amdfile.add_text_record (23, ptr))
      error_window ("Record Error", "Couldn't add new record!\n   %s", get_errmsg(err_code));
    else {
      sprintf (buffer, "%d", 23);
      index_list.add_tail (buffer);
      index_list.move_abs (index_list.count() - 1);
     }
   }

  post_values();
  redraw = TRUE;
 }


void menu_del (const short, const pos_t, const pos_t) {
  get_values();

  if (display_texts) {

    if (amdfile.num_texts == 0)
      error_window ("Record Error", "Nothing to delete!");
    else if (amdfile.del_text_record (index_list.selected)) {
      create_textlists();
      index_list.draw();
     }
    else
      error_window ("Record Error", "Couldn't delete record!\n   %s", get_errmsg(err_code));

   }
  else {

    if (!amdfile.del_art_record (current_record))
      error_window ("Record Error", "Couldn't delete record!\n   %s", get_errmsg(err_code));
    else if (current_record >= amdfile.num_records)
      current_record--;

   }

  post_values();
  redraw = TRUE;
 }


void menu_toggle (const short, const pos_t, const pos_t) {
  get_values();
  display_texts = (boolean) !display_texts;

  if (display_texts) {
    text_window.draw();
   }
  else {
    art_window.draw();
   }

  post_values();
  redraw = TRUE;
 }


void menu_edittext (const short, const pos_t, const pos_t) {
  char buffer[24];

  sprintf (buffer, "%d", amdfile.text_index[index_list.selected]);

  if (input_window (buffer, 5, "Edit Index", "Enter New Text Index (23-255)")) {
    amdfile.text_index[index_list.selected] = (short) strtol (buffer, NULL, 0);
    create_textlists();
    index_list.draw();
   }

 }


void menu_check (const short, const pos_t, const pos_t) {
  short old_error;

  get_values();

  if (amdfile.check())
    msg_window ("Check Project", "No problems found in current project...");
  else {
    old_error = err_code;

	/* Move to specified problem record */
    display_texts = dfart_check_text;

    if (display_texts) {
      index_list.move_abs (dfart_check_index);
      post_values();
      text_window.draw();
     }
    else {
      current_record = dfart_check_index;
      post_values();
      art_window.draw();
     }

    error_window ("Check Project", "A problem was found which should be corrected\n\n     %s", get_errmsg(old_error));
   }

 }

/*========== End of Menu Functions ========================================*/
