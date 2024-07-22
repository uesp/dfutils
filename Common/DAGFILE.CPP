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
#include "dagfile.h"
#include "dagdata.h"

	/* The main data for loading... */
DAG_FILE dagfile;


#define DEBUG TRUE


/*========== Class DAG_RECORD Constructor =================================*/
BASE_RECORD::BASE_RECORD (void) {
  data = NULL;
  size = 0;
  num_sizes = 0;
  type = 0;
  data_length = 0;
  offset = 0;
  loaded = FALSE;
 }
/*========== End of Class DAG_RECORD Constructor ==========================*/


/*========== Class DAG_RECORD Destructor ==================================*/
void BASE_RECORD::destroy (void) {

  if (data) {
    delete data;
    data = NULL;
   }

  size = 0;
  loaded = FALSE;
  num_sizes = 0;
  data_length = 0;
  type = 0;
  offset = 0;
 }
/*========== End of Class DAG_RECORD Destructor ===========================*/


/*========== Attempts to Read The Default Field Data ======================*/
boolean BASE_RECORD::load_record (FILE *f) {

	/* Make sure there is something to load from */
  if (!f) {
    err_code = ERR_FILE;
    return (FALSE);
   }

	/* Make sure a good record size is present */
  if (size < 5) {
    write_log_entry ("Invalid field size of %d received (Offset = %lu)!", size, ftell(f));
    err_code = ERR_INDEX;
    return (FALSE);
   }

	/* Allocate the memory */
  data_length = (short) size - 5;
  data = create_ptr (data_length);

	/* Read data from file, or attempt to */
  if (fread(data, sizeof(char), data_length, f) != data_length) {
    write_log_entry ("Read past EOF");
    err_code = ERR_READ;
    return (FALSE);
   }

  loaded = TRUE;
  err_code = ERR_NONE;
  return (TRUE);
 }
/*========== End of Function BASE_RECORD::load_record() ===================*/


/*========== Saves Only the Record Data ===================================*/
boolean BASE_RECORD::save_record (FILE *f) {

	/* Make sure ther eis something to write to */
  if (!f) {
    err_code = ERR_FILE;
    return (FALSE);
   }

	/* Something to write? */
  if (!data || !data_length) {
    err_code = DF_ERR_NODATA;
    return (TRUE);
   }

  if (fwrite (data, sizeof(char), data_length, f) != data_length) {
    err_code = ERR_WRITE;
    return (FALSE);
   }

  err_code = ERR_NONE;
  return (TRUE);
 }
/*========== End of Function BASE_RECORD::save_record() ===================*/


/*========== Class DAG_FILE Constructor ===================================*/
DAG_FILE::DAG_FILE (void) {
  short i;	/* Loop counter */

  for (i = 0; i < MAX_RECORDS; i++)
    records[i] = NULL;

  post_data = NULL;
  post_length = 0;
  num_records = 0;
 }
/*========== End of Class DAG_FILE Constructor ============================*/


/*========== Class DAG_FILE Destructor ====================================*/
void DAG_FILE::destroy (void) {
  short i;	/* Loop counter */

  if (post_data) {
    delete post_data;
    post_data = NULL;
   }

  for (i = 0; i < MAX_RECORDS; i++) {

    if (records[i]) {
      delete records[i];
      records[i] = NULL;
     }

   }

  post_length = 0;
  num_records = 0;
 }
/*========== End of Class DAG_FILE Destructor =============================*/


/*========== Attempts to Delete a Record ==================================*/
boolean DAG_FILE::del_record (const short pos) {
  short i;

	/* Make sure it's a valid position */
  if (pos < 0 || pos >= num_records || !records[pos]) {
    err_code = ERR_INDEX;
    return (FALSE);
   }

	/* Delete the record */
  delete records[pos];

	/* Move everything */
  for (i = pos; i < num_records; i++) {
    records[i] = records[i + 1];
   }

  num_records--;
  err_code = ERR_NONE;
  return (TRUE);
 }
