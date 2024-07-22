	/* Standard C Includes */
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <ctype.h>
#include <string.h>
#include <dos.h>

	/* User Defined Includes */
#include "fileutil.h"
#include "cmstring.h"
#include "daggen.h"
#include "dagfile1.h"
#include "dfart.h"

	/* For check error messages */
short dfart_check_index = 0;
boolean dfart_check_text = FALSE;


/*========== Class AMD_TYPE Constructor ===================================*/
void AMD_TYPE::AMD_TYPE (void) {
  short i;	/* Loop counter */

  minor_version = DFART_MINOR_VERSION;
  major_version = DFART_MAJOR_VERSION;
  num_records = 0;
  num_texts = 0;
  path[0] = 0;
  author[0] = 0;
  strcpy (filename, "none.amd");

  for (i = 0; i < MAX_ARTIFACTS; i++) {
    records[i] = NULL;
    texts[i] = NULL;
    text_index[i] = 23 + i;
   }
 }
/*========== End of Class AMD_TYPE Constructor ============================*/


/*========== Class AMD_TYPE Destructor ====================================*/
void AMD_TYPE::destroy (void) {
  short i;	/* Loop counter */

  num_records = 0;
  num_texts = 0;
  author[0] = 0;
  path[0] = 0;
  strcpy (filename, "none.amd");

	/* Delete any allocated records */
  for (i = 0; i < MAX_ARTIFACTS; i++) {

    if (records[i]) {
      delete records[i];
      records[i] = NULL;
     }

    if (texts[i]) {
      delete texts[i];
      texts[i] = NULL;
     }
   }

 }
/*========== End of Class AMD_TYPE Destructor =============================*/


/*========== Attempts to Add a Record at the End of List ==================*/
boolean AMD_TYPE::add_art_record (void) {
  short i;	/* Loop counter */

	/* Make sure its a valid location */
  if (num_records >= MAX_ARTIFACTS) {
    err_code = ERR_INDEX;
    return (FALSE);
   }

	/* Attempt to allocate the new record */
  if (!(records[num_records] = new AMD_RECORD))
    bug (MEM_ERR_MSG, "AMD_TYPE::add_art_record() - *records[] (%d)", sizeof(AMD_RECORD));

  records[num_records]->magic_type = 2;
  records[num_records]->index = 23;
  records[num_records]->uses = 0;
  records[num_records]->u1 = 0;
  records[num_records]->material = 0;
  records[num_records]->group = 2;
  records[num_records]->subgroup = 2;
  strcpy (records[num_records]->name, "None");

  for (i = 0; i < 10; i++)
    records[num_records]->enchantments[i] = -1;

  num_records++;
  err_code = ERR_NONE;
  return (TRUE);
 }
/*========== End of Function AMD_TYPE::add_art_record() ===================*/


/*========== Attempts to Add a New Text Record to the End of List =========*/
boolean AMD_TYPE::add_text_record (const short index, const char *text) {

	/* Make sure its a valid location */
  if (num_texts >= MAX_ARTIFACTS) {
    err_code = ERR_INDEX;
    return (FALSE);
   }

	/* Attempt to allocate the new record */
  if (text)
    texts[num_texts] = create_ptr (text);
  else
    texts[num_texts] = create_ptr (" ");

  text_index[num_texts] = index;

  num_texts++;
  err_code = ERR_NONE;
  return (TRUE);
 }
/*========== End of Function AMD_TYPE::add_text_record() ==================*/


/*========== Attempts to Delete a Record at the Desired Position ==========*/
boolean AMD_TYPE::del_art_record (const short where) {
  short i;	/* Loop counter */
			/* Make sure its a valid location */
  if (where >= num_records || where < 0) {
    err_code = ERR_INDEX;
    return (FALSE);
   }

	/* Delete the record from memory */
  if (records[where]) delete records[where];

	/* Shift things ... */
  for (i = where; i < num_records; i++) {
    records[i] = records[i + 1];
   }

  num_records--;
  err_code = ERR_NONE;
  return (TRUE);
 }
/*========== End of Function AMD_TYPE::del_art_record() ===================*/


