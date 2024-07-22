/*===========================================================================
 *
 * DFThief.H - Dave Humphrey (uesp@m0use.net), Thursday, November 30, 2000
 *
 *=========================================================================*/
#ifndef __DFTHIEF_H
#define __DFTHIEF_H

	/* Required Include */
#include "genutil.h"


/*===========================================================================
 *
 * Begin Defines
 *
 *=========================================================================*/

	/* Character record type in SaveTree.DAT */
  #define DFTHIEF_CHARRECORD_TYPE 0x03

	/* Size of the SaveTree header */
  #define DFTHIEF_SAVETREE_HEADERSIZE 0x13

	/* Size of v2.13 of FALL.EXE */
  #define DFTHIEF_FALL213_FILESIZE 1864183l

	/* Flag offsets relative to character offset and values  */
  #define DFTHIEF_THIEFFLAG1_OFFSET (0x213+0x47)
  #define DFTHIEF_THIEFFLAG2_OFFSET (0x222+0x47)
  #define DFTHIEF_DBFLAG1_OFFSET (0x217+0x47)
  #define DFTHIEF_DBFLAG2_OFFSET (0x21F+0x47)
  #define DFTHIEF_THIEFFLAG1_VALUE 0x08
  #define DFTHIEF_THIEFFLAG2_VALUE 0x06
  #define DFTHIEF_DBFLAG1_VALUE 0x08
  #define DFTHIEF_DBFLAG2_VALUE 0x0F

	/* Backup filename for the save game file */
  #define DFTHIEF_BACKUP_NAME "SaveTree.DBK"

	/* Program version/date information */
  #define DFTHIEF_VERSION "0.1b"
  #define DFTHIEF_DATE    "30 November 2000"
  #define DFTHIEF_NAME    "DFThief"
  #define DFTHIEF_AUTHOR  "Dave Humphrey (uesp@m0use.net)"
  	
/*===========================================================================
 *		End of Defines
 *=========================================================================*/

/*===========================================================================
 *
 * Begin Type and Structure Definitions
 *
 *=========================================================================*/

	/* Three state flag */
  typedef enum { 
    DFTHIEF_FLAG_CLEAR = -1,
    DFTHIEF_FLAG_NONE  =  0,
    DFTHIEF_FLAG_SET   =  1
   } dfthief_flag_t;

/*===========================================================================
 *		End of Type and Structure Definitions
 *=========================================================================*/
 

/*===========================================================================
 *
 * Begin Function Prototypes
 *
 *=========================================================================*/

	/* Outputs error message and quits program */
  void AbortProgram (void);

	/* Attempt to ensure we are in the root DF directory */
  boolean CheckDFPath (void);

	/* Modify the required flags in the save game file */
  boolean ModifyDBFlags (void);
  boolean ModifyThiefFlags (void);

	/* Outputs help text to stdout */
  void OutputHelp (void);

	/* Outputs the program title to stdout */
  void OutputTitle (void);

	/* Parse one command line parameter */
  boolean ParseCommandParameter (char* pString);

	/* Attempts to parse the given SaveTree.DAT file */
  boolean ParseSaveTree (const int SaveNumber);

	/* Input one record from the SaveTree file */
  boolean ReadSaveTreeRecord (FILE* pFileHandle);

/*===========================================================================
 *		End of Function Prototypes
 *=========================================================================*/


#endif
/*===========================================================================
 *		End of File DFThief.H
 *=========================================================================*/