/*========== End of Function DAG_FILE::del_record() =======================*/


/*========== Attempts to Load a Savetree.DAT File into Memory =============*/
boolean DAG_FILE::load (const char *filename, const short type) {
  FILE *f;	/* File pointer */
  boolean done = FALSE;	/* Loop counter */
  long size;

#if DEBUG
  long offset;
#endif

  short num_sizes, rec_type;

	/* Display the memory status */
  write_log_entry ("Heap is %s", get_heapstring());
  write_log_entry ("Used/Free/Total Memory: %lu/%lu/%lu bytes", get_memused(), get_memleft(), get_memtotal());

	/* Attempt to open the file for reading */
  if (!(f = openfile(filename, "rb"))) {
    err_code = ERR_FILE;
    return (FALSE);
   }

	/* Clear any data currently in memory */
  destroy();

	/* Read in the 19 byte header */
  if (fread(pre_data, sizeof(char), 19, f) != 19) {
    err_code = ERR_READ;
    fclose (f);
    return (FALSE);
   }

	/* Read in records */
  while (!done && !feof(f)) {
    num_sizes = 0;

#if DEBUG
    offset = ftell(f);
#endif

	/* Read in the first size field */
    size = read_long(f);

	/* Need to look for a second field? */
    if (!size) {
      num_sizes++;

		/* Read in the first size field, Check for End of Fields */
      size = read_long(f);
      if (!size) done = TRUE;
     }

	/* Read the main type of record */
    rec_type = fgetc(f);
    if (!size) rec_type = DF_EOF;

	/* Create the record */
    if (rec_type == DF_ITEM) {

      if (!(records[num_records] = new ITEM_RECORD))
	bug (MEM_ERR_MSG, "DAG_FILE::load() - records[] (*ITEM_RECORD = %d)", sizeof(ITEM_RECORD));

     }
    else if (rec_type == DF_CHAR) {

      if (!(records[num_records] = new CHAR_RECORD))
	bug (MEM_ERR_MSG, "DAG_FILE::load() - records[] (*CHAR_RECORD = %d)", sizeof(CHAR_RECORD));
     }
    else if (rec_type == DF_GUILD) {

      if (!(records[num_records] = new GUILD_RECORD))
	bug (MEM_ERR_MSG, "DAG_FILE::load() - records[] (*GUILD_RECORD = %d)", sizeof(GUILD_RECORD));
     }
    else {	/* Default unknown type */

      if (!(records[num_records] = new BASE_RECORD))
	bug (MEM_ERR_MSG, "DAG_FILE::load() - records[] (*BASE_RECORD = %d)", sizeof(BASE_RECORD));
     }

	/* Assign the loaded values */
    records[num_records]->offset = ftell(f);
    records[num_records]->type = rec_type;
    records[num_records]->size = size;
    records[num_records]->num_sizes = num_sizes;

	/* Read the actual record */
    if (type && rec_type != 0x07 && (type & DFRT_ALL ||
		 records[num_records]->type == DF_ITEM && type & DFRT_ITEM ||
		 records[num_records]->type == DF_CHAR && type & DFRT_CHAR ||
		 records[num_records]->type == DF_GUILD && type & DFRT_GUILD)) {

		 /* Load the actual record data */
      if (!records[num_records]->load_record (f)) {
	if (!err_code) err_code = ERR_READ;
	fclose (f);
	return (FALSE);
       }

     }
		/* The strange field... */
    else if (rec_type == 0x07) {
      fseek (f, -1, SEEK_CUR);
      done = TRUE;
     }
    else  {	/* Read nothing, Move to the end of the field */
      fseek (f, records[num_records]->size - 1, SEEK_CUR);
      records[num_records]->data_length = 0;
     }

#if DEBUG
    write_log_entry ("  Record %3u (0x%06lX) :Type = 0x%02X,  Size = %4lu", num_records, offset, (unsigned int)records[num_records]->type, records[num_records]->size);
#endif

    num_records++;
    if (num_records >= MAX_RECORDS) done = TRUE;
   }

	/* Read the rest of the data */
  size = get_filesize(f) - ftell(f);

  if (size >= 0xFFFFl) {
    err_code = DF_ERR_TOOMUCH;
    fclose (f);
    return (FALSE);
   }

  post_length = (unsigned short) size;
  post_data = create_ptr (post_length);
  fread (post_data, sizeof(char), post_length, f);

#if DEBUG
  write_log_entry ("%u bytes left in file", post_length);
#endif

  fclose (f);

	/* Display the memory status */
  write_log_entry ("Heap is %s", get_heapstring());
  write_log_entry ("Used/Free/Total Memory: %lu/%lu/%lu bytes", get_memused(), get_memleft(), get_memtotal());

  err_code = ERR_NONE;
  return (TRUE);
 }
