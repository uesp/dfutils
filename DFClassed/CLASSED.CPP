/*=========================================================================
 *
 * CLASSED.CPP - 25 January 1999, Dave Humphrey
 *
 *=======================================================================*/

	/* Include Files */
#include "genutil.h"
#include "guitext.h"
#include "gui_rc.h"
#include "classed.h"
#include "cdfc.h"


	/* The GUI objects */
WINDOW_TEXT main_window, about_window;
LIST_TEXT uninstalled_list, installed_list;
LABEL_TEXT uninstalled_label, installed_label, desc_label;
LABEL_TEXT about_name_label, about_version_label, about_author_label;
LABEL_TEXT about_date_label, about_desc_label;
BUTTON_TEXT install_button, uninstall_button, uninstall_all_button;
BUTTON_TEXT exit_button, about_button, about_ok_button;
TEXTBOX_TEXT desc_textbox;

	/* Main loop variable */
boolean done = FALSE, ok_done = FALSE;

	/* Main class data object */
CDFCType ClassData;

	/* Which index is currently displayed in the description textbox */
int current_desc = -1;


/*=========================================================================
 *
 * Procedure - void create_lists (void);
 *
 *=======================================================================*/
void create_lists (void) {
  int i;	/* Loop counter */

	/* Clear the current list items */
  uninstalled_list.clear_list();
  installed_list.clear_list();

	/* Add items */
  for (i = 0; i < ClassData.num_records; i++) {
    if (ClassData.records[i]->installed == -1) {
      uninstalled_list.add_dual_tail (ClassData.records[i]->title, (long)i);
     }
    else {
      installed_list.add_dual_tail (ClassData.records[i]->title, (long)i);
     }
   }

 }
/*=========================================================================
 *		End of Procedure create_lists()
 *=======================================================================*/


/*=========================================================================
 *
 * GUI User Event Functions
 *
 *=======================================================================*/
void OnExit (BASE_GUI *) {
  done = TRUE;
 }

void OnAbout (BASE_GUI *) {
  about_window.get_back();
  about_window.draw();
  ok_done = FALSE;

  while (!ok_done) {

	/* Get mouse and keyboard input */
    mouse->check();
    keyboard.check();

	/* Check input */
    about_window.event_loop();

	/* Get keyboard input */
    if (keyboard.get_esc_state()) ok_done = TRUE;
   }

  about_window.put_back();
 }

void OnAboutOk (BASE_GUI *) {
  ok_done = TRUE;
 }

void OnUninstall (BASE_GUI *) {
  int i, j;

	/* Ensure there are items to uninstall */
  if (installed_list.count() == 0) {
    msgbox.open (-1, -1, "Uninstall Warning", "No classes currently installed!");
    return;
   }

	/* Get currently selected item index */
  i = installed_list.selected;
  j = (int)installed_list.get_dual_value();

	/* Ensure there is a valid selected item in list */
  if (i < 0 || j < 0 || j >= ClassData.num_records) {
    msgbox.open (-1, -1, "Uninstall Warning", "No installed classes currently selected!");
    return;
   }

	/* Ensure the user wishes to uninstall the item */
  if (promptbox.open (-1, -1, 0, "Uninstall Class", "Are you sure you wish to uninstall the class\n   %s?", ClassData.records[j]->title) != PB_YES) return;

	/* Uninstall */
  ClassData.records[j]->installed = -1;

  create_lists();
  uninstalled_list.draw();
  installed_list.draw();
 }

void OnInstall (BASE_GUI *) {
  int i, j;

	/* Ensure there are items to install */
  if (uninstalled_list.count() == 0) {
    msgbox.open (-1, -1, "Install Warning", "No classes currently available to install!");
    return;
   }

	/* Get currently selected item index */
  i = uninstalled_list.selected;
  j = (int)uninstalled_list.get_dual_value();

	/* Ensure there is a valid selected item in list */
  if (i < 0 || j < 0 || j >= ClassData.num_records) {
    msgbox.open (-1, -1, "Install Warning", "No available classes currently selected!");
    return;
   }

	/* Ensure the user wishes to install the item */
  if (promptbox.open (-1, -1, 0, "Install Class", "Are you sure you wish to install the class\n   %s?", ClassData.records[j]->title) != PB_YES) return;

	/* Install */
  ClassData.records[j]->installed = 1;

  create_lists();
  uninstalled_list.draw();
  installed_list.draw();
 }

void OnUninstallAll (BASE_GUI *) {
  int i;

	/* Ensure there are items to uninstall */
  if (installed_list.count() == 0) {
    msgbox.open (-1, -1, "Uninstall Warning", "No classes currently installed!");
    return;
   }

	/* Ensure the user wishes to uninstall everything */
  if (promptbox.open (-1, -1, 0, "Uninstall All Classes", "Are you sure you wish to remove all\nthe currently installed classes?") != PB_YES) return;

	/* Uninstall everything */
  for (i = 0; i < ClassData.num_records; i++) {
    ClassData.records[i]->installed = -1;
   }

  create_lists();
  uninstalled_list.draw();
  installed_list.draw();
 }

