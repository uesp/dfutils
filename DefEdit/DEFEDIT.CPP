	/* Standard C Includes */
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <time.h>
#include <alloc.h>
#include <string.h>

	/* User Defined Includes */
#include "fileutil.h"
#include "guitext.h"
#include "dagfile.h"
#include "defedit.h"

	/* For converting things */
#define ENCHANT_CON(e) (((unsigned long) ((signed char)(e & 0xff))) + (((unsigned long) (e >> 8)) << 16))

#define RELEASE FALSE

	/* Holds all the important data */
MAGICDEF_TYPE deffile;


	/* The GUI Components */
WINDOW_TEXT main_window (1, 1, 80, 50, DEFEDIT_VERSION, def_colors);
BUTTON_TEXT save_button (1, 1, 8, 3, TRUE, "Save", menu_save);
BUTTON_TEXT exit_button (1, 1, 8, 3, TRUE, "Exit", menu_exit);
BUTTON_TEXT next_button (1, 1, 8, 3, TRUE, "Next", menu_next);
BUTTON_TEXT prev_button (1, 1, 8, 3, TRUE, "Prev", menu_prev);
BUTTON_TEXT add_button (1, 1, 8, 3, TRUE, "Add", menu_add);
BUTTON_TEXT del_button (1, 1, 8, 3, TRUE, "Del", menu_del);
TEXT_FIELD_TEXT name (1, 1, 30, "Name:");
DRAGLIST_TEXT type, material, magic_type;
LABEL_TEXT enchant_label (1, 1, "Enchantments", WHITE, BLUE);
TEXT_FIELD_TEXT enchant[10];
BUTTON_TEXT enchant_but[10];
TEXT_FIELD_TEXT usage (1, 1, 8, "Uses:");
TEXT_FIELD_TEXT unknown1 (1, 1, 12, "Unknown:");
LABEL_TEXT index_label (1, 1, "Record   0 of   0 ", CYAN, BLUE);
short enchant_index[10];
short enchant_values[10], item_group, item_subgroup;

	/* The enchant window */
WINDOW_TEXT enchant_window (10, 10, 56, 24, "Edit Enchantments", TRUE, TRUE, LIGHTGRAY, BLACK, WHITE, BLUE);
TEXT_FIELD_TEXT enchant_label2 (1, 1, 30, "Change From:", DARKGRAY, LIGHTGRAY, DARKGRAY, LIGHTGRAY);
BUTTON_TEXT ok (1, 1, 8, 3, TRUE, "Ok", NULL, BLACK, BLACK, GREEN);
BUTTON_TEXT cancel (1, 1, 8, 3, TRUE, "Cancel", NULL, BLACK, BLACK, GREEN);
LIST_TEXT enchant_list(1, 1, 40, 16, 16);
short enchantlist_index, ok_index, cancel_index;


	/* Main loop counter */
boolean done = FALSE;
boolean redraw = FALSE;

	/* Which one are we looking at? */
short current_record = -1;

	/* For Copying/Pasting Records */
MAGICDEF_RECORD clip;


/*========== Initializes the GUI Objects and Various other Things =========*/
void defedit_init (void) {
  short i;	/* Loop counter */
  long l;

  type.init (30, 10,"Type:", FALSE);
  material.init (12, 10, "Material:", FALSE);
  magic_type.init (16, 3, "Magic Type:", FALSE);
  main_window.open (68, 10, &next_button);
  main_window.open (68, 15, &prev_button);

  main_window.open (68, 23, &add_button);
  main_window.open (68, 28, &del_button);

  main_window.open (68, 35, &save_button);
  main_window.open (68, 40, &exit_button);

  main_window.open (16, 4, &name);
  main_window.open (16, 6, &magic_type);
  main_window.open (16, 8, &type);
  main_window.open (16, 10, &usage);
  main_window.open (16, 12, &material);
  main_window.open (16, 14, &unknown1);
  main_window.open (5, 18, &enchant_label);
  main_window.open (3, 45, &index_label);

  for (i = 0; i < 10; i++) {
    enchant[i].init (1, 1, 40, "");
    enchant[i].enabled = FALSE;
    enchant_but[i].init (1, 1, 8, 1, FALSE, "Change", NULL, BLACK, BLACK, GREEN);
    main_window.open (10, 20 + i*2, &enchant[i]);
    enchant_index[i] = main_window.open (52, 20 + i*2, &enchant_but[i]);
   }

	/* Create the enchantment editting window */
  enchantlist_index = enchant_window.open (3, 3, &enchant_list);
  enchant_window.open (16, 20, &enchant_label2);
  ok_index = enchant_window.open (45, 5, &ok);
  cancel_index = enchant_window.open (45, 13, &cancel);

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
  magic_type.add_dual_tail ("Regular", 0);
  magic_type.add_dual_tail ("Artifact", 1);
  magic_type.add_dual_tail ("Ext Artifact", 2);
  i = 0;

  while (!(enchant_types[i].type == -1u && enchant_types[i].subtype == -1)) {
    l = ((unsigned long) enchant_types[i].type) + ((((unsigned long) enchant_types[i].subtype) << 16) & 0xFFFF0000l);
    enchant_list.add_dual_tail (enchant_types[i].name, l);
    i++;
   }

  enchant_list.add_dual_tail ("<none>", -1);
  i = 0;

  while (item_types[i].group != -1) {
    type.add_dual_sort (item_types[i].name, i);
    i++;
   }

  type.add_dual_tail ("? Unknown", -1);



  textmode (C4350);
  _setcursortype (_NOCURSOR);
 }
