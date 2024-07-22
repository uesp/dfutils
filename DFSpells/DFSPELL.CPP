/*=========================================================================
 *
 * DFSpell.CPP - June 1998, Dave Humphrey
 *
 * Attempt to edit the Daggerfall file SPELLS.STD
 *
 *=======================================================================*/


	/* Include files */
#include "guivga.h"
#include "daggen.h"
#include "spells.h"
#include "gui_rc.h"

	/* Program Information */
#define DFSPELL_NAME "DFSpell"
#define DFSPELL_VERSION "0.1a"
#define DFSPELL_DATE "August 1998"
#define DFSPELL_AUTHOR "Dave Humphrey"



	/* The main data object */
SPELLS_STD_TYPE spells_std;

	/* The VGA GUI Objects */
WINDOW_VGA     main_window;
MENU_VGA       file_menu, edit_menu, help_menu;
BUTTON_VGA     add_button;
BUTTON_VGA     del_button;
BUTTON_VGA     next_button;
BUTTON_VGA     prev_button;
BUTTON_VGA     load_button;
BUTTON_VGA     save_button;
BUTTON_VGA     exit_button;
LABEL_VGA      count_label, file_label;
TEXT_FIELD_VGA spell_name;
TEXT_FIELD_VGA spell_type1, spell_type2, spell_type3;
TEXT_FIELD_VGA spell_subtype1, spell_subtype2, spell_subtype3;
DRAGLIST_VGA   spell_list1, spell_list2, spell_list3;
TEXT_FIELD_VGA spell_dur11, spell_dur21, spell_dur31;
TEXT_FIELD_VGA spell_dur12, spell_dur22, spell_dur32;
TEXT_FIELD_VGA spell_dur13, spell_dur23, spell_dur33;
LABEL_VGA      dur_label1, dur_label2, dur_label3;
TEXT_FIELD_VGA spell_chance11, spell_chance21, spell_chance31;
TEXT_FIELD_VGA spell_chance12, spell_chance22, spell_chance32;
TEXT_FIELD_VGA spell_chance13, spell_chance23, spell_chance33;
LABEL_VGA      chance_label1, chance_label2, chance_label3;
TEXT_FIELD_VGA spell_mag11, spell_mag21, spell_mag31, spell_mag41, spell_mag51;
TEXT_FIELD_VGA spell_mag12, spell_mag22, spell_mag32, spell_mag42, spell_mag52;
TEXT_FIELD_VGA spell_mag13, spell_mag23, spell_mag33, spell_mag43, spell_mag53;
LABEL_VGA      mag_label1, mag_label2, mag_label3;
FRAME_VGA      frame1, frame2, frame3;
DRAGLIST_VGA   element_list;
DRAGLIST_VGA   target_list;
TEXT_FIELD_VGA cost, icon, index;
FILE_VGA       filediag;

	/* For the Help-About Window */
WINDOW_VGA debug_window;
LABEL_VGA  debug_label;
BUTTON_VGA debug_button;
LIST_VGA   debug_list;



	/* Main loop counter */
boolean done = FALSE;

	/* Need to redraw GUI objects? */
boolean redraw = FALSE;

	/* The currently display spell */
short current_spell = 0;

	/* File and pathnames */
char filename[101] = "";
char path[101] = "";

short dbut_index = -1;


/*=========================================================================
 *
 * Procedure get_spell(void)
 *
 * Gets the spell information from the GUI objects and saves to the spell
 * object.
 *
 *=======================================================================*/
