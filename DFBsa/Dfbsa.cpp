/*=========================================================================
 *
 * DFBSA.CPP - Dave Humphrey (uesp@m0use.net), 29 October 2000
 *
 *=======================================================================*/

	/* Include Files */
#include "dfbsa.h"
#include "dfcommon.h"


#undef  __FUNC__
#define __FUNC__ "CDFBSAFile::CDFBSAFile()"
/*=========================================================================
 *
 * Class CDFBSAFile Constructor
 *
 *=======================================================================*/
CDFBSAFile::CDFBSAFile() {
  DirectoryType = DFBSA_FILENAME_DIRECTORY;
  NumRecords = 0;
  pFileHandle = NULL;
  pBSAFilename = NULL;
  HeaderRead = FALSE;
  DirectoryRead = FALSE;
 }
/*=========================================================================
 *		End of Class CDFBSAFile Constructor
 *=======================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFBSAFile::Destroy()"
/*=========================================================================
 *
 * Class CDFBSAFile Destructor
 *
 *=======================================================================*/
void CDFBSAFile::Destroy (void) {

	/* Close the file if its open */
  Close();

  	/* Delete the record data */
  ClearRecords();

	/* Delete the directory data */
  ClearDirectory();
  
  DirectoryType = DFBSA_FILENAME_DIRECTORY;
  HeaderRead = FALSE;
  DirectoryRead = FALSE;
 }
/*=========================================================================
 *		End of Class CDFBSAFile Destructor
 *=======================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFBSAFile::ClearDirectory"
/*=========================================================================
 *
 * Class CDFBSAFile Method - void ClearDirectory (void);
 *
 * Deletes all the directory entries. Protected class method.
 *
 *=======================================================================*/
void CDFBSAFile::ClearDirectory (void) {
  int LoopCounter;

	/* Delete all record directory entries */
  for (LoopCounter = 0; LoopCounter < NumRecords; LoopCounter++) {
    DestroyPointer(pDirectory[LoopCounter]);
   }

 }
/*=========================================================================
 *		End of Class Method CDFBSAFile::ClearDirectory();
 *=======================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFBSAFile::Dump()"
/*===========================================================================
 *
 * Class CDFBSAFile Method - void Dump (pFileHandle);
 *
 * Outputs debug information about the object to the given file stream.
 *
 *=========================================================================*/
void CDFBSAFile::Dump (FILE* pFileHandle) {
  int LoopCounter;
  int MaxCount;
  int FirstCount;
  
	/* Ignore any invalid handles */
  if (pFileHandle == NULL) return;

  fprintf (pFileHandle, "CDFBSAFile Object (0x%08p)\n", this);
  fprintf (pFileHandle, "\tNumRecords = %d\n", NumRecords);
  fprintf (pFileHandle, "\tDirectoryType = 0x%04X\n", DirectoryType);

	/* Output first five directory records */
  if (NumRecords >= 5) 
    MaxCount = 5;
  else 
    MaxCount = NumRecords;

  fprintf (pFileHandle, "\tDirectory Entries...\n");

  for (LoopCounter = 0; LoopCounter < MaxCount; LoopCounter++) 
    DumpDirectoryEntry(pFileHandle, LoopCounter);

	/* Output last five directory records */
  if (NumRecords >= 10)
    FirstCount = NumRecords - 5;
  else
    FirstCount = MaxCount;

  fprintf (pFileHandle, "\t\t....\n");

  for (LoopCounter = FirstCount; LoopCounter < NumRecords; LoopCounter++) 
    DumpDirectoryEntry(pFileHandle, LoopCounter);

 }
/*===========================================================================
 *		End of Class Method CDFBSAFile::Dump()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFBSAFile::DumpDirectoryEntry()"
/*===========================================================================
 *
 * Class CDFBSAFile Method - void DumpDirectoryEntry (pFileHandle, DirIndex);
 *
 * Dumps the specified directory entry to the given file stream.  Protected
 * class method.
 *
 *=========================================================================*/