/*========== End of Function DAG_FILE::load() =============================*/


/*========== Attempts to Save the Data File ===============================*/
boolean DAG_FILE::save (const char *load_file, const char *save_file, const short type) {
  FILE *fi, *fo;	/* File pointers */
  unsigned long size;
  short i, j;		/* Loop counters */
  char *buf = NULL;	/* Input/Output buffer */

	/* Attempt to open the two files */
  if (!(fi = openfile (load_file, "rb"))) {
    err_code = ERR_FILE;
    return (FALSE);
   }

  if (!(fo = openfile (save_file, "wb"))) {
    err_code = ERR_FILE;
    fclose (fi);
    return (FALSE);
   }

	/* Write the header information */
  if (fwrite(pre_data, sizeof(char), 19, fo) != 19) {
    err_code = ERR_WRITE;
    fclose (fi);
    fclose (fo);
    return (FALSE);
   }

	/* Write the records */
  for (i = 0; i < num_records; i++) {

	/* Write the record header */
    for (j = 0; j < records[i]->num_sizes; j++)
      write_long (fo, 0);

    write_long(fo, records[i]->size);

	/* The special record??? */
    if (records[i]->type == 0x07) break;

    if (records[i]->type != DF_EOF) {
      if (fputc (records[i]->type, fo) == EOF) goto WRITE_ERROR;
     }

	/* Write the data it from the input file */
    if (!type || !records[i]->loaded) {

		/* Move to field offset */
      fseek (fi, records[i]->offset, SEEK_SET);

		/* Make sure there is a valid size.... */
      size = records[i]->size - 1;

      if (size > 1) {
	buf = create_ptr ((unsigned int) size);
	if (fread (buf, sizeof(char), (short)size, fi) != size) goto WRITE_ERROR;
	if (fwrite (buf, sizeof(char), (short)size, fo) != size) goto WRITE_ERROR;
	delete buf;
	buf = NULL;
       }
     }
    else {
      if (!records[i]->save_record (fo)) goto WRITE_ERROR;
     }
   }

	/* Write the trailing bytes */
  if (post_length && post_data) {
    if (fwrite (post_data, sizeof(char), post_length, fo) != post_length) goto WRITE_ERROR;
   }

  fclose (fi);
  fclose (fo);
  err_code = ERR_NONE;
  return (TRUE);

WRITE_ERROR:
  err_code = ERR_WRITE;
  if (buf) delete buf;
  fclose (fi);
  fclose (fo);
  return (FALSE);
 }
/*========== End of Function DAG_FILE::save() =============================*/