/*========== Attempts to Delete the Specified Text Record =================*/
boolean AMD_TYPE::del_text_record (const short where) {
  short i;	/* Loop counter */

			/* Make sure its a valid location */
  if (where >= num_texts || where < 0) {
    err_code = ERR_INDEX;
    return (FALSE);
   }

  if (texts[where]) delete texts[where];

	/* Shift things ... */
  for (i = where; i < num_texts; i++) {
    texts[i] = texts[i + 1];
    text_index[i] = text_index[i + 1];
   }

  num_texts--;
  err_code = ERR_NONE;
  return (TRUE);
 }
/*========== End of Function AMD_TYPE::del_text_record() ==================*/


/*========== Checks Values for Errors =====================================*/
boolean AMD_TYPE::check (const short first_err) {
  short i, j;
  short count = 0;

	/* Check the artifact section first */
  dfart_check_text = FALSE;

  for (i = 0; i < num_records; i++) {
    dfart_check_index = i;

	/* Make sure there is a record where there should be one */
    if (!records[i]) {
      count++;

      if (count >= first_err) {
	err_code = DF_ERR_BADREC;
	return (FALSE);
       }
     }

	/* Check for a valid index */
    if (records[i]->index < 23) {
      count++;

      if (count >= first_err) {
	err_code = DF_ERR_ARTMIN;
	return (FALSE);
       }
     }

    if (records[i]->index > 255) {
      count++;

      if (count >= first_err) {
	err_code = DF_ERR_ARTMAX;
	return (FALSE);
       }
     }
   }

	/* Check the text section */
  dfart_check_text = TRUE;

  for (i = 0; i < num_texts; i++) {
    dfart_check_index = i;

	/* Make sure there is a record where there should be one */
    if (!texts[i]) {
      count++;

      if (count >= first_err) {
	err_code = DF_ERR_BADREC;
	return (FALSE);
       }
     }

	/* Check the index again */
    if (text_index[i] < 23) {
      count++;

      if (count >= first_err) {
	err_code = DF_ERR_ARTMIN;
	return (FALSE);
       }
     }

    if (text_index[i] > 255) {
      count++;

      if (count >= first_err) {
	err_code = DF_ERR_ARTMAX;
	return (FALSE);
       }
     }
   }

	/* Check to make sure there are matching artifact + text records */
  dfart_check_text = FALSE;

  for (i = 0; i < num_records; i++) {
    dfart_check_index = i;

    for (j = 0; j < num_texts; j++) {
      if (records[i]->index == text_index[j]) break;
     }

	/* Didn't find one */
    if (j == num_texts) {
      count++;

      if (count >= first_err) {
	err_code = DF_ERR_NOTEXT;
	return (FALSE);
       }
     }

   }

	/* Check to make sure there are matching text + artifact records */
  dfart_check_text = TRUE;

  for (i = 0; i < num_texts; i++) {
    dfart_check_index = i;

    for (j = 0; j < num_records; j++) {
      if (records[j]->index == text_index[i]) break;
     }

	/* Didn't find one */
    if (j == num_records) {
      count++;

      if (count >= first_err) {
	err_code = DF_ERR_NOART;
	return (FALSE);
       }
     }

   }

  return (TRUE);
 }
/*========== End of Function AMD_TYPE::check() ============================*/


