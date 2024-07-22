/*===========================================================================
 *
 * DFThief.CPP - Dave Humphrey (uesp@m0use.net), 30 November 2000
 *
 * Quick program which will edit the specified savegame and reset the flags
 * to initiate the Thief or Dark Brotherhood invitation quests.  This allows
 * one who has been kicked out (due to normal means or Vampirsim) to rejoin.
 * Has only been tested in the most recent version of Daggerfall, v2.13.
 *
 * The basic structure of the program is as follows:
 *	- Check if in DF directory by looking for FALL.EXE (abort if not)
 *	- Ensure the current version is v2.13 (size of FALL.EXE) and 
 *	  given warning if not.
 *	- Parse command line parameters.
 *	- Ensure the required parameters were specified.  If not exit
 *	  with a message.
 *	- Parse the entire savetree.dat file as specified by the user. Record
 *	  the position of the character data when found.  Ensure the entire
 *	  save game file is properly read.  If it isn't for any reason, abort
 *	  with an error message.
 *	- Prompt user with message about modifying save game files, allowing
 *	  them a chance to abort.
 *	- Create a backup SAVETREE.DBK file if it doesn't already exist.
 *	  Abort if the backup cannot be created.
 *	- Attempt to make the desired modifications to the save game file.
 *	- Output success or failure.
 * 
 *=========================================================================*/

	/* Required Includes */
#include "dfthief.h"

	/* Global variables */
int     NumZeroRecords  = 0;	/* Used to count seperating zero-sized records */
int     DungeonIndex    = -1;	/* Index of the dungeon record, if any */
int     NumRecords	= 0;	/* Total number of records found in the SaveTree.DAT */
long    CharacterOffset = -1;	/* Offset to the character record in SaveTree.DAT */
int     SaveGameNumber  = -1;	/* The savegame to edit (0-5) */
boolean NoPrompts       = FALSE;/* Do not prompt the user with any prompts */

	/* Flags for setting/clearing the values in the character record */
dfthief_flag_t SetThiefFlags = DFTHIEF_FLAG_NONE;
dfthief_flag_t SetDBFlags    = DFTHIEF_FLAG_NONE;


#undef  __FUNC__
#define __FUNC__ "AbortProgram()"
/*===========================================================================
 *
 * Function - void AbortProgram (void)
 *
 * Outputs the current error messages plus a standard description to stdout
 * and exits program.
 *
 *=========================================================================*/
void AbortProgram (void) {
  printf ("\nProgram Error...\n%s\n\n", ErrorHandler.GetExtErrorMessage());
  printf ("Read the DFTHIEF.TXT file or type DFThief /? for more information on proper\n");
  printf ("use of this program.  Feel free to e-mail the author at uesp@m0use.net for\n");
  printf ("further help or comments.\n\n");

  exit(EXIT_FAILURE);
 }
/*===========================================================================
 *		End of Function AbortProgram()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CheckDFPath()"
/*===========================================================================
 *
 * Function - boolean CheckDFPath (void)
 *
 * Attempts to determine if we are currently in the Daggerfall root path.
 * Looks for the file FALL.EXE and checks its size.  If it does not exist
 * function returns FALSE.  If it exists but is not the right size for
 * version v2.13 a warning message is printed but TRUE is returned.
 *
 *=========================================================================*/
boolean CheckDFPath (void) {
  int InputChar;


	/* The main DF EXE should exist in the DF path */
  if (!FileExists("Fall.exe")) {
    SET_EXT_ERROR2(ERR_CUSTOM, "\tThis program must be run in the root Daggerfall directory!");
    return (FALSE);
   }

	/* Check the file size offering the user a chance to abort if Daggerfall
	 * is not patched to v2.13. */
  if (GetFilesize("fall.exe") != DFTHIEF_FALL213_FILESIZE) {
    printf ("  This program has only been verified for v2.13 of Daggerfall, which\n");
    printf ("  I don't think you currently have.  Do you wish to continue (Y/N)?\n");
    if (NoPrompts) return (TRUE);

    do {
      InputChar = tolower(getch());
    } while (InputChar != 'y' && InputChar != 'n' && InputChar != 27);

    if (InputChar == 'y') return (TRUE);
    SET_EXT_ERROR2(ERR_CUSTOM, "\tUser aborted program!");
    return (FALSE);
   }	

  return (TRUE);
 }
