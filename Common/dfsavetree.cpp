/*===========================================================================
 *
 * DFSaveTree.CPP - Dave Humphrey (uesp@m0use.net), 28 November 2000
 *
 *=========================================================================*/

	/* Include Files */
#include "dfsavetree.h"


#undef  __FUNC__
#define __FUNC__ "CDFSaveTreeRecord::CDFSaveTreeRecord()"
/*===========================================================================
 *
 * Class CDFSaveTreeRecord Constructor
 *
 *=========================================================================*/
CDFSaveTreeRecord::CDFSaveTreeRecord () {
  RecordSize = 0;
  RecordOffset = 0;
  RecordType = 0;
  HeaderInput = FALSE;
  pData = NULL;
  
 }
/*===========================================================================
 *		End of Class CDFSaveTreeRecord Constructor
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFSaveTreeRecord::Destroy()"
/*===========================================================================
 *
 * Class CDFSaveTreeRecord Destructor
 *
 *=========================================================================*/
void CDFSaveTreeRecord::Destroy (void) {

	/* Unallocate any raw data */
  DestroyPointerArray(pData);

  RecordSize = 0;
  RecordOffset = 0;
  RecordType = 0;
  HeaderInput = FALSE;
  Tagged = FALSE;
 }
/*===========================================================================
 *		End of Class CDFSaveTreeRecord Destructor
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFSaveTreeRecord::Compare()"
/*===========================================================================
 *
 * Class CDFSaveTreeRecord Method - boolean Compare (CDFSaveTreeRecord* pRecord)
 *
 * Compares two records, outputting any differences to the SystemLog file.
 * Returns FALSE on any error.
 *
 *=========================================================================*/
