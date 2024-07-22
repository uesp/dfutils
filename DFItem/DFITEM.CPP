
	/* Standard C Includes */
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <time.h>
#include <string.h>
#include <alloc.h>
#include <ctype.h>

	/* User Defined Includes */
#include "fileutil.h"
#include "daggen.h"
#include "dagdata.h"
#include "dagfile.h"
#include "dfitem.h"

	/* GUI Includes */
#include "guitext.h"

#define RELEASE FALSE



WINDOW_TEXT main_window;	/* The main window */
TEXT_FIELD_TEXT name;	/* Item fields */
LABEL_TEXT enchant_label, item_label, file_label;
TEXT_FIELD_TEXT search_label;
TEXT_FIELD_TEXT cost, usage, max_usage, calc_cost;
TEXT_FIELD_TEXT weight, construction, color;
TEXT_FIELD_TEXT enchant_points;
TEXT_FIELD_TEXT enchant[10];
BUTTON_TEXT enchant_but[10];
CHECK_TEXT check;
DRAGLIST_TEXT type;
DRAGLIST_TEXT material;
DRAGLIST_TEXT picture;
//DRAGLIST_TEXT color;
BUTTON_TEXT next_but, prev_but, load_but, save_but, exit_but;
BUTTON_TEXT next_magic_but, prev_magic_but;
BUTTON_TEXT del_but;
BUTTON_TEXT fix_but, sell_but;
BUTTON_TEXT find_but, findnext_but, findprev_but;

	/* Field indices */
short enchant_but_index[10];
short next_index, prev_index, load_index, save_index, exit_index;
short next_magic_index, prev_magic_index;
short del_index, fix_index, sell_index;
short find_index, findnext_index, findprev_index;
short picture_value = 0, item_group = 0, item_subgroup = 0;

	/* For the Enchant window... */
WINDOW_TEXT enchant_window;
BUTTON_TEXT ok_but, cancel_but;
TEXT_FIELD_TEXT enchant_label2;
LIST_TEXT enchant_list;
LABEL_TEXT enchant_label1;
short ok_index, cancel_index, list_index;
long enchant_values[10];

	/* For the load/save window */
WINDOW_TEXT load_window;
BUTTON_TEXT loadcancel_but;
BUTTON_TEXT game_but[6];
LABEL_TEXT game_label[6];
short loadcancel_index, game_index[6];

	/* Which item are we currently viewing */
short current_item = -1;

	/* The current file loaded */
char filename[MAX_STRING_LENGTH];
boolean file_loaded = FALSE;

	/* How much have we sold for currently this session? */
long total_cost = 0;

	/* Index for the character data */
short char_index = -1;

	/* For searching for items */
char search_string[MAX_STRING_LENGTH + 1] = "";

	/* Loop counter, Need to refresh screen objects? */
boolean done = FALSE, redraw = FALSE;