/*===========================================================================
 *		End of Function CheckDFPath()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CreateBackup()"
/*===========================================================================
 *
 * Function - boolean CreateBackup (const int SaveNumber)
 *
 * Creates a backup of the savegame file, if it doesn't already exist.
 * Returns FALSE on any error.
 *
 *=========================================================================*/
boolean CreateBackup (const int SaveNumber) {
  char    OutputFile[32];
  char    InputFile[32];
  boolean Result;

	/* Create the filenames */
  sprintf (OutputFile, "save%1d\\%s", SaveNumber, DFTHIEF_BACKUP_NAME);
  sprintf (InputFile, "save%1d\\savetree.dat", SaveNumber);

	/* Ignore if the output backup already exists */
  if (FileExists(OutputFile)) {
    printf ("\tSave game backup already exists.\n");
    return (TRUE);
   }

	/* Attempt to create the backup */
  Result = CopyFile(InputFile, OutputFile);

  if (!Result) {
    SET_EXT_ERROR2(ERR_WRITE, "Failed to create the save game backup!");
   }

  printf ("\tCreated the save game backup.\n");
  return (Result);
 }
/*===========================================================================
 *		End of Function CreateBackup()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "ModifyDBFlags()"
/*===========================================================================
 *
 * Function - boolean ModifyDBFlags (void)
 *
 * Sets or clears the DB flags as required.  Returns FALSE on any error.
 *
 *=========================================================================*/
boolean ModifyDBFlags (void) {
  FILE* pFileHandle;
  char  Filename[32];
  int   Result;

	/* Ignore if nothing to do */
  if (SetDBFlags == DFTHIEF_FLAG_NONE) return (TRUE);

	/* Attempt to open the savegame file for output */
  sprintf (Filename, "save%1d\\savetree.dat", SaveGameNumber);
  pFileHandle = openfile (Filename, "r+b");
  if (pFileHandle == NULL) return (FALSE);

	/* Jump to the appropriate offset */
  fseek (pFileHandle, CharacterOffset + DFTHIEF_DBFLAG1_OFFSET, SEEK_SET);

	/* Write the appropiate byte */
  if (SetDBFlags == DFTHIEF_FLAG_CLEAR)
    Result = fputc(0x00, pFileHandle);
  else
    Result = fputc(DFTHIEF_DBFLAG1_VALUE, pFileHandle);

	/* Ensure success */
  if (Result < 0) {
    SET_EXT_ERROR2(ERR_WRITE, "Failed to write to the save game file!");
    fclose (pFileHandle); 
    return (FALSE);
   }

	/* Jump to the appropriate offset */
  fseek (pFileHandle, CharacterOffset + DFTHIEF_DBFLAG2_OFFSET, SEEK_SET);

	/* Write the appropiate byte */
  if (SetDBFlags == DFTHIEF_FLAG_CLEAR)
    Result = fputc(0x00, pFileHandle);
  else
    Result = fputc(DFTHIEF_DBFLAG2_VALUE, pFileHandle);

	/* Ensure success */
  if (Result < 0) {
    SET_EXT_ERROR2(ERR_WRITE, "Failed to write to the save game file!");
    fclose (pFileHandle); 
    return (FALSE);
   }

  if (SetDBFlags == DFTHIEF_FLAG_CLEAR)
    printf ("\tSuccessfully cleared the Dark Brotherhood flags...\n");
  else
    printf ("\tSuccessfully set the Dark Brotherhood flags...\n");

  fclose (pFileHandle);
  return (TRUE);
 }
/*===========================================================================
 *		End of Function ModifyDBFlags()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "ModifyThiefFlags()"
/*===========================================================================
 *
 * Function - boolean ModifyThiefFlags (void)
 *
 * Sets or clears the thief flags as required.  Returns FALSE on any error.
 *
 *=========================================================================*/