boolean CDFSaveTreeRecord::Compare (CDFSaveTreeRecord* pRecord) {

	/* Ensure valid input */
  if (pRecord == NULL) {
    SystemLog.Printf ("\t\tInput record is NULL!");
    return (FALSE);
   }

	/* Compare record sizes and types */
  CompareRecordSize(pRecord);
  CompareRecordType(pRecord);

  	/* Compare the record data */
  CompareRawData(pRecord);
  CompareRecordData(pRecord);
  
  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFSaveTreeRecord::Compare()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFSaveTreeRecord::CompareRawData()"
/*===========================================================================
 *
 * Class CDFSaveTreeRecord Method - boolean CompareRawData (CDFSaveTreeRecord* pRecord)
 *
 * Compares the raw data of the two records, outputting any differences to 
 * the SystemLog file. Returns FALSE on any error.  Protected class method.
 *
 *=========================================================================*/
boolean CDFSaveTreeRecord::CompareRawData (CDFSaveTreeRecord* pRecord) {
  int   Result;
  int   Index;
  byte* pDataPtr1;
  byte* pDataPtr2;
	
	/* Compare the raw data, if any */
  if (GetRawDataSize() == 0 && pRecord->GetRawDataSize() != 0) {
   }
  else if (GetRawDataSize() == 0)
    SystemLog.Printf ("\t\tRecord #1 has no raw data!");
  else if (pRecord->GetRawDataSize() == 0)
    SystemLog.Printf ("\t\tRecord #2 has no raw data!");
  else if (GetRawDataSize() != pRecord->GetRawDataSize())
    SystemLog.Printf ("\t\tRecord raw data sizes differ (%ld / %ld bytes)!", GetRawDataSize(), pRecord->GetRawDataSize());
  else {
    Result = memcmp(pData, pRecord->GetRawData(), GetRawDataSize());
    if (Result == 0) return (TRUE);
    SystemLog.Printf ("\t\tRecord raw data differs (0x%08lX, 0x%08lX)!", GetRecordOffset(), pRecord->GetRecordOffset());
    pDataPtr1 = pData;
    pDataPtr2 = pRecord->GetRawData();

    for (Index = 0; Index < GetRawDataSize(); Index++, pDataPtr1++, pDataPtr2++) {
      if (*pDataPtr1 != *pDataPtr2) {
        SystemLog.Printf ("\t\t\t0x%04X: 0x%02X != 0x%02X", Index, *pDataPtr1, *pDataPtr2);
       }
     }
   }

  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFSaveTreeRecord::CompareRawData()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFSaveTreeRecord::CompareRecordSize()"
/*===========================================================================
 *
 * Class CDFSaveTreeRecord Method - boolean CompareRecordSize (CDFSaveTreeRecord* pRecord)
 *
 * Compares the record sizes of the two records, outputting any differences 
 * to the SystemLog file. Returns FALSE on any error. Protected class method.
 *
 *=========================================================================*/
boolean CDFSaveTreeRecord::CompareRecordSize (CDFSaveTreeRecord* pRecord) {

	/* Compare record sizes */
  if (RecordSize != pRecord->GetRecordSize()) {
    SystemLog.Printf ("\t\tRecord sizes are different (%ld / %ld)!", RecordSize, pRecord->GetRecordSize());
   }
	
  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFSaveTreeRecord::CompareRecordSize()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFSaveTreeRecord::CompareRecordType()"
/*===========================================================================
 *
 * Class CDFSaveTreeRecord Method - boolean CompareRecordType (CDFSaveTreeRecord* pRecord)
 *
 * Compares the record types of the two records, outputting any differences 
 * to the SystemLog file. Returns FALSE on any error. Protected class method.
 *
 *=========================================================================*/
boolean CDFSaveTreeRecord::CompareRecordType (CDFSaveTreeRecord* pRecord) {

	/* Compare record types */
  if (RecordType != pRecord->GetRecordType()) {
    SystemLog.Printf ("\t\tRecord types are different (%d / %d)!", RecordType, pRecord->GetRecordType());
   }
	
  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFSaveTreeRecord::CompareRecordType()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFSaveTreeRecord::Read()"
/*===========================================================================
 *
 * Class CDFSaveTreeRecord Method - boolean Read (pFileHandle)
 *
 * Read in the raw record data.  Returns FALSE on any error.
 *
 *=========================================================================*/
boolean CDFSaveTreeRecord::Read (FILE* pFileHandle) {
  boolean Result;

	/* Ensure valid input */
  ASSERT(pFileHandle != NULL);

	/* Input the record header */
  Result = ReadHeader(pFileHandle);
  if (Result) Result = ReadRecordHeader(pFileHandle);
  if (!Result) return (FALSE);

	/* Debug output */
  SystemLog.Printf ("Opening SaveTree Record at 0x%08lX, 0x%08lX bytes (0x%02X type)...", RecordOffset, GetRawDataSize(), RecordType);

  Result = ReadRawData(pFileHandle);
  if (!Result) return (FALSE);

  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFSaveTreeRecord::Read()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFSaveTreeRecord::ReadHeader()"
/*===========================================================================
 *
 * Class CDFSaveTreeRecord Method - boolean ReadHeader (pFileHandle)
 *
 * Read in the record data header.  Returns FALSE on any error.  Protected
 * class method.
 *
 *=========================================================================*/
boolean CDFSaveTreeRecord::ReadHeader (FILE* pFileHandle) {
  boolean Result;

	/* Ignore if header already input */
  if (HasReadHeader()) return (TRUE);
	
	/* Save the record offset */
  RecordOffset = ftell(pFileHandle);

	/* Input the record size */
  Result = read_long(pFileHandle, RecordSize);
  if (!Result) return (FALSE);

	/* Input the record type, if any */
  Result = ReadType(pFileHandle);
  if (!Result) return (FALSE);
 
  HeaderInput = TRUE;
  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFSaveTreeRecord::ReadHeader()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFSaveTreeRecord::ReadRawData()"
/*===========================================================================
 *
 * Class CDFSaveTreeRecord Method - boolean ReadRawData (pFileHandle)
 *
 * Read in the raw record data.  Returns FALSE on any error.  Protected
 * class method.
 *
 *=========================================================================*/
boolean CDFSaveTreeRecord::ReadRawData (FILE* pFileHandle) {
  int Result;

	/* Ignore if recordsize is invalid */
  if (GetRawDataSize() <= 0) return (TRUE);

	/* Allocate the raw record data */
  CreatePointerArray(pData, byte, GetRawDataSize());

	/* Input the raw record data */
  Result = fread(pData, 1, GetRawDataSize(), pFileHandle);

  if (Result != GetRawDataSize()) {
    SET_EXT_ERROR4(ERR_READ, "Failed to input the raw record data (received %d of %d bytes)!", Result, GetRawDataSize());
    return (FALSE);
   }
	
  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFSaveTreeRecord::ReadRawData()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFSaveTreeRecord::ReadType()"
/*===========================================================================
 *
 * Class CDFSaveTreeRecord Method - boolean ReadType (pFileHandle)
 *
 * Read in the record type byte.  Returns FALSE on any error.  Protected
 * class method.
 *
 *=========================================================================*/
boolean CDFSaveTreeRecord::ReadType (FILE* pFileHandle) {
  int Result;
	
	 /* Load the record type */
  Result = fgetc(pFileHandle);

  if (Result < 0) {
    SET_EXT_ERROR2(ERR_READ, "Failed to input the record type!");
    return (FALSE);
   }
 
  RecordType = (byte) Result;
  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFSaveTreeRecord::ReadType()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFSaveTreeBase::CDFSaveTreeBase()"
/*===========================================================================
 *
 * Class CDFSaveTreeBase Constructor
 *
 *=========================================================================*/
CDFSaveTreeBase::CDFSaveTreeBase() {
	
	/* Reset the record header data */
  memset(&RecordHeader, 0, DFSAVETREE_RECORDHEADER_SIZE);
 }
/*===========================================================================
 *		End of Class CDFSaveTreeBase Constructor 
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFSaveTreeBase::Destroy()"
/*===========================================================================
 *
 * Class CDFSaveTreeBase Destructor
 *
 *=========================================================================*/
void CDFSaveTreeBase::Destroy (void) {
	
	/* Reset the record header data */
  memset(&RecordHeader, 0, DFSAVETREE_RECORDHEADER_SIZE);

	/* Call the base class method */
  CDFSaveTreeRecord::Destroy();
 }
/*===========================================================================
 *		End of Class CDFSaveTreeBase Destructor 
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFSaveTreeBase::CompareRecordHeader()"
/*===========================================================================
 *
 * Class CDFSaveTreeBase Method - boolean CompareRecordHeader (CDFSaveTreeRecord* pRecord)
 *
 * Compares the record header of the two records, outputting any differences to 
 * the SystemLog file. Returns FALSE on any error.  Protected class method.
 *
 *=========================================================================*/
boolean CDFSaveTreeBase::CompareRecordHeader (CDFSaveTreeRecord* pRecord) {
  dfsavetree_recordheader_t* pHeader;
  int Result;

	/* Get the record header for the second record */
  pHeader = ((CDFSaveTreeBase *)pRecord)->GetRecordHeader();
  	
	/* Compare the object IDs */
  if (RecordHeader.ObjectID != pHeader->ObjectID) 
    SystemLog.Printf ("\t\tObject IDs differ (%d / %d)", RecordHeader.ObjectID, pHeader->ObjectID);
  
  	/* Compare the parent IDs */
  if (RecordHeader.ParentID != pHeader->ParentID) 
    SystemLog.Printf ("\t\tParent IDs differ (%d / %d)", RecordHeader.ParentID, pHeader->ParentID);

  	/* Compare the parent types */
  if (RecordHeader.ParentType != pHeader->ParentType) 
    SystemLog.Printf ("\t\tParent types differ (%d / %d)", RecordHeader.ParentType, pHeader->ParentType);

	/* Compare the quest IDs */
  if (RecordHeader.QuestID != pHeader->QuestID) 
    SystemLog.Printf ("\t\tQuest IDs differ (%d / %d)", RecordHeader.QuestID, pHeader->QuestID);

	/* Compare the remaining portions of the structures */
  Result = memcmp(&RecordHeader, pHeader, DFSAVETREE_RECORDHEADER_SIZE);

  if (Result != 0)
    SystemLog.Printf ("\t\tRecord headers differ!");

  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFSaveTreeBase::CompareRecordHeader()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFSaveTreeBase::ReadRecordHeader()"
/*===========================================================================
 *
 * Class CDFSaveTreeBase Method - boolean ReadRecordHeader (pFileHandle)
 *
 * Read in the base record header.  Returns FALSE on any error.  Protected
 * class method.
 *
 *=========================================================================*/
boolean CDFSaveTreeBase::ReadRecordHeader (FILE* pFileHandle) {
  int Result;

  Result = fread (&RecordHeader, 1, DFSAVETREE_RECORDHEADER_SIZE, pFileHandle);

  if (Result != DFSAVETREE_RECORDHEADER_SIZE) {
    SET_EXT_ERROR4(ERR_READ, "Failed to read base record header (only %d of %d bytes received)!", Result, DFSAVETREE_RECORDHEADER_SIZE);
    return (FALSE);
   }

  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFSaveTreeBase::ReadRecordHeader()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFSaveTreeGuild::CompareRecordData()"
/*===========================================================================
 *
 * Class CDFSaveTreeGuild Method - boolean CompareRecordData (CDFSaveTreeRecord* pRecord)
 *
 * Compares the record data of the two records, outputting any differences to 
 * the SystemLog file. Returns FALSE on any error.  Protected class method.
 *
 *=========================================================================*/
boolean CDFSaveTreeGuild::CompareRecordData (CDFSaveTreeRecord* pRecord) {
  dfsavetree_guild_t* pGuild;

	/* Get the guild data */
  pGuild = ((CDFSaveTreeGuild *) pRecord)->GetGuildData();

	/* Compare guild IDs */
  if (GuildData.GuildID != pGuild->GuildID)
    SystemLog.Printf ("\t\tGuild IDs differ (%d / %d)!", GuildData.GuildID, pGuild->GuildID);

	/* Compare guild levels */
  if (GuildData.Level != pGuild->Level)
    SystemLog.Printf ("\t\tGuild levels differ (%d / %d)!", GuildData.Level, pGuild->Level);

	/* Compare guild times */
  if (GuildData.Time1 != pGuild->Time1)
    SystemLog.Printf ("\t\tGuild time1s differ (%ld / %ld)!", GuildData.Time1, pGuild->Time1);

	/* Compare guild times */
  if (GuildData.Time2 != pGuild->Time2)
    SystemLog.Printf ("\t\tGuild time2s differ (%ld / %ld)!", GuildData.Time2, pGuild->Time2);

	/* Compare guild unknowns */
  if (GuildData.Unknown1 != pGuild->Unknown1)
    SystemLog.Printf ("\t\tGuild unknown1s differ (%d / %d)!", GuildData.Unknown1, pGuild->Unknown1);

  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFSaveTreeGuild::CompareRecordData()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFSaveTreeGuild::ReadRawData()"
/*===========================================================================
 *
 * Class CDFSaveTreeGuild Method - boolean ReadRawData (pFileHandle)
 *
 * Read in the guild and raw record data.  Returns FALSE on any error.  
 * Protected class method.
 *
 *=========================================================================*/
boolean CDFSaveTreeGuild::ReadRawData (FILE* pFileHandle) {
  int Result;

	/* Read in the guild data */
  Result = fread(&GuildData, 1, DFSAVETREE_GUILD_SIZE, pFileHandle);

  if (Result != DFSAVETREE_GUILD_SIZE) {
    SET_EXT_ERROR4(ERR_READ, "Failed to read guild data (only %d of %d bytes received)!", Result, DFSAVETREE_GUILD_SIZE);
    return (FALSE);
   }

	/* Read in any remaining raw data */
  return (CDFSaveTreeBase::ReadRawData(pFileHandle));
 }
/*===========================================================================
 *		End of Class Method CDFSaveTreeGuild::ReadRawData()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFSaveTree::CDFSaveTree()"
/*===========================================================================
 *
 * Class CDFSaveTree Constructor
 *
 *=========================================================================*/
CDFSaveTree::CDFSaveTree() {
	
	/* Reset the header data */
  memset(&Header, 0, DFSAVETREE_HEADER_SIZE);
  NumRecords = 0;
  DungeonRecordIndex = -1;
  NumZeroSizedRecords = 0;
 }
/*===========================================================================
 *		End of Class CDFSaveTree Constructor 
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFSaveTree::Destroy()"
/*===========================================================================
 *
 * Class CDFSaveTree Destructor
 *
 *=========================================================================*/
void CDFSaveTree::Destroy (void) {
  int LoopCounter = 0;

	/* Unallocate all the valid savetree records */
  for (LoopCounter = 0; LoopCounter < NumRecords; LoopCounter++) {
    DestroyPointer(pRecords[LoopCounter]);
   }

	/* Reset the header data */
  memset(&Header, 0, DFSAVETREE_HEADER_SIZE);
  NumRecords = 0;
  DungeonRecordIndex = -1;
  NumZeroSizedRecords = 0;
 }
/*===========================================================================
 *		End of Class CDFSaveTree Destructor 
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFSaveTree::Compare()"
/*===========================================================================
 *
 * Class CDFSaveTree Method - boolean Compare (CDFSaveTree& SaveTree)
 *
 * Compares the given SaveTree file to the current one on a record 
 * by record basis.  Returns FALSE on any error.  Results are output
 * to the SystemLog file object.
 *
 *=========================================================================*/
boolean CDFSaveTree::Compare (CDFSaveTree& SaveTree) {
  CDFSaveTreeRecord*  pRecord;
  CDFSaveTreeTown*    pTown1;
  CDFSaveTreeTown*    pTown2;
  CDFSaveTreeDungeon* pDungeon1;
  CDFSaveTreeDungeon* pDungeon2;
  long		      ObjectID;
  int		      Index;
  int		      LoopCounter;

	/* Output header mesage */
  SystemLog.Printf ("Comparing SaveTree.DAT files...");

	/* Compare overall number of records */
  if (NumRecords != SaveTree.GetNumRecords()) {
    SystemLog.Printf ("\tNumber of records differ (%d / %d)", NumRecords, SaveTree.GetNumRecords());
   }

	/* Compare town records */
  SystemLog.Printf ("Comparing Town Records...");
  pTown1 = GetTownRecord();
  pTown2 = SaveTree.GetTownRecord();

  if (pTown1 == NULL && pTown2 == NULL) {
   }
  else if (pTown1 == NULL) {
    pTown2->SetTag();
    SystemLog.Printf ("\t\tNo town record in file #1!");
   }
  else if (pTown2 == NULL) 
    SystemLog.Printf ("\t\tNo town record in file #2!");
  else {
    pTown1->Compare(pTown2);
    pTown2->SetTag();
   }

	/* Compare the middle records */
  for (LoopCounter = 1; LoopCounter < DungeonRecordIndex && LoopCounter < NumRecords; LoopCounter++) {

		/* Ignore any NULL sized records */
//    if (pRecords[LoopCounter]->GetRecordSize() == 0) continue;

    SystemLog.Printf ("\tComparing record %d of %d (ID 0x%08lX, Size %ld bytes, Type 0x%02X '%s')", LoopCounter+1, NumRecords, pRecords[LoopCounter]->GetObjectID(), pRecords[LoopCounter]->GetRecordSize(), pRecords[LoopCounter]->GetRecordType(), DFGetSaveTreeType(pRecords[LoopCounter]->GetRecordType()));

		/* Attempt to find the given object ID in the supplied SaveTree file */
    ObjectID = pRecords[LoopCounter]->GetObjectID();
    Index = SaveTree.FindObjectID(ObjectID);
    pRecord = SaveTree.GetRecord(Index);
    if (pRecord != NULL) pRecord->SetTag();

		/* Compare the two records */
    pRecords[LoopCounter]->Compare(pRecord);
   }

	/* Compare dungeon records */
  SystemLog.Printf ("Comparing Dungeon Records...");
  pDungeon1 = GetDungeonRecord();
  pDungeon2 = SaveTree.GetDungeonRecord();

  if (pDungeon1 == NULL && pDungeon2 == NULL) {
   }
  else if (pDungeon1 == NULL) {
    SystemLog.Printf ("\tNo dungeon record in file #1!");
    pDungeon2->SetTag();
   }
  else if (pDungeon2 == NULL)  
    SystemLog.Printf ("\tNo dungeon record in file #2!");
  else {
    pDungeon1->Compare(pDungeon2);
    pDungeon2->SetTag();
   }

	/* Output any untagged records in SaveTree data */
  SaveTree.CompareOutputUntagged();
 
  return (TRUE); 
 }
/*===========================================================================
 *		End of Class Method CDFSaveTree::Compare()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFSaveTree::CompareOutputUntagged()"
/*===========================================================================
 *
 * Class CDFSaveTree Method - boolean CompareOutputUntagged (void)
 *
 * Outputs any untagged records to the SystemLog file.  Returns FALSE
 * on any error.
 *
 *=========================================================================*/
boolean CDFSaveTree::CompareOutputUntagged (void) {
  int LoopCounter;

  SystemLog.Printf ("\tUntagged Records...");

  for (LoopCounter = 0; LoopCounter < NumRecords; LoopCounter++) {

		/* Ignore NULL and tagged records */
    if (pRecords[LoopCounter]->GetRecordSize() == 0) continue;
    if (pRecords[LoopCounter]->IsTagged()) continue;

    SystemLog.Printf ("\t\tRecord %d of %d (ID 0x%08lX, Size %ld, Type 0x%02X '%s')", LoopCounter+1, NumRecords, pRecords[LoopCounter]->GetObjectID(), pRecords[LoopCounter]->GetRecordSize(), pRecords[LoopCounter]->GetRecordType(), DFGetSaveTreeType(pRecords[LoopCounter]->GetRecordType()));
   }

  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFSaveTree::CompareOutputUntagged()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFSaveTree::ClearTags()"
/*===========================================================================
 *
 * Class CDFSaveTree Method - void ClearTags (void)
 *
 * Clears the tag property of all the current records.
 *
 *=========================================================================*/
void CDFSaveTree::ClearTags (void) {
  int LoopCounter;

	/* Clear tags on all records */
  for (LoopCounter = 0; LoopCounter < NumRecords; LoopCounter++) {
    pRecords[LoopCounter]->SetTag(FALSE);
   }

 }
/*===========================================================================
 *		End of Class Method CDFSaveTree::ClearTags()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFSaveTree::CreateRecord()"
/*===========================================================================
 *
 * Class CDFSaveTree Method - boolean CreateRecord (pFileHandle)
 *
 * Creates a new SaveTree record depending on the content of the record.
 * Protected class method.  Returns FALSE on any error
 *
 *=========================================================================*/
boolean CDFSaveTree::CreateRecord (FILE* pFileHandle) {
  long RecordSize;
  long RecordOffset;
  int  RecordType;
  int  Result;

  	/* Ensure we don't allocate too many records */
  if (NumRecords >= DFSAVETREE_MAX_RECORDS) {
    SET_EXT_ERROR3(ERR_MAXINDEX, "Maximum number of SaveTree records %d exceeded!", DFSAVETREE_MAX_RECORDS);
    return (FALSE);
   }

	/* If this is the first record, it should be the town record */
  if (NumRecords == 0) {
    CreatePointer(pRecords[0], CDFSaveTreeTown);
    NumRecords = 1;
    return (TRUE);
   }

	/* If this is the second to last record, it should be the special
	 * dungeon record. */
  if (NumZeroSizedRecords == 2 && DungeonRecordIndex < 0) {
    CreatePointer(pRecords[NumRecords], CDFSaveTreeDungeon);
    DungeonRecordIndex = NumRecords;
    NumRecords++;
    return (TRUE);
   }

	/* Save the record offset */
  RecordOffset = ftell(pFileHandle);
 	
	/* Load the record size */
  Result = read_long(pFileHandle, RecordSize);
  if (!Result) return (FALSE);

  	/* Check for zero-size records */
  if (RecordSize == 0) {
    CreatePointer(pRecords[NumRecords], CDFSaveTreeNULL);
    pRecords[NumRecords]->SetReadHeader(TRUE);
    pRecords[NumRecords]->SetRecordOffset(RecordOffset);
    pRecords[NumRecords]->SetRecordSize(0);
    NumRecords++;
    NumZeroSizedRecords++;
    return (TRUE);
   }

	/* Load the record type */
  RecordType = fgetc(pFileHandle);

  if (RecordType < 0) {
    SET_EXT_ERROR2(ERR_READ, "Failed to input the record type!");
    return (FALSE);
   }

	/* Create the desired record */
  CreatePointer(pRecords[NumRecords], CDFSaveTreeBase);
  NumRecords++;

	/* Set the record values */ 
  pRecords[NumRecords-1]->SetReadHeader(TRUE);
  pRecords[NumRecords-1]->SetRecordOffset(RecordOffset);
  pRecords[NumRecords-1]->SetRecordSize(RecordSize);
  pRecords[NumRecords-1]->SetRecordType((byte)RecordType);

  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFSaveTree::CreateRecord()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFSaveTree::FindObjectID()"
/*===========================================================================
 *
 * Class CDFSaveTree Method - int FindObjectID (ObjectID)
 *
 * Look through the record array for a matching object ID.  Returns -1 on
 * any error.
 *
 *=========================================================================*/
int CDFSaveTree::FindObjectID (const long ObjectID) {
  int LoopCounter;

	/* Search through entire array */
  for (LoopCounter = 0; LoopCounter < NumRecords; LoopCounter++) {
    if (ObjectID == pRecords[LoopCounter]->GetObjectID()) return (LoopCounter);
   }

	/* No match found */
  return (-1);
 }
/*===========================================================================
 *		End of Class Method CDFSaveTree::FindObjectID()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFSaveTree::Load()"
/*===========================================================================
 *
 * Class CDFSaveTree Method - boolean Load (const char* pFilename)
 *
 * Loads the desired SaveTree.DAT file.
 *
 *=========================================================================*/
boolean CDFSaveTree::Load (const char* pFilename) {
  FILE*   pFileHandle;
  long    Offset;
  long    FileSize;
  boolean Result;

	/* Attempt to open the file for input */
  pFileHandle = openfile(pFilename, "rb");
  if (pFileHandle == NULL) return (FALSE);

	/* Destroy the current information */
  Destroy();

	/* Read in the savetree header and records */
  Result = ReadHeader(pFileHandle);
  if (Result) Result = ReadRecords(pFileHandle);

  Offset = ftell(pFileHandle);
  fseek(pFileHandle, 0, SEEK_END);
  FileSize = ftell(pFileHandle);

  SystemLog.Printf ("Extra bytes left over: %ld", FileSize - Offset);
  SystemLog.Printf ("NumRecords = %d", NumRecords);

	/* Close the file and return success */
  fclose (pFileHandle);
  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFSaveTree::Load()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFSaveTree::ReadHeader()"
/*===========================================================================
 *
 * Class CDFSaveTree Method - boolean ReadHeader (FILE* pFileHandle)
 *
 * Inputs the savetree header from the given filestream.  Returns FALSE
 * on any error.  Protected class method.
 *
 *=========================================================================*/
boolean CDFSaveTree::ReadHeader (FILE* pFileHandle) {
  int Result;

	/* Move to start of file */
  Result = fseek(pFileHandle, 0, SEEK_SET);

  if (Result != 0) {
    SET_EXT_ERROR2(ERR_READ, "Failed to move to start of file!");
    return (FALSE);
   }

	/* Read in the header all at once */
  Result = fread (&Header, 1, DFSAVETREE_HEADER_SIZE, pFileHandle);

  if (Result != DFSAVETREE_HEADER_SIZE) {
    SET_EXT_ERROR4(ERR_READ, "Failed to read SaveTree header (received %d of %d bytes)!", Result, DFSAVETREE_HEADER_SIZE);
    return (FALSE);
   }

  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFSaveTree::ReadHeader()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFSaveTree::ReadRecords()"
/*===========================================================================
 *
 * Class CDFSaveTree Method - boolean ReadRecords (FILE* pFileHandle)
 *
 * Inputs the record data from the given filestream.  Returns FALSE
 * on any error.  Protected class method.
 *
 *=========================================================================*/
boolean CDFSaveTree::ReadRecords (FILE* pFileHandle) {
  int  Result;
       
	/* Read in all the savetree records */
  while (!feof(pFileHandle) && NumZeroSizedRecords < 3) {

		/* Attempt to create a new record */
    Result = CreateRecord(pFileHandle);
    if (!Result) return (FALSE);

		/* Attempt to read the record */
    Result = pRecords[NumRecords-1]->Read(pFileHandle);
    if (!Result) return (FALSE);
   }

  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFSaveTree::ReadRecords()
 *=========================================================================*/

 
#undef  __FUNC__
#define __FUNC__ "DFGetSaveTreeType()"
/*===========================================================================
 *
 * Function - char* DFGetSaveTreeType (const int Type)
 *
 * Converts a record type from the SaveTree.DAT file into a descriptive 
 * string.  Never returns NULL.
 *
 *=========================================================================*/
char* DFGetSaveTreeType (const int Type) {
  static char Buffer[32];

  switch (Type) {
    case 0x02: return "Item";
    case 0x03: return "Character";
    case 0x04: return "Character Parent";
    case 0x05: return "Character Parent";
    case 0x06: return "Unknown 0x06";   
    case 0x08: return "Unknown 0x08";
    case 0x09: return "Spells";
    case 0x0a: return "Guild";
    case 0x0e: return "QBN Record";
    case 0x10: return "Parent QBN";
    case 0x12: return "Parent QBN";
    case 0x16: return "Spellcasting Creatures";
    case 0x17: return "Control Settings";
    case 0x18: return "Location Names";
    case 0x19: return "Unknown 0x19";
    case 0x1f: return "Potion";
    case 0x20: return "Unknown Town 0x020";
    case 0x21: return "Unknown Dungeon, 0x21";
    case 0x22: return "Creature"; 
    case 0x24: return "Store Items";
    case 0x27: return "Mystery 0x27";
    case 0x28: return "Location names1";
    case 0x29: return "Location names2";
    case 0x2b: return "Mystery 0x2B";
    case 0x2c: return "Creature";
    case 0x2d: return "NPC";
    case 0x2e: return "Generic NPC";
    case 0x33: return "Dungeon Record";
    case 0x34: return "Container Record";   
    case 0x40: return "Container?";
    case 0x41: return "Quest?";
    default:
      sprintf (Buffer, "Unknown 0x%02X", Type);
      return (Buffer);
    }
 }
/*===========================================================================
 *		End of Function DFGetSaveTreeType()
 *=========================================================================*/



/*
Temple of Akatosh
Temple of Arkay
Temple of Dibella
Temple of Julianos
Temple of Kynareth
Temple of Mara
Temple of Stendarr
Temple of Zen
Order of the Raven
Knights of the Dragon
Knights of the Owl
Order of the Candle
Knights of the Flame
Host of the Horn
Knights of the Rose
Knights of the Wheel
Order of the Scarab
Knights of the Hawk
Mages Guild
Fighters Guild


Thieves Guild
Dark Brotherhood
Temple of Theodorus
Knights of Daggerfall
Order of Nyeraad
Gryphon Knights
Order of the Lance
Knights of the Isle
Border Knights
Crusaders
Blades
Dreadknights
Order of the Lamp
Raen
Arkay
Zenithar
Notorgo
Mara
Shandar
Akatosh
Julianos
The Bandit God
Dibella
Jephre
Vigryl
Q'Olwen
Stendarr
Ephen

God of Agriculture
God of Birth and Death
God of Work and Commerce
Messenger God
Goddess of Love
God of War
God of Time
God of Logic
God of Thieves
Goddess of Beauty
the Storyteller
God of the Sea
(wisdom?)
God of Mercy
God of the Wild */