/*===========================================================================
 *
 * DFMaps.CPP, Dave Humphrey (uesp@m0use.net), 14 November 2000
 *
 *=========================================================================*/

//#define DOSTATS

	/* Include Files */
#include "dfmaps.h"
#include "dfcommon.h"


	/* Initialize statistics if required */
DEF_STAT_VAR(PItemUnknown1)
DEF_STAT_VAR(DItemUnknown1)
DEF_STAT_VAR(PItemUnknown2)
DEF_STAT_VAR(DItemUnknown2)
DEF_STAT_VAR(PItemUnknown3)
DEF_STAT_VAR(DItemUnknown3)
DEF_STAT_VAR(PItemUnknown4)
DEF_STAT_VAR(DItemUnknown4)
DEF_STAT_VAR(PItemUnknown5)
DEF_STAT_VAR(DItemUnknown5)
DEF_STAT_VAR(PItemUnknown6)
DEF_STAT_VAR(DItemUnknown6)
DEF_STAT_VAR(DItemUnknown7)
DEF_STAT_VAR(DItemUnknown8)
DEF_STAT_VAR(DItemUnknown9)
DEF_STAT_VAR(DItemUnknown10)
DEF_STAT_REC(PItemNumPreRec)
DEF_STAT_REC(DItemNumPreRec)
DEF_STAT_ARR(PItemPreRec, 6)
DEF_STAT_ARR(DItemPreRec, 6)
DEF_STAT_VAR(PItemXPos)
DEF_STAT_VAR(PItemYPos)
DEF_STAT_VAR(DItemXPos)
DEF_STAT_VAR(DItemYPos)
DEF_STAT_REC(PItemNumPostRec)
DEF_STAT_REC(DItemNumPostRec)
DEF_STAT_REC(NumDunPostRec)
DEF_STAT_REC(TotalDungeons)
DEF_STAT_REC(TotalLocations)
DEF_STAT_ARR(PItemPostNulls, 16)
DEF_STAT_VAR(PItemHouseName)
DEF_STAT_VAR(PItemHouseType)
DEF_STAT_VAR(PItemPostU1)
DEF_STAT_VAR(PItemPostU2)
DEF_STAT_VAR(PItemPostU3)
DEF_STAT_ARR(PItemNulls, 380)
DEF_STAT_ARR(DItemPostUnknowns, 4)
DEF_STAT_CNT(DItemPostUn2)
DEF_STAT_CNT(DItemPostUn3)
DEF_STAT_CNT(DItemPostUn4)
DEF_STAT_CNT(PItemBlock1)
DEF_STAT_CNT(PItemBlock2)
DEF_STAT_CNT(PItemBlock3)


#undef  __FUNC__
#define __FUNC__ "CDFMapItemRecord::CDFMapItemRecord()"
/*===========================================================================
 *
 * Class CDFMapItemRecord Constructor
 *
 *=========================================================================*/
CDFMapItemRecord::CDFMapItemRecord() {
  NumPreRecords = 0;
  RecordOffset = 0;
  RecordSize = 0;
  Dungeon = FALSE;
  pPreRecords = NULL;
 }
/*===========================================================================
 *		End of Class CDFMapItemRecord Constructor
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFMapItemRecord::Destroy()"
/*===========================================================================
 *
 * Class CDFMapItemRecord Destructor
 *
 *=========================================================================*/
void CDFMapItemRecord::Destroy (void) {

	/* Unallocate memory */
  DestroyPointerArray(pPreRecords);
  RecordSize = 0;
  RecordOffset = 0;
  NumPreRecords = 0;
  Dungeon = FALSE;
 }
/*===========================================================================
 *		End of Class CDFMapItemRecord Constructor
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFMapItemRecord::Clear()"
/*===========================================================================
 *
 * Class CDFMapItemRecord Method - void Clear (void);
 *
 * Removes unwanted data.
 *
 *=========================================================================*/
void CDFMapItemRecord::Clear (void) {

	/* Unallocate memory */
  DestroyPointerArray(pPreRecords);
  RecordOffset = 0;
  NumPreRecords = 0;
 }
/*===========================================================================
 *		End of Class Method CDFMapItemRecord::Clear()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFMapItemRecord::CreatePreRecords()"
/*===========================================================================
 *
 * Class CDFMapItemRecord Method - void CreatePreRecords (void);
 *
 * Creates the pre-record array.  Protected class method.
 *
 *=========================================================================*/
void CDFMapItemRecord::CreatePreRecords (void) {
 
	/* Ensure there is something to create */
  pPreRecords = NULL;
  if (NumPreRecords == 0) return;

	/* Allocate the pointer array */
  CreatePointerArray(pPreRecords, dfmap_pitem_prerecord_t, NumPreRecords);	
 }
/*===========================================================================
 *		End of Class Method CDFMapItemRecord::CreatePreRecords()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFMapItemRecord::Dump()"
/*===========================================================================
 *
 * Class CDFMapItemRecord Method - void Dump (pFileHandle, DumpFlags);
 *
 * Output object info to a file stream.
 *
 *=========================================================================*/
void CDFMapItemRecord::Dump (FILE* pFileHandle, const dfmap_dumpflags_t DumpFlags) {
  char TypeChar = Dungeon ? 'D' : 'P';

	/* Ensure we wish to output dump information */
  if ((DumpFlags & DFMAP_DUMP_ITEMINFO) == 0) return;

	/* Ensure valid input */
  if (pFileHandle == NULL) { 
    SET_EXT_ERROR2(ERR_NULL, "Invalid NULL file handle received!");
    return;
   }

	/* Output basic information */
  fprintf (pFileHandle, "\t\t\t%cI: Name='%s', Position=(%ld, %ld), ID=0x%04X,\n", TypeChar, Header.Name, Header.XPosition, Header.YPosition, Header.LocationID&0xFFFF);
  fprintf (pFileHandle, "\t\t\t%cI: Offset=0x%08lX, PreRecords=%d\n", TypeChar, RecordOffset, NumPreRecords);
  
	/* Output unknown information if required */
  if (DumpFlags & DFMAP_DUMP_PITEMS_UNKNOWNS) {
    fprintf (pFileHandle, "\t\t\t%cI: Unknowns = 0x%08lX 0x%08lX 0x%08lX 0x%08lX 0x%04lX 0x%08lX\n", TypeChar, Header.Unknown1, Header.Unknown2,  Header.Unknown3, Header.Unknown4&0xFFFF, Header.Unknown5);
   }
 }
/*===========================================================================
 *		End of Class Method CDFMapItemRecord::Dump()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFMapItemRecord::Read()"
/*===========================================================================
 *
 * Class CDFMapItemRecord Method - boolean Read (FILE* pFileHandle);
 *
 * Attempts to read in the data from the current position in the file
 * stream.  Returns FALSE on any error.
 *
 *=========================================================================*/