void CDFBSAFile::DumpDirectoryEntry (FILE* pFileHandle, const int DirIndex) {

	/* Ensure a valid index */
  if (!IsValidRecordIndex(DirIndex)) return;

	/* Output based on the directory type */
  if (DirectoryType == DFBSA_FILENAME_DIRECTORY) 
    fprintf (pFileHandle, "\t\t%d) '%s' = 0x%08lX bytes at 0x%08lX\n", DirIndex, pDirectory[DirIndex]->Filename, pDirectory[DirIndex]->RecordSize, pDirectory[DirIndex]->RecordOffset);
  else 
    fprintf (pFileHandle, "\t\t%d) 0x%08lX = 0x%08lX bytes at 0x%08lX\n", DirIndex, pDirectory[DirIndex]->Value, pDirectory[DirIndex]->RecordSize, pDirectory[DirIndex]->RecordOffset);
   
 }
/*===========================================================================
 *		End of Class Method CDFBSAFile::DumpDirectoryEntry()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFBSAFile::ExtractRecords()"
/*===========================================================================
 *
 * Class CDFBSAFile Method - boolean ExtractRecords (pCallBack);
 *
 * Saves all BSA record information to seperate files in the current
 * directory.  Returns FALSE on any error.  The callback function, if
 * not NULL, is called before each record is extracted.
 *
 *=========================================================================*/
boolean CDFBSAFile::ExtractRecords (PDFBSA_EXTRACT_PROC pCallBack) {
  boolean HadToOpenFile = FALSE;
  int     LoopCounter;
  boolean Result = TRUE;

	/* Ensure file is current open */
  if (!IsOpen()) {
    if (!Open()) return (FALSE);
    HadToOpenFile = TRUE;
   }
	
	/* Output all records */
  for (LoopCounter = 0; LoopCounter < NumRecords; LoopCounter++) {
    if (pCallBack != NULL) pCallBack(LoopCounter, NumRecords, *pDirectory[LoopCounter], DirectoryType);
    Result = WriteRawRecord(LoopCounter);
    if (!Result) break;
   }

  if (HadToOpenFile) Close();
  return (Result);
 }
/*===========================================================================
 *		End of Class Method CDFBSAFile::ExtractRecords()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFBSAFile::FindRecordIndex()"
/*=========================================================================
 *
 * Class CDFBSAFile Method - int FindRecordIndex (pFilename);
 *
 * Searches for the given filename in the current directory and returns
 * the record index for it.  If the current directory is value type,
 * the file is not found, or on other errors, the function returns
 * DFBSA_ERROR.
 *
 *=======================================================================*/
int CDFBSAFile::FindRecordIndex (const char* pFilename) {
  int LoopCounter;
  int Result;
  int MinResult = 0;
  int MinIndex;

	/* Ignore any invalid input */
  if (pFilename == NULL) {
    SET_EXT_ERROR2(ERR_NULL, "Invalid NULL filename received!");
    return (DFBSA_ERROR);
   }

	/* Ensure correct directory type */
  if (DirectoryType != DFBSA_FILENAME_DIRECTORY) {
    SET_EXT_ERROR3(DFERR_BSA_DIRTYPE, "BSA filename directory type not in use!", pFilename);
    return (DFBSA_ERROR);
   }

	/* Search through all directory records */
  for (LoopCounter = 0; LoopCounter < NumRecords; LoopCounter++) {

		/* Ignore any NULL array records */
    if (pDirectory[LoopCounter] == NULL) continue;

		/* Compare record filename values */
    Result = strnicmp(pFilename, pDirectory[LoopCounter]->Filename, 13);
    if (Result == 0) return (LoopCounter);

		/* For debugging purposes mainly */
    if (LoopCounter == 0) {
      MinResult = Result;
      MinIndex = 0;
     }
    else if (MinResult > Result) {
      MinResult = Result;
      MinIndex = LoopCounter;
     }
   }

	/* No matching record found */
  if (NumRecords == 0) {
    SET_EXT_ERROR3(DFERR_BSA_NOTFOUND, "No matching record found for '%s', no directory records defined!", pFilename);
   }
  else {
    SET_EXT_ERROR5(DFERR_BSA_NOTFOUND, "No matching record found for '%s'. Closest match is %d[%s]", pFilename, MinIndex, pDirectory[MinIndex]->Filename);
   }

  return (DFBSA_ERROR);
 }