boolean ModifyThiefFlags (void) {
  FILE* pFileHandle;
  char  Filename[32];
  int   Result;

	/* Ignore if nothing to do */
  if (SetThiefFlags == DFTHIEF_FLAG_NONE) return (TRUE);

	/* Attempt to open the savegame file for output */
  sprintf (Filename, "save%1d\\savetree.dat", SaveGameNumber);
  pFileHandle = openfile (Filename, "r+b");
  if (pFileHandle == NULL) return (FALSE);

	/* Jump to the appropriate offset */
  fseek (pFileHandle, CharacterOffset + DFTHIEF_THIEFFLAG1_OFFSET, SEEK_SET);

	/* Write the appropiate byte */
  if (SetThiefFlags == DFTHIEF_FLAG_CLEAR)
    Result = fputc(0x00, pFileHandle);
  else
    Result = fputc(DFTHIEF_THIEFFLAG1_VALUE, pFileHandle);

	/* Ensure success */
  if (Result < 0) {
    SET_EXT_ERROR2(ERR_WRITE, "Failed to write to the save game file!");
    fclose (pFileHandle); 
    return (FALSE);
   }

	/* Jump to the appropriate offset */
  fseek (pFileHandle, CharacterOffset + DFTHIEF_THIEFFLAG2_OFFSET, SEEK_SET);

	/* Write the appropiate byte */
  if (SetThiefFlags == DFTHIEF_FLAG_CLEAR)
    Result = fputc(0x00, pFileHandle);
  else
    Result = fputc(DFTHIEF_THIEFFLAG2_VALUE, pFileHandle);

	/* Ensure success */
  if (Result < 0) {
    SET_EXT_ERROR2(ERR_WRITE, "Failed to write to the save game file!");
    fclose (pFileHandle); 
    return (FALSE);
   }

  if (SetThiefFlags == DFTHIEF_FLAG_CLEAR)
    printf ("\tSuccessfully cleared the Thief Guild flags...\n");
  else
    printf ("\tSuccessfully set the Thief Guild flags...\n");

  fclose (pFileHandle);
  return (TRUE);
 }
/*===========================================================================
 *		End of Function ModifyThiefFlags()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "OutputHelp()"
/*===========================================================================
 *
 * Function - void OutputHelp (void)
 *
 * Outputs program help text to stdout.
 *
 *=========================================================================*/
void OutputHelp (void) {
  printf ("\n");
  printf ("   Format:  DFThief [Options] [SaveNumber]\n\n");
  printf ("   Options (At least one option is required)\n");
  printf ("   \t/t ..... Set the Thief Guild flags (/-t or /t- clears them)\n");
  printf ("   \t/d ..... Set the Dark Brotherhood flags (/-d or /d- clears them)\n");
  printf ("   \t/p ..... No prompts (automatically answers Yes)\n\n");

  printf ("   Save Number\n");
  printf ("   \tA number ranging from 0 to 5 indicating which save game to modify.\n");
  printf ("   \tThe save games are arranged as shown   0   3\n");
  printf ("   \tto the right when in the Daggerfall    1   4\n");
  printf ("   \tload/save screen.                      2   5\n");
  printf ("\n");
  printf ("   DFThief must be run from the Daggerfall directory.  Read the file\n");
  printf ("   DFThief.TXT for more information.\n\n");

 }
/*===========================================================================
 *		End of Function OutputHelp()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "OutputTitle()"
/*===========================================================================
 *
 * Function - void OutputTitle (void)
 *
 * Outputs the program title to stdout.
 *
 *=========================================================================*/
void OutputTitle (void) {
  printf ("\n%s v%s, %s, by %s\n", DFTHIEF_NAME, DFTHIEF_VERSION, DFTHIEF_DATE, DFTHIEF_AUTHOR);
 }
/*===========================================================================
 *		End of Function OutputTitle()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "ParseCommandParameter()"
/*===========================================================================
 *
 * Function - boolean ParseCommandParameter (char* pString)
 *
 * Parses the given command line parameter.  Returns FALSE on any error.
 * The string might be modified.
 *
 *=========================================================================*/
boolean ParseCommandParameter (char* pString) {
  boolean FlagState = FALSE;
	
	/* Check for special switches */
  if (*pString == '/' || *pString == '\\') {
    pString = trim(pString + 1);

		/* Special case for flag inversion */
    if (*pString == '-') {
      pString++;
      FlagState = TRUE;
     }

    if (pString[1] == '-') FlagState = !FlagState;

    switch (tolower(*pString)) {

		/* Output help text */
      case '?':
        OutputHelp();
        exit(EXIT_SUCCESS);

		/* No prompts */
      case 'p':
        NoPrompts = TRUE;
	break;

		/* Set Thief flags */
      case 't':
        SetThiefFlags = FlagState ? DFTHIEF_FLAG_CLEAR : DFTHIEF_FLAG_SET;
        break;

		/* Set Dark Brotherhood flags */
      case 'd':
        SetDBFlags = FlagState ? DFTHIEF_FLAG_CLEAR : DFTHIEF_FLAG_SET;
        break;

		/* Unknown command option */
      default:
        printf ("\tUnknown option /%c!\n", *pString);
        break;
     }

    return (TRUE);
   }

	/* If not a switch it should be a savegame number */
  if (!StringDigit(pString)) {
    printf ("\tInvalid save game number '%s'!\n", pString);
    return (FALSE);
   }

  SaveGameNumber = atoi(pString);
  return (TRUE);
 }
