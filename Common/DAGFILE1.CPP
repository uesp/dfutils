	/* Standard C Includes */
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <ctype.h>
#include <string.h>

	/* User Defined Includes */
#include "fileutil.h"
#include "cmstring.h"
#include "daggen.h"
#include "dagfile1.h"

	/* Are we debugging? */
#define DEBUG TRUE


/*========== Class MAGICDEF_TYPE Constructor ==============================*/
void MAGICDEF_TYPE::MAGICDEF_TYPE (void) {
  short i;	/* Loop counter */
  num_records = 0;

  for (i = 0; i < MAX_DEF; i++) {
    records[i] = NULL;
   }
 }
/*========== End of Class MAGICDEF_TYPE Constructor =======================*/


/*========== Class MAGICDEF_TYPE Destructor ===============================*/
void MAGICDEF_TYPE::destroy (void) {
  short i;	/* Loop counter */
  num_records = 0;

  for (i = 0; i < MAX_DEF; i++) {
    if (records[i]) {
      delete records[i];
      records[i] = NULL;
     }
   }

 }
/*========== End of Class MAGICDEF_TYPE Destructor ========================*/


/*========== Attempts to Add a Record a the Required Location =============*/
boolean MAGICDEF_TYPE::add_record (const short where) {
  short i;	/* Loop counter */

	/* Make sure its a valid location */
  if (where > num_records || where < 0) {
    err_code = ERR_INDEX;
    return (FALSE);
   }
  else if (num_records >= 255) {
    err_code = ERR_MAXINDEX;
    return (FALSE);
   }

  for (i = num_records; i > where; i--)
    records[i] = records[i - 1];

	/* Attempt to create the new record */
  if (!(records[where] = new MAGICDEF_RECORD))
    bug (MEM_ERR_MSG, "add_record() - *records[] (%d)", sizeof(MAGICDEF_RECORD));

  records[where]->magic_type = 0;
  records[where]->uses = 0;
  records[where]->u1 = 0;
  records[where]->material = 0;
  records[where]->group = 2;
  records[where]->subgroup = 2;
  strcpy (records[where]->name, "None");

  for (i = 0; i < 10; i++)
    records[where]->enchantments[i] = -1;

  num_records++;
  err_code = ERR_NONE;
  return (TRUE);
 }
/*========== End of Function MAGICDEF_TYPE::add_record() ==================*/


/*========== Attempts to Delete a Record at the Desired Position ==========*/
boolean MAGICDEF_TYPE::del_record (const short where) {
  short i;	/* Loop counter */

	/* Make sure its a valid location */
  if (where >= num_records || where < 0) {
    err_code = ERR_INDEX;
    return (FALSE);
   }

  delete records[where];

	/* Shift things ... */
  for (i = where; i < num_records; i++)
    records[i] = records[i + 1];

  num_records--;
  err_code = ERR_NONE;
  return (TRUE);
 }
/*========== End of Function MAGICDEF_TYPE::del_record() ==================*/