/*=========================================================================
 *		End of Class Method CDFBSAFile::FindRecordIndex();
 *=======================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFBSAFile::FindRecordIndex()"
/*=========================================================================
 *
 * Class CDFBSAFile Method - int FindRecordIndex (Value);
 *
 * Searches for the given value in the current directory and returns
 * the record index for it.  If the current directory is filenametype,
 * the file is not found, or on other errors, the function returns
 * DFBSA_ERROR.
 *
 *=======================================================================*/
int CDFBSAFile::FindRecordIndex (const long Value) {
  int  LoopCounter;
  long Result;
  long MinResult;
  int  MinIndex;

	/* Ensure correct directory type */
  if (DirectoryType != DFBSA_VALUE_DIRECTORY) {
    SET_EXT_ERROR3(DFERR_BSA_DIRTYPE, "BSA value directory type not in use!", Value);
    return (DFBSA_ERROR);
   }

	/* Search through all directory records */
  for (LoopCounter = 0; LoopCounter < NumRecords; LoopCounter++) {

		/* Ignore any NULL array records */
    if (pDirectory[LoopCounter] == NULL) continue;

		/* Compare record values */
    Result = labs(Value - pDirectory[LoopCounter]->Value);
    if (Result == 0) return (LoopCounter);

		/* For debugging purposes mainly */
    if (LoopCounter == 0) {
      MinResult = Result;
      MinIndex = 0;
     }
    else if (MinResult > Result) {
      MinResult = Result;
      MinIndex = LoopCounter;
     }
   }

	/* No matching record found */
  SET_EXT_ERROR5(DFERR_BSA_NOTFOUND, "No matching record found for %ld. Closest match is %d[%s]", Value, MinIndex, pDirectory[MinIndex]->Filename);
  return (DFBSA_ERROR);
 }
/*=========================================================================
 *		End of Class Method CDFBSAFile::FindRecordIndex();
 *=======================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFBSAFile::GetRecordSize()"
/*=========================================================================
 *
 * Class CDFBSAFile Method - long GetRecordSize (pFilename);
 *
 * Returns the record size of the given filename record, or DFBSA_ERROR
 * on any error.
 *
 *=======================================================================*/
long CDFBSAFile::GetRecordSize (const char* pFilename) {
  int Result;

	/* Get the matching record index, if any */
  Result = FindRecordIndex(pFilename);
  if (Result == DFBSA_ERROR) return(DFBSA_ERROR);

	/* Return the record size */
  return (pDirectory[Result]->RecordSize);
 }
/*=========================================================================
 *		End of Class Method CDFBSAFile::GetRecordSize()
 *=======================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFBSAFile::GetRecordSize()"
/*=========================================================================
 *
 * Class CDFBSAFile Method - long GetRecordSize (Value);
 *
 * Returns the record size of the given value record, or DFBSA_ERROR
 * on any error.
 *
 *=======================================================================*/
long CDFBSAFile::GetRecordSize (const long Value) {
  int Result;

	/* Get the matching record index, if any */
  Result = FindRecordIndex(Value);
  if (Result == DFBSA_ERROR) return(DFBSA_ERROR);

	/* Return the record size */
  return (pDirectory[Result]->RecordSize);
 }
/*=========================================================================
 *		End of Class Method CDFBSAFile::GetRecordSize()
 *=======================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFBSAFile::Open()"
/*=========================================================================
 *
 * Class CDFBSAFile Method - FILE* Open (pFilename, ReadMode);
 *
 * Attempt to open the given BSA file for reading/writing.  Attempts to
 * read/write the BSA header and directory.  Returns TRUE on success or
 * FALSE on any error. Use Close() to close the file after opening.
 *
 *=======================================================================*/