boolean CDFMapItemRecord::Read (FILE* pFileHandle) {
  boolean Result;

	/* Ensure valid input */
  if (pFileHandle == NULL) { 
    SET_EXT_ERROR2(ERR_NULL, "Invalid NULL file handle received!");
    return (FALSE);
   }

	/* Delete the current contents */
  Clear();

	/* Save the record offset */
  RecordOffset = ftell(pFileHandle);

  if (RecordOffset < 0) { 
    SET_EXT_ERROR2(ERR_READ, "Failed to retrieve PItem record offset!");
    return (FALSE);
   }

	/* Read in record data */
  Result = ReadPreRecords(pFileHandle);
  if (Result) Result = ReadHeader(pFileHandle);
  if (Result) Result = ReadPostHeader(pFileHandle);
  if (Result) Result = ReadPostRecords(pFileHandle);
  if (Result) Result = ReadExtra(pFileHandle);

  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFMapItemRecord::Read()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFMapItemRecord::ReadHeader()"
/*===========================================================================
 *
 * Class CDFMapItemRecord Method - boolean ReadHeader (FILE* pFileHandle);
 *
 * Attempts to read in the header data from the current position in 
 * the file stream.  Returns FALSE on any error. Protected class method.
 *
 *=========================================================================*/
boolean CDFMapItemRecord::ReadHeader (FILE* pFileHandle) {
  int Result;

	/* Read in the header data all at once */
  Result = fread (&Header, 1, sizeof(dfmap_pitem_header_t), pFileHandle);

  if (Result != sizeof(dfmap_pitem_header_t)) {
    SET_EXT_ERROR3(ERR_READ, "Error reading PItem header data (Offset 0x%08lX)!", ftell(pFileHandle));
    return (FALSE);
   }

	/* Perform statistics */
  #ifdef DFMAP_DOSTATS
    
    if (Dungeon) {
      DO_STAT_VAR(DItemXPos, Header.XPosition) 
      DO_STAT_VAR(DItemYPos, Header.YPosition) 
      DO_STAT_VAR(DItemUnknown1, Header.Unknown1)
      DO_STAT_VAR(DItemUnknown2, Header.Unknown2)
      DO_STAT_VAR(DItemUnknown3, Header.Unknown3)
      DO_STAT_VAR(DItemUnknown4, Header.Unknown4)
      DO_STAT_VAR(DItemUnknown5, Header.Unknown5)
     }
    else {
      DO_STAT_VAR(PItemXPos, Header.XPosition) 
      DO_STAT_VAR(PItemYPos, Header.YPosition) 
      DO_STAT_VAR(PItemUnknown1, Header.Unknown1)
      DO_STAT_VAR(PItemUnknown2, Header.Unknown2)
      DO_STAT_VAR(PItemUnknown3, Header.Unknown3)
      DO_STAT_VAR(PItemUnknown4, Header.Unknown4)
      DO_STAT_VAR(PItemUnknown5, Header.Unknown5)
     }
  #endif
 
  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFMapItemRecord::ReadHeader()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFMapItemRecord::ReadPostHeader()"
/*===========================================================================
 *
 * Class CDFMapItemRecord Method - boolean ReadPostHeader (FILE* pFileHandle);
 *
 * Attempts to read in the post header data from the current position in 
 * the file stream.  Returns FALSE on any error. Protected class method.
 *
 *=========================================================================*/
boolean CDFMapItemRecord::ReadPostHeader (FILE* pFileHandle) {
  int Result;

	/* Read the extra 9-bytes */
  Result = fread (Unknowns1, 1, 9, pFileHandle);

  if (Result != 9) {
    SET_EXT_ERROR3(ERR_READ, "Error reading 9 bytes of unknown post-header data (Offset 0x%08lX)!", ftell(pFileHandle));
    return (FALSE);
   }

	/* Read in the number of post records */
  clearerr(pFileHandle);
  NumPostRecords = (ushort) read_short(pFileHandle);

  if (ferror(pFileHandle) || feof(pFileHandle)) {
    SET_EXT_ERROR3(ERR_READ, "Error reading the number of post records in location (Offset 0x%08lX)!", ftell(pFileHandle));
    return (FALSE);
   }

	/* Perform statistics */
  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFMapItemRecord::ReadPostHeader()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFMapItemRecord::ReadPreRecords()"
/*===========================================================================
 *
 * Class CDFMapItemRecord Method - boolean ReadPreRecords (FILE* pFileHandle);
 *
 * Attempts to read in the pre-record data from the current position in 
 * the file stream.  Returns FALSE on any error. Protected class method.
 *
 *=========================================================================*/
boolean CDFMapItemRecord::ReadPreRecords (FILE* pFileHandle) {
  int Result;

	/* Read in the number of pre-records */
  clearerr(pFileHandle);
  NumPreRecords = read_long(pFileHandle);

	/* Ensure input was successful */
  if (ferror(pFileHandle) || feof(pFileHandle)) {
    SET_EXT_ERROR3(ERR_READ, "Error reading number of prerecords from PItem data (Offset 0x%08lX)!", ftell(pFileHandle));
    return (FALSE);
   }

  if (NumPreRecords == 0) return (TRUE);
  CreatePreRecords();

	/* Read in the prerecord data all at once */
  Result = fread (pPreRecords, sizeof(dfmap_pitem_prerecord_t), NumPreRecords, pFileHandle);

  if (Result != NumPreRecords) {
    SET_EXT_ERROR3(ERR_READ, "Error reading prerecord data from PItem record (Offset 0x%08lX)!", ftell(pFileHandle));
    return (FALSE);
   }

   	/* Perform statistics */
  #ifdef DOSTATS

    if (Dungeon) { 
      DO_STAT_REC2(DItemNumPreRec, NumPreRecords)
      int I;
    
      for (I = 0; I < NumPreRecords; I++) {
        DO_STAT_ARR(DItemPreRec, pPreRecords[I].Data, 6)
       }
     }
    else {
      DO_STAT_REC2(PItemNumPreRec, NumPreRecords)
      int I;
    
      for (I = 0; I < NumPreRecords; I++) {
        DO_STAT_ARR(PItemPreRec, pPreRecords[I].Data, 6)
       }
     }

  #endif
 
  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFMapItemRecord::ReadPreRecords()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFMapPItemRecord::CDFMapPItemRecord()"
/*===========================================================================
 *
 * Class CDFMapPItemRecord Constructor
 *
 *=========================================================================*/
CDFMapPItemRecord::CDFMapPItemRecord() {
  pPostRecords = NULL;
  NumPostRecords = 0;
  Dungeon = FALSE;
 }
/*===========================================================================
 *		End of Class CDFMapPItemRecord Constructor
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFMapPItemRecord::Destroy()"
/*===========================================================================
 *
 * Class CDFMapPItemRecord Destructor
 *
 *=========================================================================*/
void CDFMapPItemRecord::Destroy (void) {

	/* Call the base class method */
  CDFMapItemRecord::Destroy();	

	/* Unallocate memory */
  DestroyPointerArray(pPostRecords);
  NumPostRecords = 0;
 }
/*===========================================================================
 *		End of Class CDFMapPItemRecord Destructor
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFMapPItemRecord::Clear()"
/*===========================================================================
 *
 * Class CDFMapPItemRecord Method - void Clear (void);
 *
 * Removes unwanted data.
 *
 *=========================================================================*/
void CDFMapPItemRecord::Clear (void) {

	/* Call the base class method */
  CDFMapItemRecord::Clear();

	/* Unallocate memory */
  DestroyPointerArray(pPostRecords);
  NumPostRecords = 0;
 }
/*===========================================================================
 *		End of Class Method CDFMapPItemRecord::Clear()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFMapPItemRecord::CreatePostRecords()"
/*===========================================================================
 *
 * Class CDFMapPItemRecord Method - void CreatePostRecords (void);
 *
 * Creates the post-record array.  Protected class method.
 *
 *=========================================================================*/
void CDFMapPItemRecord::CreatePostRecords (void) {
 
	/* Ensure there is something to create */
  pPostRecords = NULL;
  if (NumPostRecords == 0) return;

	/* Allocate the pointer array */
  CreatePointerArray(pPostRecords, dfmap_pitem_postrecord_t, NumPostRecords);	
 }
/*===========================================================================
 *		End of Class Method CDFMapPItemRecord::CreatePostRecords()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFMapPItemRecord::Dump()"
/*===========================================================================
 *
 * Class CDFMapPItemRecord Method - void Dump (pFileHandle, DumpFlags);
 *
 * Output object info to a file stream.
 *
 *=========================================================================*/
void CDFMapPItemRecord::Dump (FILE* pFileHandle, const dfmap_dumpflags_t DumpFlags) {

	/* Ensure valid input */
  if (pFileHandle == NULL) { 
    SET_EXT_ERROR2(ERR_NULL, "Invalid NULL file handle received!");
    return;
   }

	/* Call the base class method first */
  CDFMapItemRecord::Dump(pFileHandle, DumpFlags);

	/* Ensure we wish to output dump information */
  if ((DumpFlags & DFMAP_DUMP_PITEMS) == 0) return;

 }
/*===========================================================================
 *		End of Class Method CDFMapPItemRecord::Dump()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFMapPItemRecord::ReadExtra()"
/*===========================================================================
 *
 * Class CDFMapPItemRecord Method - boolean ReadExtra (FILE* pFileHandle);
 *
 * Attempts to read in the extra data from the current position in 
 * the file stream.  Returns FALSE on any error. Protected class method.
 *
 *=========================================================================*/
boolean CDFMapPItemRecord::ReadExtra (FILE* pFileHandle) {
  int Result;

  	/* Read the footer data all at once  */
  Result = fread (&Footer, 1, sizeof(dfmap_pitem_footer_t), pFileHandle);

  if (Result != sizeof(dfmap_pitem_footer_t)) {
    SET_EXT_ERROR3(ERR_READ, "Error reading footer data location record (Offset 0x%08lX)!", ftell(pFileHandle));
    return (FALSE);
   }

	/* Perform statistics */
  #ifdef DOSTATS
    DO_STAT_VAR(PItemUnknown6, Unknown6)
    DO_STAT_ARR(PItemNulls, ExtraBytes, 380)

    DO_STAT_CNT(PItemBlock1, ExtraBytes[13])
    DO_STAT_CNT(PItemBlock2, ExtraBytes[77])
    DO_STAT_CNT(PItemBlock3, ExtraBytes[141])

    long FilePos = ftell(pFileHandle);

    if (FilePos != RecordOffset + RecordSize) 
      SystemLog.Printf ("\tPItem extra bytes: %ld (%ld, Dungeon = %d)!", FilePos - RecordOffset - RecordSize, Header.Unknown5, Dungeon); 
  #endif
 
  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFMapPItemRecord::ReadExtra()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFMapPItemRecord::ReadPostHeader()"
/*===========================================================================
 *
 * Class CDFMapPItemRecord Method - boolean ReadPostHeader (FILE* pFileHandle);
 *
 * Attempts to read in the post header data from the current position in 
 * the file stream.  Returns FALSE on any error. Protected class method.
 *
 *=========================================================================*/
boolean CDFMapPItemRecord::ReadPostHeader (FILE* pFileHandle) {
  int Result;

	/* Call the base class method first */
  Result = CDFMapItemRecord::ReadPostHeader(pFileHandle);
  if (!Result) return (FALSE);

	/* Read the extra 5-bytes */
  Result = fread (Unknowns2, 1, 5, pFileHandle);

  if (Result != 5) {
    SET_EXT_ERROR3(ERR_READ, "Error reading 5 bytes of unknown post-header data (Offset 0x%08lX)!", ftell(pFileHandle));
    return (FALSE);
   }

	/* Perform statistics */
  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFMapPItemRecord::ReadPostHeader()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFMapPItemRecord::ReadPostRecords()"
/*===========================================================================
 *
 * Class CDFMapPItemRecord Method - boolean ReadPostRecords (FILE* pFileHandle);
 *
 * Attempts to read in the post-record data from the current position in 
 * the file stream.  Returns FALSE on any error. Protected class method.
 *
 *=========================================================================*/
boolean CDFMapPItemRecord::ReadPostRecords (FILE* pFileHandle) {
  int Result;

	/* Allocate the post record array if required */
  if (NumPostRecords == 0) return (TRUE);
  CreatePostRecords();

	/* Read in the post record data all at once */
  Result = fread (pPostRecords, sizeof(dfmap_pitem_postrecord_t), NumPostRecords, pFileHandle);

  if (Result != NumPostRecords) {
    SET_EXT_ERROR3(ERR_READ, "Error reading postrecord data from PItem record (Offset 0x%08lX)!", ftell(pFileHandle));
    return (FALSE);
   }

   	/* Perform statistics */
  #ifdef DOSTATS
    int LoopCounter;

    for (LoopCounter = 0; LoopCounter < NumPostRecords; LoopCounter++) {
      DO_STAT_VAR(PItemHouseName, pPostRecords[LoopCounter].HouseName)
      DO_STAT_VAR(PItemHouseType, pPostRecords[LoopCounter].HouseType)
      DO_STAT_VAR(PItemPostU1, pPostRecords[LoopCounter].Unknown1)
      DO_STAT_VAR(PItemPostU2, pPostRecords[LoopCounter].Unknown2)
      DO_STAT_VAR(PItemPostU3, pPostRecords[LoopCounter].Unknown3)
      DO_STAT_ARR(PItemPostNulls, pPostRecords[LoopCounter].NullValues, 16)
     }
   	
    DO_STAT_REC2(PItemNumPostRec, NumPostRecords)
  #endif
  
  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFMapPItemRecord::ReadPostRecords()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFMapDItemRecord::CDFMapDItemRecord()"
/*===========================================================================
 *
 * Class CDFMapDItemRecord Constructor
 *
 *=========================================================================*/
CDFMapDItemRecord::CDFMapDItemRecord() {
  pPostRecords = NULL;
  Dungeon = TRUE;
  NumDungeonPostRecords = 0;
  ExtraBytesSize = 0;
 }
/*===========================================================================
 *		End of Class CDFMapDItemRecord Constructor
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFMapDItemRecord::Destroy()"
/*===========================================================================
 *
 * Class CDFMapDItemRecord Destructor
 *
 *=========================================================================*/
void CDFMapDItemRecord::Destroy (void) {

	/* Call the base class method */
  CDFMapItemRecord::Destroy();	

	/* Unallocate memory */
  DestroyPointerArray(pPostRecords);
  NumDungeonPostRecords = 0;
  ExtraBytesSize = 0;
 }
/*===========================================================================
 *		End of Class CDFMapDItemRecord Destructor
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFMapDItemRecord::Clear()"
/*===========================================================================
 *
 * Class CDFMapDItemRecord Method - void Clear (void);
 *
 * Removes unwanted data.
 *
 *=========================================================================*/
void CDFMapDItemRecord::Clear (void) {

	/* Call the base class method */
  CDFMapItemRecord::Clear();

	/* Unallocate memory */
  DestroyPointerArray(pPostRecords);
  NumDungeonPostRecords = 0;
  ExtraBytesSize = 0;
 }
/*===========================================================================
 *		End of Class Method CDFMapDItemRecord::Clear()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFMapDItemRecord::CreatePostRecords()"
/*===========================================================================
 *
 * Class CDFMapDItemRecord Method - void CreatePostRecords (void);
 *
 * Creates the post-record array.  Protected class method.
 *
 *=========================================================================*/
void CDFMapDItemRecord::CreatePostRecords (void) {
 
	/* Ensure there is something to create */
  pPostRecords = NULL;
  if (NumDungeonPostRecords == 0) return;

	/* Allocate the pointer array */
  CreatePointerArray(pPostRecords, dfmap_ditem_postrecord_t, NumDungeonPostRecords);	
 }
/*===========================================================================
 *		End of Class Method CDFMapDItemRecord::CreatePostRecords()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFMapDItemRecord::Dump()"
/*===========================================================================
 *
 * Class CDFMapDItemRecord Method - void Dump (pFileHandle, DumpFlags);
 *
 * Output object info to a file stream.
 *
 *=========================================================================*/
void CDFMapDItemRecord::Dump (FILE* pFileHandle, const dfmap_dumpflags_t DumpFlags) {

	/* Ensure valid input */
  if (pFileHandle == NULL) { 
    SET_EXT_ERROR2(ERR_NULL, "Invalid NULL file handle received!");
    return;
   }

	/* Call the base class method first */
  CDFMapItemRecord::Dump(pFileHandle, DumpFlags);

	/* Ensure we wish to output dump information */
  if ((DumpFlags & DFMAP_DUMP_DITEMS) == 0) return;

 }
/*===========================================================================
 *		End of Class Method CDFMapDItemRecord::Dump()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFMapDItemRecord::ReadExtra()"
/*===========================================================================
 *
 * Class CDFMapDItemRecord Method - boolean ReadExtra (FILE* pFileHandle);
 *
 * Attempts to read in the extra data from the current position in 
 * the file stream.  Returns FALSE on any error. Protected class method.
 *
 *=========================================================================*/
boolean CDFMapDItemRecord::ReadExtra (FILE* pFileHandle) {
  int Result;

  	/* Read the extra three short numbers */
  Unknown8  = read_short(pFileHandle);
  Unknown9  = read_short(pFileHandle);
  Unknown10 = read_short(pFileHandle);
    
  if (ferror(pFileHandle) || feof(pFileHandle)) {
    SET_EXT_ERROR3(ERR_READ, "Error reading post header data (Offset 0x%08lX)!", ftell(pFileHandle));
    return (FALSE);
   }

  	/* Read the extra bytes */
  ExtraBytesSize = DFMAP_DITEM_EXTRABYTES - 8 - NumDungeonPostRecords*4;
  Result = fread (ExtraBytes, 1, ExtraBytesSize, pFileHandle);

  if (Result != ExtraBytesSize) {
    SET_EXT_ERROR3(ERR_READ, "Error reading extra bytes of location record (Offset 0x%08lX)!", ftell(pFileHandle));
    return (FALSE);
   }

	/* Perform statistics */
  #ifdef DOSTATS
    DO_STAT_VAR(DItemUnknown8, Unknown6)
    DO_STAT_VAR(DItemUnknown9, Unknown6)
    DO_STAT_VAR(DItemUnknown10, Unknown6)
    long FilePos = ftell(pFileHandle);

    if (FilePos != RecordOffset + RecordSize) {
      SystemLog.Printf ("\tDItem extra bytes: %ld (0x%08lX)!", FilePos - RecordOffset - RecordSize, RecordOffset); 
     }
  #endif
 
  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFMapDItemRecord::ReadExtra()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFMapDItemRecord::ReadPostHeader()"
/*===========================================================================
 *
 * Class CDFMapDItemRecord Method - boolean ReadPostHeader (FILE* pFileHandle);
 *
 * Attempts to read in the post header data from the current position in 
 * the file stream.  Returns FALSE on any error. Protected class method.
 *
 *=========================================================================*/
boolean CDFMapDItemRecord::ReadPostHeader (FILE* pFileHandle) {
  int Result;

	/* Call the base class method first */
  Result = CDFMapItemRecord::ReadPostHeader(pFileHandle);
  if (!Result) return (FALSE);

	/* Read the extra two longs and short number of dungeon post-records */
  Unknown6 = read_long(pFileHandle);
  Unknown7 = read_long(pFileHandle);
  NumDungeonPostRecords = (ushort) read_short(pFileHandle);
  
  if (ferror(pFileHandle) || feof(pFileHandle)) {
    SET_EXT_ERROR3(ERR_READ, "Error reading post header data (Offset 0x%08lX)!", ftell(pFileHandle));
    return (FALSE);
   }

	/* Perform statistics */
  DO_STAT_VAR(DItemUnknown6, Unknown6)
  DO_STAT_VAR(DItemUnknown7, Unknown7)
  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFMapDItemRecord::ReadPostHeader()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFMapDItemRecord::ReadPostRecords()"
/*===========================================================================
 *
 * Class CDFMapDItemRecord Method - boolean ReadPostRecords (FILE* pFileHandle);
 *
 * Attempts to read in the post-record data from the current position in 
 * the file stream.  Returns FALSE on any error. Protected class method.
 *
 *=========================================================================*/
boolean CDFMapDItemRecord::ReadPostRecords (FILE* pFileHandle) {
  int Result;

	/* Allocate the post record array if required */
  if (NumDungeonPostRecords == 0) return (TRUE);
  CreatePostRecords();

	/* Read in the post record data all at once */
  Result = fread (pPostRecords, sizeof(dfmap_ditem_postrecord_t), NumDungeonPostRecords, pFileHandle);

  if (Result != NumDungeonPostRecords) {
    SET_EXT_ERROR3(ERR_READ, "Error reading postrecord data from DItem record (Offset 0x%08lX)!", ftell(pFileHandle));
    return (FALSE);
   }

	/* Perform statistics */
  #ifdef DOSTATS
    int LoopCounter;

    for (LoopCounter = 0; LoopCounter < NumDungeonPostRecords; LoopCounter++) {
      DO_STAT_ARR(DItemPostUnknowns, pPostRecords[LoopCounter].Unknowns, 4)

      DO_STAT_CNT(DItemPostUn2, pPostRecords[LoopCounter].Unknowns[1])
      DO_STAT_CNT(DItemPostUn3, pPostRecords[LoopCounter].Unknowns[2])
      DO_STAT_CNT(DItemPostUn4, pPostRecords[LoopCounter].Unknowns[3])
     }
   	
    DO_STAT_REC2(NumDunPostRec, NumDungeonPostRecords)
  #endif

  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFMapDItemRecord::ReadPostRecords()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFMapTableRecord::CDFMapTableRecord()"
/*===========================================================================
 *
 * Class CDFMapTableRecord Constructor
 *
 *=========================================================================*/
CDFMapTableRecord::CDFMapTableRecord() {
  memset(&TableData, 0, sizeof(dfmap_table_t));
  RecordOffset = 0;
 }
/*===========================================================================
 *		End of Class CDFMapTableRecord Constructor
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFMapTableRecord::Destroy()"
/*===========================================================================
 *
 * Class CDFMapTableRecord Destructor
 *
 *=========================================================================*/
void CDFMapTableRecord::Destroy (void) {
  memset(&TableData, 0, sizeof(dfmap_table_t));
  RecordOffset = 0;
 }
/*===========================================================================
 *		End of Class CDFMapTableRecord Constructor
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFMapTableRecord::Dump()"
/*===========================================================================
 *
 * Class CDFMapTableRecord Method - void Dump (pFileHandle, DumpFlags);
 *
 * Output object info to a file stream.
 *
 *=========================================================================*/
void CDFMapTableRecord::Dump (FILE* pFileHandle, const dfmap_dumpflags_t DumpFlags) {

	/* Ensure we wish to output dump information */
  if ((DumpFlags & DFMAP_DUMP_MAPTABLE) == 0) return;

	/* Ensure valid input */
  if (pFileHandle == NULL) { 
    SET_EXT_ERROR2(ERR_NULL, "Invalid NULL file handle received!");
    return;
   }

	/* Output basic information */
  fprintf (pFileHandle, "\t\t\tMT: Position = %d, %d  (Type = %d)\n", TableData.XPosition, TableData.YPosition, TableData.LocationType);

	/* Output unknown information if required */
  if (DumpFlags & DFMAP_DUMP_MAPTABLE_UNKNOWNS) {
    fprintf (pFileHandle, "\t\t\tMT: Unknowns = 0x%02X 0x%08lX 0x%04X 0x%08lX\n", TableData.Unknown1&0xFF, TableData.Unknown2, TableData.Unknown3&0xFFFF, TableData.Unknown4);
   }
 }
/*===========================================================================
 *		End of Class Method CDFMapTableRecord::Dump()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFMapTableRecord::Read()"
/*===========================================================================
 *
 * Class CDFMapTableRecord Method - boolean Read (FILE* pFileHandle);
 *
 * Attempts to read in the data from the current position in the file
 * stream.  Returns FALSE on any error.
 *
 *=========================================================================*/
boolean CDFMapTableRecord::Read (FILE* pFileHandle) {
  int Result;

	/* Ensure valid input */
  if (pFileHandle == NULL) { 
    SET_EXT_ERROR2(ERR_NULL, "Invalid NULL file handle received!");
    return (FALSE);
   }

	/* Delete the current contents */
  Destroy();

	/* Save the record offset */
  RecordOffset = ftell(pFileHandle);

  if (RecordOffset < 0) { 
    SET_EXT_ERROR2(ERR_READ, "Failed to retrieve PItem record offset!");
    return (FALSE);
   }

	/* Read in record data all at once */
  Result = fread (&TableData, 1, sizeof(dfmap_table_t), pFileHandle);

  if (Result != sizeof(dfmap_table_t)) {
    SET_EXT_ERROR3(ERR_READ, "CDFMapTableRecord::Read() - Error reading map table data (Offset 0x%08lX)!", ftell(pFileHandle));
    return (FALSE);
   }

  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFMapTableRecord::Read()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFMapRegion::CDFMapRegion()"
/*===========================================================================
 *
 * Class CDFMapRegion Constructor
 *
 *=========================================================================*/
CDFMapRegion::CDFMapRegion() {
  RegionFlags = DFMAP_REGION_NONE;
  RegionNumber = -1;
  NumLocations = 0;
  NumDungeons = 0;
  pMapTables = NULL;
  pMapNames = NULL;
  pPItemRecords = NULL;
  pDItemRecords = NULL;
  pPItemOffsets = NULL;
  pDItemOffsets = NULL;
  MapPItemOffset = 0;
  MapDItemOffset = 0;
  MapTableOffset = 0;
  MapNamesOffset = 0;
 }
/*===========================================================================
 *		End of Class CDFMapRegion Constructor
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFMapRegion::Destroy()"
/*===========================================================================
 *
 * Class CDFMapRegion Destructor
 *
 *=========================================================================*/
void CDFMapRegion::Destroy (void) {

  	/* Destroy allocated memory */
  DestroyPointerArray(pPItemRecords);
  DestroyPointerArray(pDItemRecords);
  DestroyPointerArray(pPItemOffsets);
  DestroyPointerArray(pDItemOffsets);
  DestroyPointerArray(pMapTables);
  DestroyPointerArray(pMapNames);

  RegionFlags = DFMAP_REGION_NONE;
  RegionNumber = -1;
  NumLocations = 0;
  NumDungeons = 0;
  MapPItemOffset = 0;
  MapDItemOffset = 0;
  MapTableOffset = 0;
  MapNamesOffset = 0;
 }
/*===========================================================================
 *		End of Class CDFMapRegion Destructor
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFMapRegion::Dump()"
/*===========================================================================
 *
 * Class CDFMapRegion Method - void Dump (pFileHandle, DumpFlags);
 *
 * Output object info to a file stream.
 *
 *=========================================================================*/
void CDFMapRegion::Dump (FILE* pFileHandle, const dfmap_dumpflags_t DumpFlags) {
  int LoopCounter;

	/* Output region dump information only if required */
  if ((DumpFlags & DFMAP_DUMP_REGION) == 0) return;

	/* Ensure valid input */
  if (pFileHandle == NULL) { 
    SET_EXT_ERROR2(ERR_NULL, "Invalid NULL file handle received!");
    return;
   }

	/* Output basic information */
  fprintf (pFileHandle, "\tRegion %d, %d Locations, %d Dungeons  (Region Flags = %d)\n", RegionNumber, NumLocations, NumDungeons, RegionFlags);
  fprintf (pFileHandle, "\tOffsets: PItem=0x%08lX, DItem=0x%08lX, Table=0x%08lX, Names=0x%08lX\n", MapPItemOffset, MapDItemOffset, MapTableOffset, MapNamesOffset);
  fprintf (pFileHandle, "\tLocation Data...\n");

  if (RegionFlags == 0) {
    fprintf (pFileHandle, "\t\tNot loaded!\n");
    fflush (pFileHandle);
    return;
   }

	/* Output Location information */
  for (LoopCounter = 0; LoopCounter < NumLocations; LoopCounter++) {

		/* Output map name if loaded */
    if ((RegionFlags & DFMAP_REGION_NAMES) && (DumpFlags & DFMAP_DUMP_MAPNAMES))
      fprintf (pFileHandle, "\t\tName: '%s'\n", pMapNames[LoopCounter].Name);
    else 
      fprintf (pFileHandle, "\t\tBegin Location:\n");

		/* Output pItems if loaded */
    if ((RegionFlags & DFMAP_REGION_PITEM))
      pPItemRecords[LoopCounter].Dump(pFileHandle, DumpFlags);
    else 
      fprintf (pFileHandle, "\t\tMapPItem: Not loaded!\n");

		/* Output table if loaded */
    if ((RegionFlags & DFMAP_REGION_TABLE))
      pMapTables[LoopCounter].Dump(pFileHandle, DumpFlags);
    else 
      fprintf (pFileHandle, "\t\tMapTable: Not loaded!\n");
   }
	
	/* Output Dungeon information */
  fprintf (pFileHandle, "\tDungeon Data...\n");

  if (!(RegionFlags & DFMAP_REGION_DITEM)) {
    fprintf (pFileHandle, "\t\tMapDItem: Not loaded!\n");
    fflush (pFileHandle);
    return;
   }

  for (LoopCounter = 0; LoopCounter < NumDungeons; LoopCounter++) {
    pDItemRecords[LoopCounter].Dump(pFileHandle, DumpFlags);
   }

  fflush (pFileHandle);
 }
/*===========================================================================
 *		End of Class Method CDFMapRegion::Dump()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFMapRegion::FindLocation()"
/*===========================================================================
 *
 * Class CDFMapRegion Method - boolean FindLocation (PixelX, PixelY, RegionIndex, LocationIndex);
 *
 * Attempts to find a location in the region matching the given land
 * pixel coordinates. Returns TRUE on success and sets the region and
 * location indices.  Use the fact that the maptable is sorted by Y-position
 * to help speed the search.
 *
 *=========================================================================*/
boolean CDFMapRegion::FindLocation (const int PixelX, const int PixelY, int& RegionIndex, int& LocationIndex) {
  int LoopCounter;
  int MapPixelY;
  int MapPixelX;

	/* Check each location */
  for (LoopCounter = 0; LoopCounter < NumLocations; LoopCounter++) {

		/* Convert maptable coordinates to pixel coordinates */
    MapPixelX = pMapTables[LoopCounter].GetXPosition()/128;
    MapPixelY = DFWOODS_MAP_HEIGHT - 1 -  pMapTables[LoopCounter].GetYPosition()/128;

		/* Do we have a match yet? */
    if (MapPixelX == PixelX && MapPixelY == PixelY) {
      RegionIndex = RegionNumber;
      LocationIndex = LoopCounter;
      SystemLog.Printf ("Found location at pixel (%d, %d), %d in region %d (%s).", PixelX, PixelY,LocationIndex, RegionIndex, pMapNames[LocationIndex].Name);
      return (TRUE);
     }
   }

	/* No match found */
  return (FALSE);
 }
/*===========================================================================
 *		End of Class Method CDFMapRegion::FindLocation()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFMapRegion::GetMapPItem()"
/*===========================================================================
 *
 * Class CDFMapRegion Method - CDFMapPItemRecord* GetMapPItem (Location);
 *
 * Attempt to return the given PItem data for the location.  Returns NULL
 * on any error.
 *
 *=========================================================================*/
CDFMapPItemRecord* CDFMapRegion::GetMapPItem (const int LocationIndex) {

	/* Ensure a valid location index */
  if (!IsValidLocation(LocationIndex) || pPItemRecords == NULL) {
    SET_EXT_ERROR3(ERR_INDEX, "Invalid location index %d specified!", LocationIndex);
    return (NULL);
   }

  return (&pPItemRecords[LocationIndex]);
 }
/*===========================================================================
 *		End of Class Method CDFMapRegion::GetMapPItem()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFMapRegion::GetMapDtem()"
/*===========================================================================
 *
 * Class CDFMapRegion Method - CDFMapDItemRecord* GetMapDItem (Location);
 *
 * Attempt to return the given DItem data for the location.  Returns NULL
 * on any error.
 *
 *=========================================================================*/
CDFMapDItemRecord* CDFMapRegion::GetMapDItem (const int LocationIndex) {

	/* Ensure a valid location index */
  if (!IsValidLocation(LocationIndex) || pDItemRecords == NULL) {
    SET_EXT_ERROR3(ERR_INDEX, "Invalid location index %d specified!", LocationIndex);
    return (NULL);
   }

  return (&pDItemRecords[LocationIndex]);
 }
/*===========================================================================
 *		End of Class Method CDFMapRegion::GetMapDItem()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFMapRegion::GetMapTable()"
/*===========================================================================
 *
 * Class CDFMapRegion Method - CDFMapTableRecord* GetMapTable (Location);
 *
 * Attempt to return the given table data for the location.  Returns NULL
 * on any error.
 *
 *=========================================================================*/
CDFMapTableRecord* CDFMapRegion::GetMapTable (const int LocationIndex) {

	/* Ensure a valid location index */
  if (!IsValidLocation(LocationIndex) || pMapTables == NULL) {
    SET_EXT_ERROR3(ERR_INDEX, "Invalid location index %d specified!", LocationIndex);
    return (NULL);
   }

  return (&pMapTables[LocationIndex]);
 }
/*===========================================================================
 *		End of Class Method CDFMapRegion::GetMapTable()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFMapRegion::GetMapName()"
/*===========================================================================
 *
 * Class CDFMapRegion Method - char* GetMapName (Location);
 *
 * Attempt to return the given map name for the location.  Returns NULL
 * on any error.
 *
 *=========================================================================*/
char* CDFMapRegion::GetMapName (const int LocationIndex) {

	/* Ensure a valid location index */
  if (!IsValidLocation(LocationIndex) || pMapNames == NULL) {
    SET_EXT_ERROR3(ERR_INDEX, "CDFMapRegion::GetMapName() - Invalid location index %d specified!", LocationIndex);
    return (NULL);
   }

  return (pMapNames[LocationIndex].Name);
 }
/*===========================================================================
 *		End of Class Method CDFMapRegion::GetMapName()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFMapRegion::Read()"
/*===========================================================================
 *
 * Class CDFMapRegion Method - boolean Read (pFileHandle, Flags);
 *
 * Attempts to read in the data for the region as specified in the
 * flags parameter.  Returns FALSE on any error.
 *
 *=========================================================================*/
boolean CDFMapRegion::Read (FILE* pFileHandle, const dfmap_regionflags_t Flags) {
  boolean Result;

	/* Ignore if the given components are already read */
  if ((Flags & RegionFlags) == Flags) return (TRUE);

	/* Ensure valid input */
  if (pFileHandle == NULL) {
    SET_EXT_ERROR2(ERR_NULL, "Invalid NULL file handle received!");
    return (FALSE);
   }

	/* Read in the number of locations if required */
  if (!(RegionFlags & DFMAP_REGION_NUMLOCATIONS)) {
    Result = ReadNumLocations(pFileHandle);
    if (!Result) return (FALSE);
   }

	/* Read in the number of dungeons if required */
  if (!(RegionFlags & DFMAP_REGION_NUMDUNGEONS)) {
    Result = ReadNumDungeons(pFileHandle);
    if (!Result) return (FALSE);
   }

	/* Read the PItem offset list if required */
  if (((Flags & DFMAP_REGION_PITEM) || (Flags & DFMAP_REGION_POFFSET)) && !(RegionFlags & DFMAP_REGION_POFFSET)) {
    Result = ReadPItemOffsets(pFileHandle);
    if (!Result) return (FALSE);
   }

	/* Read the DItem offset list if required */
  if (((Flags & DFMAP_REGION_DITEM) || (Flags & DFMAP_REGION_DOFFSET)) && !(RegionFlags & DFMAP_REGION_DOFFSET) ) {
    Result = ReadDItemOffsets(pFileHandle);
    if (!Result) return (FALSE);
   }

	/* Read the PItem records if required */
  if ((Flags & DFMAP_REGION_PITEM) && !(RegionFlags & DFMAP_REGION_PITEM)) {
    Result = ReadPItemRecords(pFileHandle);
    if (!Result) return (FALSE);
   }

	/* Read the PItem records if required */
  if ((Flags & DFMAP_REGION_DITEM) && !(RegionFlags & DFMAP_REGION_DITEM)) {
    Result = ReadDItemRecords(pFileHandle);
    if (!Result) return (FALSE);
   }
  	
	/* Read the Maptable records if required */
  if ((Flags & DFMAP_REGION_TABLE) && !(RegionFlags & DFMAP_REGION_TABLE)) {
    Result = ReadMapTableRecords(pFileHandle);
    if (!Result) return (FALSE);
   }

	/* Read the Maptable records if required */
  if ((Flags & DFMAP_REGION_NAMES) && !(RegionFlags & DFMAP_REGION_NAMES)) {
    Result = ReadMapNamesRecords(pFileHandle);
    if (!Result) return (FALSE);
   }

  return (Result);
 }
/*===========================================================================
 *		End of Class Method CDFMapRegion::Read()
 *========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFMapRegion::ReadNumLocations()"
/*===========================================================================
 *
 * Class CDFMapRegion Method - boolean ReadNumLocations (pFileHandle);
 *
 * Attempts to read in the number of locations in the region.
 * Returns FALSE on any error.  Protected class method.
 *
 *=========================================================================*/
boolean CDFMapRegion::ReadNumLocations (FILE* pFileHandle) {
  int Result;

	/* Move to the begining of the map names data */
  Result = fseek (pFileHandle, MapNamesOffset, SEEK_SET);

  if (Result < 0) {
    SET_EXT_ERROR2(ERR_READ, "Failed to move file position to MapNames data!");
    return (FALSE);
   }

	/* Read in the number of locations */
  clearerr(pFileHandle);
  NumLocations = read_long(pFileHandle);
  
  if (ferror(pFileHandle) || feof(pFileHandle) || NumLocations < 0) {
    SET_EXT_ERROR2(ERR_READ, "Failed to read the number of region locations!");
    NumLocations = 0;
    return (FALSE);
   }

	/* Do optional statistics */
  #ifdef DOSTATS
    DO_STAT_REC2(TotalLocations, NumLocations)
    TotalLocationsTotalCount += NumLocations - 1; 
  #endif

	/* Flag that this has been read */
  RegionFlags |= DFMAP_REGION_NUMLOCATIONS;
  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFMapRegion::ReadNumLocations()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFMapRegion::ReadNumDungeons()"
/*===========================================================================
 *
 * Class CDFMapRegion Method - boolean ReadNumDungeons (pFileHandle);
 *
 * Attempts to read in the number of dungeons in the region.
 * Returns FALSE on any error.  Protected class method.
 *
 *=========================================================================*/
boolean CDFMapRegion::ReadNumDungeons (FILE* pFileHandle) {
  int Result;

	/* Move to the begining of the mapditem data */
  Result = fseek (pFileHandle, MapDItemOffset, SEEK_SET);

  if (Result < 0) {
    SET_EXT_ERROR2(ERR_READ, "Failed to move file position to MapDItem data!");
    return (FALSE);
   }

	/* Read in the number of dungeons */
  clearerr(pFileHandle);
  NumDungeons = read_long(pFileHandle);
  
  if (ferror(pFileHandle) || feof(pFileHandle) || NumDungeons < 0) {
    SET_EXT_ERROR2(ERR_READ, "Failed to read the number of region dungeons!");
    NumLocations = 0;
    return (FALSE);
   }

	/* Do optional statistics */
  #ifdef DOSTATS
    DO_STAT_REC2(TotalDungeons, NumDungeons)
    TotalDungeonsTotalCount += NumDungeons - 1; 
  #endif

  	/* Flag that this has been read */
  RegionFlags |= DFMAP_REGION_NUMDUNGEONS;
  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFMapRegion::ReadNumDungeons()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFMapRegion::ReadPItemOffsets()"
/*===========================================================================
 *
 * Class CDFMapRegion Method - boolean ReadPItemOffsets (pFileHandle);
 *
 * Attempts to read in the offset list to the pitem data.
 * Returns FALSE on any error.  Protected class method.
 *
 *=========================================================================*/
boolean CDFMapRegion::ReadPItemOffsets (FILE* pFileHandle) {
  int Result;

	/* Destroy and Reallocate the offset array */
  DestroyPointerArray(pPItemOffsets);

  if (NumLocations > 0) {
    CreatePointerArray(pPItemOffsets, dfmap_pitem_offset_t, NumLocations);
   }

	/* Move to the begining of the pitem data */
  Result = fseek (pFileHandle, MapPItemOffset, SEEK_SET);

  if (Result < 0) {
    SET_EXT_ERROR2(ERR_READ, "Failed to move file position to MapPItem data!");
    return (FALSE);
   }

	/* Read in the data all at once */
  Result = fread (pPItemOffsets, sizeof(dfmap_pitem_offset_t), NumLocations, pFileHandle);

  if (Result != NumLocations) {
    SET_EXT_ERROR2(ERR_READ, "Failed to read the MapPItem record offset list!");
    return (FALSE);
   }

	/* Flag that this has been read */
  RegionFlags |= DFMAP_REGION_POFFSET;
  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFMapRegion::ReadPItemOffsets()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFMapRegion::ReadPItemRecords()"
/*===========================================================================
 *
 * Class CDFMapRegion Method - boolean ReadPItemRecords (pFileHandle);
 *
 * Attempts to read in the records of the pitem data.
 * Returns FALSE on any error.  Protected class method.
 *
 *=========================================================================*/
boolean CDFMapRegion::ReadPItemRecords (FILE* pFileHandle) {
  int  Result;
  int  LoopCounter;
  long StartOffset;

	/* Destroy and Reallocate the records array */
  DestroyPointerArray(pPItemRecords);

  if (NumLocations > 0) {
    CreatePointerArray(pPItemRecords, CDFMapPItemRecord, NumLocations);
   }

	/* Compute first record offset */
  StartOffset = MapPItemOffset + NumLocations * sizeof(dfmap_pitem_offset_t);

	/* Read all the PItem records */
  for (LoopCounter = 0; LoopCounter < NumLocations; LoopCounter++) {

		/* Move position to start of record data */
    Result = fseek (pFileHandle, StartOffset + pPItemOffsets[LoopCounter], SEEK_SET);

    if (Result < 0) {
      SET_EXT_ERROR3(ERR_READ, "Failed to move file position to MapPItem record %d!", LoopCounter);
      return (FALSE);
     }

  	/* Set the record size */
    if (LoopCounter == NumLocations - 1)
      pPItemRecords[LoopCounter].SetRecordSize(MapDItemOffset - StartOffset - pPItemOffsets[LoopCounter]);
    else
      pPItemRecords[LoopCounter].SetRecordSize(pPItemOffsets[LoopCounter+1] - pPItemOffsets[LoopCounter]);

		/* Attempt to read in the record */
    Result = pPItemRecords[LoopCounter].Read(pFileHandle);
    if (!Result) return (FALSE);
   }

	/* Flag that this has been read */
  RegionFlags |= DFMAP_REGION_PITEM;
  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFMapRegion::ReadPItemRecords()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFMapRegion::ReadDItemOffsets()"
/*===========================================================================
 *
 * Class CDFMapRegion Method - boolean ReadDItemOffsets (pFileHandle);
 *
 * Attempts to read in the offset list to the Ditem data.
 * Returns FALSE on any error.  Protected class method.
 *
 *=========================================================================*/
boolean CDFMapRegion::ReadDItemOffsets (FILE* pFileHandle) {
  int Result;

	/* Destroy and Reallocate the offset array */
  DestroyPointerArray(pDItemOffsets);

  if (NumDungeons > 0) {
    CreatePointerArray(pDItemOffsets, dfmap_ditem_offset_t, NumDungeons);
   }

	/* Move to the begining of the ditem data, skipping number of dungeons which
	 * has been previously read */
  Result = fseek (pFileHandle, MapDItemOffset+4, SEEK_SET);

  if (Result < 0) {
    SET_EXT_ERROR2(ERR_READ, "Failed to move file position to MapDItem data!");
    return (FALSE);
   }

	/* Read in the data all at once */
  Result = fread (pDItemOffsets, sizeof(dfmap_ditem_offset_t), NumDungeons, pFileHandle);

  if (Result != NumDungeons) {
    SET_EXT_ERROR2(ERR_READ, "Failed to read the MapDItem record offset list!");
    return (FALSE);
   }

	/* Flag that this has been read */
  RegionFlags |= DFMAP_REGION_DOFFSET;
  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFMapRegion::ReadDItemOffsets()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFMapRegion::ReadDItemRecords()"
/*===========================================================================
 *
 * Class CDFMapRegion Method - boolean ReadDItemRecords (pFileHandle);
 *
 * Attempts to read in the records of the DItem data.
 * Returns FALSE on any error.  Protected class method.
 *
 *=========================================================================*/
boolean CDFMapRegion::ReadDItemRecords (FILE* pFileHandle) {
  int  Result;
  int  LoopCounter;
  long StartOffset;

	/* Destroy and Reallocate the records array */
  DestroyPointerArray(pDItemRecords);

  if (NumDungeons > 0) {
    CreatePointerArray(pDItemRecords, CDFMapDItemRecord, NumDungeons);
   }

	/* Compute first record offset */
  StartOffset = MapDItemOffset + 4 + NumDungeons * sizeof(dfmap_ditem_offset_t);

	/* Read all the DItem records */
  for (LoopCounter = 0; LoopCounter < NumDungeons; LoopCounter++) {

		/* Move position to start of record data */
    Result = fseek (pFileHandle, StartOffset + pDItemOffsets[LoopCounter].Offset, SEEK_SET);

    if (Result < 0) {
      SET_EXT_ERROR3(ERR_READ, "Failed to move file position to MapDItem record %d!", LoopCounter);
      return (FALSE);
     }

	     /* Set the record size */
    if (LoopCounter == NumDungeons - 1)
      pDItemRecords[LoopCounter].SetRecordSize(MapTableOffset - StartOffset - pDItemOffsets[LoopCounter].Offset);
    else
      pDItemRecords[LoopCounter].SetRecordSize(pDItemOffsets[LoopCounter+1].Offset - pDItemOffsets[LoopCounter].Offset);

		/* Attempt to read in the record */
    Result = pDItemRecords[LoopCounter].Read(pFileHandle);
    if (!Result) return (FALSE);
   }

	/* Flag that this has been read */
  RegionFlags |= DFMAP_REGION_DITEM;
  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFMapRegion::ReadDItemRecords()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFMapRegion::ReadMapTableRecords()"
/*===========================================================================
 *
 * Class CDFMapRegion Method - boolean ReadMapTableRecords (pFileHandle);
 *
 * Attempts to read in the records of the maptable data.
 * Returns FALSE on any error.  Protected class method.
 *
 *=========================================================================*/
boolean CDFMapRegion::ReadMapTableRecords (FILE* pFileHandle) {
  int  Result;
  int  LoopCounter;
  
	/* Destroy and Reallocate the records array */
  DestroyPointerArray(pMapTables);
  CreatePointerArray(pMapTables, CDFMapTableRecord, NumLocations);

	/* Move position to start of maptable data */
  Result = fseek (pFileHandle, MapTableOffset, SEEK_SET);

  if (Result < 0) {
    SET_EXT_ERROR2(ERR_READ, "Failed to move file position to MapTable data!");
    return (FALSE);
   }

	/* Read all the MapTable records */
  for (LoopCounter = 0; LoopCounter < NumLocations; LoopCounter++) {

		/* Attempt to read in the record */
    Result = pMapTables[LoopCounter].Read(pFileHandle);
    if (!Result) return (FALSE);
   }

	/* Flag that this has been read */
  RegionFlags |= DFMAP_REGION_TABLE;
  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFMapRegion::ReadMapTableRecords()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFMapRegion::ReadMapNamesRecords()"
/*===========================================================================
 *
 * Class CDFMapRegion Method - boolean ReadMapNamesRecords (pFileHandle);
 *
 * Attempts to read in the records of the maptable data.
 * Returns FALSE on any error.  Protected class method.
 *
 *=========================================================================*/
boolean CDFMapRegion::ReadMapNamesRecords (FILE* pFileHandle) {
  int  Result;
    
	/* Destroy and Reallocate the records array */
  DestroyPointerArray(pMapNames);
  CreatePointerArray(pMapNames, dfmap_name_t, NumLocations);

	/* Move position to start of mapnames data */
  Result = fseek (pFileHandle, MapNamesOffset + 4, SEEK_SET);

  if (Result < 0) {
    SET_EXT_ERROR2(ERR_READ, "Failed to move file position to MapNames data!");
    return (FALSE);
   }

	/* Read all the MapName records all at once */
  Result = fread (pMapNames, sizeof(dfmap_name_t), NumLocations, pFileHandle);

  if (Result != NumLocations) {
    SET_EXT_ERROR2(ERR_READ, "Failed to read MapNames data!");
    return (FALSE);
   }
  
	/* Flag that this has been read */
  RegionFlags |= DFMAP_REGION_NAMES;
  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFMapRegion::ReadMapNamesRecords()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFMapsFile::CDFMapsFile()"
/*===========================================================================
 *
 * Class CDFMapsFile Constructor
 *
 *=========================================================================*/
CDFMapsFile::CDFMapsFile() {
  SetFilename("arena2\\maps.bsa");
 }
/*===========================================================================
 *		End of Class CDFMapsFile Constructor
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFMapsFile::Destructor()"
/*===========================================================================
 *
 * Class CDFMapsFile Destructor
 *
 *=========================================================================*/
void CDFMapsFile::Destroy (void) {
  int LoopCounter;

	/* Destroy each region's data */
  for (LoopCounter = 0; LoopCounter < DFMAP_MAX_REGIONS; LoopCounter++) {
    Regions[LoopCounter].Destroy();
   }

 }
/*===========================================================================
 *		End of Class CDFMapsFile Destructor
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFMapsFile::Dump()"
/*===========================================================================
 *
 * Class CDFMapsFile Method - void Dump (pFileHandle, DumpFlags);
 *
 * Output object info to a file stream.
 *
 *=========================================================================*/
void CDFMapsFile::Dump (FILE* pFileHandle, const dfmap_dumpflags_t DumpFlags) {
  int LoopCounter;

	/* Ensure valid input */
  if (pFileHandle == NULL) { 
    SET_EXT_ERROR2(ERR_NULL, "Invalid NULL file handle received!");
    return;
   }

	/* Output base object information */
  CDFBSAFile::Dump(pFileHandle);

	/* Output region dump information if required */
  if ((DumpFlags & DFMAP_DUMP_REGION) == 0) return;

  for (LoopCounter = 0; LoopCounter < DFMAP_MAX_REGIONS; LoopCounter++) {
    fprintf (pFileHandle, "Region (%d), %s\n", LoopCounter, DFRegionNames[LoopCounter]);
    Regions[LoopCounter].Dump(pFileHandle, DumpFlags);
   }

  fflush (pFileHandle);
 }
/*===========================================================================
 *		End of Class Method CDFMapsFile::Dump()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFMapsFile::FindLandPixel()"
/*===========================================================================
 *
 * Class CDFMapsFile Method - boolean FindLandPixel (PixelX, PixelY, RegionIndex, LocationIndex);
 *
 * Searches for a location in the given landscape pixel.  On success returns
 * TRUE and sets the region and location indices.  On any error returns FALSE.
 *
 *=========================================================================*/
boolean CDFMapsFile::FindLandPixel (const int PixelX, const int PixelY, int& RegionIndex, int& LocationIndex) {
  int     LoopCounter;
  boolean Result;

	/* Ensure valid coordinates */
  if (PixelX < 0 || PixelY < 0 || PixelX >= DFWOODS_MAP_WIDTH || PixelY >= DFWOODS_MAP_HEIGHT) {
    return (FALSE);
   }

	/* Look for locations in all regions */
  for (LoopCounter = 0; LoopCounter < DFMAP_MAX_REGIONS; LoopCounter++) {
    Result = Regions[LoopCounter].FindLocation (PixelX, PixelY, RegionIndex, LocationIndex);
    if (Result) {
      return (TRUE);
     }
   }

	/* No locations found */
  return (FALSE);
 }
/*===========================================================================
 *		End of Class Method CDFMapsFile::FindLandPixel()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFMapsFile::GetMapPItem()"
/*===========================================================================
 *
 * Class CDFMapsFile Method - CDFMapPItemRecord* GetMapPItem (Region, Location);
 *
 * Attempt to return the given PItem data for the location.  Returns NULL
 * on any error.
 *
 *=========================================================================*/
CDFMapPItemRecord* CDFMapsFile::GetMapPItem (const int RegionIndex, const int LocationIndex) {
  boolean Result;

	/* Ensure a valid region index */
  if (!IsValidRegion(RegionIndex)) {
    SET_EXT_ERROR3(ERR_INDEX, "Invalid region index %d specified!", RegionIndex);
    return (NULL);
   }

	/* Load the region data if required */
  if (!Regions[RegionIndex].IsLoaded(DFMAP_REGION_PITEM)) {
    Result = ReadRegion(RegionIndex, DFMAP_REGION_PITEM);
    if (!Result) return (NULL);
   }

  return Regions[RegionIndex].GetMapPItem(LocationIndex);
 }
/*===========================================================================
 *		End of Class Method CDFMapsFile::GetMapPItem()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFMapsFile::GetMapPItem()"
/*===========================================================================
 *
 * Class CDFMapsFile Method - CDFMapDItemRecord* GetMapPItem (Region, Location);
 *
 * Attempt to return the given DItem data for the location.  Returns NULL
 * on any error.
 *
 *=========================================================================*/
CDFMapDItemRecord* CDFMapsFile::GetMapDItem (const int RegionIndex, const int LocationIndex) {
  boolean Result;

	/* Ensure a valid region index */
  if (!IsValidRegion(RegionIndex)) {
    SET_EXT_ERROR3(ERR_INDEX, "Invalid region index %d specified!", RegionIndex);
    return (NULL);
   }

	/* Load the region data if required */
  if (!Regions[RegionIndex].IsLoaded(DFMAP_REGION_DITEM)) {
    Result = ReadRegion(RegionIndex, DFMAP_REGION_DITEM);
    if (!Result) return (NULL);
   }


  return Regions[RegionIndex].GetMapDItem(LocationIndex);
 }
/*===========================================================================
 *		End of Class Method CDFMapsFile::GetMapDItem()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFMapsFile::GetMapTable"
/*===========================================================================
 *
 * Class CDFMapsFile Method - CDFMapPItemRecord* GetMapTable (Region, Location);
 *
 * Attempt to return the given table data for the location.  Returns NULL
 * on any error.
 *
 *=========================================================================*/
CDFMapTableRecord* CDFMapsFile::GetMapTable (const int RegionIndex, const int LocationIndex) {
  boolean Result;

	/* Ensure a valid region index */
  if (!IsValidRegion(RegionIndex)) {
    SET_EXT_ERROR3(ERR_INDEX, "CDFMapsFile::GetMapTable() - Invalid region index %d specified!", RegionIndex);
    return (NULL);
   }

	/* Load the region data if required */
  if (!Regions[RegionIndex].IsLoaded(DFMAP_REGION_TABLE)) {
    Result = ReadRegion(RegionIndex, DFMAP_REGION_PITEM);
    if (!Result) return (NULL);
   }

  return Regions[RegionIndex].GetMapTable(LocationIndex);
 }
/*===========================================================================
 *		End of Class Method CDFMapsFile::GetMapTable()
 *=========================================================================*/



#undef  __FUNC__
#define __FUNC__ "CDFMapsFile::GetMapName()"
/*===========================================================================
 *
 * Class CDFMapsFile Method - char* GetMapName (Region, Location);
 *
 * Attempt to return the given map name for the location.  Returns NULL
 * on any error.
 *
 *=========================================================================*/
char* CDFMapsFile::GetMapName (const int RegionIndex, const int LocationIndex) {
  boolean Result;

	/* Ensure a valid region index */
  if (!IsValidRegion(RegionIndex)) {
    SET_EXT_ERROR3(ERR_INDEX, "Invalid region index %d specified!", RegionIndex);
    return (NULL);
   }

	/* Load the region data if required */
  if (!Regions[RegionIndex].IsLoaded(DFMAP_REGION_NAMES)) {
    Result = ReadRegion(RegionIndex, DFMAP_REGION_PITEM);
    if (!Result) return (NULL);
   }

  return Regions[RegionIndex].GetMapName(LocationIndex);
 }
/*===========================================================================
 *		End of Class Method CDFMapsFile::GetMapName()
 *=========================================================================*/

 
  
#undef  __FUNC__
#define __FUNC__ "CDFMapsFile::ReadDirectory()"
/*===========================================================================
 *
 * Class CDFMapsFile Method - boolean ReadDirectory ();
 *
 * Virtual function to ensure the region offsets are set after each 
 * directory read.  Protected class method.
 *
 *=========================================================================*/
boolean CDFMapsFile::ReadDirectory (void) {
  boolean Result;

	/* Call the base class method */
  Result = CDFBSAFile::ReadDirectory();
  if (!Result) return (FALSE);

	/* Update the offset list */
  SetRegionOffsets();
  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFMapsFile::ReadDirectory()
 *=========================================================================*/



#undef  __FUNC__
#define __FUNC__ "CDFMapsFile::ReadRegion()"
/*===========================================================================
 *
 * Class CDFMapsFile Method - boolean ReadRegion (RegionIndex, Flags);
 *
 * Attempts to read in the data for the given region as specified in the
 * flags parameter.  Returns FALSE on any error.
 *
 *=========================================================================*/
boolean CDFMapsFile::ReadRegion (const int RegionIndex, const dfmap_regionflags_t Flags) {
  boolean HadToOpen = FALSE;
  boolean Result;

  	/* Ensure a valid region index */
  if (!IsValidRegion(RegionIndex)) {
    SET_EXT_ERROR3(ERR_INDEX, "Invalid region index %d!", RegionIndex);
    return (FALSE);
   }

	/* Ensure file is currently open */
  if (!IsOpen()) { 
    Result = Open();
    if (!Result) return (FALSE);
    HadToOpen = TRUE;
   }

	/* Initialize statistics if required */
  #ifdef DFMAP_DOSTATS
    static boolean DoStatsInit = FALSE;

    if (!DoStatsInit) {
      INIT_STAT_ARR(PItemPreRec, 6)
      INIT_STAT_ARR(DItemPreRec, 6)
      INIT_STAT_ARR(PItemPostNulls, 16)
      INIT_STAT_ARR(DItemPostUnknowns, 4)
      INIT_STAT_ARR(PItemNulls, 380)
      INIT_STAT_CNT(DItemPostUn2)
      INIT_STAT_CNT(DItemPostUn3)
      INIT_STAT_CNT(DItemPostUn4)
      INIT_STAT_CNT(PItemBlock1)
      INIT_STAT_CNT(PItemBlock2)
      INIT_STAT_CNT(PItemBlock3)
      DoStatsInit = TRUE;
     }
  #endif

	/* Read the region data */
  Result = Regions[RegionIndex].Read(pFileHandle, Flags);
	
	/* Close the file only if we had to open it */
  if (HadToOpen) Close();
  return (Result);
 }
/*===========================================================================
 *		End of Class Method CDFMapsFile::ReadRegion()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFMapsFile::Read()"
/*===========================================================================
 *
 * Class CDFMapsFile Method - boolean Read (Flags);
 *
 * Attempts to read in the data for all regions as specified in the
 * flags parameter.  Returns FALSE on any error.
 *
 *=========================================================================*/
boolean CDFMapsFile::Read (const dfmap_regionflags_t Flags) {
  boolean HadToOpen = FALSE;
  boolean Result;
  int     LoopCounter;

	/* Ensure file is currently open */
  if (!IsOpen()) { 
    Result = Open();
    if (!Result) return (FALSE);
    HadToOpen = TRUE;
   }

	/* Read the region data */
  for (LoopCounter = 0; LoopCounter < DFMAP_MAX_REGIONS; LoopCounter++) {
    Result = Regions[LoopCounter].Read(pFileHandle, Flags);
    if (!Result) break;
   }
	
	/* Close the file only if we had to open it */
  if (HadToOpen) Close();
  return (Result);
 }
/*===========================================================================
 *		End of Class Method CDFMapsFile::Read()
 *=========================================================================*/




#undef  __FUNC__
#define __FUNC__ "CDFMapsFile::SetRegionOffsets()"
/*===========================================================================
 *
 * Class CDFMapsFile Method - void SetRegionOffsets (void);
 *
 * Set the region offsets from the BSA directory.
 *
 *=========================================================================*/
void CDFMapsFile::SetRegionOffsets (void) {
  int LoopCounter;
  int RecordCounter;
  
	/* Set offsets for all regions or region directories, whichever is less */
  for (LoopCounter = 0, RecordCounter = 0; 
       LoopCounter < DFMAP_MAX_REGIONS && RecordCounter+3 < NumRecords;
       LoopCounter++) {
    Regions[LoopCounter].SetMapPItemOffset(pDirectory[RecordCounter++]->RecordOffset);
    Regions[LoopCounter].SetMapDItemOffset(pDirectory[RecordCounter++]->RecordOffset);
    Regions[LoopCounter].SetMapTableOffset(pDirectory[RecordCounter++]->RecordOffset);
    Regions[LoopCounter].SetMapNamesOffset(pDirectory[RecordCounter++]->RecordOffset);
    Regions[LoopCounter].SetRegionNumber(LoopCounter);
   }

 }
/*===========================================================================
 *		End of Class Method CDFMapsFile::SetRegionOffsets()
 *=========================================================================*/


#ifdef DOSTATS


#undef  __FUNC__
#define __FUNC__ "OutputStats()"
/*===========================================================================
 *
 * Function - void OutputStats (void);
 *
 * Outputs the current statistics to the system log file.
 *
 *=========================================================================*/
void OutputStats (void) {
  OUT_STAT_REC(TotalLocations)
  OUT_STAT_REC(TotalDungeons)
  OUT_STAT_REC(PItemNumPreRec)
  OUT_STAT_REC(DItemNumPreRec)
  OUT_STAT_REC(PItemNumPostRec)
  OUT_STAT_REC(DItemNumPostRec)
  OUT_STAT_REC(NumDunPostRec)
  OUT_STAT_VAR(PItemXPos)
  OUT_STAT_VAR(DItemYPos)
  OUT_STAT_VAR(PItemUnknown1)
  OUT_STAT_VAR(PItemUnknown2)
  OUT_STAT_VAR(PItemUnknown3)
  OUT_STAT_VAR(PItemUnknown4)
  OUT_STAT_VAR(PItemUnknown5)
  OUT_STAT_VAR(PItemUnknown6)
  OUT_STAT_VAR(DItemUnknown1)
  OUT_STAT_VAR(DItemUnknown2)
  OUT_STAT_VAR(DItemUnknown3)
  OUT_STAT_VAR(DItemUnknown4)
  OUT_STAT_VAR(DItemUnknown5)
  OUT_STAT_VAR(DItemUnknown6)
  OUT_STAT_VAR(DItemUnknown7)
  OUT_STAT_VAR(DItemUnknown8)
  OUT_STAT_VAR(DItemUnknown9)
  OUT_STAT_VAR(DItemUnknown10)
  OUT_STAT_ARR(PItemPreRec, 6)
  OUT_STAT_ARR(DItemPreRec, 6)
  OUT_STAT_VAR(PItemHouseName)
  OUT_STAT_VAR(PItemHouseType)
  OUT_STAT_ARR(PItemPostNulls, 16)
  OUT_STAT_VAR(PItemPostU1)
  OUT_STAT_VAR(PItemPostU2)
  OUT_STAT_VAR(PItemPostU3)
  OUT_STAT_ARR(PItemNulls, 380)
  OUT_STAT_ARR(DItemPostUnknowns, 4)
  OUT_STAT_CNT(DItemPostUn2)
  OUT_STAT_CNT(DItemPostUn3)
  OUT_STAT_CNT(DItemPostUn4)
  OUT_STAT_CNT(PItemBlock1)
  OUT_STAT_CNT(PItemBlock2)
  OUT_STAT_CNT(PItemBlock3)
 }
/*===========================================================================
 *		End of Function OutputStats()
 *========================================================================*/
#endif