/*===========================================================================
 *		End of Function ParseCommandParameter()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "ParseSaveTree()"
/*===========================================================================
 *
 * Function - boolean ParseSaveTree (const int SaveNumber)
 *
 * Parses the entire SaveTree.DAT file as given by the SaveNumber (0-5).
 * Assumes the current directory is the Daggerfall directory.  Returns FALSE
 * on any error.
 *
 *=========================================================================*/
boolean ParseSaveTree (const int SaveNumber) {
  FILE*   pFileHandle;
  boolean Result = FALSE;
  long    RecordOffset;
  long    FileSize;
  char    Filename[64];

	/* Ensure a valid number */
  if (SaveNumber < 0 || SaveNumber > 5) { 
    SET_EXT_ERROR3(ERR_INDEX, "\tInvalid save game number %d!", SaveNumber);
    return (FALSE);
   }

	/* Create the filename string and attempt to open file */
  sprintf (Filename, "save%1d\\savetree.dat", SaveNumber);
  pFileHandle = openfile(Filename, "rb");

  if (pFileHandle == NULL) {
    SET_EXT_ERROR3(ERR_FILE, "\tFailed to open file '%s'!", Filename);
    return (FALSE);
   }

  printf ("\tParsing save game file '%s'...\n", Filename);

	/* Skip the SaveTree.DAT file header */
  fseek (pFileHandle, DFTHIEF_SAVETREE_HEADERSIZE, SEEK_SET);

	/* Read until the end of file */
  while (!feof(pFileHandle) && NumZeroRecords < 3) { 
    Result = ReadSaveTreeRecord(pFileHandle);
    if (!Result) break;
   }  
	
  RecordOffset = ftell(pFileHandle);
  fseek(pFileHandle, 0, SEEK_END);
  FileSize = ftell(pFileHandle);
  fclose (pFileHandle);

	/* Ensure we read the entire file correctly */
  if (RecordOffset != FileSize) {
    SET_EXT_ERROR2(ERR_CUSTOM, "\tDid not correctly read the file, invalid format!");
    SystemLog.Printf ("FileSize/Offset = 0x%08lX / 0x%08lX", FileSize, RecordOffset);
    return (FALSE);
   }

  	/* Ensure we found the character record */
  if (CharacterOffset < 0) {
    SET_EXT_ERROR2(ERR_CUSTOM, "\tFailed to find character data in file!");
    return (FALSE);
   }

  return (Result);
 }
/*===========================================================================
 *		End of Function ParseSaveTree()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "PromptUser()"
/*===========================================================================
 *
 * Function - void PromptUser (void)
 *
 * Gives user a prompt ensuring they wish to modify the save game.  If the
 * user selects No, the program exits with EXIT_FAILURE.
 *
 *=========================================================================*/
void PromptUser (void) {
  int InputChar;

	/* Ignore if no prompts flag is set */
  if (NoPrompts) return;

  printf ("\n  About to modify your savegame file.  A backup will be created in the savegame\n");
  printf ("  directory as %s.  Are you sure you wish to proceed (Y/N)?\n\n", DFTHIEF_BACKUP_NAME);

  do {
    InputChar = tolower(getch());
  } while (InputChar != 'y' && InputChar != 'n' && InputChar != 27);

  if (InputChar == 'y') return;

  SET_EXT_ERROR2(ERR_CUSTOM, "\tUser aborted program!");
  AbortProgram();
 }
/*===========================================================================
 *		End of Function PromptUser()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "ReadSaveTreeRecord()"
/*===========================================================================
 *
 * Function - boolean ReadSaveTreeRecord (pFileHandle)
 *
 * Reads in the record size and optionally the record type of the next
 * record in the given SaveTree file.  Only called by ParseSaveTree().
 * Returns FALSE on any error.
 *
 *=========================================================================*/