boolean CDFBSAFile::Open (const char* pFilename, const boolean ReadMode) {
  boolean Result;

	/* Write mode not yet supported */
  if (!ReadMode) {
    SET_EXT_ERROR3(ERR_FILE, "Write mode not yet supported!", pFilename);
    return (NULL);
   }

	/* Ignore if file is already open */
  if (IsOpen()) return (TRUE);

	/* Attempt to open file, use previous filename if none specified */
  if (pFilename == NULL) {
    pFileHandle = openfile(pBSAFilename, (ReadMode ? "rb" : "wb"));
   }
  else {
    pFileHandle = openfile(pFilename, (ReadMode ? "rb" : "wb"));
    if (pFileHandle != NULL) SetFilename(pFilename);
   }

	/* Ensure the open command was successfull */
  if (pFileHandle == NULL) {
    SET_EXT_ERROR3(ERR_FILE, "Failed to open file %s!", (pFilename == NULL) ? pBSAFilename : pFilename);
    return (FALSE);
   }

  if (DirectoryRead) return (TRUE);

	/* Read in file header and directory */
  if (ReadMode) {
    Result  = ReadHeader();
    if (Result) Result = ReadDirectory();
   }
  else
    Result = TRUE;

	/* Check for input/output errors */
  if (!Result) {
    Close();
    return (FALSE);
   }

  return (TRUE);
 }
/*=========================================================================
 *		End of Class Method CDFBSAFile::Open()
 *=======================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFBSAFile::ReadDirectory()"
/*=========================================================================
 *
 * Class CDFBSAFile Method - boolean ReadDirectory (void);
 *
 * Attempt to read the BSA directory from the current file stream.
 * Returns FALSE on any error, TRUE on success. Protected class method.
 *
 *=======================================================================*/
boolean CDFBSAFile::ReadDirectory (void) {
  int  Result;
  int  LoopCounter;
  long LastRecordOffset = 4;	/* Skip the 4 bytes of BSA header */

	/* Move to start of directory at end of file */
  Result = fseek (pFileHandle, -GetDirectorySize(), SEEK_END);

  if (Result != 0) {
    SET_EXT_ERROR2(ERR_READ, "Error moving to start of BSA directory!");
    return (FALSE);
   }

	/* Read in all directory records */
  for (LoopCounter = 0; LoopCounter < NumRecords; LoopCounter++) {
    CreatePointer(pDirectory[LoopCounter], dfbsa_dir_record_t);
    errno = 0;

		/* Read in the filename 0x0100 type */
    if (DirectoryType == DFBSA_FILENAME_DIRECTORY) {
      Result = fread (pDirectory[LoopCounter]->Filename, 1, 14, pFileHandle);
      if (Result != 14) errno = EFAULT;
     }
		/* Read in the value 0x0200 type */
    else if (DirectoryType == DFBSA_VALUE_DIRECTORY) {
      pDirectory[LoopCounter]->Value = read_long(pFileHandle);
     }

		/* Input the record size */
    pDirectory[LoopCounter]->RecordSize = read_long(pFileHandle);

		/* Check for errors in any of the above reads */
    if (errno != 0 || feof(pFileHandle)) {
      SET_EXT_ERROR3(ERR_READ, "Failed to read directory record %d", LoopCounter);
      return (FALSE);
     }

		/* Update the record offset */
    pDirectory[LoopCounter]->RecordOffset = LastRecordOffset;
    LastRecordOffset += pDirectory[LoopCounter]->RecordSize;
   }

	/* Return success */
  DirectoryRead = TRUE;
  return (TRUE);
 }
/*=========================================================================
 *		End of Class Method CDFBSAFile::ReadDirectory()
 *=======================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFBSAFile::ReadHeader()"
/*=========================================================================
 *
 * Class CDFBSAFile Method - boolean ReadHeader (void);
 *
 * Attempt to read the BSA header from the given open file stream.
 * Returns FALSE on any error, TRUE on success. Protected class method.
 *
 *=======================================================================*/
boolean CDFBSAFile::ReadHeader (void) {
  int  Result;

	/* Move to start of file */
  Result = fseek (pFileHandle, 0, SEEK_SET);

  if (Result != 0) {
    SET_EXT_ERROR2(ERR_READ, "Error moving to start of BSA file!");
    return (FALSE);
   }

	/* Read in the number of records. Assumes that all
	 * previous data has been cleared by this point. */
  NumRecords = read_short(pFileHandle);

  if (NumRecords <= 0 || NumRecords >= MAX_DFBSA_RECORDS) {
    SET_EXT_ERROR3(ERR_READ, "Invalid number of BSA records received (%d)!", NumRecords);
    return (FALSE);
   }

	/* Read in the directory type */
  DirectoryType = (dfbsa_dir_t) read_short(pFileHandle);

  if (DirectoryType <= 0) {
    SET_EXT_ERROR3(ERR_READ, "Invalid BSA directory type received (%d)!", DirectoryType);
    return (FALSE);
   }

	/* Return success */
  HeaderRead = TRUE;
  return (TRUE);
 }