/*========== End of Procedure defedit_init() ==============================*/


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
    enchant_label2.change_text (enchant_types[j].name);
   }

  enchant_window.draw();

  do {
	/* Check for input */
    keyboard.check();
    mouse.check();

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
  MAGICDEF_RECORD *ptr = deffile.records[current_record];
  short i;

	/* Make sure the current record is valid */
  if (current_record >= deffile.num_records || current_record < 0 || !ptr)
    return;

  strcpy (ptr->name, name.text);
  ptr->magic_type = (char) magic_type.get_value();
  ptr->uses = (short) strtoul(usage.text, NULL, 0);
  ptr->u1 = (long) strtol(unknown1.text, NULL, 0);
  ptr->material = (char) material.get_value();

  for (i = 0; i < 10; i++)
    ptr->enchantments[i] = enchant_values[i];

  i = type.get_value();

  if (i == -1) {
    ptr->group = item_group;
    ptr->subgroup = item_subgroup;
   }
  else {
    ptr->group = item_types[i].group;
    ptr->subgroup = item_types[i].sub_group;
   }
 }
/*========== End of Procedure get_values() ================================*/


/*========== Posts values from memory to GUI objects ======================*/
void post_values (void) {
  MAGICDEF_RECORD *ptr;
  char buffer[41];
  short i, j;
  long l;

  sprintf (buffer, "Record %3d of %3d ", current_record + 1, deffile.num_records);
  index_label.change_text (buffer);

	/* Make sure the current record is valid */
  if (current_record >= deffile.num_records || current_record < 0) {
    name.change_text ("");
    type.change_text ("");
    magic_type.change_text ("");
    usage.change_text ("");
    unknown1.change_text ("");
    material.change_text ("");

    for (i = 0; i < 10; i++)
      enchant[i].change_text ("");
   }

  ptr = deffile.records[current_record];
  name.change_text (ptr->name);

  magic_type.list->selected = ptr->magic_type;
  magic_type.change_text (magic_type.list->get_name());

  sprintf (buffer, "%d", ptr->uses);
  usage.change_text (buffer);
  sprintf (buffer, "0x%08X", ptr->u1);
  unknown1.change_text (buffer);

  material.list->selected = ptr->material;
  material.change_text (material.list->get_name());

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
    type.change_text (item_types[i].name);
    set_typelist (i);
   }

  for (i = 0; i < 10; i++) {

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
	enchant[i].change_text (enchant_types[j].name);

     }

    enchant_values[i] = ptr->enchantments[i];
   }

 }
/*========== End of Procedure post_values() ===============================*/


/*========== Sets the Item Type List according to the groups ==============*/
void set_typelist (const short index) {
  unsigned char *ptr;
  short i = 0, j, k;

  ptr = (unsigned char *) type.list->get_head();

  while (ptr) {
    j = strlen((char *) ptr) + 1;
    k = (short) ((unsigned short)ptr[j] + ((unsigned short)ptr[j + 1] << 8));

    if (index == k) {
      type.list->selected = i;
      type.list->start = i;
      return;
     }

    ptr = (unsigned char *) type.list->next();
    i++;
   }

  type.list->selected = 0;
  type.list->start = 0;
 }
/*========== End of Procedure set_typelist() ==============================*/