boolean ReadSaveTreeRecord (FILE* pFileHandle) {
  long    RecordSize;
  long    RecordOffset;
  int     RecordType;
  boolean Result;
	
	/* Input the record size and offset */
  Result = read_long(pFileHandle, RecordSize);
  RecordOffset = ftell(pFileHandle);

  if (!Result) {
    SET_EXT_ERROR3(ERR_READ, "\tFailed to read record size from SaveTree.DAT file (Offset = 0x%08lX)!", RecordOffset);
    return (FALSE);
   }

  SystemLog.Printf ("Reading record at 0x%08lX, Size = 0x%04X", RecordOffset, RecordSize);

	/* Is this the town record? */
  if (NumRecords == 0) {
    fseek (pFileHandle, RecordSize, SEEK_CUR);
    NumRecords++;
    return (TRUE);
   }

	/* Is this the dungeon record? */
  if (DungeonIndex < 0 && NumZeroRecords == 2) {  
    fseek (pFileHandle, RecordSize, SEEK_CUR);
    DungeonIndex = NumRecords;
    NumRecords++;
    return (TRUE);
   }

	/* Check for zero-sized records */
  if (RecordSize == 0) {
    NumZeroRecords++;
    NumRecords++;
    return (TRUE);
   }

	/* Read the record type */
  RecordType = fgetc(pFileHandle);

  if (RecordType < 0) {
    SET_EXT_ERROR3(ERR_READ, "\tFailed to read record type from SaveTree.DAT file (Offset = 0x%08lX)!", RecordOffset);
    return (FALSE);
   }

	/* Check for the character record */
  if (RecordType == DFTHIEF_CHARRECORD_TYPE) {
    CharacterOffset = RecordOffset;
    printf ("\tFound character at offset 0x%08lX\n", RecordOffset);
   }

  fseek (pFileHandle, RecordSize-1, SEEK_CUR); 

  return (TRUE);
 }
/*===========================================================================
 *		End of Function ReadSaveTreeRecord()
 *=========================================================================*/


/*===========================================================================
 *
 * Begin Main Program
 *
 *=========================================================================*/
int main (int argc, char* argv[]) {
  boolean Result;
  int     LoopCounter;

	/* Open log file for debugging output */
  SystemLog.Open ("DFThief.Log");
  OutputTitle();
	
	/* DEBUG: Ensure we are in the DF directory */
//  chdir("f:\\dagger");

	/* Attempt to ensure a valid directory */
  Result = CheckDFPath();
  if (!Result) AbortProgram();

	/* Parse the command line parameters */
  for (LoopCounter = 1; LoopCounter < argc; LoopCounter++) {
    ParseCommandParameter (argv[LoopCounter]);
   }

	/* Do we have the required information to do anything */
  if (SaveGameNumber < 0 || 
      (SetThiefFlags == DFTHIEF_FLAG_NONE && SetDBFlags == DFTHIEF_FLAG_NONE)) {
    printf ("\tYou must specify something to modify, for example 'dfthief 3 /t'\n");
    printf ("\tto modify save game number 3 to set the thief flags.  Type\n");
    printf ("\t'dfthief /?' for more information.\n\n");
    return (EXIT_FAILURE);
   }
  else if (SetThiefFlags == DFTHIEF_FLAG_SET && SetDBFlags == DFTHIEF_FLAG_SET) {
    printf ("  Note that setting both the Thief and Dark Brotherhood flags may\n");
    printf ("  not work as expected.  It is advisable that you set one, complete\n");
    printf ("  the quest, and then set the other.  Do you wish to continue (Y/N)?\n\n");

    if (!NoPrompts) {
      int InputChar;

      do {
        InputChar = tolower(getch());
      } while (InputChar != 'y' && InputChar != 'n' && InputChar != 27);

      if (InputChar != 'y') {
        SET_EXT_ERROR2(ERR_CUSTOM, "\tUser aborted program!");
        AbortProgram();
       }
     }
   }
   
  
	/* Attempt to parse the specified savetree file */  
  Result = ParseSaveTree(SaveGameNumber);
  if (!Result) AbortProgram();

	/* Prompt user to make any desired changes */
  PromptUser();

	/* Create the backup file */
  Result = CreateBackup(SaveGameNumber);
  if (!Result) AbortProgram();



	/* Modify the Thief flags if required */
  Result = ModifyThiefFlags();
  if (!Result) AbortProgram();
  
	/* Modify the DB flags if required */
  Result = ModifyDBFlags();
  if (!Result) AbortProgram();

  return (EXIT_SUCCESS);
 }
/*===========================================================================
 *		End of Main Program
 *=========================================================================*/