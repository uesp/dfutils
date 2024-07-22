#ifndef __DFITEM_H
#define __DFITEM_H


#include "guitype.h"

	/* The version number */
#define DFITEM_VERSION "DFItem v0.1b"

	/* Picture for the letter of credit */
#define CREDIT_PICTURE 0x6888


/*========== Function and Procedure Prototypes ============================*/

	/* Inits All the Values, GUI Variables etc... */
  void dfitem_init (void);

	/* Edits the Enchantment Type, Returns Enchantment value */
  long edit_enchantment (const long enchant);

	/* Finds the Character Data in the Records, Returns array index or -1 */
  short find_char (void);

	/* Searches for the Next Item containing a string */
  short find_searchitem (const short current, const short dir, const char *string);

	/* Attempt to find the Next Item after the current one */
  short find_nextitem (const short current, const short dir = 1);

	/* Attempts to find the Next Magical Item after the current one */
  short find_next_magicitem (const short current, const short dir = 1);

	/* Gets Item Values from the GUI Fields and saves them to the record */
  void get_values (void);

	/* Displays Load Window, Prompts User to Load a Game */
  boolean load_game (void);

	/* Posts Item Values from Records to the GUI Fields */
  void post_values (void);

	/* Attempts to Sell the Specified Item, Returns true on success */
  boolean sell_item (const short item);

	/* Searchs the Picture List and sets the appropiate selected */
  void set_piclist (const short type);
  void set_typelist (const short index);

	/* Menu functions, called by the button presses/keystrokes etc... */
  void menu_sell (const short button = -1, const pos_t mx = -1, const pos_t my = -1);
  void menu_fix (const short button = -1, const pos_t mx = -1, const pos_t my = -1);
  void menu_exit (const short button = -1, const pos_t mx = -1, const pos_t my = -1);
  void menu_load (const short button = -1, const pos_t mx = -1, const pos_t my = -1);
  void menu_save (const short button = -1, const pos_t mx = -1, const pos_t my = -1);
  void menu_next (const short button = -1, const pos_t mx = -1, const pos_t my = -1);
  void menu_prev (const short button = -1, const pos_t mx = -1, const pos_t my = -1);
  void menu_nextmag (const short button = -1, const pos_t mx = -1, const pos_t my = -1);
  void menu_prevmag (const short button = -1, const pos_t mx = -1, const pos_t my = -1);
  void menu_find (const short button = -1, const pos_t mx = -1, const pos_t my = -1);
  void menu_findnext (const short button = -1, const pos_t mx = -1, const pos_t my = -1);
  void menu_findprev (const short button = -1, const pos_t mx = -1, const pos_t my = -1);

/*========== End of Function and Procedure Prototypes =====================*/


	/* End of __DFITEM_H */
#endif