/*=========================================================================
 *		End of Class Method CDFBSAFile::ReadHeader()
 *=======================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFBSAFile::WriteRawRecord()"
/*===========================================================================
 *
 * Class CDFBSAFile Method - boolean WriteRawRecord (Index);
 *
 * Attempts to output the given BSA record data to a filename derived from
 * the items directory entry. Returns FALSE on any error.
 *
 *=========================================================================*/
boolean CDFBSAFile::WriteRawRecord (const int Index) {
  FILE*   pOutputHandle;
  boolean HadToOpenFile;
  char    Buffer[20];
  boolean Result;

	/* Ensure valid record index */
  if (!IsValidIndex(Index)) {
    SET_EXT_ERROR3(ERR_NULL, "Invalid record index %d specified!", Index);
    return (FALSE);
   }

	/* Open file if required */
  if (!IsOpen()) {
    if (!Open()) return (FALSE);
    HadToOpenFile = TRUE;
   }

	/* Generate the filename from the directory entry */
  if (DirectoryType == DFBSA_FILENAME_DIRECTORY)
    strcpy (Buffer, pDirectory[Index]->Filename);
  else 
    sprintf (Buffer, "%08lX.dat", pDirectory[Index]->Value);

	/* Attempt to open file for output */
  pOutputHandle = openfile (Buffer, "wb");

  if (pOutputHandle == NULL) {
    SET_EXT_ERROR3(ERR_NULL, "Failed to open file '%s' for output!", Buffer);
    if (HadToOpenFile) Close();
    return (FALSE);
   }

	/* Output the record */
  Result = WriteRawRecord(pOutputHandle, Index);

	/* Close files if we had to open it */
  fclose (pOutputHandle);
  if (HadToOpenFile) Close();
  return (Result);
 }
/*===========================================================================
 *		End of Class Method CDFBSAFile::WriteRawRecord()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFBSAFile::WriteRawRecord()"
/*===========================================================================
 *
 * Class CDFBSAFile Method - boolean WriteRawRecord (pOutputHandle, Index);
 *
 * Attempts to output the given BSA record data to the specified file.
 * Returns FALSE on any error.
 *
 *=========================================================================*/
boolean CDFBSAFile::WriteRawRecord (FILE* pOutputHandle, const int Index) {
  byte*   pData;
  boolean HadToOpenFile = FALSE;
  int	  Result;

	/* Ensure valid input */
  if (pOutputHandle == NULL) {
    SET_EXT_ERROR2(ERR_NULL, "Invalid NULL file handle received!");
    return (FALSE);
   }

	/* Ensure valid record index */
  if (!IsValidIndex(Index)) {
    SET_EXT_ERROR3(ERR_NULL, "Invalid record index %d specified!", Index);
    return (FALSE);
   }

	/* Open file if required */
  if (!IsOpen()) {
    if (!Open()) return (FALSE);
    HadToOpenFile = TRUE;
   }

	/* Allocate a temporary pointer for input */
  CreatePointerArray(pData, byte, pDirectory[Index]->RecordSize);

	/* Jump to record and read it */
  fseek (pFileHandle, pDirectory[Index]->RecordOffset, SEEK_SET);
  Result = fread (pData, 1, pDirectory[Index]->RecordSize, pFileHandle);

  if (Result != pDirectory[Index]->RecordSize) {
    SET_EXT_ERROR3(ERR_READ, "Failed to read BSA record index %d from file!", Index);
    if (HadToOpenFile) Close();
    DestroyPointerArray(pData);
    return (FALSE);
   }

	/* Output data record to new file */
  Result = fwrite (pData, 1, pDirectory[Index]->RecordSize, pOutputHandle);
  if (HadToOpenFile) Close();
  DestroyPointerArray(pData);

  if (Result != pDirectory[Index]->RecordSize) {
    SET_EXT_ERROR3(ERR_READ, "Failed to write raw BSA record index %d to file!", Index);
    return (FALSE);
   }

  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFBSAFile::WriteRawRecord()
 *=========================================================================*/