void get_spell (void) {
  SPELL_TYPE *spell_ptr;

	/* Make sure the current spell is valid */
  if (current_spell < 0 || current_spell >= spells_std.num_records) {
    return;
   }

	/* Save all the appropiate values */
  strncpy (spells_std.records[current_spell].name, spell_name.text, 24);

  spell_ptr = &spells_std.records[current_spell].spells[0];
  spell_ptr->effect = (char) atoi(spell_type1.text);
  spell_ptr->effect1 = (char) atoi(spell_subtype1.text);
  spell_ptr->duration[0] = (char) atoi(spell_dur11.text);
  spell_ptr->duration[1] = (char) atoi(spell_dur21.text);
  spell_ptr->duration[2] = (char) atoi(spell_dur31.text);
  spell_ptr->chance[0] = (char) atoi(spell_chance11.text);
  spell_ptr->chance[1] = (char) atoi(spell_chance21.text);
  spell_ptr->chance[2] = (char) atoi(spell_chance31.text);
  spell_ptr->strength[0] = (char) atoi(spell_mag11.text);
  spell_ptr->strength[1] = (char) atoi(spell_mag21.text);
  spell_ptr->strength[2] = (char) atoi(spell_mag31.text);
  spell_ptr->strength[3] = (char) atoi(spell_mag41.text);
  spell_ptr->strength[4] = (char) atoi(spell_mag51.text);

  spell_ptr = &spells_std.records[current_spell].spells[1];
  spell_ptr->effect = (char) atoi(spell_type2.text);
  spell_ptr->effect1 = (char) atoi(spell_subtype2.text);
  spell_ptr->duration[0] = (char) atoi(spell_dur12.text);
  spell_ptr->duration[1] = (char) atoi(spell_dur22.text);
  spell_ptr->duration[2] = (char) atoi(spell_dur32.text);
  spell_ptr->chance[0] = (char) atoi(spell_chance12.text);
  spell_ptr->chance[1] = (char) atoi(spell_chance22.text);
  spell_ptr->chance[2] = (char) atoi(spell_chance32.text);
  spell_ptr->strength[0] = (char) atoi(spell_mag12.text);
  spell_ptr->strength[1] = (char) atoi(spell_mag22.text);
  spell_ptr->strength[2] = (char) atoi(spell_mag32.text);
  spell_ptr->strength[3] = (char) atoi(spell_mag42.text);
  spell_ptr->strength[4] = (char) atoi(spell_mag52.text);

  spell_ptr = &spells_std.records[current_spell].spells[2];
  spell_ptr->effect = (char) atoi(spell_type3.text);
  spell_ptr->effect1 = (char) atoi(spell_subtype3.text);
  spell_ptr->duration[0] = (char) atoi(spell_dur13.text);
  spell_ptr->duration[1] = (char) atoi(spell_dur23.text);
  spell_ptr->duration[2] = (char) atoi(spell_dur33.text);
  spell_ptr->chance[0] = (char) atoi(spell_chance13.text);
  spell_ptr->chance[1] = (char) atoi(spell_chance23.text);
  spell_ptr->chance[2] = (char) atoi(spell_chance33.text);
  spell_ptr->strength[0] = (char) atoi(spell_mag13.text);
  spell_ptr->strength[1] = (char) atoi(spell_mag23.text);
  spell_ptr->strength[2] = (char) atoi(spell_mag33.text);
  spell_ptr->strength[3] = (char) atoi(spell_mag43.text);
  spell_ptr->strength[4] = (char) atoi(spell_mag53.text);
  spells_std.records[current_spell].element = element_list.get_value();
  spells_std.records[current_spell].target = target_list.get_value();
  spells_std.records[current_spell].cost = atol(cost.text);
  spells_std.records[current_spell].icon = atoi(icon.text);
  spells_std.records[current_spell].index = atoi(index.text);
 }
/*=========================================================================
 *		End of Procedure get_spell()
 *=======================================================================*/


/*=========================================================================
 *
 * Procedure post_spell(void)
 *
 * Puts the spell information into the GUI objects for display.
 *
 *=======================================================================*/