/*========== Attempts to Load a Character Record ==========================*/
boolean CHAR_RECORD::load_record (FILE *f) {
  unsigned long l;

	/* Make sure there is something to read from */
  if (!f) {
    err_code = ERR_FILE;
    return (FALSE);
   }

	/* Read the pre-data header */
  if (fread(pre_data, sizeof(char), 70, f) != 70) {
    err_code = ERR_READ;
    return (FALSE);
   }

	/* Read the characters name */
  if (fread(name, sizeof(char), 32, f) != 32) {
    err_code = ERR_READ;
    return (FALSE);
   }

  str = read_int(f);
  intel = read_int(f);
  wil = read_int(f);
  agi = read_int(f);
  end = read_int(f);
  per = read_int(f);
  spd = read_int(f);
  luc = read_int(f);;

  if (fread (mid_data, sizeof(char), 85, f) != 85) {
    err_code = ERR_READ;
    return (FALSE);
   }

  gold = read_long(f);

#if DEBUG
  write_log_entry ("Found Character '%s', Gold = %ld", name, gold);
#endif

	/* Read the rest of the data */
  l = size - 208;

  if (l > 0) {
    data_length = (unsigned short) l;
    data = create_ptr ((unsigned short) l);

    if (fread(data, sizeof(char), data_length, f) != l) {
      err_code = ERR_READ;
      return (FALSE);
     }
   }
  else
    data_length = 0;

	/* Make sure we haven't read past the eof */
  if (feof(f)) {
    err_code = ERR_READ;
    return (FALSE);
   }

  loaded = TRUE;
  return (TRUE);
 }
/*========== End of Function CHAR_RECORD::load_record() ===================*/


/*========== Attempts to Save a Character Record ==========================*/
boolean CHAR_RECORD::save_record (FILE *f) {

	/* Make sure there is something to read from */
  if (!f) {
    err_code = ERR_FILE;
    return (FALSE);
   }

	/* Read the pre-data header */
  if (fwrite(pre_data, sizeof(char), 70, f) != 70) {
    err_code = ERR_WRITE;
    return (FALSE);
   }

	/* Read the characters name */
  if (fwrite(name, sizeof(char), 32, f) != 32) {
    err_code = ERR_WRITE;
    return (FALSE);
   }

  write_int(f, str);
  write_int(f, intel);
  write_int(f, wil);
  write_int(f, agi);
  write_int(f, end);
  write_int(f, per);
  write_int(f, spd);
  write_int(f, luc);

  if (fwrite (mid_data, sizeof(char), 85, f) != 85) {
    err_code = ERR_WRITE;
    return (FALSE);
   }

  write_long(f, gold);

	/* Write the rest of the data */
  if (data_length > 0) {

    if (fwrite(data, sizeof(char), data_length, f) != data_length) {
      err_code = ERR_WRITE;
      return (FALSE);
     }
   }

  err_code = ERR_NONE;
  return (TRUE);
 }
/*========== End of Function CHAR_RECORD::save_record() ===================*/


/*========== Loads a Guild Data from a File ===============================*/
boolean GUILD_RECORD::load_record (FILE *f) {
  long offset;

	/* Make sure there is something to read from */
  if (!f) {
    err_code = ERR_FILE;
    return (FALSE);
   }

	/* Save the current offset in file */
  offset = ftell(f);

	/* Start reading things */
  u1 = read_int(f);
  u2 = read_int(f);
  u3 = read_int(f);
  u4 = read_long(f);
  u5 = read_long(f);
  u6 = read_long(f);
  u7 = read_int(f);
  u8 = read_int(f);
  u9 = read_int(f);
  u10 = read_int(f);
  u11 = read_int(f);
  u12 = read_int(f);
  u13 = read_int(f);
  u14 = read_int(f);
  u15 = read_int(f);
  u16 = read_int(f);
  u17 = read_int(f);
  u18 = read_long(f);
  u19 = read_long(f);
  u20 = read_long(f);
  u21 = read_int(f);
  u22 = read_long(f);
  u23 = read_long(f);
  u24 = read_long(f);
  u25 = read_long(f);
  u26 = read_int(f);
  u27 = fgetc(f);
  guild_id = read_int(f);
  u28 = fgetc(f);
  u29 = fgetc(f);
  u30 = fgetc(f);
  u31 = fgetc(f);
  u32 = fgetc(f);
  u33 = read_long(f);

	/* Read the rest of the data, if any */
  offset = size - 1 - (ftell(f) - offset);

  if (offset > 0) {
    data_length = (short) offset;
    data = create_ptr (data_length);

    if (fread(data, sizeof(char), data_length, f) != data_length) {
      err_code = ERR_READ;
      return (FALSE);
     }
   }
  else
    data_length = 0;

	/* Make sure we haven't exceed the EOF */
  if (feof(f)) {
    err_code = ERR_READ;
    return (FALSE);
   }

	/* Return success */
  loaded = TRUE;
  err_code = ERR_NONE;
  return (TRUE);
 }