/*========== Begin Main Program ===========================================*/
int main (void) {
  GUI_RETURN ret;
  unsigned long initial_mem = farcoreleft();
  short i, j;
  long l;

  open_log_file ("defedit.log");

	/* Attempt to load things */
#if RELEASE
  if (!deffile.load ("arena2\\magic.def")) {
#else
  if (!deffile.load ("magic.def")) {
#endif
    printf ("\nError: Couldn't load file MAGIC.DEF. Make sure DEFEdit is in the\n");
    printf ("       root Daggerfall directory.\n\n");
    exit(FALSE);
   }

	/* Init the gui objects etc... */
  defedit_init();
  main_window.draw();

  current_record = 0;
  post_values();
  redraw = TRUE;

	/* Main event loop */
  do {
    if (redraw) {
      redraw = FALSE;
      _setcursortype (_NOCURSOR);
      index_label.draw();
      main_window.focus = -1;
      name.draw();
      type.draw();
      magic_type.draw();
      usage.draw();
      unknown1.draw();
      material.draw();

      for (i = 0; i < 10; i++)
	enchant[i].draw();
     }

	/* Get events */
    keyboard.check();
    mouse.check();

	/* Check GUI events */
    ret = main_window.event_loop();

    if (ret.code == GUIRET_EVENT) {

      for (i = 0; i < 10; i++) {
	if (enchant_index[i] == ret.index) {
	  main_window.focus = -1;
	  _setcursortype (_NOCURSOR);
	  l = ENCHANT_CON(enchant_values[i]);
	  l = edit_enchantment (l);
	  enchant_values[i] = ((unsigned char) (l & 0xFFFF)) + (((unsigned char) (l >> 16))<<8);
	  j = find_enchantment (l);

	  if (j == -1)
	    enchant[i].change_text ("??? Unknown Enchantment ???");
	  else
	    enchant[i].change_text (enchant_types[j].name);

	  redraw = TRUE;
	  break;
	 }
       }

     }

	/* Check keyboard events */
    if (keyboard.get_alt_key (SCAN_X))
      menu_exit();
    else if (keyboard.get_alt_key (SCAN_S))
      menu_save();
    else if (keyboard.get_pgup_state())
      menu_next();
    else if (keyboard.get_pgdn_state())
      menu_prev();
    else if (keyboard.get_home_state() && deffile.num_records > 0) {
      get_values();
      current_record = 0;
      post_values();
      redraw = TRUE;
     }
    else if (keyboard.get_end_state()) {
      get_values();
      current_record = deffile.num_records - 1;
      post_values();
      redraw = TRUE;
     }


  } while (!done);

  textmode (C80);
  deffile.destroy();
  printf ("Finished %s: Created by Dave Humphrey - aj589@freenet.carleton.ca\n", DEFEDIT_VERSION);
  printf ("   Initial / Final Memory = %ld / %ld bytes\n", initial_mem, farcoreleft());
  printf ("   FarHeapCheck() = %d\n", farheapcheck());
  return (TRUE);
 }
/*========== End Main Program =============================================*/


/*========== Various Menu/Button Functions ================================*/
short menu_exit (__CPPARGS) {
  done = TRUE;
  return (TRUE);
 }

short menu_save (__CPPARGS) {
  get_values();

#if RELEASE
  if (!deffile.save("arena2\\magic.def")) {
#else
  if (!deffile.save("magic.def")) {
#endif
    error_window ("File Error", "Couldn't save DEF file!\n   %s", df_err_msg[df_err_code]);
   }
  else
    msg_window ("File Saved", "File MAGIC.DEF was successfully saved");

  return (TRUE);
 }

short menu_next (__CPPARGS) {
  get_values();
  current_record++;
  if (current_record >= deffile.num_records) current_record = 0;
  post_values();
  redraw = TRUE;
  return (TRUE);
 }

short menu_prev (__CPPARGS) {
  get_values();
  current_record--;
  if (current_record < 0) current_record = deffile.num_records - 1;
  post_values();
  redraw = TRUE;
  return (TRUE);
 }

short menu_add (__CPPARGS) {
  get_values();

  if (!deffile.add_record (deffile.num_records)) {
    error_window ("Record Error", "Couldn't add new record!\n   %s", df_err_msg[df_err_code]);
   }
  else
    current_record = deffile.num_records - 1;

  post_values();
  redraw = TRUE;
  return (TRUE);
 }

short menu_del (__CPPARGS) {
  get_values();

  if (!deffile.del_record (current_record)) {
    error_window ("Record Error", "Couldn't delete record!\n   %s", df_err_msg[df_err_code]);
   }
  else if (current_record >= deffile.num_records)
    current_record--;

  post_values();
  redraw = TRUE;
  return (TRUE);
 }


/*========== End of Menu Functions ========================================*/
