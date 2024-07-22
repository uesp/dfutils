#ifndef __DAGDATA_H
#define __DAGDATA_H

	/* Array sizes */
#define MAX_ITEMS        600
#define MAX_PICTURES     1500
#define MAX_ENCHANTS     600
#define MAX_MOBS	 100


	/* Flags for loading the DF data file */
#define DFFL_NONE     0
#define DFFL_ITEMS    1
#define DFFL_PICTURES 2
#define DFFL_ENCHANTS 4
#define DFFL_MOBS     8
#define DFFL_ALL      0xFFFF


	/* Types for loading purposes */
#define READING_NOTHING 0
#define READING_ITEM    1
#define READING_PICTURE 2
#define READING_MOB     3
#define READING_ENCHANT 4


/*========== Holds the Item Group/Subgrouping and Name ====================*/
class ITEM_TYPE {
public:
  char group;
  char sub_group;
  long cost;
  unsigned short picture;
  char *name;

	/* Class Constructor */
  ITEM_TYPE (void) { name = NULL; }
  ITEM_TYPE (const char *string) { name = create_ptr(string); }

	/* Class Destructor */
  ~ITEM_TYPE (void) { destroy(); }
  void destroy (void) { DESTROY(name); }

 };
/*========== End of Class ITEM_TYPE Definition ============================*/


/*========== Holds Picture Information ====================================*/
class PICTURE_TYPE {
public:
  unsigned short index;
  char *name;


	/* Class Constructor */
  PICTURE_TYPE (void) { name = NULL; }
  PICTURE_TYPE (const char *string) { name = create_ptr(string); }

	/* Class Destructor */
  ~PICTURE_TYPE (void) { destroy(); }
  void destroy (void) { DESTROY(name); }

 };
/*========== End of Class PICTURE_TYPE ====================================*/


/*========== Structure to hold the Material Cost Multipliers ==============*/
class MAT_TYPE {

public:
  int material;
  int constr;
  int mult;
 };
/*========== End of structure MAT_TYPE ====================================*/


/*========== Structure to Hold the Enchantments Cost ======================*/
class ENCHANT_TYPE {

public:
  unsigned short type;
  short subtype;
  long cost;
  char *name;


	/* Class Constructor */
  ENCHANT_TYPE (void) { name = NULL; }
  ENCHANT_TYPE (const char *string) { name = create_ptr(string); }

	/* Class Destructor */
  ~ENCHANT_TYPE (void) { destroy(); }
  void destroy (void) { DESTROY(name); }

 };
/*========== End of structure ENCHANT_TYPE ================================*/


/*========== Structure to Hold the Monster Data ===========================*/
class MOB_TYPE {

public:
  short index;
  long soul_cost;
  char *name;

	/* Class Constructor */
  MOB_TYPE (void) { name = NULL; }
  MOB_TYPE (const char *string) { name = create_ptr(string); }

	/* Class Destructor */
  ~MOB_TYPE (void) { destroy(); }
  void destroy (void) { DESTROY(name); }
 };
/*========== End of structure MOB_TYPE ====================================*/


/*=========== External Variables ==========================================*/
  extern ENCHANT_TYPE *enchants[];
  extern PICTURE_TYPE *pictures[];
  extern ITEM_TYPE *items[];
  extern MOB_TYPE *mobs[];
  extern MAT_TYPE armor_mult[];
  extern MAT_TYPE weapon_mult[];
  extern short num_items;
  extern short num_mobs;
  extern short num_pictures;
  extern short num_enchants;
/*=========== End of External Variable Definitions ========================*/


/*=========== Begin Function and Procedure Prototypes =====================*/

	/* Clears the Data Arrays */
  void destroy_dag_data (void);

	/* Attempts to Load Item Data From a Data File */
  boolean load_dag_data (const char *filename, const short flags);

/*=========== End of Function and Procedure Prototypes ====================*/

	/* End of __DAGDATA_H */
#endif