/*========== End of Function GUILD_RECORD::load_record() ==================*/


/*========== Saves Guild Data to File =====================================*/
boolean GUILD_RECORD::save_record (FILE *f) {

	/* Make sure there is something to write to */
  if (!f) {
    err_code = ERR_FILE;
    return (FALSE);
   }

	/* Write the rest of the data, if any */
  if (data_length && data) {
    if (fwrite (data, sizeof(char), data_length, f) != data_length) {
      err_code = ERR_WRITE;
      return (FALSE);
     }
   }

  err_code = ERR_NONE;
  return (TRUE);
 }
/*========== End of Function GUILD_RECORD::save_record() ==================*/


/*========== Attempts to Compute the Cost of the Item =====================*/
long ITEM_RECORD::compute_cost (void) {
  int i = 0, j;
  long cost = 10;

	/* Make sure there is something to compare to */
  if (!this || !loaded) return (0);

	/* Search for the item's base cost */
  j = find_item (group, subgroup);
  if (j != -1) cost = items[j]->cost;

	/* Is this a piece of armor/weapon */
  if (group == 0x02) {
    i = 0;

    while (armor_mult[i].material != -1) {
      if (armor_mult[i].material == material && (armor_mult[i].constr == -1 || armor_mult[i].constr == construction)) {
	cost *= armor_mult[i].mult;
	break;
       }
      i++;
     }

   }
  else if (group == 0x03) {
    i = 0;

    while (weapon_mult[i].material != -1) {
      if (weapon_mult[i].material == material && (weapon_mult[i].constr == -1 || weapon_mult[i].constr == construction)) {
	cost *= weapon_mult[i].mult;
	break;
       }
      i++;
     }
   }

	/* Now, find the costs for the enchantments */
  for (i = 0; i < 10; i++) {

    if (enchantments[i] != -1) {
      j = find_enchantment (enchantments[i]);
      if (j != -1) cost += enchants[j]->cost;
     }
   }

  return (cost);
 }
/*========== End of Function ITEM_RECORD::compute_cost() ==================*/


/*========== Loads an Item from a File ====================================*/
boolean ITEM_RECORD::load_record (FILE *f) {
  short i;	/* Loop counter */
  long offset;

	/* Make sure there is something to read from */
  if (!f) {
    err_code = ERR_FILE;
    return (FALSE);
   }

	/* Save the current offset in file */
  offset = ftell(f);

	/* Start reading things */
  if (fread (null1, sizeof(char), 30, f) != 30) {
    err_code = ERR_READ;
    return (FALSE);
   }

  placement = read_int(f);

  if (fread (null2, sizeof(char), 22, f) != 22) {
    err_code = ERR_READ;
    return (FALSE);
   }

  index1 = read_long (f);
  index2 = read_long (f);
  null3 = read_long (f);
  token1 = read_long (f);

  if (fread (name, sizeof(char), 32, f) != 32) {
    err_code = ERR_READ;
    return (FALSE);
   }

  group = read_int (f);
  subgroup = read_int (f);
  cost = read_long (f);
  null4 = read_int (f);
  id = fgetc (f);
  null5 = fgetc (f);
  wear = read_int (f);
  max_wear = read_int (f);
  null6 = read_int (f);
  picture1 = read_int (f);
  picture2 = read_int (f);
  material = fgetc(f);
  construction = fgetc (f);
  color = fgetc (f);
  weight = fgetc (f);
  null7 = fgetc (f);
  null8 = read_int (f);
  enchant_pts = read_int (f);
  null9 = read_long (f);

  for (i = 0; i < 10; i++)
    enchantments[i] = read_long (f);

	/* Read the rest of the data, if any */
  offset = size - 1 - (ftell(f) - offset);

  if (offset > 0) {
    data_length = (short) offset;
    data = create_ptr (data_length);

    if (fread(data, sizeof(char), data_length, f) != data_length) {
      err_code = ERR_READ;
      return (FALSE);
     }
   }
  else
    data_length = 0;

	/* Make sure we haven't exceed the EOF */
  if (feof(f)) {
    err_code = ERR_READ;
    return (FALSE);
   }

	/* Return success */
  err_code = ERR_NONE;
  loaded = TRUE;
  return (TRUE);
 }