void UpdateDesc (const int i) {

	/* Ensure the text is different */
  if (i == current_desc) return;

	/* Is the index valid? */
  if (i < 0 || i >= ClassData.num_records)
    desc_textbox.parse_text("");
  else
    desc_textbox.parse_text(ClassData.records[i]->desc);

  current_desc = i;
  desc_textbox.draw();
 }

void OnInstalledChange (BASE_GUI *) {
  UpdateDesc((int)installed_list.get_dual_value());
 }

void OnUninstalledChange (BASE_GUI *) {
  UpdateDesc((int)uninstalled_list.get_dual_value());
 }

boolean OnInstalledGotFocus (BASE_GUI *) {
  UpdateDesc((int)installed_list.get_dual_value());
  return (TRUE);
 }

boolean OnUninstalledGotFocus (BASE_GUI *) {
  UpdateDesc((int)uninstalled_list.get_dual_value());
  return (TRUE);
 }
/*=========================================================================
 *		End of GUI User Event Functions
 *=======================================================================*/



/*=========================================================================
 *
 * Procedure - void init_rc (void);
 *
 * Initializes the RC parser with the appropiate objects.
 *
 *=======================================================================*/
void init_rc (void) {
  RCADDVAR(main_window);
  RCADDVAR(desc_label);
  RCADDVAR(desc_textbox);
  RCADDVAR(uninstalled_label);
  RCADDVAR(uninstalled_list);
  RCADDVAR(installed_label);
  RCADDVAR(installed_list);
  RCADDVAR(install_button);
  RCADDVAR(uninstall_button);
  RCADDVAR(uninstall_all_button);
  RCADDVAR(about_button);
  RCADDVAR(exit_button);
  RCADDVAR(about_window);
  RCADDVAR(about_ok_button);
  RCADDVAR(about_name_label);
  RCADDVAR(about_version_label);
  RCADDVAR(about_author_label);
  RCADDVAR(about_date_label);
  RCADDVAR(about_desc_label);
  RCADDFUNC(OnAboutOk);
  RCADDFUNC(OnExit);
  RCADDFUNC(OnInstall);
  RCADDFUNC(OnUninstall);
  RCADDFUNC(OnUninstalledChange);
  RCADDFUNC(OnInstalledChange);
  RCADDFUNC(OnUninstallAll);
  RCADDFUNC(OnAbout);
  uninstalled_list.user_got_focus = OnUninstalledGotFocus;
  installed_list.user_got_focus = OnInstalledGotFocus;
 }
/*=========================================================================
 *		End of Procedure init_rc()
 *=======================================================================*/



/*=========================================================================
 *
 * Begin Main Program
 *
 *=======================================================================*/
int main (void) {
  GUI_RETURN ret;	/* Return event code */

	/* Open log for debugging output */
  open_log_file (DFC_LOG_FILE);

	/* Attempt to open the class info file */
  if (!ClassData.load(DFC_DATA_FILE))
    error_handler.bug (ERROR_MSG, "Failed to load the DFC data file '%s'!", DFC_DATA_FILE);

	/* Attempt to initialize the text graphics GUI */
  if (!init_text_mode())
    error_handler.bug (ERROR_MSG, "Failed to initialize the text mode GUI!");

	/* Set the RC variable and function pointers */
  init_rc();

	/* Call the RC parser */
  if (!rc_parser.parse_file(DFC_RC_FILE))
    error_handler.bug (ERROR_MSG, "Failed to load/parse the RC file '%s'!", DFC_RC_FILE);

	/* Fix the mouse window area */
  mouse->window (0, 0, 639, 199);

	/* Size the class description textbox */
  desc_textbox.width = 60;
  desc_textbox.height = 5;

	/* Initialize the window's lists */
  create_lists();

	/* Draw the main window and all child objects */
  main_window.draw();


	/* Main input/processing loop */
  while (!done) {

		/* Get mouse and keyboard input */
    mouse->check();
    keyboard.check();

		/* Check input */
    ret = main_window.event_loop();

		/* Parse any keyboard events */
    if (ret.code == GUIRET_FALSE) {
      if (keyboard.get_alt_key(SCAN_X) || keyboard.get_alt_key(SCAN_Q))
	done = TRUE;
     }
   }

	/* Save the current settings */
  ClassData.save (DFC_DATA_FILE);

	/* Clear the screen and reset things */
  textattr(LIGHTGRAY);
  cursor->show();
  clrscr();

	/* Return success */
  return (0);
 }
/*=========================================================================
 *		End of Main Program
 *=======================================================================*/






