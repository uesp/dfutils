#ifndef __DFACOMP_H
#define __DFACOMP_H

	/* The version information */
#define DFACOMP_VERSION "DFAComp v0.0b"



/*========== Begin Function and Procedure Prototypes ======================*/

	/* Sets the Item Type List according to the groups */
  void set_typelist (const short index);

	/* Menu functions */
  void menu_load     (const short = 0, const pos_t = 0, const pos_t = 0);
  void menu_save     (const short = 0, const pos_t = 0, const pos_t = 0);
  void menu_new      (const short = 0, const pos_t = 0, const pos_t = 0);
  void menu_exit     (const short = 0, const pos_t = 0, const pos_t = 0);
  void menu_next     (const short = 0, const pos_t = 0, const pos_t = 0);
  void menu_prev     (const short = 0, const pos_t = 0, const pos_t = 0);
  void menu_addart   (const short = 0, const pos_t = 0, const pos_t = 0);
  void menu_addtext  (const short = 0, const pos_t = 0, const pos_t = 0);
  void menu_del      (const short = 0, const pos_t = 0, const pos_t = 0);
  void menu_toggle   (const short = 0, const pos_t = 0, const pos_t = 0);
  void menu_edittext (const short = 0, const pos_t = 0, const pos_t = 0);
  void menu_check    (const short = 0, const pos_t = 0, const pos_t = 0);

/*========== End of Function and Procedure Prototypes =====================*/


	/* __FDACOMP_H */
#endif