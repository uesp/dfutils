	/* Standard C Includes */
#include <stdlib.h>
#include <stdio.h>
#include <mem.h>

	/* User Defined Includes */
#include "genutil.h"
#include "fileutil.h"
#include "dagbutil.h"

	/* Holds the current error status */
int dagb_error = DAGB_ERR_NONE;

	/* Error Messages */
char *dagbutil_errmsg[] = {
  "No Error",
  "Failed to Open File for Read/Write",
  "Unexpected End of File on Read/Write",
  "Book File contains too many pages",
  "Page Exceeds 64kb",
  "File Does Not Fit Known Book Format"
 };



/*========== The Class DAGBOOK_TYPE Constructor ===========================*/
void DAGBOOK_TYPE::DAGBOOK_TYPE (void) {
  int i;	/* Loop counter */

	/* Set pointers to nothing initially */
  for (i = 0; i < MAX_PAGES; i++) {
    pages[i] = NULL;
    page_offsets[i] = 0;
   }

	/* Init the default values */
  init();
 }
/*========== End of the Class Constructor =================================*/


/*========== Deletes the Entire Text ======================================*/
void DAGBOOK_TYPE::destroy (void) {
  int i;	/* Loop counter */

	/* Delete any allocated pointer */
  for (i = 0; i < MAX_PAGES; i++) {
    if (pages[i]) {
      delete pages[i];
      pages[i] = NULL;
     }

    page_offsets[i] = 0;
   }

  num_pages = 0;
 }
/*========== End of Procedure DAGBOOK_TYPE::destroy() =====================*/


/*========== Init the BOOK Variables with Defaults ========================*/
void DAGBOOK_TYPE::init (void) {
  memset (title, 0, TITLE_WIDTH);
  memset (author, 0, AUTHOR_WIDTH);
  num_pages = 0;
  header[0] = 0xE8;
  header[1] = 0x03;
  header[2] = header[3] = 0;
  header[4] = 1;
  header[5] = 0;
  header[6] = 0xD2;
  header[7] = 0x04;
  header[8] = 0x29;
  header[9] = 0x09;
 }
/*========== End of Procedure DAGBOOK_TYPE::init() ========================*/


/*========== Attempts to Load a Book File Into Memory =====================*/
boolean DAGBOOK_TYPE::load_book (const char *filename) {
  FILE *f;	/* File Pointer */
  int i;	/* Loop counter */
  long size;

	/* Attempt to open file for reading */
  if (!(f = openfile (filename, "rb"))) {
    dagb_error = DAGB_ERR_FILE;
    return (FALSE);
   }

	/* Is the file a book file? */
  if (!check_book_file(f)) {
    fclose (f);
    dagb_error = DAGB_ERR_FORMAT;
    return (FALSE);
   }

	/* Clear the current information */
  destroy();

	/* Read the Header information */
  if (fread (title, sizeof(char), TITLE_WIDTH, f) != TITLE_WIDTH) {
    dagb_error = DAGB_ERR_READ;
    return (FALSE);
   }

  if (fread (author, sizeof(char), AUTHOR_WIDTH, f) != AUTHOR_WIDTH) {
    dagb_error = DAGB_ERR_READ;
    return (FALSE);
   }

  if (fread (header, sizeof(char), 10, f) != 10) {
    dagb_error = DAGB_ERR_READ;
    return (FALSE);
   }

  num_pages = read_int(f);

	/* Make sure we can handle this file... */
  if (num_pages >= MAX_PAGES) {
    dagb_error = DAGB_ERR_MAXPAGES;
    return (FALSE);
   }

	/* Read in the Page Offsets */
  for (i = 0; i < num_pages; i++) {
    if ((page_offsets[i] = read_long(f)) == -1) {
      dagb_error = DAGB_ERR_READ;
      return (FALSE);
     }
   }

  page_offsets[num_pages] = get_filesize(f);

	/* Now the Main Text */
  for (i = 0; i < num_pages; i++) {
    size = page_offsets[i + 1] - page_offsets[i];

	/* Make sure the page will fit into memory */
    if (size > 64000l) {
      dagb_error = DAGB_ERR_PAGESIZE;
      return (FALSE);
     }
    else {
      pages[i] = create_ptr ((unsigned int)size + 1);

      if (fread (pages[i], sizeof(char), (unsigned int)size, f) != (unsigned int)size) {
	dagb_error = DAGB_ERR_READ;
	return (FALSE);
       }
     }
   }

  fclose (f);
  dagb_error = DAGB_ERR_NONE;
  return (TRUE);
 }
/*========== End of Function DAGBOOK_TYPE::load_book() ====================*/


/*========== Attempts to Save a Book File to File =========================*/
boolean DAGBOOK_TYPE::save_book (const char *filename) {
  FILE *f;	/* File Pointer */
  int i;	/* Loop counter */
  long size;

	/* Attempt to open file for reading */
  if (!(f = openfile (filename, "wb"))) {
    dagb_error = DAGB_ERR_FILE;
    return (FALSE);
   }

	/* Write the Header information */
  if (fwrite (title, sizeof(char), TITLE_WIDTH, f) != TITLE_WIDTH) {
    dagb_error = DAGB_ERR_READ;
    return (FALSE);
   }

  if (fwrite (author, sizeof(char), AUTHOR_WIDTH, f) != AUTHOR_WIDTH) {
    dagb_error = DAGB_ERR_NONE;
    return (FALSE);
   }

  if (fwrite (header, sizeof(char), 10, f) != 10) {
    dagb_error = DAGB_ERR_NONE;
    return (FALSE);
   }

  write_int (f, num_pages);

	/* Write all the Page Offsets */
  for (i = 0; i < num_pages; i++)
    write_long (f, page_offsets[i]);

	/* Now Write the Main Text */
  for (i = 0; i < num_pages; i++) {
    size = page_offsets[i + 1] - page_offsets[i];

    if (fwrite (pages[i], sizeof(char), (unsigned int)size, f) != (unsigned int)size) {
      dagb_error = DAGB_ERR_NONE;
      return (FALSE);
     }
   }

  fclose (f);
  dagb_error = DAGB_ERR_NONE;
  return (TRUE);
 }
/*========== End of Function DAGBOOK_TYPE::save_book() ====================*/


/*========== Tests to See if the File is a Book File or Not ===============*/
boolean check_book_file (FILE *f) {
  long offset = ftell(f);

	/* Move to header offset */
  if (fseek (f, 224, SEEK_SET)) return(FALSE);

  switch (read_int(f)) {
    case 0x03E8:
    case 0x012C:
    case 0x0190:
    case 0x000A:
    case 0x1388:
    case 0x01F4:
    case 0x2710:
      break;
    default:
      return (FALSE);
   };

  switch (read_int(f)) {
    case 0x0000:
      break;
    default:
      return (FALSE);
   }

  switch (read_int(f)) {
    case 1:
    case 2:
    case 3:
    case 4:
      break;
    default:
      return (FALSE);
   }

  switch (read_int(f)) {
    case 0x04D2:
    case 0x04D3:
      break;
    default:
      return (FALSE);
   }

  switch (read_int(f)) {
    case 0x0929:
    case 0x092A:
      break;
    default:
      return (FALSE);
   }

	/* Return to the initial position */
  fseek (f, offset, SEEK_SET);
  return (TRUE);
 }
/*========== End of Function check_book_file() ============================*/