/*========== Attempt to Load a DEF File ===================================*/
boolean MAGICDEF_TYPE::load (const char *filename) {
  FILE *f;	/* File pointer */
  short i, j;	/* Loop counters */

	/* Attempt to open file */
  if (!(f = openfile(filename, "rb"))) {
    err_code = ERR_FILE;
    return (FALSE);
   }

	/* Clear any current data */
  destroy();

	/* Read the number of records */
  num_records = (short) read_long(f);

	/* Read the records */
  for (i = 0; i < num_records; i++) {

		/* Create the record */
    if (!records[i]) {
      if (!(records[i] = new MAGICDEF_RECORD))
	bug (MEM_ERR_MSG, "MAGICDEF_TYPE::load() - *records[] (%d)", sizeof(MAGICDEF_RECORD));
     }

		/* Load stuff */
    if (fread(records[i]->name, sizeof(char), 32, f) != 32) {
      err_code = ERR_READ;
      fclose (f);
      return (FALSE);
     }

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

  fclose (f);
  err_code = ERR_NONE;
  return (TRUE);
 }
/*========== End of Function MAGICDEF_TYPE::load() ========================*/


/*========== Attempt to Save a DEF File ===================================*/
boolean MAGICDEF_TYPE::save (const char *filename) {
  FILE *f;	/* File pointer */
  short i, j;	/* Loop counters */

	/* Attempt to open file */
  if (!(f = openfile(filename, "wb"))) {
    err_code = ERR_FILE;
    return (FALSE);
   }

	/* Write the number of records */
  write_long (f, (long) num_records);

	/* Write the records */
  for (i = 0; i < num_records; i++) {

		/* Write stuff */
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

  fclose (f);
  err_code = ERR_NONE;
  return (TRUE);
 }
/*========== End of Function MAGICDEF_TYPE::save() ========================*/


/*========== Attempts to Add a Record to the RSC File =====================*/
boolean RSC_TYPE::add_record (const char *filename, const short type, const char *data, const boolean backup) {
  FILE *fi, *fo;
  string temp_str;
  char *buf, ch;
  unsigned short data_size = strlen(data), size;
  short i, j, dl = 0, k, dsize;

	/* Attempt to open the two files */
  if (!(fi = openfile (filename, "rb"))) {
    err_code = ERR_FILE;
    return (FALSE);
   }

  if (!(fo = openfile ("text.ttt", "wb"))) {
    fclose (fi);
    err_code = ERR_FILE;
    return (FALSE);
   }

	/* Does this type already exist? */
  j = find_type (type);

  if (j == -1) {
    j = num_texts - 2;
    dl = 6;
   }

	/* Write the overall header length */
  header_offset += dl;
  write_int (fo, header_offset);

	/* Write the headers up to the new/modified one */
  for (i = 0; i < j; i++) {
    header[i].offset += dl;
    write_int (fo, header[i].type);
    write_long (fo, header[i].offset);
   }

	/* Need to add a new record, or replace one? */
  if (header[i].type == type) {
    dsize = data_size - (unsigned short) (header[i + 1].offset - header[i].offset);

    for (k = i + 1; k < num_texts; k++) {
      header[k].offset = header[k].offset + dsize;
     }
   }
  else {
    dsize = data_size;

    for (k = num_texts; k > i; k--) {
      header[k].offset = header[k - 1].offset + dl + dsize;
      header[k].type = header[k - 1].type;
     }

    header[k].offset += dl;
    header[k].type = type;
    num_texts++;
   }

	/* Write the rest of the headers */
  for (; i < num_texts; i++) {
    write_int (fo, header[i].type);
    write_long (fo, header[i].offset);
   }

	/* Read + Write the texts up to the new/modified one */
  buf = create_ptr (64000u);

  for (i = 0; i < j; i++) {
    size = header[i + 1].offset - header[i].offset;
    fseek (fi, header[i].offset - dl, SEEK_SET);

	/* Read the original text */
    if (fread (buf, sizeof(char), size, fi) != size) {
      fclose (fi);
      fclose (fo);
      delete buf;
      write_log_entry ("   Read Error @0x%08lX,  i = %d,  size=%u", header[i].offset, i, size);
      err_code = ERR_READ;
      return (FALSE);
     }

	/* Write the text */
    fseek (fo, header[i].offset, SEEK_SET);
    fwrite (buf, sizeof(char), size, fo);
   }

	/* Write the new/modified text */
  fseek (fo, header[i].offset, SEEK_SET);
  fwrite (data, sizeof(char), data_size, fo);
  i++;

	/* Write the remaining texts */
  for (; i < num_texts - 1; i++) {
    size = header[i + 1].offset - header[i].offset;
    fseek (fi, header[i].offset - dl - dsize, SEEK_SET);

	/* Read the original text */
    if (fread (buf, sizeof(char), size, fi) != size) {
      fclose (fi);
      fclose (fo);
      delete buf;
      write_log_entry ("   Read Error @0x%08lX,  i = %d,  size=%u", header[i].offset, i, size);
      err_code = ERR_READ;
      return (FALSE);
     }

	/* Write the text */
    fseek (fo, header[i].offset, SEEK_SET);
    fwrite (buf, sizeof(char), size, fo);
   }

  fclose (fi);
  fclose (fo);
  delete buf;

	/* Create a backup of the original */
  if (backup) {
    temp_str = "copy ";
    temp_str += filename;
    temp_str += " ";
    temp_str += filename;
    temp_str -= 3;	/* Remove RSC extension */
    temp_str += "bak > nul";
    write_log_entry ("%s", temp_str.value());

    if (system (temp_str.value())) {
      printf ("Warning: Couldn't create backup of file, Continue [Y/N]?\n");

      do {
	ch = tolower(getch());
      } while (ch != 'y' && ch != 'n');

      if (ch != 'y') {
	err_code = ERR_FILE;
	return (FALSE);
       }
     }
   }


	/* Copy the temp to the original */
  temp_str = "copy text.ttt ";
  temp_str += filename;
  temp_str += " > nul";

  if (system (temp_str.value())) {
    err_code = ERR_FILE;
    return (FALSE);
   }

	/* Delete the temp file */
  unlink ("text.ttt");

  err_code = ERR_NONE;
  return (TRUE);
 }
/*========== End of Function RSC_TYPE::add_record() =======================*/


/*========== Attempts to Delete a Record from the File ====================*/
boolean RSC_TYPE::del_record (const char *filename, const short type) {
  FILE *fo, *fi;	/* File pointers */
  string temp_str;
  long l;
  unsigned short dsize, size;
  short i, j;
  char *buf;		/* Temp input buffer */

	/* Attempt to find the index */
  if ((i = find_type(type)) == -1) {
    err_code = ERR_INDEX;
    return (FALSE);
   }

	/* Attempt to open the files */
  if (!(fi = openfile (filename, "rb"))) {
    err_code = ERR_FILE;
    return (FALSE);
   }

  if (!(fo = openfile ("text.ttt", "wb"))) {
    fclose (fi);
    err_code = ERR_FILE;
    return (FALSE);
   }

	/* Write the header up to deleted entry */
  write_int (fo, header_offset - 6);

  for (j = 0; j < i; j++) {
    write_int (fo, header[j].type);
    write_long (fo, (header[j].offset - 6));
   }

	/* Compute the size difference */
  dsize = (unsigned short) (header[i+1].offset - header[i].offset);

	/* Write the rest of the header entries */
  for (j = i + 1; j < num_texts; j++) {
    write_int (fo, header[j].type);
    l = header[j].offset - 6 - dsize;
    write_long (fo, l);
   }

	/* Write the texts up to deleted entry */
  buf = create_ptr (64000u);

  for (j = 0; j < i; j++) {
    size = (unsigned short) (header[j + 1].offset - header[j].offset);
    fseek (fi, header[j].offset, SEEK_SET);

	/* Read the original text */
    if (fread (buf, sizeof(char), size, fi) != size) {
      fclose (fi);
      fclose (fo);
      delete buf;
      write_log_entry ("   Read Error @0x%08lX,  i = %d,  size=%u", header[j].offset, j, size);
      err_code = ERR_READ;
      return (FALSE);
     }

	/* Write the text */
    fseek (fo, header[j].offset - 6, SEEK_SET);
    fwrite (buf, sizeof(char), size, fo);
   }

	/* Write the rest of the texts */
  for (j = i + 1; j < num_texts - 1; j++) {
    size = (unsigned short) (header[j + 1].offset - header[j].offset);
    fseek (fi, header[j].offset, SEEK_SET);

	/* Read the original text */
    if (fread (buf, sizeof(char), size, fi) != size) {
      fclose (fi);
      fclose (fo);
      delete buf;
      write_log_entry ("   Read Error @0x%08lX,  i = %d,  size=%u", header[j].offset, j, size);
      err_code = ERR_READ;
      return (FALSE);
     }

	/* Write the text */
    fseek (fo, header[j].offset - 6 - dsize, SEEK_SET);
    fwrite (buf, sizeof(char), size, fo);
   }

  delete buf;
  fclose (fi);
  fclose (fo);

	/* Create a backup of the original */
  temp_str = "copy ";
  temp_str += filename;
  temp_str += " ";
  temp_str += filename;
  temp_str -= 3;	/* Remove RSC extension */
  temp_str += "bak > nul";
  write_log_entry ("%s", temp_str.value());

  if (system (temp_str.value())) {
    printf ("Warning: Couldn't create backup of file, Continue [Y/N]?");

    while (i != 'y' && i != 'n')
      i = tolower(getch());

    printf ("\n");

    if (i == 'n') {
      err_code = ERR_FILE;
      return (FALSE);
     }
   }

	/* Copy the temp to the original */
  temp_str = "copy text.ttt ";
  temp_str += filename;
  temp_str += " > nul";

  if (system (temp_str.value())) {
    err_code = ERR_FILE;
    return (FALSE);
   }

	/* Delete the temp file */
  unlink ("text.ttt");
  err_code = ERR_NONE;
  return (TRUE);
 }
/*========== End of Function RSC_TYPE::del_record() =======================*/


/*========== Attempts to extract text from file ===========================*/
char *RSC_TYPE::extract_text (const char *filename, const short type) {
  FILE *f;	/* File pointer */
  unsigned char *ptr, *buf;
  unsigned short size, j, k;
  short i;

	/* Attempt to find record */
  i = find_type (type);

  if (i < 0 ||  i >= num_texts - 1) {
    err_code = ERR_INDEX;
    return (NULL);
   }

	/* Compute the size of the text field */
  size = (unsigned short) (header[i+1].offset - header[i].offset);

	/* Attempt to open file for reading */
  if (!(f = openfile(filename, "rb"))) {
    err_code = ERR_FILE;
    return (NULL);
   }

  buf = (unsigned char *) create_ptr (size + 2);
  fseek (f, header[i].offset, SEEK_SET);

  if (fread (buf, sizeof(char), size, f) != size) {
    err_code = ERR_READ;
    delete buf;
    return (NULL);
   }

  fclose (f);

	/* Parse the text into readable form */
  ptr = (unsigned char *) create_ptr (size + 100);
  j = 0;
  k = 0;

  while (buf[j]) {
    if (buf[j] == 0xFC) {
      ptr[k] = 10;
      k++;
     }
    else if (buf[j] == 0xFD) {
      ptr[k] = 10;
      k++;
     }
    else if (buf[j] == 0xFE) {
//      ptr[k] = '~';
//      k++;
     }
    else if (buf[j] == 0xFF) {
//      ptr[k] = '~';
//      k++;
     }
    else {
      ptr[k] = buf[j];
      k++;
     }

    j++;
   }

  delete buf;
  err_code = ERR_NONE;
  return ((char *) ptr);
 }
/*========== End of Function RSC_TYPE::extract_text() =====================*/


/*========== Searches Through Records for an Index ========================*/
short RSC_TYPE::find_type (const short type) {
  short i;	/* Loop counter */

  for (i = 0; i < num_texts; i++) {
    if (header[i].type == type) return (i);
   }

  return (-1);
 }
/*========== End of Function RSC_TYPE::find_type() ========================*/


/*========== Reads a RSC type Header ======================================*/
boolean RSC_TYPE::read_header (const char *filename) {
  FILE *f;	/* File pointer */

	/* Attempt to open file */
  if (!(f = openfile (filename, "rb"))) {
    err_code = ERR_FILE;
    return (FALSE);
   }

  num_texts = 0;
  header_offset = read_int(f);

  while (ftell(f) < header_offset) {
    header[num_texts].type = read_int(f);
    header[num_texts].offset = read_long(f);

    num_texts++;

    if (num_texts >= MAX_RSC_RECORDS - 1) {
      fclose (f);
      err_code = ERR_MAXINDEX;
      return (FALSE);
     }
   }

  err_code = ERR_NONE;
  write_log_entry ("Found %d texts in file", num_texts);
  fclose (f);
  return (TRUE);
 }
/*========== End of Function RSC_TYPE::read_header() ======================*/


/*========== Attempts to Read in the Text File, Converting it to RSC Format*/
char *RSC_TYPE::read_textfile (const char *filename) {
  FILE *f;
  char *input, *ptr;
  long size;
  unsigned short i, j, k;

	/* Attempt to open file */
  if (!(f = openfile(filename, "rt"))) {
    err_code = ERR_FILE;
    return (FALSE);
   }

	/* Check the size of the file */
  size = get_filesize(f);

  if (size > 64000u) {
    err_code = ERR_64KB;
    fclose (f);
    return (FALSE);
   }

	/* Input the file text all at once */
  input = create_ptr ((unsigned short)size + 2);
  fread (input, sizeof(char), (unsigned short) size, f);
  fclose(f);

	/* Parse the text */
  i = strlen(input);
  ptr = create_ptr (i + 100);

  for (j = 0, k = 0; j < i; j++) {
    if (input[j] == 13) {	/* CR */
      ptr[k] = 0xFC;
      k++;
     }
    else if (input[j] == 10) { 	/* LF */
      ptr[k] = 0xFC;
      k++;
     }
    else if (input[j] == 9) {   /* TAB */
      ptr[k++] = 32;
      ptr[k++] = 32;
      ptr[k++] = 32;
      ptr[k++] = 32;
      ptr[k++] = 32;
     }
    else {
      ptr[k] = input[j];
      k++;
     }
   }

  ptr[k] = 0xFE;	/* End-of-Text marker */
  k++;
  ptr[k] = 0;

  return (ptr);
 }
/*========== End of Function read_textfile() ==============================*/
