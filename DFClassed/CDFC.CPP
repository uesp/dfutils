/*=========================================================================
 *
 * CDFC.CPP - 25 January 1999, Dave Humphrey
 *
 *=======================================================================*/


#include "genutil.h"
#include "cdfc.h"


/*=========================================================================
 *
 * Class CDFCRecord Constructor
 *
 *=======================================================================*/
CDFCRecord::CDFCRecord (void) {
  installed = -1;
  origname = NULL;
  newname = NULL;
  title = NULL;
  desc = NULL;
 }
/*=========================================================================
 *		End of Class CDFCRecord Constructor
 *=======================================================================*/


/*=========================================================================
 *
 * Class CDFCRecord Destructor
 *
 *=======================================================================*/
void CDFCRecord::destroy (void) {
  installed = -1;
  DESTROY(origname);
  DESTROY(newname);
  DESTROY(title);
  DESTROY(desc);
 }
/*=========================================================================
 *		End of Class CDFCRecord Destructor
 *=======================================================================*/


/*=========================================================================
 *
 * Class CDFCRecord Method - void AddDesc (string);
 *
 *=======================================================================*/
void CDFCRecord::AddDesc (const char *string) {
  char *ptr;

	/* Does a desc currently exist? */
  if (!desc) {
    SetDesc(string);
    return;
   }

	/* Create a new pointer to hold both strings */
  ptr = create_ptr(strlen(desc) + strlen(string) + 4);
  strcpy(ptr, desc);
  chrcat(ptr, '\n');
  strcat(ptr, string);
  DESTROY(desc);
  desc = ptr;
 }
/*=========================================================================
 *		End of Class Method CDFCRecord::AddDesc()
 *=======================================================================*/


/*=========================================================================
 *
 * Class CDFCRecord Method - boolean write (f);
 *
 * Outputs a record to an open file stream.
 *
 *=======================================================================*/
boolean CDFCRecord::write (FILE *f) {
  char *ptr, *temp_ptr;

  fprintf (f, "class=%s\n", title);
  if (origname) fprintf (f, "%corigname=%s\n", 9, origname);
  if (newname) fprintf (f, "%cnewame=%s\n", 9, newname);

	/* Special case for the description */
  if (desc) {
    temp_ptr = desc;
    ptr = strchr(desc, '\n');

    while (ptr != NULL) {
      *ptr = 0;
      fprintf (f, "%cdesc=%s\n", 9, temp_ptr);
      *ptr = '\n';
      temp_ptr = ptr + 1;
      ptr = strchr (temp_ptr, '\n');
     }

    fprintf (f, "%cdesc=%s\n", 9, temp_ptr);
   }

  fprintf (f, "%cinstalled=%d\n", 9, installed);
  fprintf (f, "end_class\n");

  return (TRUE);
 }
/*=========================================================================
 *		End of Class Method CDFCRecord::write()
 *=======================================================================*/


/*=========================================================================
 *
 * Class CDFCType Constructor
 *
 *=======================================================================*/
CDFCType::CDFCType (void) {
  num_records = 0;
 }
/*=========================================================================
 *		End of Class CDFCType Constructor
 *=======================================================================*/


/*=========================================================================
 *
 * Class CDFCType Destructor
 *
 *=======================================================================*/
void CDFCType::destroy (void) {
  int i;	/* Loop counter */

  for (i = 0; i < num_records; i++) {
    DESTROY(records[i]);
   }

  num_records = 0;
 }
/*=========================================================================
 *		End of Class CDFCType Destructor
 *=======================================================================*/


/*=========================================================================
 *
 * Class CDFCType Method - CDFCRecord *add_class (title);
 *
 * Adds a new class record to the array and returns a pointer to the new
 * object on success.  On error, returns NULL.
 *
 *=======================================================================*/
CDFCRecord *CDFCType::add_class (const char *name) {

	/* Ignore invalid input */
  if (name == NULL) {
    err_code = ERR_NULL;
    return (NULL);
   }

	/* Can we add a new class record? */
  if (num_records >= MAX_DFC_RECORDS) {
    write_log_entry ("   Maximum of %d DFC records exceeded with addition of class '%s'!", MAX_DFC_RECORDS, name);
    err_code = ERR_MAXINDEX;
    return (NULL);
   }

	/* Create the new object */
  if ((records[num_records] = new CDFCRecord) == NULL) error_handler.bug (ERR_MEM, "CDFCType::add_class() - *records[] (%u)", sizeof(CDFCRecord));
  num_records++;

	/* Set the class title */
  records[num_records-1]->SetTitle(name);
  return (records[num_records-1]);
 }
