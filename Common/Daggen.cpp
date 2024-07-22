	/* User Defined Includes */
#include "fileutil.h"

	/* Daggerfall specific includes */
#include "daggen.h"


/*========== Defines the Daggerfall Specific Error Codes ==================*/
void post_df_errors (void) {
  ErrorHandler.AddError (DF_ERR_NODATA,    "No data to save, NULL pointers found");
  ErrorHandler.AddError (DF_ERR_TOOMUCH,   "Too much unknown data in file");
  ErrorHandler.AddError (DF_ERR_64SIZE,    "Input exceeded the maximum of 64kb");
  ErrorHandler.AddError (DF_ERR_NOTAMD,    "File is not an AMD type");
  ErrorHandler.AddError (DF_ERR_MIDITRACK, "Did not find MIDI track text entry");
  ErrorHandler.AddError (DF_ERR_MIDISONG,  "Did not find MIDI song text entry");
  ErrorHandler.AddError (DF_ERR_ARTMIN,    "Artifact Index cannot be less than 23");
  ErrorHandler.AddError (DF_ERR_ARTMAX,    "Artifact Index cannot be greated than 255");
  ErrorHandler.AddError (DF_ERR_BADREC,    "Critical: Bad record found");
  ErrorHandler.AddError (DF_ERR_NOTEXT,    "Warning: Artifact has no matching text entry");
  ErrorHandler.AddError (DF_ERR_NOART,     "Warning: Text has no matching artifact entry");
  ErrorHandler.AddError (DF_ERR_MAXSPELL,  "Error: Spell data file contains too many spells to load!");
 }
/*========== End of Procedure post_df_errors() ============================*/



