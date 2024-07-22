/*===========================================================================
 *
 * DFBlocks.CPP - Dave Humphrey (uesp@m0use.net), 14 November 2000
 *
 *=========================================================================*/

	/* Undefine to remove all statistics from file */
//#define DOSTATS

	/* Include Files */
#include "dfblocks.h"
#include "dfcommon.h"

DEF_STAT_REC(RMBNumRecords1)
DEF_STAT_REC(RMBNumRecords2)
DEF_STAT_REC(RMBNumRecords3)
DEF_STAT_REC(RMBNumSec1Rec)
DEF_STAT_REC(RMBNumSec2Rec)
DEF_STAT_REC(RMBNumSec3Rec)
DEF_STAT_REC(RMBNumSec4Rec)
DEF_STAT_REC(RMBNumSec5Rec)
DEF_STAT_ARR(RMBHeaderUnknowns, 6)
DEF_STAT_VAR(RMBSec1U1)
DEF_STAT_VAR(RMBSec1U2)
DEF_STAT_VAR(RMBSec1U3)
DEF_STAT_VAR(RMBSec1U4)
DEF_STAT_VAR(RMBSec1U5)
DEF_STAT_VAR(RMBSec1U6)
DEF_STAT_VAR(RMBSec1U7)
DEF_STAT_VAR(RMBSec1U8)
DEF_STAT_VAR(RMBSec1X1)
DEF_STAT_VAR(RMBSec1Y1)
DEF_STAT_VAR(RMBSec1Z1)
DEF_STAT_VAR(RMBSec1X2)
DEF_STAT_VAR(RMBSec1Y2)
DEF_STAT_VAR(RMBSec1Z2)
DEF_STAT_VAR(RMBSec2U1)
DEF_STAT_VAR(RMBSec2U2)
DEF_STAT_VAR(RMBSec2U3)
DEF_STAT_VAR(RMBSec3U1)
DEF_STAT_VAR(RMBSec3U2)
DEF_STAT_VAR(RMBSec3U3)
DEF_STAT_VAR(RMBSec4U1)
DEF_STAT_VAR(RMBSec4U2)
DEF_STAT_VAR(RMBSec4U3)
DEF_STAT_VAR(RMBSec5U1)
DEF_STAT_VAR(RMBSec5U2)
DEF_STAT_VAR(RMBSec5U3)
DEF_STAT_VAR(RMBSec5U4)


#undef  __FUNC__
#define __FUNC__ "CDFRMBRecord::CDFRMBRecord()"
/*===========================================================================
 *
 * Class CDFRMBRecord Constructor
 *
 *=========================================================================*/
CDFRMBRecord::CDFRMBRecord() {

	/* Initialize array pointers */
  p3DObjectRecords = NULL;
  pFlatObjectRecords = NULL;
  pSection3Records = NULL;
  pPeopleRecords = NULL;
  pDoorRecords = NULL;

  ExtraByte = 0;
  HasExtraByte = FALSE;

  	/* Initialize header data */
  memset(&Header, 0, sizeof(dfblk_rmb_header_t));
 }
/*===========================================================================
 *		End of Class CDFRMBRecord Constructor	
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFRMBRecord::Destroy()"
/*===========================================================================
 *
 * Class CDFRMBRecord Destructor
 *
 *=========================================================================*/
void CDFRMBRecord::Destroy (void) {

	/* Unallocate memory */
  DestroyPointerArray(p3DObjectRecords);
  DestroyPointerArray(pFlatObjectRecords);
  DestroyPointerArray(pSection3Records);
  DestroyPointerArray(pPeopleRecords);
  DestroyPointerArray(pDoorRecords);

  ExtraByte = 0;
  HasExtraByte = FALSE;

	/* Clear header data */
  memset(&Header, 0, sizeof(dfblk_rmb_header_t));
 }
/*===========================================================================
 *		End of Class CDFRMBRecord Destructor
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFRMBRecord::Read()"
/*===========================================================================
 *
 * Class CDFRMBRecord Method - boolean Read (pFileHandle);
 *
 * Attempts to read the record from the current position in the file stream.
 * Returns FALSE on any error.
 *
 *=========================================================================*/
boolean CDFRMBRecord::Read (FILE* pFileHandle) {
  boolean Result;

	/* Ensure valid input */
  if (pFileHandle == NULL) {
    SET_EXT_ERROR2(ERR_NULL, "Received invalid NULL file handle!");
    return (FALSE);
   }

	/* Attempt to read in all the record data */
  Result = ReadHeader(pFileHandle);
  if (Result) Result = Read3DObjects (pFileHandle);
  if (Result) Result = ReadFlatObjects (pFileHandle);
  if (Result) Result = ReadSection3 (pFileHandle);
  if (Result) Result = ReadPeople (pFileHandle);
  if (Result) Result = ReadDoors (pFileHandle);

  return (Result);
 }
