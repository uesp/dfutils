	/* Standard Includes */
#include <stdlib.h>
#include <stdio.h>

	/* User Defined Includes */
#include "fileutil.h"
#include "daggen.h"
#include "dagdata.h"
#include "dagfile.h"

	/* The Types of Items */
ITEM_TYPE *items[MAX_ITEMS];
short num_items = 0;

	/* Enchantment list */
ENCHANT_TYPE *enchants[MAX_ENCHANTS];
short num_enchants = 0;

	/* Monster related data */
MOB_TYPE *mobs[MAX_MOBS];
short num_mobs = 0;

	/* Pictures of Items */
PICTURE_TYPE *pictures[MAX_PICTURES];
short num_pictures = 0;


	/* Holds The Cost Multipliers for armor */
MAT_TYPE armor_mult[] = {
  { 0,  0, 1},
  { 0,  1, 2},
  { 0,  2, 3},
  { 1, -1, 6},
  { 2, -1, 12},
  { 3, -1, 24},
  { 4, -1, 48},
  { 5, -1, 96},
  { 6, -1, 192},
  { 7, -1, 384},
  { 8, -1, 768},
  { 9, -1, 1536},
  {-1, -1, -1}
 };


	/* Cost multipliers for weapons */
MAT_TYPE weapon_mult[] = {
  { 0, -1, 3},
  { 1, -1, 6},
  { 2, -1, 12},
  { 3, -1, 24},
  { 4, -1, 48},
  { 5, -1, 96},
  { 6, -1, 192},
  { 7, -1, 384},
  { 8, -1, 768},
  { 9, -1, 1536},
  {-1, -1, -1}
 };



/*========== Clears the Data Arrays =======================================*/
void destroy_dag_data (void) {
  short i;	/* Loop counter */

  for (i = 0; i < num_items; i++) {
    DESTROY(items[i]);
   }

  for (i = 0; i < num_mobs; i++) {
    DESTROY(mobs[i]);
   }

  for (i = 0; i < num_enchants; i++) {
    DESTROY(enchants[i]);
   }

  for (i = 0; i < num_pictures; i++) {
    DESTROY(pictures[i]);
   }

  num_mobs = 0;
  num_items = 0;
  num_enchants = 0;
  num_pictures = 0;
 }
/*========== End of Procedure destroy_dag_data() ===========================*/