/*========== Inits All the Values, GUI Variables etc... ===================*/
void dfitem_init (void) {
  int i;	/* Loop counter */
  long l;
  char buffer[21];

	/* Allocate the mouse object */
  create_text_mouse();

	/* Post the daggerfall error messages */
  post_df_errors();

	/* Load the DF data */
  load_dag_data("dagdata.dat", DFFL_ALL);

	/* Init the GUI System */
  main_window.init (1, 1, 80, 50, DFITEM_VERSION);
  name.init (1, 1, 32, "Name:");
  type.init (32, 10, "Type:", FALSE);
  cost.init (1, 1, 12, "Cost:");
  calc_cost.init (1, 1, 12, "Calc Cost:", YELLOW, BLUE, WHITE, BLUE);
  calc_cost.enabled = FALSE;
  usage.init (1, 1, 8, "Uses:");
  max_usage.init (1, 1, 8, "/");
  weight.init (1, 1, 6, "Weight:");
  enchant_points.init (1, 1, 8, "EP:");
  enchant_label.init (1, 1, "Enchantments");
  item_label.init (1, 1, "Record   0 of   0 ", CYAN, BLUE);
  file_label.init (1, 1, "<none>", CYAN, BLUE);
  next_but.init (1, 1, 8, 3, TRUE, "Next", menu_next);
  prev_but.init (1, 1, 8, 3, TRUE, "Prev", menu_prev);
  next_magic_but.init (1, 1, 8, 3, TRUE, "+Mag", menu_nextmag);
  prev_magic_but.init (1, 1, 8, 3, TRUE, "-Mag", menu_prevmag);
  load_but.init (1, 1, 8, 3, TRUE, "Load", menu_load);
  save_but.init (1, 1, 8, 3, TRUE, "Save", menu_save);
  exit_but.init (1, 1, 8, 3, TRUE, "Exit", menu_exit);
  del_but.init (1, 1, 16, 3, TRUE, "Del Record");
  del_but.visible = FALSE;
  sell_but.init (1, 1, 8, 3, TRUE, "Sell", menu_sell);
  check.init (1, 1, FALSE, "Testing Check", BLUE, WHITE, WHITE);
  fix_but.init (1, 1, 8, 3, TRUE, "Fix", menu_fix);
  find_but.init (1, 1, 11, 3, TRUE, "Find Item", menu_find);
  findnext_but.init (1, 1, 11, 3, TRUE, "Find Next", menu_findnext);
  findprev_but.init (1, 1, 11, 3, TRUE, "Find Prev", menu_findprev);
  search_label.init (1, 1, 32, "Find:", CYAN, BLUE, CYAN, BLUE);
  search_label.enabled = FALSE;

  for (i = 0; i < 10; i++) {
    enchant[i].init (1, 1, 35, "");
    enchant[i].enabled = FALSE;
    enchant_but[i].init (1, 1, 8, 1, FALSE, "Change", NULL, WHITE, BLACK, GREEN);
   }

  material.init (16, 10, "Material:", FALSE);
  construction.init (1, 1, 5, "Construc:");
  color.init (1, 1, 5, "Color:");
  picture.init (32, 10, "Picture:", FALSE);

	/* Open the objects in the window */
  main_window.open (15, 2, &name);
  main_window.open (15, 4, &type);
  main_window.open (15, 6, &cost);
  main_window.open (15, 7, &calc_cost);
  main_window.open (40, 6, &usage);
  main_window.open (52, 6, &max_usage);
  main_window.open (15, 9, &weight);
  main_window.open (40, 9, &enchant_points);
  main_window.open (15, 11, &material);
  main_window.open (60, 11, &construction);
  main_window.open (15, 13, &picture);
  main_window.open (60, 13, &color);
  main_window.open (5, 17, &enchant_label);
  main_window.open (3, 45, &item_label);
  main_window.open (3, 46, &file_label);
  main_window.open (23, 44, &check);
  fix_index = main_window.open (69, 3, &fix_but);
  sell_index = main_window.open (69, 7, &sell_but);
  next_magic_index = main_window.open (69, 13, &next_magic_but);
  prev_magic_index = main_window.open (69, 17, &prev_magic_but);
  next_index = main_window.open (69, 22, &next_but);
  prev_index = main_window.open (69, 26, &prev_but);
  load_index = main_window.open (69, 32, &load_but);
  save_index = main_window.open (69, 36, &save_but);
  exit_index = main_window.open (69, 40, &exit_but);
  del_index = main_window.open (40, 38, &del_but);
  find_index = main_window.open (55, 32, &find_but);
  findnext_index = main_window.open (55, 36, &findnext_but);
  findprev_index = main_window.open (55, 40, &findprev_but);

  main_window.open (10, 47, &search_label);

  for (i = 0; i < 10; i++) {
    main_window.open (8, i*2 + 18, &enchant[i]);
    enchant_but_index[i] = main_window.open (44, i*2 + 18, &enchant_but[i]);
   }

	/* For the Enchant window */
  enchant_window.init (10, 10, 54, 20, "Edit Enchantment", TRUE, TRUE, LIGHTGRAY, BLACK, WHITE, BLUE);
  ok_but.init (1, 1, 8, 3, TRUE, "Ok", NULL, BLACK, RED, CYAN);
  cancel_but.init (1, 1, 8, 3, TRUE, "Cancel", NULL, BLACK, RED, CYAN);
  enchant_list.init (1, 1, 36, 12, 12);
  enchant_list.high_back_color = GREEN;
  enchant_label1.init (1, 1, "Select the desired enchantment", DARKGRAY, LIGHTGRAY);
  enchant_label2.init (1, 1, 36, "Change From:", DARKGRAY, LIGHTGRAY, DARKGRAY, LIGHTGRAY);
  enchant_label2.enabled = FALSE;
  ok_index = enchant_window.open (42, 5, &ok_but);
  cancel_index = enchant_window.open (42, 12, &cancel_but);
  list_index = enchant_window.open (2, 4, &enchant_list);
  enchant_window.open (3, 2, &enchant_label1);
  enchant_window.open (17, 17, &enchant_label2);

	/* For the Load/Save Window */
  load_window.init (5, 10, 70, 22, "Load Daggerfall Save Game", TRUE, TRUE, LIGHTGRAY, BLACK, WHITE, BLUE);
  loadcancel_but.init (1, 1, 8, 1, FALSE, "Cancel", NULL, BLACK, WHITE, GREEN);

  for (i = 0; i < 6; i++) {
    sprintf (buffer, "Save%d", i);
    game_but[i].init (1, 1, 8, 3, TRUE, buffer);
    game_label[i].init (1, 1, "<unknown>", DARKGRAY, LIGHTGRAY);
   }

  loadcancel_index = load_window.open (55, 18, &loadcancel_but);
  game_index[0] = load_window.open (3,  3, &game_but[0]);
  game_index[1] = load_window.open (3,  8, &game_but[1]);
  game_index[2] = load_window.open (3, 13, &game_but[2]);
  game_index[3] = load_window.open (35,  3, &game_but[3]);
  game_index[4] = load_window.open (35,  8, &game_but[4]);
  game_index[5] = load_window.open (35, 13, &game_but[5]);
  load_window.open (13,  4, &game_label[0]);
  load_window.open (13,  9, &game_label[1]);
  load_window.open (13, 14, &game_label[2]);
  load_window.open (45,  4, &game_label[3]);
  load_window.open (45,  9, &game_label[4]);
  load_window.open (45, 14, &game_label[5]);

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
    enchant_list.add_dual_tail (enchants[i]->name, l);
   }

  enchant_list.add_dual_tail ("<none>", -1);

  for (i = 0; i < num_items; i++) {

    if (items[i]->group) {
      type.add_dual_sort (items[i]->name, i);
     }

   }

  type.add_dual_tail ("? Unknown", -1);

  for (i = 0; i < num_pictures; i++) {
    picture.add_dual_sort (pictures[i]->name, (long)pictures[i]->index);
   }

  write_log_entry ("  Picture: %d", picture.count());

  picture.add_dual_sort ("? Unknown", -1l);

	/* Change to a Higher-Res Text mode */
  textmode (C4350);
  _setcursortype (_NOCURSOR);
  mouse->show();
 }