void post_spell (void) {
  SPELL_TYPE *spell_ptr;
  char buffer[80], *temp_ptr;
  long l;
  int i;

	/* Change the spell numbers */
  sprintf (buffer, "Spell %3d of %3d", current_spell + 1, spells_std.num_records);
  count_label.change_text (buffer);
  redraw = TRUE;

	/* Make sure the current spell is valid */
  if (current_spell < 0 || current_spell >= spells_std.num_records) {
    spell_name.change_text ("");
    spell_type1.change_text ("");
    spell_subtype1.change_text ("");
    return;
   }

	/* Set all the appropiate values */
  spell_name.change_text (spells_std.records[current_spell].name);

  spell_ptr = &spells_std.records[current_spell].spells[0];
  spell_type1.change_text ("%d", spell_ptr->effect);
  spell_subtype1.change_text ("%d", spell_ptr->effect1);
  spell_dur11.change_text ("%d", spell_ptr->duration[0]);
  spell_dur21.change_text ("%d", spell_ptr->duration[1]);
  spell_dur31.change_text ("%d", spell_ptr->duration[2]);
  spell_chance11.change_text ("%d", spell_ptr->chance[0]);
  spell_chance21.change_text ("%d", spell_ptr->chance[1]);
  spell_chance31.change_text ("%d", spell_ptr->chance[2]);
  spell_mag11.change_text ("%d", spell_ptr->strength[0]);
  spell_mag21.change_text ("%d", spell_ptr->strength[1]);
  spell_mag31.change_text ("%d", spell_ptr->strength[2]);
  spell_mag41.change_text ("%d", spell_ptr->strength[3]);
  spell_mag51.change_text ("%d", spell_ptr->strength[4]);

  spell_ptr = &spells_std.records[current_spell].spells[1];
  spell_type2.change_text ("%d", spell_ptr->effect);
  spell_subtype2.change_text ("%d", spell_ptr->effect1);
  spell_dur12.change_text ("%d", spell_ptr->duration[0]);
  spell_dur22.change_text ("%d", spell_ptr->duration[1]);
  spell_dur32.change_text ("%d", spell_ptr->duration[2]);
  spell_chance12.change_text ("%d", spell_ptr->chance[0]);
  spell_chance22.change_text ("%d", spell_ptr->chance[1]);
  spell_chance32.change_text ("%d", spell_ptr->chance[2]);
  spell_mag12.change_text ("%d", spell_ptr->strength[0]);
  spell_mag22.change_text ("%d", spell_ptr->strength[1]);
  spell_mag32.change_text ("%d", spell_ptr->strength[2]);
  spell_mag42.change_text ("%d", spell_ptr->strength[3]);
  spell_mag52.change_text ("%d", spell_ptr->strength[4]);

  spell_ptr = &spells_std.records[current_spell].spells[2];
  spell_type3.change_text ("%d", spell_ptr->effect);
  spell_subtype3.change_text ("%d", spell_ptr->effect1);
  spell_dur13.change_text ("%d", spell_ptr->duration[0]);
  spell_dur23.change_text ("%d", spell_ptr->duration[1]);
  spell_dur33.change_text ("%d", spell_ptr->duration[2]);
  spell_chance13.change_text ("%d", spell_ptr->chance[0]);
  spell_chance23.change_text ("%d", spell_ptr->chance[1]);
  spell_chance33.change_text ("%d", spell_ptr->chance[2]);
  spell_mag13.change_text ("%d", spell_ptr->strength[0]);
  spell_mag23.change_text ("%d", spell_ptr->strength[1]);
  spell_mag33.change_text ("%d", spell_ptr->strength[2]);
  spell_mag43.change_text ("%d", spell_ptr->strength[3]);
  spell_mag53.change_text ("%d", spell_ptr->strength[4]);

	/* Update the spell effect list text */
  temp_ptr = spell_list1.get_head();
  l = (spells_std.records[current_spell].spells[0].effect << 8) + ((unsigned char) spells_std.records[current_spell].spells[0].effect1);
  i = 0;

  while (temp_ptr) {
    spell_list1.change_selected(i);

    if (spell_list1.get_value() == l) {
      spell_list1.change_start (i);
      spell_list1.change_text (temp_ptr);
      break;
     }

    temp_ptr = spell_list1.next();
    i++;
   }

	/* Didn't find any */
  if (!temp_ptr) {
    spell_list1.change_selected(-1);
    spell_list1.change_start (0);
    spell_list1.change_text ("Unknown");
   }

	/* Update the spell effect list text */
  temp_ptr = spell_list2.get_head();
  l = (spells_std.records[current_spell].spells[1].effect << 8) + ((unsigned char) spells_std.records[current_spell].spells[1].effect1);
  i = 0;

  while (temp_ptr) {
    spell_list2.change_selected(i);

    if (spell_list2.get_value() == l) {
      spell_list2.change_start (i);
      spell_list2.change_text (temp_ptr);
      break;
     }

    temp_ptr = spell_list2.next();
    i++;
   }

	/* Didn't find any */
  if (!temp_ptr) {
    spell_list2.change_selected(-1);
    spell_list2.change_start (0);
    spell_list2.change_text ("Unknown");
   }

	/* Update the spell effect list text */
  temp_ptr = spell_list3.get_head();
  l = (spells_std.records[current_spell].spells[2].effect << 8) + ((unsigned char) spells_std.records[current_spell].spells[2].effect1);
  i = 0;

  while (temp_ptr) {
    spell_list3.change_selected(i);

    if (spell_list3.get_value() == l) {
      spell_list3.change_start (i);
      spell_list3.change_text (temp_ptr);
      break;
     }

    temp_ptr = spell_list3.next();
    i++;
   }

	/* Didn't find any */
  if (!temp_ptr) {
    spell_list3.change_selected(-1);
    spell_list3.change_start (0);
    spell_list3.change_text ("Unknown");
   }

  element_list.change_selected(spells_std.records[current_spell].element);
  element_list.change_text (element_list.get_name());
  target_list.change_selected(spells_std.records[current_spell].target);
  target_list.change_text (target_list.get_name());
  cost.change_text ("%ld", spells_std.records[current_spell].cost);
  icon.change_text ("%d", spells_std.records[current_spell].icon);
  index.change_text ("%d", spells_std.records[current_spell].index);
 }
