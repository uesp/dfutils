
	/* User Defined Includes */
#include "fileutil.h"

	/* Daggerfall specific includes */
#include "daggen.h"
#include "dagsnd.h"


/*========== Checks Values of Things ======================================*/
void check_debug (void) {
  FILE *f;	/* File pointer */
  short i;   	/* Loop counters */

  if (!(f = openfile ("midi.txt", "wt"))) return;

  fprintf (f, "Song   Tick    FT   NumT  Length\n");

  for (i = 0; i < dagmidi.num_records; i++) {
    fprintf (f, "%3d)  0x%04X  0x%02X  %3d  %5ld\n", i, dagmidi.records[i]->tick_size, dagmidi.records[i]->file_type, dagmidi.records[i]->num_tracks, dagmidi.records[i]->length);
   }

  fclose (f);
 }
/*========== End of Procedure check_debug() ===============================*/


/*========== Begin Main Program ===========================================*/
int main (void) {

	/* Open the text file for debugging */
  open_log_file ("dagmidi.log");

  clrscr();

	/* Attempt to load the header info */
  if (dagmidi.load_header ("midi.bsa"))
    printf ("Header successfully read\n");
  else {
    printf ("An error occurred\n     %s\n", df_err_msg[df_err_code]);
    }

  check_debug();

  if (dagmidi.extract_midi ("midi.bsa", "trial.mid", 1))
    printf ("Saved Midi file successfully\n");
  else
    printf ("Error saving Midi File!\n     %s\n", df_err_msg[df_err_code]);


  return (TRUE);
 }
/*========== End Main Program =============================================*/