/*========== Attempt to Load an AMD File ==================================*/
boolean AMD_TYPE::load (const char *filename) {
  FILE *f;	/* File pointer */
  short i, j;	/* Loop counters */
  unsigned short size;
  char buffer[8];

	/* Attempt to open file */
  if (!(f = openfile(filename, "rb"))) {
    err_code = ERR_FILE;
    return (FALSE);
   }

	/* Clear any current data */
  destroy();

	/* Read the 5 byte header */
  if (fread (buffer, sizeof(char), 3, f) != 3) goto AMD_READ_ERR;
  major_version = fgetc (f);
  minor_version = fgetc (f);

	/* Make sure its a AMD file */
  if (buffer[0] != 'A' || buffer[1] != 'M' || buffer[2] != 'D') {
    fclose (f);
    err_code = DF_ERR_NOTAMD;
    return (FALSE);
   }

	/* Read the save date and time */
  fread (&save_date, sizeof(save_date), 1, f);
  fread (&save_time, sizeof(save_time), 1, f);

	/* Read the authors name */
  fread (author, sizeof(char), 16, f);
  author[16] = 0;

	/* Read and parse the options, if any */
  i = read_int (f);

	/* Read the number of records */
  num_records = read_int(f);

	/* Read the records */
  for (i = 0; i < num_records; i++) {

		/* Create the record */
    if (!records[i]) {
      if (!(records[i] = new AMD_RECORD))
	bug (MEM_ERR_MSG, "AMD_TYPE::load() - *records[] (%d)", sizeof(AMD_RECORD));
     }

		/* Load the record data */
    records[i]->index = read_int (f);
    if (fread(records[i]->name, sizeof(char), 32, f) != 32) goto AMD_READ_ERR;
    records[i]->magic_type = fgetc(f);
    records[i]->group = fgetc(f);
    records[i]->subgroup = fgetc(f);

	/* Read the enchantments */
    for (j = 0; j < 10; j++)
      records[i]->enchantments[j] = read_int(f);

    records[i]->uses = read_int(f);
    records[i]->u1 = read_long(f);
    records[i]->material = fgetc(f);
   }

	/* Read the number of texts */
  num_texts = read_int (f);

	/* Read the texts */
  for (i = 0; i < num_texts; i++) {

	/* Read the text index */
    text_index[i] = read_int (f);

	/* Read the text */
    size = (unsigned short) read_int (f);
    texts[i] = create_ptr (size + 4);
    if (fread (texts[i], sizeof(char), size, f) != size) goto AMD_READ_ERR;
    texts[size] = 0;
   }

  fclose (f);
  err_code = ERR_NONE;
  return (TRUE);

AMD_READ_ERR:
  fclose (f);
  err_code = ERR_READ;
  return (FALSE);
 }
/*========== End of Function AMD_TYPE::load() =============================*/


/*========== Attempt to Save an AMD File ==================================*/
boolean AMD_TYPE::save (const char *filename) {
  FILE *f;	/* File pointer */
  short i, j;	/* Loop counters */
  unsigned short size;

	/* Attempt to open file */
  if (!(f = openfile(filename, "wb"))) {
    err_code = ERR_FILE;
    return (FALSE);
   }

	/* Write the 5 byte header */
  fwrite ("AMD", sizeof(char), 3, f);
  fputc (major_version, f);
  fputc (minor_version, f);

	/* Write the save date and time */
  gettime (&save_time);
  getdate (&save_date);
  fwrite (&save_date, sizeof(save_date), 1, f);
  fwrite (&save_time, sizeof(save_time), 1, f);

	/* Write the authors name */
  fwrite (author, sizeof(char), 16, f);

	/* Parse and Write the options, if any */
  write_int (f, 0);

	/* Write the number of records */
  write_int (f, num_records);

	/* Write the records */
  for (i = 0; i < num_records; i++) {

		/* Write all the record data */
    write_int (f, records[i]->index);

    if (fwrite(records[i]->name, sizeof(char), 32, f) != 32) {
      err_code = ERR_WRITE;
      fclose (f);
      return (FALSE);
     }

    fputc (records[i]->magic_type, f);
    fputc (records[i]->group, f);
    fputc (records[i]->subgroup, f);

	/* Write the enchantments */
    for (j = 0; j < 10; j++)
      write_int (f, records[i]->enchantments[j]);

    write_int (f, records[i]->uses);
    write_long (f, records[i]->u1);
    fputc (records[i]->material, f);
   }

	/* Write the number of texts */
  write_int (f, num_texts);

	/* Write the texts */
  for (i = 0; i < num_texts; i++) {

	/* Write the text index */
    write_int (f, text_index[i]);

	/* Write the text */
    size = strlen(texts[i]);
    write_int (f, size);
    fwrite (texts[i], sizeof(char), size, f);
   }

  fclose (f);
  err_code = ERR_NONE;
  return (TRUE);
 }
/*========== End of Function AMD_TYPE::save() =============================*/