/*=========================================================================
 *		End of Procedure post_spell()
 *=======================================================================*/


/*=========================================================================
 *
 * GUI Event Functions
 *
 *=======================================================================*/
void add_click (BASE_GUI *) {

	/* Make sure there is enough room to add record */
  if (spells_std.num_records >= MAX_SPELLS) {
    errorbox.open (-1, -1, "Warning", "The limit of %d spells has been reached!", MAX_SPELLS);
    return;
   }

	/* Save values */
  get_spell();

  current_spell = spells_std.num_records;
  spells_std.num_records++;

	/* Get new spell values */
  post_spell();
 }

void del_click (BASE_GUI *) {
  short i;	/* Loop counter */

	/* Is there a record to delete */
  if (current_spell < 0 || spells_std.num_records <= 0) {
    errorbox.open (-1, -1, "Warning", "Nothing to delete!");
    return;
   }

	/* Shift spells to delete the current one */
  for (i = current_spell; i < spells_std.num_records - 1; i++)
    spells_std.records[i] = spells_std.records[i + 1];

  spells_std.num_records--;
  if (current_spell >= spells_std.num_records) current_spell--;

	/* Get the new values */
  post_spell();
 }


void exit_click (BASE_GUI *) {
  done = TRUE;
 }

void load_click (BASE_GUI *) {

	/* Get the filename to load */
  if (!filediag.open (100, 50, 0, "Load Spells", "Load", filename, "STD (*.std)|*.std|All Files (*.*)|*.*", path)) return;

	/* Load the specified STD file */
  strcpy (filename, filediag.filename);
  strcpy (path, filediag.get_path());

  if (!spells_std.load (filename)) {
    errorbox.open (-1, -1, "Load Error", "Could not load file '%s'!", filename);
    file_label.change_text ("<none>");
   }
  else
    file_label.change_text (filename);



	/* Post the new data */
  post_spell();
 }


void next_click (BASE_GUI *) {

	/* Ignore if no spells */
  if (spells_std.num_records <= 0) return;

	/* Save the current spell values */
  get_spell();

	/* Change the current spell number */
  current_spell++;
  if (current_spell >= spells_std.num_records) current_spell = 0;

	/* Put new values */
  post_spell();
 }

void save_click (BASE_GUI *) {

	/* Update variables */
  get_spell();

	/* Get the filename to load */
  if (!filediag.open (100, 50, 0, "Save Spells", "Save", filename, "STD (*.std)|*.std|All Files (*.*)|*.*", path)) return;

	/* Save the specified STD file */
  strcpy (filename, filediag.filename);
  strcpy (path, filediag.get_path());


  if (!spells_std.save (filename)) {
    errorbox.open (-1, -1, "Save Error", "Could not save file '%s'!", filename);
   }
  else
    file_label.change_text (filename);

 }

void prev_click (BASE_GUI *) {

	/* Ignore if no spells */
  if (spells_std.num_records <= 0) return;

	/* Save the current spell values */
  get_spell();

	/* Change the current spell number */
  current_spell--;
  if (current_spell < 0) current_spell = spells_std.num_records - 1;

	/* Put new values */
  post_spell();
 }

void spell1_event (BASE_GUI *) {
  long l = spell_list1.get_value();

  spell_subtype1.change_text ("%d", (signed char) (l & 0xff));
  spell_subtype1.draw();

  spell_type1.change_text ("%d", (signed char)((l >> 8) & 0xff));
  spell_type1.draw();
 }