/*=========================================================================
 *		End of Class Method CDFCType::add_class()
 *=======================================================================*/


/*=========================================================================
 *
 * Class CDFCType Method - boolean load (filename);
 *
 * Attempts to load the specified DFC file into memory.  Returns FALSE
 * on any error.
 *
 *=======================================================================*/
boolean CDFCType::load (const char *filename) {
  char buf[256];	/* Temp input string */
  char *ptr, *temp_ptr;	/* Temp string pointers */
  CDFCRecord *classptr; /* Current input class pointer */
  FILE *f;		/* File pointer */
  int line = 0;		/* Input file line counter */

	/* Attempt to open file for input */
  if ((f = openfile(filename, "rt")) == NULL) return (FALSE);

	/* Clear the current data */
  destroy();
  classptr = NULL;

	/* Main input loop */
  while (!feof(f)) {
    line++;

		/* Read in one line from file */
    if (read_eol(f, buf, 255) == READ_MSL) {
      write_log_entry ("   %d: Exceeded maximum line length of %d characters!", line, 255);
      read_eol(f);
     }

		/* Remove any comments and whitespace */
    if ((ptr = strchr(buf, '#')) != NULL) *ptr = 0;
    ptr = trim(buf);
    if (*ptr == 0) continue;

		/* Attempt to find the the '=' character */
    if ((temp_ptr = strchr(ptr, '=')) != NULL) {
      *temp_ptr = 0;
      temp_ptr = ltrim(temp_ptr + 1);
      rtrim(ptr);
     }

    strlwr(ptr);

		/* End of a class record? */
    if (classptr && !strcmp(ptr, "end_class")) {
      classptr = NULL;
      continue;
     }
    else if (temp_ptr == NULL) {
      write_log_entry ("   %d: No '=' variable separator found!", line);
      continue;
     }

		/* Parse the variable/value pair */
    if (!strcmp(ptr, "class")) {
      if (classptr != NULL) write_log_entry ("   %d: New class '%s' before previous 'end_class' tag found!", temp_ptr, line);
      classptr = add_class(temp_ptr);
      continue;
     }
    else if (*temp_ptr == 0) {
      write_log_entry ("   %d: Empty string variable found!", line);
      continue;
     }
    else if (!classptr) {
      write_log_entry ("   %d: No class currently defined!", line);
      continue;
     }

		/* Parse the interior class variable/value pairs */
    if (!strcmp(ptr, "title"))
      classptr->SetTitle(temp_ptr);
    else if (!strcmp(ptr, "origname"))
      classptr->SetOrigName(temp_ptr);
    else if (!strcmp(ptr, "newname"))
      classptr->SetNewName(temp_ptr);
    else if (!strcmp(ptr, "desc"))
      classptr->AddDesc(temp_ptr);
    else if (!strcmp(ptr, "installed"))
      classptr->installed = atoi(temp_ptr);
    else
      write_log_entry ("   %d: Unknown variable tag '%s'!", line, ptr);

   }

  fclose (f);
  return (TRUE);
 }
/*=========================================================================
 *		End of Class Method CDFCType::load()
 *=======================================================================*/



/*=========================================================================
 *
 * Class CDFCType Method - boolean save (filename);
 *
 * Attempts to save the specified DFC file into memory.  Returns FALSE
 * on any error.
 *
 *=======================================================================*/
boolean CDFCType::save (const char *filename) {
  FILE *f;	/* File pointer */
  int i;	/* Loop counter */

	/* Attempt to open file for output */
  if ((f = openfile(filename, "wt")) == NULL) return (FALSE);
  fprintf (f, "#\n# Do not Edit! File read and overwritten by CLASSED.EXE...\n#\n");

  for (i = 0; i < num_records; i++) {
    records[i]->write(f);
   }

  fclose (f);
  return (TRUE);
 }
/*=========================================================================
 *		End of Class Method CDFCType::save()
 *=======================================================================*/

