/*========== End of Function ITEM_RECORD::load_record() ===================*/


/*========== Saves an Item to File ========================================*/
boolean ITEM_RECORD::save_record (FILE *f) {
  short i;	/* Loop counter */

	/* Make sure there is something to write to */
  if (!f) return (FALSE);

	/* Write all the fields */
	/* Start reading things */
  if (fwrite (null1, sizeof(char), 30, f) != 30) return (FALSE);
  write_int (f, placement);
  if (fwrite (null2, sizeof(char), 22, f) != 22) return (FALSE);
  write_long (f, index1);
  write_long (f, index2);
  write_long (f, null3);
  write_long (f, token1);
  if (fwrite (name, sizeof(char), 32, f) != 32) return (FALSE);
  write_int (f, group);
  write_int (f, subgroup);
  write_long (f, cost);
  write_int (f, null4);
  fputc(id, f);
  fputc (null5, f);
  write_int (f, wear);
  write_int (f, max_wear);
  write_int (f, null6);
  write_int (f, picture1);
  write_int (f, picture2);
  fputc (material, f);
  fputc (construction, f);
  fputc (color, f);
  fputc (weight, f);
  fputc (null7, f);
  write_int (f, null8);
  write_int (f, enchant_pts);
  write_long (f, null9);

  for (i = 0; i < 10; i++)
    write_long (f, enchantments[i]);

	/* Write the rest of the data, if any */
  if (data_length && data) {
    if (fwrite (data, sizeof(char), data_length, f) != data_length) return (FALSE);
   }

  return (TRUE);
 }
/*========== End of Function ITEM_RECORD::save_record() ===================*/


/*========== Searchs the enchantment array for a match ====================*/
short find_enchantment (const long id) {
  int i;
  short type = id & 0xFFFF;
  short sub_type = (id >> 16) & 0xFFFF;

  for (i = 0; i < num_enchants; i++) {

    if (enchants[i]->type == type && (enchants[i]->subtype == sub_type
				   || enchants[i]->subtype == ENALL)) {
      return (i);
     }

   }

  if (id == -1)
    return (i);
  else {
    write_log_entry ("Unknown Enchantment: %d / %d", type, sub_type);
    return (-1);
   }
 }
/*========== End of Function find_enchantment() ===========================*/



/*========== Searchs Through the item_type Array for a Match ==============*/
short find_item (const short group, const short sub_group) {
  int i;

  for (i = 0; i < num_items; i++) {

    if (items[i]->group != -1 && group == items[i]->group && (sub_group == -1 || sub_group == items[i]->sub_group))
      return (i);

   }

  if (group == -1 && sub_group == -1)
    return (i);
  else {
    write_log_entry ("Unknown Item: %d / %d", group, sub_group);
    return (-1);
   }
 }
/*========== End of Function find_item() ==================================*/


/*========== Searches for a Picture in the picture_types[] array ==========*/
short find_picture (const ITEM_RECORD *item) {
  short i;

  for (i = 0; i < num_pictures; i++) {
    if (pictures[i]->index == item->picture1) return (i);
   }

  write_log_entry ("Unknown Picture = 0x%04X,  Item = '%s'", item->picture1, item->name);
  return (-1);
 }
/*========== End of Function find_picture() ===============================*/