void spell2_event (BASE_GUI *) {
  long l = spell_list2.get_value();

  spell_subtype2.change_text ("%d", (signed char) (l & 0xff));
  spell_subtype2.draw();

  spell_type2.change_text ("%d", (signed char) ((l >> 8) & 0xff));
  spell_type2.draw();
 }

void spell3_event (BASE_GUI *) {
  long l = spell_list3.get_value();

  spell_subtype3.change_text ("%d", (signed char) (l & 0xff));
  spell_subtype3.draw();

  spell_type3.change_text ("%d",(signed char) ((l >> 8) & 0xff));
  spell_type3.draw();
 }


void save_menu (BASE_GUI *) {

  if (strlen(filename)) {

	/* Update variables */
    get_spell();

    if (!spells_std.save (filename))
      errorbox.open (-1, -1, "Save Error", "Could not save file '%s'!", filename);

   }
  else
    save_click (NULL);
 }

void about_menu (BASE_GUI *) {
  msgbox.open (-1, -1, "About DFSpell", "Version %s\n\nBy %s\naj589@freenet.carleton.ca\n%s\n\nThis is currently a developers version and\nhas little help currently available.\nIf you have problems just e-mail me.", DFSPELL_VERSION, DFSPELL_AUTHOR, DFSPELL_DATE);
 }

void debug_menu (BASE_GUI *) {
  GUI_RETURN ret;
  boolean ddone = FALSE;

  debug_label.change_text ("Heap is %s         Free Mem=%ld bytes\nUsed Mem=%ld bytes    Total Mem=%ld bytes", get_heapstring(), get_memleft(), get_memused(), get_memtotal());

  debug_window.get_back();
  debug_window.draw();

  while (!ddone) {
    mouse->check();
    keyboard.check();

    ret = debug_window.event_loop();
    if (ret.code == GUIRET_CLICK && ret.index == dbut_index) ddone = TRUE;

    if (keyboard.get_enter_state() || keyboard.get_esc_state()) ddone = TRUE;
   }

  debug_window.put_back();
 }



/*=========================================================================
 *		End of GUI Event Functions
 *=======================================================================*/



/*=========================================================================
 *		Begin Main Program
 *=======================================================================*/