/*===========================================================================
 *		End of Class Method CDFRMBRecord::Read()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFRMBRecord::ReadHeader()"
/*===========================================================================
 *
 * Class CDFRMBRecord Method - boolean ReadHeader (pFileHandle);
 *
 * Attempts to read the record header from the current position in the 
 * file stream. Returns FALSE on any error. Protected class method.
 *
 *=========================================================================*/
boolean CDFRMBRecord::ReadHeader (FILE* pFileHandle) {
  int Result;

	/* Attempt to read the header all at once */
  Result = fread (&Header, 1, sizeof(dfblk_rmb_header_t), pFileHandle);

  if (Result != sizeof(dfblk_rmb_header_t)) {
    SET_EXT_ERROR2(ERR_READ, "Failed to read RMB header from file!");
    return (FALSE);
   }

	/* Do optional statistics on success */
  DO_STAT_REC2(RMBNumSec1Rec, Header.Num3DObjectRecords);
  DO_STAT_REC2(RMBNumSec2Rec, Header.NumFlatObjectRecords);
  DO_STAT_REC2(RMBNumSec3Rec, Header.NumSection3Records);
  DO_STAT_REC2(RMBNumSec4Rec, Header.NumPeopleRecords);
  DO_STAT_REC2(RMBNumSec5Rec, Header.NumDoorRecords);
  DO_STAT_ARR(RMBHeaderUnknowns, Header.Unknowns, 6)

  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFRMBRecord::ReadHeader()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFRMBRecord::Read3DObjects()"
/*===========================================================================
 *
 * Class CDFRMBRecord Method - boolean Read3DObjects (pFileHandle);
 *
 * Attempts to read the record 3D Object data from the current position in the 
 * file stream. Returns FALSE on any error. Protected class method.
 *
 *=========================================================================*/
boolean CDFRMBRecord::Read3DObjects (FILE* pFileHandle) {
  int Result;

	/* Ignore if nothing to do */
  if (Header.Num3DObjectRecords == 0) return (TRUE);

	/* Allocate pointer array */
  CreatePointerArray(p3DObjectRecords, dfblk_rmb_3dobject_t, Header.Num3DObjectRecords);

	/* Attempt to read the data all at once */
  Result = fread (p3DObjectRecords, sizeof(dfblk_rmb_3dobject_t), Header.Num3DObjectRecords, pFileHandle);

  if (Result != Header.Num3DObjectRecords) {
    SET_EXT_ERROR2(ERR_READ, "Failed to read RMB 3D Object Data from file!");
    return (FALSE);
   }

	/* Do optional statistics on success */
  #ifdef DOSTATS
    int LoopCounter;

    for (LoopCounter = 0; LoopCounter < Header.Num3DObjectRecords; LoopCounter++) {
      DO_STAT_VAR(RMBSec1U1, p3DObjectRecords[LoopCounter].Unknown1)
      DO_STAT_VAR(RMBSec1U2, p3DObjectRecords[LoopCounter].Unknown2)
      DO_STAT_VAR(RMBSec1U3, p3DObjectRecords[LoopCounter].Unknown3)
      DO_STAT_VAR(RMBSec1U4, p3DObjectRecords[LoopCounter].Unknown4)
      DO_STAT_VAR(RMBSec1U5, p3DObjectRecords[LoopCounter].Unknown5)  
      DO_STAT_VAR(RMBSec1U6, p3DObjectRecords[LoopCounter].Unknown6)
      DO_STAT_VAR(RMBSec1U7, p3DObjectRecords[LoopCounter].Unknown7)
      DO_STAT_VAR(RMBSec1U8, p3DObjectRecords[LoopCounter].Unknown8)
      DO_STAT_VAR(RMBSec1X1, p3DObjectRecords[LoopCounter].XPos1)
      DO_STAT_VAR(RMBSec1Y1, p3DObjectRecords[LoopCounter].YPos1)
      DO_STAT_VAR(RMBSec1Z1, p3DObjectRecords[LoopCounter].ZPos1)
      DO_STAT_VAR(RMBSec1X2, p3DObjectRecords[LoopCounter].XPos2)
      DO_STAT_VAR(RMBSec1Y2, p3DObjectRecords[LoopCounter].YPos2)
      DO_STAT_VAR(RMBSec1Z2, p3DObjectRecords[LoopCounter].ZPos2)

//      if (pSection1Records[LoopCounter].Unknown8 != 0)
//        SystemLog.Printf ("Sec1YU8: 0x%08lX", pSection1Records[LoopCounter].Unknown8);
     }
  #endif

  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFRMBRecord::Read3DObjects()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFRMBRecord::ReadFlatObjects()"
/*===========================================================================
 *
 * Class CDFRMBRecord Method - boolean ReadFlatObjects (pFileHandle);
 *
 * Attempts to read the record section2 data from the current position in the 
 * file stream. Returns FALSE on any error. Protected class method.
 *
 *=========================================================================*/
boolean CDFRMBRecord::ReadFlatObjects (FILE* pFileHandle) {
  int Result;

	/* Ignore if nothing to do */
  if (Header.NumFlatObjectRecords == 0) return (TRUE);

	/* Allocate pointer array */
  CreatePointerArray(pFlatObjectRecords, dfblk_rmb_flatobject_t, Header.NumFlatObjectRecords);

	/* Attempt to read the data all at once */
  Result = fread (pFlatObjectRecords, sizeof(dfblk_rmb_flatobject_t), Header.NumFlatObjectRecords, pFileHandle);

  if (Result != Header.NumFlatObjectRecords) {
    SET_EXT_ERROR2(ERR_READ, "Failed to read RMB Flat Object Data from file!");
    return (FALSE);
   }

	/* Do optional statistics on success */
  #ifdef DOSTATS
    int LoopCounter;

    for (LoopCounter = 0; LoopCounter < Header.NumFlatObjectRecords; LoopCounter++) {
      DO_STAT_VAR(RMBSec2U1, pFlatObjectRecords[LoopCounter].Unknown1)
      DO_STAT_VAR(RMBSec2U2, pFlatObjectRecords[LoopCounter].Unknown2)
      DO_STAT_VAR(RMBSec2U3, pFlatObjectRecords[LoopCounter].Unknown3)
     }
  #endif

  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFRMBRecord::ReadFlatObjects()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFRMBRecord::ReadSection2()"
/*===========================================================================
 *
 * Class CDFRMBRecord Method - boolean ReadSection3 (pFileHandle);
 *
 * Attempts to read the record section3 data from the current position in the 
 * file stream. Returns FALSE on any error. Protected class method.
 *
 *=========================================================================*/
boolean CDFRMBRecord::ReadSection3 (FILE* pFileHandle) {
  int Result;

	/* Ignore if nothing to do */
  if (Header.NumSection3Records == 0) return (TRUE);

	/* Allocate pointer array */
  CreatePointerArray(pSection3Records, dfblk_rmb_section3_t, Header.NumSection3Records);

	/* Attempt to read the data all at once */
  Result = fread (pSection3Records, sizeof(dfblk_rmb_section3_t), Header.NumSection3Records, pFileHandle);

  if (Result != Header.NumSection3Records) {
    SET_EXT_ERROR2(ERR_READ, "Failed to read RMB Section3 Data from file!");
    return (FALSE);
   }

	/* Do optional statistics on success */
  #ifdef DOSTATS
    int LoopCounter;
    
    for (LoopCounter = 0; LoopCounter < Header.NumSection3Records; LoopCounter++) {
      DO_STAT_VAR(RMBSec3U1, pSection3Records[LoopCounter].Unknown1) 
      DO_STAT_VAR(RMBSec3U2, pSection3Records[LoopCounter].Unknown2) 
      DO_STAT_VAR(RMBSec3U3, pSection3Records[LoopCounter].Unknown3) 

      if (pSection3Records[LoopCounter].Unknown1 != 0) {
        SystemLog.Printf ("\tSec3U1: 0x%02X", pSection3Records[LoopCounter].Unknown1);
       }

     if (pSection3Records[LoopCounter].Unknown3 != 0) {
        SystemLog.Printf ("\tSec3U3: \t\t0x%02X", pSection3Records[LoopCounter].Unknown3);
       }
     }
  #endif

  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFRMBRecord::ReadSection3()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFRMBRecord::ReadPeople()"
/*===========================================================================
 *
 * Class CDFRMBRecord Method - boolean ReadPeople (pFileHandle);
 *
 * Attempts to read the record section4 data from the current position in the 
 * file stream. Returns FALSE on any error. Protected class method.
 *
 *=========================================================================*/
boolean CDFRMBRecord::ReadPeople (FILE* pFileHandle) {
  int Result;

	/* Ignore if nothing to do */
  if (Header.NumPeopleRecords == 0) return (TRUE);

	/* Allocate pointer array */
  CreatePointerArray(pPeopleRecords, dfblk_rmb_people_t, Header.NumPeopleRecords);

	/* Attempt to read the data all at once */
  Result = fread (pPeopleRecords, sizeof(dfblk_rmb_people_t), Header.NumPeopleRecords, pFileHandle);

  if (Result != Header.NumPeopleRecords) {
    SET_EXT_ERROR2(ERR_READ, "Failed to read RMB People Data from file!");
    return (FALSE);
   }

	/* Do optional statistics on success */
  #ifdef DOSTATS
    int LoopCounter;

    for (LoopCounter = 0; LoopCounter < Header.NumPeopleRecords; LoopCounter++) {
      DO_STAT_VAR(RMBSec4U1, pPeopleRecords[LoopCounter].Unknown1)
      DO_STAT_VAR(RMBSec4U2, pPeopleRecords[LoopCounter].Unknown2)
      DO_STAT_VAR(RMBSec4U3, pPeopleRecords[LoopCounter].Unknown3)
     }
  #endif

  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFRMBRecord::ReadPeople()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFRMBRecord::ReadDoors()"
/*===========================================================================
 *
 * Class CDFRMBRecord Method - boolean ReadDoors (pFileHandle);
 *
 * Attempts to read the record Door data from the current position in the 
 * file stream. Returns FALSE on any error. Protected class method.
 *
 *=========================================================================*/
boolean CDFRMBRecord::ReadDoors (FILE* pFileHandle) {
  int Result;

	/* Ignore if nothing to do */
  if (Header.NumDoorRecords == 0) return (TRUE);

	/* Allocate pointer array */
  CreatePointerArray(pDoorRecords, dfblk_rmb_door_t, Header.NumDoorRecords);

	/* Attempt to read the data all at once */
  Result = fread (pDoorRecords, sizeof(dfblk_rmb_door_t), Header.NumDoorRecords, pFileHandle);

  if (Result != Header.NumDoorRecords) {
    SET_EXT_ERROR2(ERR_READ, "Failed to read RMB Door Data from file!");
    return (FALSE);
   }

	/* Do optional statistics on success */
  #ifdef DOSTATS
    int LoopCounter;
    
    for (LoopCounter = 0; LoopCounter < Header.NumDoorRecords; LoopCounter++) { 
      DO_STAT_VAR(RMBSec5U1, pDoorRecords[LoopCounter].Unknown1) 
      DO_STAT_VAR(RMBSec5U2, pDoorRecords[LoopCounter].Unknown2) 
      DO_STAT_VAR(RMBSec5U3, pDoorRecords[LoopCounter].Unknown3) 
      DO_STAT_VAR(RMBSec5U4, pDoorRecords[LoopCounter].Unknown4) 
     }
  #endif

  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFRMBRecord::ReadDoors()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFRMBFile::CDFRMBFile()"
/*===========================================================================
 *
 * Class CDFRMBFile Constructor
 *
 *=========================================================================*/
CDFRMBFile::CDFRMBFile() {

	/* Initialize array pointers */
  pInsideRecords = NULL;
  pOutsideRecords = NULL;
  p3DObjects = NULL;
  pFlatObjects = NULL;
  
  RecordOffset = 0;
  RecordSize = 0;

	/* Initialize fixed length data */
  memset(&FLDData, 0, sizeof(dfblk_rmbfld_t));
 }
/*===========================================================================
 *		End of Class CDFRMBFile Constructor	
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFRMBFile::Destroy()"
/*===========================================================================
 *
 * Class CDFRMBFile Destructor
 *
 *=========================================================================*/
void CDFRMBFile::Destroy (void) {

	/* Unallocate memory */
  DestroyPointerArray(pOutsideRecords);
  DestroyPointerArray(pInsideRecords);
  DestroyPointerArray(p3DObjects);
  DestroyPointerArray(pFlatObjects);

  RecordOffset = 0;
  RecordSize = 0;

	/* Initialize fixed length data */
  memset(&FLDData, 0, sizeof(dfblk_rmbfld_t));
 }
/*===========================================================================
 *		End of Class CDFRMBFile Destructor
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFRMBFile::CreateBlockRecords()"
/*===========================================================================
 *
 * Class CDFRMBFile Method - void CreateBlockRecords (void);
 *
 * Allocates the block records. A virtual and protected class method.
 *
 *=========================================================================*/
void CDFRMBFile::CreateBlockRecords (void) {
  CreatePointerArray(pOutsideRecords, CDFRMBRecord, FLDData.NumBlocks);
  CreatePointerArray(pInsideRecords,  CDFRMBRecord, FLDData.NumBlocks);
 }
/*===========================================================================
 *		End of Class Method CDFRMBFile::CreateBlockRecords()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFRMBFile::Read()"
/*===========================================================================
 *
 * Class CDFRMBFile Method - boolean Read (pFileHandle, RecordSize);
 *
 * Attempts to read the RMB file from the current position in the file stream.
 * Returns FALSE on any error.
 *
 *=========================================================================*/
boolean CDFRMBFile::Read (FILE* pFileHandle, const long Size) {
  boolean Result;

	/* Ensure valid input */
  if (pFileHandle == NULL) {
    SET_EXT_ERROR2(ERR_NULL, "Received invalid NULL file handle!");
    return (FALSE);
   }

	/* Delete any current information */
  Destroy();

	/* Save record information */
  RecordOffset = ftell(pFileHandle);
  RecordSize = Size;

	/* Attempt to read in all the record data */
  Result = ReadFLD(pFileHandle);
  if (Result) Result = ReadBlocks (pFileHandle);
  if (Result) Result = Read3DObjects (pFileHandle);
  if (Result) Result = ReadFlatObjects (pFileHandle);

  	/* Do optional statistics */
  #ifdef DOSTATS 
    long RecSize;
    long FilePos;
    FilePos = ftell(pFileHandle);
    RecSize = FilePos - RecordOffset;
    if (RecSize != RecordSize) SystemLog.Printf ("Did not properly read RMB file, %ld byte error!", RecordSize - RecSize);
    DO_STAT_REC2(RMBNumRecords1, FLDData.NumSubRecords1);
    DO_STAT_REC2(RMBNumRecords2, FLDData.NumSubRecords2);
    DO_STAT_REC2(RMBNumRecords3, FLDData.NumSubRecords3);
  #endif

  return (Result);
 }
/*===========================================================================
 *		End of Class Method CDFRMBFile::Read()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFRMBFile::ReadFLD()"
/*===========================================================================
 *
 * Class CDFRMBFile Method - boolean ReadFLD (pFileHandle);
 *
 * Attempts to read the RMB file FLD data from the current position in the
 * file stream.  Returns FALSE on any error. Protected class method.
 *
 *=========================================================================*/
boolean CDFRMBFile::ReadFLD (FILE* pFileHandle) {
  int Result;

	/* Read in FLD all at once */
  Result = fread (&FLDData, 1, sizeof(dfblk_rmbfld_t), pFileHandle);

  if (Result != sizeof(dfblk_rmbfld_t)) {
    SET_EXT_ERROR2(ERR_READ, "Failed to read Fixed Length Data!");
    return (FALSE);
   }

  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFRMBFile::ReadFLD()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFRMBFile::ReadBlocks()"
/*===========================================================================
 *
 * Class CDFRMBFile Method - boolean ReadRecords1 (pFileHandle);
 *
 * Attempts to read the RMB subrecord1 data from the current position in the
 * file stream.  Returns FALSE on any error. Protected class method.
 *
 *=========================================================================*/
boolean CDFRMBFile::ReadBlocks (FILE* pFileHandle) {
  boolean Result;
  int     LoopCounter;
  long    RecOffset;
  long    ExtraBytes;

	/* Initialize record arrays if required */
  if (FLDData.NumBlocks == 0) return (TRUE);
  CreateBlockRecords();

	/* Read all subrecords */
  for (LoopCounter = 0; LoopCounter < FLDData.NumBlocks; LoopCounter++) {
    RecOffset = ftell(pFileHandle);

		/* Attempt to read in the first record set */
    Result = pOutsideRecords[LoopCounter].Read(pFileHandle);
    if (!Result) return (FALSE);

	    /* Attempt to read in the second record set */
    Result = pInsideRecords[LoopCounter].Read(pFileHandle);
    if (!Result) return (FALSE);

		/* Compute amount of extra bytes remaining */
    ExtraBytes = ftell(pFileHandle) - RecOffset - FLDData.BlockSizes[LoopCounter].RecordSize;

		/* Special case of 1 extra byte at end of record */
    if (ExtraBytes == -1)
      pInsideRecords[LoopCounter].SetExtraByte(fgetc(pFileHandle));
    else if (ExtraBytes != 0)
      SystemLog.Printf ("Failed to read correct record size, %d spare bytes!", LoopCounter, ExtraBytes); 
     
    		/* Jump to the proper record offset for the next data */
    fseek (pFileHandle, RecOffset + FLDData.BlockSizes[LoopCounter].RecordSize, SEEK_SET);
   }

  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFRMBFile::ReadBlocks()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFRMBFile::Read3DObjects()"
/*===========================================================================
 *
 * Class CDFRMBFile Method - boolean Read3DObjects (pFileHandle);
 *
 * Attempts to read the RMB 3D Object data from the current position in the
 * file stream.  Returns FALSE on any error. Protected class method.
 *
 *=========================================================================*/
boolean CDFRMBFile::Read3DObjects (FILE* pFileHandle) {
  boolean Result;

	/* Initialize record array if required */
  if (FLDData.Num3DObjects == 0) return (TRUE);
  CreatePointerArray(p3DObjects, dfblk_rmb_3dobject_t, FLDData.Num3DObjects);

	/* Read all subrecords at once */
  Result = fread (p3DObjects, sizeof(dfblk_rmb_3dobject_t), FLDData.Num3DObjects, pFileHandle);
  
  if (Result != FLDData.Num3DObjects) {
    SET_EXT_ERROR2(ERR_READ, "Failed to read 3D Object data from RMB file!");
    return (FALSE);
   }
  
  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFRMBFile::Read3DObjects()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFRMBFile::ReadFlatObjects()"
/*===========================================================================
 *
 * Class CDFRMBFile Method - boolean ReadFlatObjects (pFileHandle);
 *
 * Attempts to read the RMB Flat Object data from the current position in the
 * file stream.  Returns FALSE on any error. Protected class method.
 *
 *=========================================================================*/
boolean CDFRMBFile::ReadFlatObjects (FILE* pFileHandle) {
  boolean Result;

	/* Initialize record array if required */
  if (FLDData.NumFlatObjects == 0) return (TRUE);
  CreatePointerArray(pFlatObjects, dfblk_rmb_flatobject_t, FLDData.NumFlatObjects);

	/* Read all subrecords at once */
  Result = fread (pFlatObjects, sizeof(dfblk_rmb_flatobject_t), FLDData.NumFlatObjects, pFileHandle);
  
  if (Result != FLDData.NumFlatObjects) {
    SET_EXT_ERROR2(ERR_READ, "Failed to read flat object data from RMB file!");
    return (FALSE);
   }
  
  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFRMBFile::ReadRecords3()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFBlocksFile::CDFBlocksFile()"
/*===========================================================================
 *
 * Class CDFBlocksFile Constructor
 *
 *=========================================================================*/
CDFBlocksFile::CDFBlocksFile() {
  int LoopCounter;

	/* Allocate records array */
  CreatePointerArray(pRecords, CDFRMBFile*, DFBLK_MAX_RECORDS);

	/* Initialize array */
  for (LoopCounter = 0; LoopCounter < DFBLK_MAX_RECORDS; LoopCounter++) 
    pRecords[LoopCounter] = NULL;

  SetFilename("arena2\\blocks.bsa");
 }
/*===========================================================================
 *		End of Class CDFBlocksFile Constructor
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFBlocksFile::~CDFBlocksFile()"
/*===========================================================================
 *
 * Class CDFBlocksFile Destructor
 *
 *=========================================================================*/
CDFBlocksFile::~CDFBlocksFile() {
  Destroy();

	/* Unallocate records array */
  DestroyPointerArray(pRecords);
 }
/*===========================================================================
 *		End of Class CDFBlocksFile Destructor
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFBlocksFile::Destroy()"
/*===========================================================================
 *
 * Class CDFBlocksFile Destructor
 *
 *=========================================================================*/
void CDFBlocksFile::Destroy (void) {
  int LoopCounter;

	/* Delete allocated records array */
  for (LoopCounter = 0; LoopCounter < DFBLK_MAX_RECORDS; LoopCounter++) {
    DestroyPointer(pRecords[LoopCounter]);
   }

	/* Call base class method */
  CDFBSAFile::Destroy();
 }
/*===========================================================================
 *		End of Class CDFBlocksFile Destructor
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFBlocksFile::CreateRMBFile()"
/*===========================================================================
 *
 * Class CDFBlocksFile Method - void CreateRMBFile (Index);
 *
 * Attempts to create a new RMBFile object in the given array index.  Protected
 * class method.
 *
 *=========================================================================*/
void CDFBlocksFile::CreateRMBFile (const int Index) {

	/* Unallocate if already allocated */
  DestroyPointer(pRecords[Index]);

	/* Create new pointer */
  CreatePointer(pRecords[Index], CDFRMBFile);
 }
/*===========================================================================
 *		End of Class Method CDFBlocksFile::CreateRMBFile()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFBlocksFile::GetRMBFile()"
/*===========================================================================
 *
 * Class CDFBlocksFile Method - CDFRMBFile* GetRMBFile (pFilename);
 *
 * Attempts to retrieve the specified RMBFile as given by its filename.
 * Will attempt to load it if required.  Returns NULL on any error.
 *
 *=========================================================================*/
CDFRMBFile* CDFBlocksFile::GetRMBFile (const char* pFilename) {
  CDFRMBFile* pFile;
  boolean     HadToOpenFile = FALSE;
  int	      Index;

	/* Open file to read directory if required */
  if (!DirectoryRead && !IsOpen()) {
    if (!Open()) return (NULL);
    HadToOpenFile = TRUE;
   }

	/* Attempt to find record index */
  Index = FindRecordIndex(pFilename);

	/* Retrieve/load required record */
  pFile = GetRMBFile(Index);

  if (HadToOpenFile) Close();
  return (pFile);
 }
/*===========================================================================
 *		End of Class Method CDFBlocksFile::GetRMBFile()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFBlocksFile::GetRMBFile"
/*===========================================================================
 *
 * Class CDFBlocksFile Method - CDFRMBFile* GetRMBFile (Index);
 *
 * Attempts to retrieve the specified RMBFile as given by its record index.
 * Will attempt to load it if required.  Returns NULL on any error.
 *
 *=========================================================================*/
CDFRMBFile* CDFBlocksFile::GetRMBFile (const int Index) {
  boolean Result;
  boolean HadToOpenFile = FALSE;

	/* Open file to read directory if required */
  if (!DirectoryRead && !IsOpen()) {
    if (!Open()) return (NULL);
    HadToOpenFile = TRUE;
   }

	/* Check record index to ensure its valid */
  if (!IsValidIndex(Index)) {
    SET_EXT_ERROR3(ERR_INDEX, "Invalid Blocks.BSA record index %d received!", Index);
    if (HadToOpenFile) Close();
    return (NULL);
   }

	/* Does the object already exist? */
  if (pRecords[Index] != NULL) {
    if (HadToOpenFile) Close();
    return (pRecords[Index]);
   }

	/* Open file to read object if required */
  if (!IsOpen()) {
    if (!Open()) return (NULL);
    HadToOpenFile = TRUE;
   }

	/* Attempt to load object */
  Result = ReadRMB(Index);
  if (HadToOpenFile) Close();

  if (!Result) { 
    DestroyPointer(pRecords[Index]);
    return (NULL);
   }

  return (pRecords[Index]);
 }
/*===========================================================================
 *		End of Class Method CDFBlocksFile::GetRMBFile()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFBlocksFile::GetRMBFile()"
/*===========================================================================
 *
 * Class CDFBlocksFile Method - boolean GetRMBFile (FileIndex, CharIndex, Number);
 *
 * Attempts to input a RMBFile into the given array object as specified by
 * the input indices.  Returns FALSE on any error.
 *
 *=========================================================================*/
CDFRMBFile* CDFBlocksFile::GetRMBFile (const int FileIndex, const int CharIndex, const int Number) {
  char    Filename[15];
  char    FileChar[3];
  int     Index;

	/* Ensure valid input */
  if (FileIndex < 0 || FileIndex >= DFBLK_MAX_FILEINDEX) {
    SET_EXT_ERROR3(ERR_INDEX, "Invalid file index %d received!", FileIndex);
    return (FALSE);
   }

	/* Ensure valid input */
  if (Number < 0 || Number >= DFBLK_MAX_FILENUMBER) {
    SET_EXT_ERROR3(ERR_INDEX, "Invalid file number %d received!", Number);
    return (FALSE);
   }

	/* Fill in the file character string */
  if (CharIndex < 0x0F)
    strcpy(FileChar, "AA");
  else if (CharIndex <= 0x1F)
    strcpy(FileChar, "DA");
  else if (CharIndex <= 0x2F)
    strcpy(FileChar, "AL");
  else if (CharIndex <= 0x3F)
    strcpy(FileChar, "DL");
  else if (CharIndex <= 0x4F)
    strcpy(FileChar, "AM");
  else if (CharIndex <= 0x5F)
    strcpy(FileChar, "DM");
  else if (CharIndex <= 0x6F)
    strcpy(FileChar, "AS");
  else if (CharIndex <= 0x7F)
    strcpy(FileChar, "DS");
  else if (CharIndex <= 0x8F)
    strcpy(FileChar, "AA");
  else if (CharIndex <= 0x9F) 
    strcpy(FileChar, "DA");
  else {
    SET_EXT_ERROR3(ERR_INDEX, "Invalid file char %d received!", FileChar);
    return (FALSE);
   }

  SystemLog.Printf ("CDFBlocksFile::ReadRMB(%d, 0x%02X, %d)", FileIndex, CharIndex, Number);
    
	/* Create the filename */
  sprintf (Filename, "%4s%2s%02d.rmb", GetDFRMBFilename(FileIndex), FileChar, Number);

	/* Find the record index of the specified file */
  Index = FindRecordIndex(Filename);

	/* Ensure record is loaded */
  return (GetRMBFile(Index));
 }
/*===========================================================================
 *		End of Class Method CDFBlocksFile::GetRMBFile()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFBlocksFile::IsRMBFile()"
/*===========================================================================
 *
 * Class CDFBlocksFile Method - boolean IsRMBFile (Index);
 *
 * Returns TRUE if the given BSA directory entry is a RMB type file.
 *
 *=========================================================================*/
boolean CDFBlocksFile::IsRMBFile (const int Index) {

	/* Ensure a valid input */
  if (!IsValidIndex(Index)) return (FALSE);

	/* Ensure a valid filename type */
  if (!CompareExtension(pDirectory[Index]->Filename, "RMB")) return (FALSE);
  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFBlocksFile::IsRMBFile()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFBlocksFile::IsRDIFile()"
/*===========================================================================
 *
 * Class CDFBlocksFile Method - boolean IsRDIFile (Index);
 *
 * Returns TRUE if the given BSA directory entry is a RDI type file.
 *
 *=========================================================================*/
boolean CDFBlocksFile::IsRDIFile (const int Index) {

	/* Ensure a valid input */
  if (!IsValidIndex(Index)) return (FALSE);

	/* Ensure a valid filename type */
  if (!CompareExtension(pDirectory[Index]->Filename, "RDI")) return (FALSE);
  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFBlocksFile::IsRDIFile()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFBlocksFile::IsRDBFile()"
/*===========================================================================
 *
 * Class CDFBlocksFile Method - boolean IsRDBFile (Index);
 *
 * Returns TRUE if the given BSA directory entry is a RDB type file.
 *
 *=========================================================================*/
boolean CDFBlocksFile::IsRDBFile (const int Index) {

	/* Ensure a valid input */
  if (!IsValidIndex(Index)) return (FALSE);

	/* Ensure a valid filename type */
  if (!CompareExtension(pDirectory[Index]->Filename, "RDB")) return (FALSE);
  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFBlocksFile::IsRDBFile()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFBlocksFile::ReadRMB()"
/*===========================================================================
 *
 * Class CDFBlocksFile Method - boolean ReadRMB (pFileName);
 *
 * Attempts to input a RMBFile into the given array object.  Returns FALSE
 * on any error.
 *
 *=========================================================================*/
boolean CDFBlocksFile::ReadRMB (const char* pFilename) {
  boolean Result;
  int     Index;

	/* Find the record index of the specified file */
  Index = FindRecordIndex(pFilename);
  Result = ReadRMB(Index);
  return (Result);
 }
/*===========================================================================
 *		End of Class Method CDFBlocksFile::ReadRMB()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFBlocksFile::ReadRMB()"
/*===========================================================================
 *
 * Class CDFBlocksFile Method - boolean ReadRMB (Index);
 *
 * Attempts to input a RMBFile into the given array object.  Returns FALSE
 * on any error.
 *
 *=========================================================================*/
boolean CDFBlocksFile::ReadRMB (const int Index) {
  boolean HadToOpenFile = FALSE;
  boolean Result;

	/* Ensure a valid input */
  if (!IsRMBFile(Index)) {
    SET_EXT_ERROR3(ERR_NULL, "BSA directory %d is not a valid RMB type!", Index);
    return (FALSE);
   }

	/* Attempt to open BSA file if required */
  if (!IsOpen()) {
    if (!Open()) return (FALSE);
    HadToOpenFile = TRUE;
   }

	/* Initialize array statistics if required */
  #ifdef DOSTATS
    static boolean DoStatsInit = FALSE;

    if (!DoStatsInit) {  
      DoStatsInit = TRUE;
      INIT_STAT_ARR(RMBHeaderUnknowns, 6)
     }
  #endif

	/* Allocate the array record */
  CreateRMBFile(Index);

	/* Move to the appropiate position in file */
  fseek (pFileHandle, pDirectory[Index]->RecordOffset, SEEK_SET);
	
	/* Attempt to read the RMB file */
  Result = pRecords[Index]->Read(pFileHandle, pDirectory[Index]->RecordSize);

  	/* Close file if required */
  if (HadToOpenFile) Close();
  if (!Result) { DestroyPointer(pRecords[Index]); }
  return (Result);
 }
/*===========================================================================
 *		End of Class Method CDFBlocksFile::ReadRMB()
 *=========================================================================*/


#ifdef DOSTATS

#undef  __FUNC__
#define __FUNC__ "DFBlocksOutputStats()"
/*===========================================================================
 *
 * Function - void DFBlocksOutputStats (void);
 *
 * Output statistics if statistic mode is defined.
 *
 *=========================================================================*/
void DFBlocksOutputStats (void) {
  OUT_STAT_REC(RMBNumRecords1);
  OUT_STAT_REC(RMBNumRecords2);
  OUT_STAT_REC(RMBNumRecords3);
  OUT_STAT_REC(RMBNumSec1Rec)
  OUT_STAT_REC(RMBNumSec2Rec)
  OUT_STAT_REC(RMBNumSec3Rec)
  OUT_STAT_REC(RMBNumSec4Rec)
  OUT_STAT_REC(RMBNumSec5Rec)
  OUT_STAT_ARR(RMBHeaderUnknowns, 6)
  OUT_STAT_VAR(RMBSec1U1)
  OUT_STAT_VAR(RMBSec1U2)
  OUT_STAT_VAR(RMBSec1U3)
  OUT_STAT_VAR(RMBSec1U4)
  OUT_STAT_VAR(RMBSec1U5)
  OUT_STAT_VAR(RMBSec1U6)
  OUT_STAT_VAR(RMBSec1U7)
  OUT_STAT_VAR(RMBSec1U8)
  OUT_STAT_VAR(RMBSec1X1)
  OUT_STAT_VAR(RMBSec1Y1)
  OUT_STAT_VAR(RMBSec1Z1)
  OUT_STAT_VAR(RMBSec1X2)
  OUT_STAT_VAR(RMBSec1Y2)
  OUT_STAT_VAR(RMBSec1Z2)
  OUT_STAT_VAR(RMBSec2U1)
  OUT_STAT_VAR(RMBSec2U2)
  OUT_STAT_VAR(RMBSec2U3)
  OUT_STAT_VAR(RMBSec3U1)
  OUT_STAT_VAR(RMBSec3U2)
  OUT_STAT_VAR(RMBSec3U3)
  OUT_STAT_VAR(RMBSec4U1)
  OUT_STAT_VAR(RMBSec4U2)
  OUT_STAT_VAR(RMBSec4U3)
  OUT_STAT_VAR(RMBSec5U1)
  OUT_STAT_VAR(RMBSec5U2)
  OUT_STAT_VAR(RMBSec5U3)
  OUT_STAT_VAR(RMBSec5U4)
 }
/*===========================================================================
 *		End of Function DFBlocksOutputStats()
 *=========================================================================*/
#endif