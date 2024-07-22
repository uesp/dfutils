	/* Standard C Includes */
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>

	/* User Defined Includes */
#include "fileutil.h"
#include "dagfile.h"
#include "dfguild.h"



/*========== Begin Main Program ===========================================*/
int main (void) {
  GUILD_RECORD *ptr;
  short i;

	/* Open log file for recording debugging messages */
  open_log_file ("dfguild.log");

	/* Attempt to load all the guild data from file */
  dagfile.load ("savetree.dat", DFRT_GUILD);

	/* Output some of the guild data */
  for (i = 0; i < dagfile.num_records; i++) {
    if (dagfile.records[i] && dagfile.records[i]->type == DF_GUILD) {
      ptr = (GUILD_RECORD *) dagfile.records[i];

      write_log_entry ("Guild Record %d (Offset = 0x%08lX )", i, ptr->offset);
      write_log_entry ("     U25=0x%04X   U26=0x%04X", abs((signed int)ptr->u27), abs((signed int)ptr->u28));
      write_log_entry ("     GuildID=%5d  (0x%04X)", ptr->guild_id, ptr->guild_id);
     }
   }


  return (TRUE);
 }
/*========== End Main Program =============================================*/
