#ifndef __DEFEDIT_H
#define __DEFEDIT_H

	/* The version information */
#define DEFEDIT_VERSION "DEFEdit v0.0b"



/*========== Begin Function and Procedure Prototypes ======================*/

	/* Sets the Item Type List according to the groups */
  void set_typelist (const short index);

	/* Menu functions */
  short menu_save (__CPPARGS);
  short menu_exit (__CPPARGS);
  short menu_next (__CPPARGS);
  short menu_prev (__CPPARGS);
  short menu_add (__CPPARGS);
  short menu_del (__CPPARGS);

/*========== End of Function and Procedure Prototypes =====================*/


	/* __DEFEDIT_H */
#endif