/*========== End of Procedure dfitem_init() ===============================*/


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
      if (ret.index == ok_index || ret.index == list_index) {
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


/*========== Finds the Character Data in the Records ======================*/
short find_char (void) {
  short i;

  for (i = 0; i < dagfile.num_records; i++) {
    if (dagfile.records[i] && dagfile.records[i]->loaded && dagfile.records[i]->type == DF_CHAR)
      return (i);
   }

  return (-1);
 }
/*========== End of Function find_char() ==================================*/


/*========== Searches for the Next Item containing a certain string =======*/
short find_searchitem (const short current, const short dir, const char *string) {
  short i = current, j;
  ITEM_RECORD *ptr;
  char buffer[MAX_STRING_LENGTH + 1];

	/* Make sure there is a valid string */
  if (!string || !*string) {
    error_window ("Search Error", "Nothing to search for!");
    return (current);
   }

	/* Make sure there is something to look for */
  if (current >= dagfile.num_records || dagfile.num_records == 0)
    return (-1);

  do {
    i += dir;

	/* Make sure we don't exceed the maximum record number */
    if (i >= dagfile.num_records)
      i = 0;
    else if (i < 0)
      i = dagfile.num_records - 1;

	/* Is the record an item */
    if (dagfile.records[i] && dagfile.records[i]->type == DF_ITEM &&
	dagfile.records[i]->loaded) {

		/* Look for the string in the item name */
      ptr = (ITEM_RECORD *) dagfile.records[i];
      strcpy (buffer, ptr->name);
      strlwr (buffer);
      if (strstr (buffer, string)) return (i);

		/* Look in the item type */
			/* Attempt to find the item type in the known list */
      j = find_item ((short) ptr->group, (short) ptr->subgroup);

      if (j != -1) {
	strcpy (buffer, items[j]->name);
	strlwr (buffer);
	if (strstr(buffer, string)) return (i);
       }
     }

  } while (i != current);

	/* Return a warning message */
  msg_window ("Search Warning", "No items matching '%s' were found", string);

	/* Make sure the current record is a valid item */
  if (dagfile.records[i] && dagfile.records[i]->type == DF_ITEM &&
      dagfile.records[i]->loaded)
    return (i);
  else
    return (-1);
 }
/*========== End of Function find_searchitem() ============================*/


/*========== Finds the Next Item after the one passed =====================*/
short find_nextitem (const short current, const short dir) {
  short i = current;

	/* Make sure there is something to look for */
  if (current >= dagfile.num_records || dagfile.num_records == 0)
    return (-1);

  do {
    i += dir;

	/* Make sure we don't exceed the maximum record number */
    if (i >= dagfile.num_records)
      i = 0;
    else if (i < 0)
      i = dagfile.num_records - 1;

    if (dagfile.records[i] && dagfile.records[i]->type == DF_ITEM &&
	dagfile.records[i]->loaded)
      return (i);

  } while (i != current);

	/* Make sure the current record is a valid item */
  if (dagfile.records[i] && dagfile.records[i]->type == DF_ITEM &&
      dagfile.records[i]->loaded)
    return (i);
  else
    return (-1);
 }
/*========== End of Function find_nextitem() ==============================*/


/*========== Finds the Next Magical Item after the one passed =============*/
short find_next_magicitem (const short current, const short dir) {
  short i = current, j;

	/* Make sure there is something to look for */
  if (current >= dagfile.num_records || current < 0 || dagfile.num_records == 0)
    return (-1);

  do {
    i += dir;

	/* Make sure we don't exceed the maximum record number */
    if (i >= dagfile.num_records)
      i = 0;
    else if (i < 0)
      i = dagfile.num_records - 1;

    if (dagfile.records[i] && dagfile.records[i]->type == DF_ITEM &&
	dagfile.records[i]->loaded) {

		/* Check for enchantments */
      for (j = 0; j < 10; j++) {
	if (((ITEM_RECORD *)dagfile.records[i])->enchantments[j] != -1)
	  return (i);
       }
     }

  } while (i != current);

	/* Make sure the current record is a valid item */
  if (dagfile.records[i] && dagfile.records[i]->type == DF_ITEM &&
      dagfile.records[i]->loaded)
    return (i);
  else
    return (-1);
 }
/*========== End of Function find_next_magicitem() ==============================*/


/*========== Gets Item Values from the GUI Fields =========================*/
void get_values (void) {
  ITEM_RECORD *ptr;
  short i;

	/* Make sure its a valid item field */
  if (current_item >= dagfile.num_records || current_item < 0 || !dagfile.records[current_item] || dagfile.records[current_item]->type != DF_ITEM)
    return;

  ptr = (ITEM_RECORD *) dagfile.records[current_item];
  strcpy (ptr->name, name.text);

  i = type.get_value();

  if (i == -1) {
    ptr->group = item_group;
    ptr->subgroup = item_subgroup;
   }
  else {
    ptr->group = items[i]->group;
    ptr->subgroup = items[i]->sub_group;
   }

  ptr->material = (unsigned char) material.get_value();
  ptr->wear = (short) strtol (usage.text, NULL, 0);
  ptr->max_wear = (short) strtol (max_usage.text, NULL, 0);
  ptr->cost = (long) strtoul(cost.text, NULL, 0);
  ptr->weight = (unsigned char) (atof(weight.text) * 4);
  ptr->enchant_pts = (short) strtol (enchant_points.text, NULL, 0);
  ptr->construction = (unsigned char) strtoul(construction.text, NULL, 0);
  ptr->color = (unsigned char) strtoul(color.text, NULL, 0);

	/* Parse the enchantments section */
  for (i = 0; i < 10; i++) {
    ptr->enchantments[i] = enchant_values[i];
   }

	/* Change the picture */
  i = picture.get_value();

  if (i != -1)
    ptr->picture1 = i;
  else
    ptr->picture1 = picture_value;

 }
/*========== End of Procedure get_values() ================================*/


/*========== Prompts User to Load a Game ==================================*/
boolean load_game (void) {
  FILE *f;
  GUI_RETURN ret;
  boolean done = FALSE, ok = TRUE;
  char buffer[MAX_STRING_LENGTH + 1];
  int i;

	/* Get/check the savegames */
  for (i = 0; i < 6; i++) {
#if RELEASE
    sprintf (buffer, "save%01d\\savetree.dat", i);
#else
    sprintf (buffer, "d:\\dagger\\save%01d\\savetree.dat", i);
#endif

    if (fileexists(buffer)) {
      game_but[i].visible = TRUE;
      game_label[i].visible = TRUE;
     }
    else {
      game_but[i].visible = FALSE;
      game_label[i].visible = FALSE;
     }

	/* Attempt to get the save name */
#if RELEASE
    sprintf (buffer, "save%01d\\savename.txt", i);
#else
    sprintf (buffer, "d:\\dagger\\save%01d\\savename.txt", i);
#endif

    if ((f = openfile (buffer, "rb")) != NULL) {
      fread (buffer, sizeof(char), 20, f);
      buffer[20] = 0;
      game_label[i].change_text (buffer);
      fclose (f);
     }
    else
      game_label[i].change_text ("<unknown>");

   }

	/* Draw the window */
  load_window.get_back();
  load_window.draw();

	/* Main event loop */
  do {
	/* Get events */
    keyboard.check();
    mouse->check();

	/* Check for events */
    ret = load_window.event_loop();

    if (ret.code == GUIRET_EVENT) {

      if (ret.index == loadcancel_index) {
	done = TRUE;
	ok = FALSE;
       }
      else {

	for (i = 0; i < 6; i++) {
	  if (game_index[i] == ret.index) {
#if RELEASE
	    sprintf (filename, "save%01d\\savetree.dat", i);
#else
	    sprintf (filename, "d:\\dagger\\save%01d\\savetree.dat", i);
#endif
	    done = TRUE;
	   }
	 }
       }
     }

    if (keyboard.get_esc_state()) {
      done = TRUE;
      ok = FALSE;
     }

  } while (!done);

	/* Erase the window */
  load_window.put_back();
  file_label.change_text (filename);
  return (ok);
 }
/*========== End of Function load_game() ==================================*/


/*========== Posts Item Values to GUI Fields ==============================*/
void post_values (void) {
  ITEM_RECORD *ptr;
  char buffer[41];
  short i, j;

	/* Clear the fields if the current item is invalid */
  if (current_item < 0) {
    name.change_text ("");
    type.change_text ("");
    usage.change_text ("");
    max_usage.change_text ("");
    cost.change_text ("");
    calc_cost.change_text ("");
    weight.change_text ("");
    enchant_points.change_text ("");
    construction.change_text ("");
    color.change_text ("");

    for (i = 0; i < 10; i++) {
      enchant[i].change_text ("");
      enchant_but[i].change_text ("Add");
     }

    return;
   }

	/* Make sure its a valid item field */
  if (current_item >= dagfile.num_records || !dagfile.records[current_item] || dagfile.records[current_item]->type != DF_ITEM)
    return;

  ptr = (ITEM_RECORD *) dagfile.records[current_item];
  name.change_text (ptr->name);

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

  material.change_selected(ptr->material);
  material.change_text (material.get_name());

  sprintf (buffer, "%d", ptr->wear);
  usage.change_text (buffer);
  sprintf (buffer, "%d", ptr->max_wear);
  max_usage.change_text (buffer);

  sprintf (buffer, "%ld", ptr->cost);
  cost.change_text (buffer);

  sprintf (buffer, "%.2f", (float)ptr->weight/4);
  weight.change_text (buffer);

  sprintf (buffer, "%d", ptr->enchant_pts);
  enchant_points.change_text (buffer);

  sprintf (buffer, "%d", ptr->construction);
  construction.change_text (buffer);

  sprintf (buffer, "%d", ptr->color);
  color.change_text (buffer);

  for (i = 0; i < 10; i++) {

    if (ptr->enchantments[i] == -1) {
      enchant[i].change_text ("<none>");
      enchant_but[i].change_text ("Add");
     }
    else {
      j = find_enchantment (ptr->enchantments[i]);

      if (j == -1)
	enchant[i].change_text ("??? Unknown Enchantment ???");
      else
	enchant[i].change_text (enchants[j]->name);

      enchant_but[i].change_text ("Change");
     }

    enchant_values[i] = ptr->enchantments[i];
   }

  sprintf (buffer, "Record %3d of %3d  ", current_item, dagfile.num_records);
  item_label.change_text (buffer);

	/* Calculate the cost of the item */
  sprintf (buffer, "%ld", ptr->compute_cost());
  calc_cost.change_text (buffer);

	/* Change the picture */
  i = find_picture (ptr);
  picture_value = ptr->picture1;

  if (i != -1) {
    picture.change_text (pictures[i]->name);
    set_piclist (picture_value);
   }
  else {
    sprintf (buffer, "? 0x%04X", picture_value);
    picture.change_text (buffer);
    set_piclist (-1);
   }

 }
/*========== End of Procedure post_values() ===============================*/


/*========== Attempts to Sell the Specified Item ==========================*/
boolean sell_item (const short item) {
  ITEM_RECORD *ptr;
  char buffer[MAX_STRING_LENGTH+1];
  long cost;
  int i;

  char_index = find_char();

  ptr = (ITEM_RECORD *) dagfile.records[item];
  if (!ptr || !ptr->loaded) return (FALSE);

  cost = ptr->compute_cost() / 2;

  if (char_index >= 0) {
    cost = cost * (((CHAR_RECORD *)dagfile.records[char_index])->per - 50)/121 + cost;
   }

	/* Create the item name */
  i = 0;
  buffer[0] = 0;

  while (ptr->name[i] && i < 32) {

	/* Special character */
    if (ptr->name[i] == '%') {

      if (ptr->name[i+1] == 'i' && ptr->name[i+2] == 't') {
	strcat (buffer, type.get_text());
	i++;
	i++;
       }
      else
	strcat (buffer, "%%");

     }
    else
      chrcat (buffer, ptr->name[i]);

    i++;
   }


	/* Make sure the user wishes to sell the item */
  if (prompt_window ("Sell Item", "Do you wish to sell the item:\n\n     %s\n\nFor a total of %ld gp......[Y/N]?", buffer, cost)) {
    total_cost += cost;

	/* Adjust the items value, changing it to a letter of credit, hopefully */
    ptr->cost = cost;
    ptr->group = 27;
    ptr->subgroup = 2;
    ptr->picture1 = CREDIT_PICTURE;
    ptr->picture2 = CREDIT_PICTURE;
    ptr->weight = 0;
    strcpy (ptr->name, "Letter of Credit");

    for (i = 0; i < 10; i++) {
      ptr->enchantments[i] = -1;
     }

   }

  return (FALSE);
 }
/*========== End of Procecure sell_item() =================================*/


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


/*========== Searchs the Picture List and sets the appropiate selected ====*/
void set_piclist (const short type) {
  unsigned char *ptr;
  short i = 0, j, k;

  ptr = (unsigned char *) picture.get_head();

  while (ptr) {
    j = strlen((char *)ptr) + 1;
    k = (short) ((unsigned short)ptr[j] + ((unsigned short)ptr[j + 1] << 8));

    if (type == k) {
      picture.change_selected(i);
      picture.change_start(i);
      return;
     }

    ptr = (unsigned char *) picture.next();
    i++;
   }

  picture.change_selected(0);
  picture.change_start(0);
 }
/*========== End of Procedure set_piclist() ===============================*/


/*========== Begin Main Program ===========================================*/
int main (void) {
  GUI_RETURN ret;
  long initial_mem = get_memleft();
  int i, j;

	/* Open log file for debugging purposes */
  open_log_file ("dfitem000.log");

	/* Initialize the required things */
  dfitem_init();
  main_window.draw();

	/* Display the item values */
  post_values();
  redraw = TRUE;

	/* Main window event loop */
  do {

	/* Updates just the required elements between records...for speed */
    if (redraw) {
      redraw = FALSE;
      main_window.change_focus(-1);
      _setcursortype (_NOCURSOR);
      file_label.draw();
      name.draw ();
      type.draw ();
      cost.draw ();
      calc_cost.draw();
      search_label.draw();
      weight.draw();
      material.draw();
      item_label.draw();
      usage.draw();
      max_usage.draw();
      enchant_points.draw();
      construction.draw();
      picture.draw();
      color.draw();

      for (i = 0; i < 10; i++) {
	enchant[i].draw();
	enchant_but[i].draw();
       }

     }

	/* Get events */
    keyboard.check();
    mouse->check();


    if (mouse->last_ext_event) {
      gotoxy (1, 1);
      cprintf ("Mouse Event = %2d / %2d", mouse->last_event, mouse->last_ext_event);
     }

	/* Check for events */
    ret = main_window.event_loop();

    if (ret.code == GUIRET_EVENT) {

      if (ret.index == del_index) {
	get_values();

	if (!dagfile.del_record (current_item)) {
	  error_window ("Record Error", "Could not delete record!\n     %s", ERROR_MSG);
	 }

	current_item = find_nextitem (current_item, 1);
	post_values();
	redraw = TRUE;
       }

      else if (current_item != -1 && file_loaded ) {

	for (i = 0; i < 10; i++) {
	  if (ret.index == enchant_but_index[i]) {
	    main_window.change_focus(-1);
	    _setcursortype (_NOCURSOR);
	    enchant_values[i] = edit_enchantment (enchant_values[i]);
	    j = find_enchantment (enchant_values[i]);

	    if (j == -1)
	      enchant[i].change_text ("??? Unknown Enchantment ???");
	    else
	      enchant[i].change_text (enchants[j]->name);

	    redraw = TRUE;
	    break;
	   }
	 }
       }

     }

	/* Keyboard events */
    if (keyboard.get_alt_key(SCAN_X))
      menu_exit();
    else if (ret.code == GUIRET_FALSE) {
      if (keyboard.get_pgup_state())
	menu_next();
      else if (keyboard.get_pgdn_state())
	menu_prev();
      else if (keyboard.get_up_state())
	menu_nextmag();
      else if (keyboard.get_down_state())
	menu_prevmag();
      else if (keyboard.get_alt_key (SCAN_S))
	menu_sell();
      else if (keyboard.get_alt_key(SCAN_F))
	menu_fix();
     }

  } while (!done);

	/* Change text mode etc... clean things up */
  textmode (C80);
  textcolor (LIGHTGRAY);
  textbackground (BLACK);
  dagfile.destroy();
  destroy_dag_data();
  clrscr();

	/* Print some debugging information */
  printf ("   Initial / Final Memory = %ld / %ld bytes\n", initial_mem, get_memleft());
  printf ("   HeapCheck() Returns %s\n", get_heapstring());
  printf ("   Total Cost of Items Sold = %ld\n\n", total_cost);
  exit(0);
  return (TRUE);
 }
/*========== End of Main Program ==========================================*/


/*========== The Menu functions, called by buttons etc... =================*/
void menu_sell (const short, const pos_t, const pos_t) {
  get_values();
  main_window.change_focus(-1);
  _setcursortype (_NOCURSOR);

  if (sell_item(current_item))
    current_item = find_nextitem (current_item, 1);

  redraw = TRUE;
  post_values();
 }


void menu_exit (const short, const pos_t, const pos_t) {
  done = TRUE;
 }

void menu_next (const short, const pos_t, const pos_t) {
  get_values();
  current_item = find_nextitem (current_item, 1);
  post_values();
  redraw = TRUE;
 }

void menu_prev (const short, const pos_t, const pos_t) {
  get_values();
  current_item = find_nextitem (current_item, -1);
  post_values();
  redraw = TRUE;
 }

void menu_nextmag (const short, const pos_t, const pos_t) {
  get_values();
  current_item = find_next_magicitem (current_item, 1);
  post_values();
  redraw = TRUE;
 }

void menu_prevmag (const short, const pos_t, const pos_t) {
  get_values();
  current_item = find_next_magicitem (current_item, -1);
  post_values();
  redraw = TRUE;
 }

void menu_find (const short, const pos_t, const pos_t) {
  get_values();
  main_window.change_focus(-1);
  _setcursortype (_NOCURSOR);

  if (input_window (search_string, 32, "Find Item", "Enter desired search string below")) {
    strlwr (search_string);
    search_label.change_text (search_string);
    current_item = find_searchitem (current_item, +1, search_string);
   }

  post_values();
  redraw = TRUE;
 }

void menu_findnext (const short, const pos_t, const pos_t) {
  get_values();
  current_item = find_searchitem (current_item, +1, search_string);
  post_values();
  redraw = TRUE;
 }

void menu_findprev (const short, const pos_t, const pos_t) {
  get_values();
  current_item = find_searchitem (current_item, -1, search_string);
  post_values();
  redraw = TRUE;
 }

void menu_fix (const short, const pos_t, const pos_t) {
  usage.change_text (max_usage.text);
  redraw = TRUE;
 }


void menu_load (const short, const pos_t, const pos_t) {
  get_values();
  main_window.change_focus(-1);
  _setcursortype (_NOCURSOR);

  if (load_game()) {

    if (!dagfile.load (filename, DFRT_ITEM | DFRT_CHAR)) {
      error_window ("File Error", "Could not load file '%s'\n     %s", filename, ERROR_MSG);
      current_item = find_nextitem (0, 1);
     }
    else {
      current_item = find_nextitem (0, 1);
      file_loaded = TRUE;
      total_cost = 0;
      char_index = find_char();
     }

    redraw = TRUE;
    post_values();
   }

 }

void menu_save (const short, const pos_t, const pos_t) {
  int i;
  char buffer[MAX_STRING_LENGTH + 1], buf[MAX_STRING_LENGTH + 1];

  main_window.change_focus(-1);
  _setcursortype (_NOCURSOR);

  if (!file_loaded) {
    error_window ("Warning", "Nothing to save!");
   }
  else {
    get_values();
    strcpy (buffer, filename);
    i = strlen(buffer) - 3;
    buffer[i] = 't';
    buffer[i + 1] = 'm';
    buffer[i + 2] = 'p';

    if (!dagfile.save (filename, buffer, DFRT_ITEM | DFRT_CHAR)) {
      error_window ("File Error", "Could not save file '%s'\n     %s", filename, ERROR_MSG);
     }
    else {
			/* Create the backup */
      sprintf (buf, "copy %s %s", filename, buffer);
      i = strlen(buf) - 3;
      buf[i] = 'b';
      buf[i + 1] = 'a';
      buf[i + 2] = 'k';
      strcat (buf, " > null");

      if (system (buf)) {
	if (!prompt_window("File Error", "Failed to create backup....Continue [Y/N]?")) {
	  return;
	 }
       }

	/* Copy the temp file to the proper filename */
      sprintf (buf, "copy %s %s > nul", buffer, filename);

      if (system (buf)) {
	error_window ("File Error", "Could not save file '%s'", filename);
       }
      else {
	sprintf (buf, "del %s > nul", buffer);
	system (buf);

		/* Need to reload for proper offsets etc... */
	dagfile.load (filename, DFRT_ITEM | DFRT_CHAR);
	msg_window ("Saved File", "Successfully saved file '%s'", filename);
       }
     }
   }

 }

/*========== End of All Menu Functions ====================================*/