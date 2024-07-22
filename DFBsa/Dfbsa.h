/*=========================================================================
 *
 * DFBSA.H - Dave Humphrey (uesp@m0use.net), 29 October 2000
 *
 *=======================================================================*/
#ifndef __DFBSA_H
#define __DFBSA_H


	/* Required Includes */
#include "genutil.h"
#include "errno.h"


/*=========================================================================
 *
 * Begin Defines
 *
 *=======================================================================*/

	/* The maximum number of records allowed in a BSA file. Limit
	 * Imposed by this source code only.  True limit is 0xFFFF, or
	 * 0x7FFF if DF uses signed integers. */
  #define MAX_DFBSA_RECORDS 0x3000

	/* Used as an error return value by some functions */
  #define DFBSA_ERROR (-1)

/*=========================================================================
 *		End of Defines
 *=======================================================================*/


/*=========================================================================
 *
 * Begin Type and Structure Definitions
 *
 *=======================================================================*/

	/* Defines the type of BSA directories */
 typedef enum { DFBSA_FILENAME_DIRECTORY = 0x0100,
		DFBSA_VALUE_DIRECTORY    = 0x0200 } dfbsa_dir_t;

	/* Type used for the BSA directory */
  typedef struct {
    union {
      char Filename[14];	/* Record Filename ID for 0x0100 types */
      long Value;		/* Record long Value ID for 0x0200 types */
     };
    long RecordSize;		/* Size of the BSA record */

    long RecordOffset;		/* Not recorded in the BSA, but useful */
   } dfbsa_dir_record_t;


	/* Extract callback function */
  typedef void (*PDFBSA_EXTRACT_PROC) (const int Index, const int MaxIndex, const dfbsa_dir_record_t& DirRecord, const dfbsa_dir_t DirType); 
  typedef void (  DFBSA_EXTRACT_PROC) (const int Index, const int MaxIndex, const dfbsa_dir_record_t& DirRecord, const dfbsa_dir_t DirType); 

/*=========================================================================
 *		End of Type and Structure Definitions
 *=======================================================================*/


/*=========================================================================
 *
 * Begin Class CDFBSAFile Definition
 *
 * The base class for all derived BSA file types.
 *
 *=======================================================================*/
class CDFBSAFile {

  /*---------- Begin Protected Class Members --------------------*/
protected:
  short 	NumRecords;	/* Number of records in BSA file */
  dfbsa_dir_t	DirectoryType;	/* Type of BSA directory */

	/* The BSA directory data */
  dfbsa_dir_record_t *pDirectory[MAX_DFBSA_RECORDS];

  FILE*   pFileHandle;		/* Handle to the file, if open */
  char*   pBSAFilename;		/* The filename, if available */
  boolean HeaderRead;		/* Have we read the header section yet? */
  boolean DirectoryRead;	/* Have we read the directory section yet? */


  /*---------- Begin Protected Class Methods --------------------*/
protected:

	/* Delete all directory entries */
  void ClearDirectory (void);

	/* Helper function for Dump() */
  void DumpDirectoryEntry (FILE* pFileHandle, const int DirIndex);

	/* Reads the BSA directory of a file */
  virtual boolean ReadDirectory (void);

	/* Reads the BSA file header */
  boolean ReadHeader (void);


  /*---------- Begin Public Class Methods -----------------------*/
public:

	/* Class Constructor/Destructor */
  CDFBSAFile();
  virtual ~CDFBSAFile() { Destroy(); DestroyPointer(pBSAFilename); }
  virtual void Destroy (void);

	/* Delete all the record data */
  virtual void ClearRecords (void) { NumRecords = 0; }

	/* Close the file if its open */
  void Close (void) { if (pFileHandle != NULL) { fclose (pFileHandle); pFileHandle = NULL; } }

	/* Outputs info on object to file stream */
  virtual void Dump (FILE* pFileHandle);

  	/* Outputs all records as raw binary files */
  boolean ExtractRecords (PDFBSA_EXTRACT_PROC pCallBack);

	/* Return the record index based on filename/value */
  int FindRecordIndex (const char* pFilename);
  int FindRecordIndex (const long  Value);

	/* Return the size of the directory records */
  long GetDirectoryRecordSize (void) { return ((DirectoryType == DFBSA_FILENAME_DIRECTORY) ? 18l: 8l); }
  long GetDirectorySize       (void) { return (NumRecords*GetDirectoryRecordSize()); }

	/* Get the record data */
  dfbsa_dir_record_t* GetRecord		(const int Index) { if (IsValidRecordIndex(Index)) return pDirectory[Index]; return (NULL); }
  char*		      GetRecordFilename (const int Index) { if (IsValidRecordIndex(Index) && DirectoryType == DFBSA_FILENAME_DIRECTORY) return pDirectory[Index]->Filename; return (NULL); }
  long		      GetRecordValue    (const int Index) { if (IsValidRecordIndex(Index) && DirectoryType == DFBSA_VALUE_DIRECTORY)    return pDirectory[Index]->Value; return (DFBSA_ERROR); }

	/* Return a record size based on filename/value */
  long GetRecordSize    (const char* pFilename);
  long GetRecordSize    (const long  Value);
  long GetRecordSizeI   (const int RecordIndex) { if (IsValidRecordIndex(RecordIndex)) return (pDirectory[RecordIndex]->RecordSize); return (DFBSA_ERROR); }
  long GetRecordOffsetI (const int RecordIndex) { if (IsValidRecordIndex(RecordIndex)) return (pDirectory[RecordIndex]->RecordOffset); return (DFBSA_ERROR); }

	/* Return the record offset from the directory */
  long GetRecordOffsetIndex (const int RecordIndex) { if (IsValidRecordIndex(RecordIndex)) return (pDirectory[RecordIndex]->RecordOffset); return (DFBSA_ERROR); }

	/* Return the number of records */
  int  GetNumRecords (void) { return (NumRecords); }

	/* Get the current filename if available */
  char* GetFilename (void) { return (pBSAFilename); }

	/* Checks a record index for validity */
  boolean IsValidRecordIndex (const int Index) { return ((Index >= 0 && Index < NumRecords) ? TRUE : FALSE);}
  boolean IsValidIndex       (const int Index) { return IsValidRecordIndex(Index); }

	/* Is the file currently open for input? */
  boolean IsOpen (void) { return ((pFileHandle == NULL) ? FALSE : TRUE); }

	/* Attempt to open a BSA file */
  boolean Open (const char* pFilename = NULL, const boolean ReadMode = TRUE);

	/* Set class members */
  void SetFilename (const char* pString) { DestroyPointer(pBSAFilename); pBSAFilename = CreateString(pString); }

	/* Outputs raw binary information to a new file */
  boolean WriteRawRecord (FILE* pFileHandle, const int Index);
  boolean WriteRawRecord (const int Index);

 };
/*=========================================================================
 *		End of Class CDFBSAFile Definition
 *=======================================================================*/


#endif
/*=========================================================================
 *		End of File DFBSA.H
 *=======================================================================*/



