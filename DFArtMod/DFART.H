#ifndef __DFART_H
#define __DFART_H

	/* Version Information */
#define DFART_VERSION "DFArtMod v1.0 - March 1998"
#define DFART_MAJOR_VERSION '1'
#define DFART_MINOR_VERSION '0'

	/* Number of artifacts we're allowed to add */
#define MAX_ARTIFACTS 240

	/* Maximum allowed index of an artifact */
#define MAX_ART_INDEX 255


/*========== Define the Extended Class AMD_RECORD =========================*/
class AMD_RECORD : public MAGICDEF_RECORD {

public:
  short index;		/* The # of the artifact */
 };
/*========== End of Class AMD_RECORD ======================================*/


/*========== Define the Class AMD_TYPE ====================================*/
class AMD_TYPE {

public:
	/* The artifact records */
  AMD_RECORD *records[MAX_ARTIFACTS];
  short num_records;

	/* The text records */
  char *texts[MAX_ARTIFACTS];
  short text_index[MAX_ARTIFACTS];
  short num_texts;

	/* The path and filename of the current file */
  char path[257];
  char filename[16];

	/* Information about the current file */
  char author[17];
  char major_version, minor_version;
  struct date save_date;
  struct time save_time;

	/* Class Constructor/Destructor */
  AMD_TYPE (void);
  ~AMD_TYPE (void) { destroy(); }
  virtual void destroy (void);

	/* Adds/deletes records at the appropiate points */
  virtual boolean add_art_record (void);
  virtual boolean del_art_record (const short where);
  virtual boolean add_text_record (const short index, const char *text);
  virtual boolean del_text_record (const short where);

	/* Checks the current values for any errors / conflicts */
  virtual boolean check (const short first_err = 0);

	/* Attempts to load the AMD file */
  virtual boolean load (const char *filename);
  virtual boolean save (const char *filename);
 };

/*========== End of Class AMD_TYPE ========================================*/


/*========== External Variable Definitions ================================*/
  extern short dfart_check_index;
  extern boolean dfart_check_text;
/*========== End of External Variable Definitions =========================*/


	/* End of __DFART_H */
#endif