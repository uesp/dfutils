#ifndef __DAGFILE1_H
#define __DAGFILE1_H

	/* Defines the number of MAGIC.DEF records allowed */
#define MAX_DEF 256

	/* Number of records in the text.rsc file allowed */
#define MAX_RSC_RECORDS 2000


/*========== The Structure for the TEXT.RSC Header ========================*/
class RSC_HEADER {
public:
  short type;
  long offset;
 };
/*========== End of Class RSC_HEADER ======================================*/


/*========== The Structure for the TEXT.RSC file ==========================*/
class RSC_TYPE {

public:
  short header_offset;
  RSC_HEADER header[MAX_RSC_RECORDS];
  short num_texts;

	/* Class Constructor */
  RSC_TYPE (void) { num_texts = 0; }

	/* Attempts to add a new record and save file */
  boolean add_record (const char *filename, const short new_type, const char *ptr, const boolean backup = TRUE);

	/* Attempts to remove a record from a file */
  boolean del_record (const char *filename, const short type);

	/* Attempts to extract text from file */
  char *extract_text (const char *filename, const short type);

	/* Searchs Through Records for an Index */
  short find_type (const short type);

	/* Attempts to read the header of the specified filename */
  boolean read_header (const char *filename);

	/* Attempts to Read in the Text File, Converting it to RSC Format */
  char *read_textfile (const char *filename);

 };
/*========== End of Class RSC_TYPE ========================================*/


/*========== The structure for the MAGIC.DEF file =========================*/
class MAGICDEF_RECORD {

public:
  char name[32];	/* The item name */
  char magic_type;
  char group, subgroup;
  short enchantments[10];
  short uses;
  long u1;
  char material;
 };
/*========== End of Class MAGICDEF_RECORD =================================*/


/*========== The Class MAGICDEF_TYPE ======================================*/
class MAGICDEF_TYPE {

public:
  MAGICDEF_RECORD *records[MAX_DEF];
  short num_records;

	/* Class Constructor/Destructor */
  MAGICDEF_TYPE (void);
  ~MAGICDEF_TYPE (void) { destroy(); }
  virtual void destroy (void);

	/* Adds/deletes records at the appropiate points */
  virtual boolean add_record (const short where);
  virtual boolean del_record (const short where);

	/* Attempts to load the def file */
  virtual boolean load (const char *filename);
  virtual boolean save (const char *filename);

 };
/*========== End of CLASS MAGICDEF_TYPE ===================================*/


	/* End of __DAGFILE1_H */
#endif