/*========== Attempts to Load Item Data From a Data File ==================*/
boolean load_dag_data (const char *filename, const short flags) {
  FILE *f;				/* File pointer */
  boolean done = FALSE; 		/* Loop counter */
  short reading = READING_NOTHING;	/* What section are we currently in? */
  char buffer[MAX_STRING_LENGTH + 1];	/* Temp Input buffer */
  char args[10][81];			/* Arguments */
  char *temp_ptr, *ptr;			/* Temp pointers */
  short line = 0;			/* Current line we are reading */
  short i, length;

	/* Make sure a valid filename and flags was passed */
  if (!filename || !flags) {
    err_code = ERR_NULL;
    return (FALSE);
   }

	/* Attempt to open filename for reading */
  if (!(f = openfile(filename, "rt"))) {
    err_code = ERR_FILE;
    return (FALSE);
   }

	/* Begin main reading loop */
  while (!done) {

	/* Read in file a line at a time */
    switch (read_eol (f, buffer, MAX_STRING_LENGTH)) {
      case READ_EOF:
	done = TRUE;
	break;
      case READ_MSL:
	read_eol(f);
	break;
     };

	/* Remove any comments */
   temp_ptr = strchr (buffer, '#');
   if (temp_ptr) *temp_ptr = 0;

	/* Trim excess spaces from string */
   temp_ptr = trim(buffer);
   length = strlen(temp_ptr);
   line++;

	/* Check for the main commands first */
   if (!stricmp(temp_ptr, "[items]")) {
     if (reading) write_log_entry ("     %4d: [items] section found before [end] reached!", line);
     reading = READING_ITEM;
    }
   else if (!stricmp(temp_ptr, "[monsters]")) {
     if (reading) write_log_entry ("     %4d: [monsters] section found before [end] reached!", line);
     reading = READING_MOB;
    }
   else if (!stricmp(temp_ptr, "[enchantments]")) {
     if (reading) write_log_entry ("     %4d: [enchantments] section found before [end] reached!", line);
     reading = READING_ENCHANT;
    }
   else if (!stricmp(temp_ptr, "[pictures]")) {
     if (reading) write_log_entry ("     %4d: [pictures] section found before [end] reached!", line);
     reading = READING_PICTURE;
    }
   else if (!stricmp(temp_ptr, "[end]")) {
     if (!reading) write_log_entry ("     %4d: Unexpected [end] found!");
     reading = READING_NOTHING;
    }

	/* Reading any type at the moment? */
   else if (reading && length) {

		/* Clear the arguments */
     for (i = 0; i < 10; i++)
       args[i][0] = 0;

		/* Parse all the arguments, separated by the comma */
     ptr = strtok(temp_ptr, ",");
     i = 0;

     while (ptr && i < 9) {
       strncpy (args[i], trim(ptr), 80);
       args[i][80] = 0;
       ptr = strtok(NULL, ",");
       i++;
      }

		/* Parse a Item data line */
     if (reading == READING_ITEM) {

       if (flags & DFFL_ITEMS && num_items < MAX_ITEMS) {

	 if (!(items[num_items] = new ITEM_TYPE (args[4])))
	   bug (get_errmsg(ERR_MEM), "load_dag_data() - *items[] (%d)", sizeof(ITEM_TYPE) + strlen(args[4]));

	 items[num_items]->group = (char) strtol(args[0], NULL, 0);
	 items[num_items]->sub_group = (char) strtol(args[1], NULL, 0);
	 items[num_items]->cost = strtol(args[2], NULL, 0);
	 items[num_items]->picture = (unsigned short) strtoul(args[3], NULL, 0);
	 num_items++;
	 if (num_items >= MAX_ITEMS) write_log_entry ("     %4d: Maximum number of items reached (%d)!", line, MAX_ITEMS);


		/* We may need to add some picture data */
	 if (items[num_items - 1]->picture != 0xFFFFu && flags & DFFL_PICTURES && num_pictures < MAX_PICTURES) {

	   if (!(pictures[num_pictures] = new PICTURE_TYPE (args[4])))
	     bug (get_errmsg(ERR_MEM), "load_dag_data() - *pictures[] (%d)", sizeof(PICTURE_TYPE) + strlen(args[4]));

	   pictures[num_pictures]->index = items[num_items - 1]->picture;
	   num_pictures++;
	   if (num_pictures >= MAX_PICTURES) write_log_entry ("     %4d: Maximum number of pictures reached (%d)!", line, MAX_PICTURES);
	  }
	}

      }

		/* Parse a Mob data line */
     else if (reading == READING_MOB) {

       if (flags & DFFL_MOBS && num_mobs < MAX_MOBS) {

	 if (!(mobs[num_mobs] = new MOB_TYPE (args[2])))
	   bug (get_errmsg(ERR_MEM), "load_dag_data() - *mobs[] (%d)", sizeof(MOB_TYPE) + strlen(args[2]));

	 mobs[num_mobs]->index = (short) strtol(args[0], NULL, 0);
	 mobs[num_mobs]->soul_cost = strtol(args[1], NULL, 0);
	 num_mobs++;
	 if (num_mobs >= MAX_MOBS) write_log_entry ("     %4d: Maximum number of mobs reached (%d)!", line, MAX_MOBS);
	}

      }

		/* Parse a Picture data line */
     else if (reading == READING_PICTURE) {

       if (flags & DFFL_PICTURES && num_pictures < MAX_PICTURES) {

	 if (!(pictures[num_pictures] = new PICTURE_TYPE (args[1])))
	     bug (get_errmsg(ERR_MEM), "load_dag_data() - *pictures[] (%d)", sizeof(PICTURE_TYPE) + strlen(args[1]));

	 pictures[num_pictures]->index = (unsigned short) strtoul(args[0], NULL, 0);
	 num_pictures++;
	 if (num_pictures >= MAX_PICTURES) write_log_entry ("     %4d: Maximum number of pictures reached (%d)!", line, MAX_PICTURES);
	}

      }

		/* Parse an Enchantment data line */
     else if (reading == READING_ENCHANT) {

       if (flags & DFFL_ENCHANTS && num_enchants < MAX_ENCHANTS) {

	 if (!(enchants[num_enchants] = new ENCHANT_TYPE (args[3])))
	   bug (get_errmsg(ERR_MEM), "load_dag_data() - *enchant[] (%d)", sizeof(MOB_TYPE) + strlen(args[3]));

	 enchants[num_enchants]->type = (unsigned short) strtoul(args[0], NULL, 0);
	 enchants[num_enchants]->subtype = (short) strtol (args[1], NULL, 0);
	 enchants[num_enchants]->cost = strtol(args[2], NULL, 0);
	 num_enchants++;
	 if (num_enchants >= MAX_ENCHANTS) write_log_entry ("     %4d: Maximum number of enchantments reached (%d)!", line, MAX_ENCHANTS);
	}

      }

		/* Unknown entry */
     else {
       write_log_entry ("     %4d: Unknown entry '%s'!", line, temp_ptr);
      }


    } /* End of if reading */

	/* Unknown entry */
   else if (length) {
     write_log_entry ("     %4d: Unknown entry '%s'!", line, temp_ptr);
    }

   }; /* End of while !done */

  fclose (f);

  write_log_entry ("   num_mobs = %d", num_mobs);
  write_log_entry ("   num_items = %d", num_items);
  write_log_entry ("   num_enchants = %d", num_enchants);
  write_log_entry ("   num_pictures = %d", num_pictures);
  err_code = ERR_NONE;
  return (TRUE);
 }
/*========== End of Function load_dag_data() ==============================*/
