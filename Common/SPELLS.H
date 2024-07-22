/*=========================================================================
 *
 * SPELLS.H, June 1998 - Dave Humphrey
 *
 * Class and Functions for handling the Daggerfall SPELLS.STD file.
 *
 *=======================================================================*/



#ifndef __SPELLS_H
#define __SPELLS_H

	/* Include Files */
#include "fileutil.h"
#include "daggen.h"


	/* Number of spells allowed in the STD file */
#define MAX_SPELLS 200


/*=========================================================================
 *
 * Structure SPELL_DESC_TYPE
 *
 *=======================================================================*/
typedef struct {
  char effect;
  char effect1;
  char *name;
 } SPELL_DESC_TYPE;
/*=========================================================================
 *		End of Structure SPELL_DESC_TYPE
 *=======================================================================*/



/*=========================================================================
 *
 * Class SPELL_TYPE Definition
 *
 *=======================================================================*/
class SPELL_TYPE {

public:
  char effect;
  char effect1;
  unsigned char duration[3];
  unsigned char chance[3];
  unsigned char strength[5];

	/* Class Constructor */
  SPELL_TYPE (void) { effect = effect1 = -1; }
 };
/*=========================================================================
 *
 *=======================================================================*/


/*=========================================================================
 *
 * Class SPELL_STD_RECORD Definition
 *
 *=======================================================================*/
class SPELLS_STD_RECORD {

public:


  SPELL_TYPE spells[3];		/* The spell data, effects, duration etc... */
  short element;		/* Fire, cold, poison, shock or magic */
  short target;			/* Caster, target-touch, target-range, area-caster, area-range */
  long cost;			/* How much does the spell cost to buy */
  short u1;
  short u2;                     /* Unknown value, usually 0 (265 or 360) */
  char name[25];		/* The spell name */
  char icon;			/* Possibly which spell icon to display */
  char index;			/* Unique index identifying spell */
  short null1;			/* Always 0 (confirmed) */
  long null2;
  long null3;
  long null4;
  char null5;
 };
/*=========================================================================
 *		End of Class SPELL_STD_RECORD Definition
 *=======================================================================*/


/*=========================================================================
 *
 * Class SPELLS_STD_TYPE Definition
 *
 *=======================================================================*/
class SPELLS_STD_TYPE {

public:

  SPELLS_STD_RECORD records[MAX_SPELLS];/* All the spell record data */
  short num_records;			/* The number of spells currently defined */

	/* Class Constructor */
  SPELLS_STD_TYPE (void) { num_records = 0; }

	/* Class Destructor */
  void destroy (void) { num_records = 0; }

	/* Attempts to load a spell data file */
  boolean load (const char *filename);

	/* Attempts to save a spell data file */
  boolean save (const char *filename);
 };
/*=========================================================================
 *		End of Class SPELLS_STD_TYPE Definition
 *=======================================================================*/


/*=========================================================================
 *		External Variable Definitions
 *=======================================================================*/
  extern SPELL_DESC_TYPE spells[];
/*=========================================================================
 *		End of External Variable Definitions
 *=======================================================================*/






#endif
/*=========================================================================
 *		End of File SPELLS.H
 *=======================================================================*/
