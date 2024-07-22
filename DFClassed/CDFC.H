/*=========================================================================
 *
 * CDFC.H - 25 January 1999, Dave Humphrey
 *
 *=======================================================================*/
#ifndef __CDCF_H
#define __CDCF_H


/*=========================================================================
 *
 * Begin Defines
 *
 *=======================================================================*/

	/* Number of class records allowed */
  #define MAX_DFC_RECORDS 100

/*=========================================================================
 *		End of Defines
 *=======================================================================*/


/*=========================================================================
 *
 * Class CDFCRecord Definition
 *
 *=======================================================================*/
class CDFCRecord {

public:
  char *title;  	/* The class title */
  char *origname;	/* The original class name */
  char *newname;	/* The new class name */
  char *desc;		/* The class description */
  int installed;	/* Which class is this installed over */


	/* Class Constructor */
  CDFCRecord (void);

	/* Class Destructors */
  ~CDFCRecord (void) { destroy(); }
  void destroy (void);

	/* Adds a string to the end of the description */
  void AddDesc (const char *string);

	/* Set the appropiate strings */
  void SetTitle    (const char *string) { DESTROY(title); title = create_ptr(string); }
  void SetOrigName (const char *string) { DESTROY(origname); origname = create_ptr(string); }
  void SetNewName  (const char *string) { DESTROY(newname); newname = create_ptr(string); }
  void SetDesc     (const char *string) { DESTROY(desc); desc = create_ptr(string); }

	/* Outputs the record to a file stream */
  boolean write (FILE *f);

 };
/*=========================================================================
 *		End of Class CDFCRecord Definition
 *=======================================================================*/


/*=========================================================================
 *
 * Class CDFCType Definition
 *
 *=======================================================================*/
class CDFCType {

public:

	/* The main record data */
  CDFCRecord *records[MAX_DFC_RECORDS];
  int num_records;


	/* Class Constructor */
  CDFCType (void);

	/* Class Destructors */
  ~CDFCType (void) { destroy(); }
  void destroy (void);

	/* Adds a new class record */
  CDFCRecord *add_class (const char *name);

	/* Attempts to load and parse in a DFC file */
  boolean load (const char *filename);

	/* Attempts to output a DFC file */
  boolean save (const char *filename);

 };
/*=========================================================================
 *		End of Class CDFCType Definition
 *=======================================================================*/


#endif
/*=========================================================================
 *		End of File CDCF.H
 *=======================================================================*/


