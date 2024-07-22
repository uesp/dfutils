	/* To prevent multiple declarations */
#ifndef __DAGFILE_H
#define __DAGFILE_H

	/* Number of records allowed in one file */
#define MAX_RECORDS 2000

	/* Types of Records */
#define  DFRT_NONE 0
#define   DFRT_ALL 1
#define  DFRT_CHAR 2
#define  DFRT_ITEM 4
#define DFRT_GUILD 8

	/* Hex values for type */
#define  DF_EOF -1
#define  DF_ITEM 2
#define  DF_CHAR 3
#define DF_GUILD 0x0a
#define  DF_LAST 7
#define DF_SPELL 9

	/* Constants for the enchantment costs */
#define ENALL -1
#define EN_SOUL 15


/*=========== The Base Class for DF Records ===============================*/
class BASE_RECORD {

public:
  boolean loaded;	/* Is the recorded values actually loaded currently? */
  long offset;		/* Position of record in the file */
  long size;		/* The size value of the field */
  short int num_sizes;	/* Number of 0 size values */
  char type;		/* Main type of field */

  char *data;		/* The rest of the data */
  unsigned short data_length;

	/* Class Constructor/Destructor */
  BASE_RECORD (void);
  virtual ~BASE_RECORD (void) { destroy(); }
  void destroy (void);

	/* Attempts to load desired values from file, not including
	 * the header information */
  virtual boolean load_record (FILE *f);

	/* Same as with load_record */
  virtual boolean save_record (FILE *f);

 };
/*=========== End of Class BASE_RECORD ====================================*/


/*=========== Define the Item Class =======================================*/
class ITEM_RECORD : public BASE_RECORD {

public:
  char  null1[30];	/* Bytes +05 to +22 */
  short placement;	/* Bytes +23 to +24 */
  char  null2[22];	/* Bytes +25 to +3A */
  long  index1;		/* Bytes +3B to +3E */
  long  index2;		/* Bytes +3F to +42 */
  long  null3;		/* Bytes +43 to +46 */
  long  token1;		/* Bytes +47 to +4A */
  char  name[32];	/* Bytes +4B to +6A */
  short group;		/* Bytes +6B to +6C */
  short subgroup;	/* Bytes +6D to +6E */
  long  cost;		/* Bytes +6F to +72 */
  short null4;		/* Bytes +73 to +74 */
  char  id;		/* Byte +75 */
  char  null5;		/* Byte +76 */
  short wear;		/* Bytes +77 to +78 */
  short max_wear;	/* Bytes +79 to +7A */
  short null6;		/* Bytes +7B to +7C */
  short picture1;	/* Byte +7D to +7E */
  short picture2;	/* Bytes +7F to +80 */
  char  material;	/* Byte +81 */
  char  construction;	/* Byte +82 */
  char  color;		/* Byte +83 */
  char  weight;		/* Byte +84 */
  char  null7;		/* Byte +85 */
  short null8;		/* Bytes +86 to +87 */
  short enchant_pts;	/* Bytes +88 to +89 */
  long  null9;		/* Bytes +8A to +8D */
  long  enchantments[10];/* Bytes +8E to ... */

  virtual ~ITEM_RECORD (void) { ; }

	/* Attempt to load the actual item data */
  virtual boolean load_record (FILE *f);
  virtual boolean save_record (FILE *f);

	/* Calculates the 'true' cost of the item */
  virtual long compute_cost (void);
 };
/*=========== End of Class ITEM_RECORD ====================================*/


/*=========== Begin Class CHAR_RECORD Definition ==========================*/
class CHAR_RECORD : public BASE_RECORD {

public:
  char pre_data[70];
  char name[32];
  short str, intel, wil, agi, end, per, spd, luc;
  char mid_data[85];
  long gold;

  virtual ~CHAR_RECORD(void) { ; }

	/* Attempts to load/save the acutual character data */
  virtual boolean load_record (FILE *f);
  virtual boolean save_record (FILE *f);

 };
/*=========== End of Class CHAR_RECORD ====================================*/



/*=========== Begin Class GUILD_RECORD Definition =========================*/
class GUILD_RECORD : public BASE_RECORD {

public:
  short u1, u2, u3;
  long u4, u5, u6;
  short u7, u8, u9, u10, u11, u12, u13, u14, u15, u16, u17;
  long u18, u19, u20;
  short u21;
  long u22, u23, u24, u25;
  short u26;
  char u27;
  short guild_id;
  char u28, u29, u30, u31, u32;
  long u33;

  virtual ~GUILD_RECORD (void) { ; }

	/* Attempts to load/save the acutual character data */
  virtual boolean load_record (FILE *f);
  virtual boolean save_record (FILE *f);

 };
/*=========== End of Class GUILD_RECORD ===================================*/



/*=========== Begin Class DAG_FILE ========================================*/
class DAG_FILE {

public:
  char pre_data[19];    /* The header information */

  BASE_RECORD *records[MAX_RECORDS];	/* All the records */
  short num_records;

  char *post_data;	/* Data found after the last record */
  unsigned short post_length;

	/* Class Constructor/Destructor */
  DAG_FILE (void);
  ~DAG_FILE (void) { destroy(); }
  void destroy (void);

	/* Adds a new record at the specified position */
  boolean add_record (const short pos, const short type);

	/* Attempts to delete the specified record number */
  boolean del_record (const short pos);

	/* Attempts to Load a Savetree.DAT File into Memory */
  boolean load (const char *filename, const short type = DFRT_NONE);

	/* Saves the load_file as name save_file with the modified data */
  boolean save (const char *load_file, const char *save_file, const short type = DFRT_ALL);
 };
/*=========== End of Class DAG_FILE =======================================*/


/*=========== Function and Procedure Prototypes ===========================*/

	/* Searchs the enchantment array for a match */
  short find_enchantment (const long id);

	/* Attempt to find the specified item type in the array */
  short find_item (const short group, const short sub_group);

	/* Attempts to find the required picture index */
  short find_picture (const ITEM_RECORD *item);

/*=========== End of Function and Procedure Prototypes ====================*/



/*=========== External Variables ==========================================*/
  extern DAG_FILE dagfile;
/*=========== End of External Variable Definitions ========================*/




	/* End of __DAGFILE_H */
#endif