int main (void) {
  GUI_RETURN ret;
  REG_PROGRAM *reg;
  char buf[256];
  short i;


	/* Create a log file for outputting debuging info */
  open_log_file ("dfspell.log");

	/* Output size of structures and classes */
  write_log_entry ("sizeof(SPELL_TYPE) = %d", sizeof(SPELL_TYPE));
  write_log_entry ("sizeof(SPELLS_STD_RECORD) = %d", sizeof(SPELLS_STD_RECORD));
  write_log_entry ("sizeof(SPELLS_STD_TYPE) = %d", sizeof(SPELLS_STD_TYPE));

	/* Create the Daggerfall specific error codes */
  post_df_errors();

	/* Initialize the VGA GUI */
  if (!init_vga_mode())
    bug (ERROR_MSG, "Could not initialize the VGA gui! (ext_err_code=%d)", ext_err_code);

	/* Update the current path */
  getcwd (path, 100);

	/* Init the help windows */
  debug_window.init (150, 120, 400, 300, "DFSpell Debug");
  debug_button.init (40, 16, "Ok");
  debug_list.init (350, 202);
  dbut_index = debug_window.open (180, 270, &debug_button);
  debug_window.open (20, 30, &debug_label);
  debug_window.open (30, 60, &debug_list);

  for (i = 0; i < registry.count_programs(); i++) {
    reg = registry.get_program (i);
    sprintf (buf, "%2d) %-13s v%-6s (%-16s)", i, reg->program, reg->version, reg->date);
    if (reg) debug_list.add_tail (buf);
   }

	/* Init the RC parser */
  rc_parser.add (&next_button, "next_button");
  rc_parser.add (&prev_button, "prev_button");
  rc_parser.add (&add_button, "add_button");
  rc_parser.add (&del_button, "del_button");
  rc_parser.add (&load_button, "load_button");
  rc_parser.add (&save_button, "save_button");
  rc_parser.add (&exit_button, "exit_button");
  rc_parser.add (&main_window, "main_window");
  rc_parser.add (&count_label, "count_label");
  rc_parser.add (&spell_name, "spell_name");
  rc_parser.add (&spell_type1, "spell_type1");
  rc_parser.add (&spell_type2, "spell_type2");
  rc_parser.add (&spell_type3, "spell_type3");
  rc_parser.add (&spell_subtype1, "spell_subtype1");
  rc_parser.add (&spell_subtype2, "spell_subtype2");
  rc_parser.add (&spell_subtype3, "spell_subtype3");
  rc_parser.add (&spell_list1, "spell_list1");
  rc_parser.add (&spell_list2, "spell_list2");
  rc_parser.add (&spell_list3, "spell_list3");
  rc_parser.add (&spell_dur11, "spell_dur11");
  rc_parser.add (&spell_dur12, "spell_dur12");
  rc_parser.add (&spell_dur13, "spell_dur13");
  rc_parser.add (&spell_dur21, "spell_dur21");
  rc_parser.add (&spell_dur22, "spell_dur22");
  rc_parser.add (&spell_dur23, "spell_dur23");
  rc_parser.add (&spell_dur31, "spell_dur31");
  rc_parser.add (&spell_dur32, "spell_dur32");
  rc_parser.add (&spell_dur33, "spell_dur33");
  rc_parser.add (&dur_label1, "dur_label1");
  rc_parser.add (&dur_label2, "dur_label2");
  rc_parser.add (&dur_label3, "dur_label3");
  rc_parser.add (&spell_chance11, "spell_chance11");
  rc_parser.add (&spell_chance12, "spell_chance12");
  rc_parser.add (&spell_chance13, "spell_chance13");
  rc_parser.add (&spell_chance21, "spell_chance21");
  rc_parser.add (&spell_chance22, "spell_chance22");
  rc_parser.add (&spell_chance23, "spell_chance23");
  rc_parser.add (&spell_chance31, "spell_chance31");
  rc_parser.add (&spell_chance32, "spell_chance32");
  rc_parser.add (&spell_chance33, "spell_chance33");
  rc_parser.add (&chance_label1, "chance_label1");
  rc_parser.add (&chance_label2, "chance_label2");
  rc_parser.add (&chance_label3, "chance_label3");
  rc_parser.add (&spell_mag11, "spell_mag11");
  rc_parser.add (&spell_mag12, "spell_mag12");
  rc_parser.add (&spell_mag13, "spell_mag13");
  rc_parser.add (&spell_mag21, "spell_mag21");
  rc_parser.add (&spell_mag22, "spell_mag22");
  rc_parser.add (&spell_mag23, "spell_mag23");
  rc_parser.add (&spell_mag31, "spell_mag31");
  rc_parser.add (&spell_mag32, "spell_mag32");
  rc_parser.add (&spell_mag33, "spell_mag33");
  rc_parser.add (&spell_mag41, "spell_mag41");
  rc_parser.add (&spell_mag42, "spell_mag42");
  rc_parser.add (&spell_mag43, "spell_mag43");
  rc_parser.add (&spell_mag51, "spell_mag51");
  rc_parser.add (&spell_mag52, "spell_mag52");
  rc_parser.add (&spell_mag53, "spell_mag53");
  rc_parser.add (&mag_label1, "mag_label1");
  rc_parser.add (&mag_label2, "mag_label2");
  rc_parser.add (&mag_label3, "mag_label3");
  rc_parser.add (&frame1, "frame1");
  rc_parser.add (&frame2, "frame2");
  rc_parser.add (&frame3, "frame3");
  rc_parser.add (&element_list, "element_list");
  rc_parser.add (&target_list, "target_list");
  rc_parser.add (&cost, "cost");
  rc_parser.add (&icon, "icon");
  rc_parser.add (&index, "index");
  rc_parser.add (&file_menu, "file_menu");
  rc_parser.add (&edit_menu, "edit_menu");
  rc_parser.add (&help_menu, "help_menu");
  rc_parser.add (&file_label, "file_label");

  rc_parser.add_func (RC_FUNC(exit_click), "exit_click");
  rc_parser.add_func (RC_FUNC(next_click), "next_click");
  rc_parser.add_func (RC_FUNC(save_click), "save_click");
  rc_parser.add_func (RC_FUNC(prev_click), "prev_click");
  rc_parser.add_func (RC_FUNC(load_click), "load_click");
  rc_parser.add_func (RC_FUNC(add_click), "add_click");
  rc_parser.add_func (RC_FUNC(del_click), "del_click");
  rc_parser.add_func (RC_FUNC(spell1_event), "spell1_event");
  rc_parser.add_func (RC_FUNC(spell2_event), "spell2_event");
  rc_parser.add_func (RC_FUNC(spell3_event), "spell3_event");
  rc_parser.add_func (RC_FUNC(save_menu), "save_menu");
  rc_parser.add_func (RC_FUNC(about_menu), "about_menu");
  rc_parser.add_func (RC_FUNC(debug_menu), "debug_menu");

	/* Load and Parse the RC file */
  rc_parser.parse_file ("dfspell.rc");

	/* Create the spell lists */
  i = 0;

  while (spells[i].name) {
    spell_list1.add_dual_tail (spells[i].name, (unsigned long) (spells[i].effect << 8) + (unsigned char) spells[i].effect1);
    i++;
   }

  spell_list2.copy_list (&spell_list1);
  spell_list3.copy_list (&spell_list1);

	/* Create the element list */
  element_list.add_dual_tail ("Fire", 0);
  element_list.add_dual_tail ("Cold", 1);
  element_list.add_dual_tail ("Poison", 2);
  element_list.add_dual_tail ("Shock", 3);
  element_list.add_dual_tail ("Magic", 4);

	/* Create the target list */
  target_list.add_dual_tail ("Caster", 0);
  target_list.add_dual_tail ("Target: Touch", 1);
  target_list.add_dual_tail ("Target: Range", 2);
  target_list.add_dual_tail ("Area: Caster", 3);
  target_list.add_dual_tail ("Area: Range", 4);

	/* Change the title of the main window */
  main_window.change_title (DFSPELL_NAME);

	/* Draw the main window and all child objects */
  main_window.draw();

	/* Attempt to load a STD file */
  if (spells_std.load ("spells.std")) {
    strcpy (filename, path);
    strcat (filename, "\\spells.std");
    file_label.change_text ("%s", filename);
   }
  else if (spells_std.load ("arena2\\spells.std")) {
    strcpy (filename, path);
    strcat (filename, "\\arena2\\spells.std");
    strcat (path, "\\arena2\\");
    file_label.change_text ("%s", filename);
   }

	/* Post the new data */
  post_spell();


	/* Main input/processing loop */
  while (!done) {

	/* Redraw some things if required */
    if (redraw) {
      redraw = FALSE;
      spell_name.draw_text();
      count_label.draw();
      spell_type1.draw_text();
      spell_type2.draw_text();
      spell_type3.draw_text();
      spell_subtype1.draw_text();
      spell_subtype2.draw_text();
      spell_subtype3.draw_text();
      spell_list1.draw();
      spell_list2.draw();
      spell_list3.draw();
      spell_dur11.draw_text();
      spell_dur12.draw_text();
      spell_dur13.draw_text();
      spell_dur21.draw_text();
      spell_dur22.draw_text();
      spell_dur23.draw_text();
      spell_dur31.draw_text();
      spell_dur32.draw_text();
      spell_dur33.draw_text();
      spell_chance11.draw_text();
      spell_chance12.draw_text();
      spell_chance13.draw_text();
      spell_chance21.draw_text();
      spell_chance22.draw_text();
      spell_chance23.draw_text();
      spell_chance31.draw_text();
      spell_chance32.draw_text();
      spell_chance33.draw_text();
      spell_mag11.draw_text();
      spell_mag12.draw_text();
      spell_mag13.draw_text();
      spell_mag21.draw_text();
      spell_mag22.draw_text();
      spell_mag23.draw_text();
      spell_mag31.draw_text();
      spell_mag32.draw_text();
      spell_mag33.draw_text();
      spell_mag41.draw_text();
      spell_mag42.draw_text();
      spell_mag43.draw_text();
      spell_mag51.draw_text();
      spell_mag52.draw_text();
      spell_mag53.draw_text();
      element_list.draw();
      target_list.draw();
      cost.draw_text();
      icon.draw_text();
      index.draw_text();
      file_label.draw();
     }

		/* Get mouse and keyboard input */
    mouse->check();
    keyboard.check();

		/* Check input */
    ret = main_window.event_loop();

		/* Parse keyboard events */
    if (ret.code == GUIRET_FALSE) {
      if (keyboard.get_pgup_state())
	next_click(NULL);
      else if (keyboard.get_pgdn_state())
	prev_click(NULL);
     }
   }

  closegraph();
  return (TRUE);
 }
/*=========================================================================
 *		End Main Program
 *=======================================================================*/


