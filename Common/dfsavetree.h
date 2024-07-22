/*===========================================================================
 *
 * DFSaveTree.H - Dave Humphrey (uesp@m0use.net), Tuesday, November 28, 2000
 *
 *=========================================================================*/
#ifndef __DFSAVETREE_H
#define __DFSAVETREE_H

	/* Required Includes */
#include "genutil.h"


/*===========================================================================
 *
 * Begin Defines
 *
 *=========================================================================*/

	/* Default record sizes */
  #define DFSAVETREE_HEADER_SIZE       sizeof(dfsavetree_header_t)
  #define DFSAVETREE_RECORDHEADER_SIZE sizeof(dfsavetree_recordheader_t)
  #define DFSAVETREE_GUILD_SIZE	       sizeof(dfsavetree_guild_t)

	/* Number of records allowed in a SAVETREE.DAT file.  Code
	 * enforced, not Daggerfall. */
  #define DFSAVETREE_MAX_RECORDS 1500

	/* Record size factor for dungeon records */
  #define DFSAVETREE_DUNGEON_FACTOR 0x27
  
/*===========================================================================
 *		End of Defines
 *=========================================================================*/


/*===========================================================================
 *
 * Begin Type and Structure Definitions
 *
 *=========================================================================*/

	/* Ensure proper structure sizes */
#ifdef _WIN32
  #pragma pack(push, save_pack)
  #pragma pack(1)
#endif

	/* SaveTree.DAT header structure, 13 bytes */
  typedef struct {
    long  NumRecords;
    short Unknown1;
    short Unknown2;
    long  Unknown3;
    short Unknown4;
    long  Unknown5;
    byte  Unknown6;
   } dfsavetree_header_t;

	/* Base record header data, 46 bytes */
  typedef struct { 
    short Unknown1;
    short Unknown2;
    short Unknown3;
    long  XPosition;
    long  YPosition;
    long  ZPosition;
    short Unknown4;
    short Unknown5;
    long  Unknown6;
    long  Unknown7;
    long  ObjectID;
    short Unknown8;
    byte  Unknown9;
    byte  QuestID;
    long  ParentID;
    long  Unknown10;
    long  Unknown11;
    long  Unknown12;
    long  Unknown13;
    long  Unknown14;
    long  Unknown15;
    byte  ParentType;	        	
    byte  Unknown16;
    short Unknown17;	  
   } dfsavetree_recordheader_t;

	/* Known guild information */
  typedef struct {
     short Level;
     byte  Unknown1;
     short GuildID;
     long  Time1;
     long  Time2;
    } dfsavetree_guild_t;
  
#ifdef _WIN32
  #pragma pack(pop, save_pack)
#endif
/*===========================================================================
 *		End of Type and Structure Definitions
 *=========================================================================*/


/*===========================================================================
 *
 * Class CDFSaveTreeRecord Definition
 *
 *=========================================================================*/
class CDFSaveTreeRecord {

  /*---------- Begin Protected Class Members ---------------------*/
protected:
  long    RecordOffset;		/* Record offset in the SaveTree file */
  long    RecordSize;		/* Size of the record */
  byte    RecordType;		/* The type of record */
  byte*   pData;		/* The raw record data */

  boolean HeaderInput;		/* Indicates that the header information has been read */
  boolean Tagged;		/* Used for sorting and comparing records */


  /*---------- Begin Protected Class Methods ---------------------*/
protected:

	/* Compare records helper functions */
  virtual boolean CompareRawData      (CDFSaveTreeRecord* pRecord);
  virtual boolean CompareRecordSize   (CDFSaveTreeRecord* pRecord);
  virtual boolean CompareRecordType   (CDFSaveTreeRecord* pRecord);
  virtual boolean CompareRecordHeader (CDFSaveTreeRecord* pRecord) { return (TRUE); }
  virtual boolean CompareRecordData   (CDFSaveTreeRecord* pRecord) { return (TRUE); }

	/* Input the record data */
  virtual boolean ReadHeader       (FILE* pFileHandle);
  virtual boolean ReadRecordHeader (FILE* pFileHandle) { return (TRUE); }
  virtual boolean ReadType         (FILE* pFileHandle);
  virtual boolean ReadRawData      (FILE* pFileHandle);


  /*---------- Begin Public Class Methods ------------------------*/
public:

	/* Class Constructor/Destructor */
  CDFSaveTreeRecord();
  virtual ~CDFSaveTreeRecord() { Destroy(); }
  virtual void Destroy (void);

	/* Compares records */
  virtual boolean Compare (CDFSaveTreeRecord* pRecord);

	/* Check for zero-sized records */
  virtual boolean IsEmpty     (void) { return (IsZeroSized()); }
  virtual boolean IsZeroSized (void) { return (boolean) (RecordSize == 0); }

	/* Check the tagged state of the record */
  virtual boolean IsTagged (void) { return (Tagged); }

	/* Get class members */
  virtual long  GetRecordSize   (void) { return (RecordSize); }
  virtual long  GetRecordOffset (void) { return (RecordOffset); }
  virtual byte  GetRecordType   (void) { return (RecordType); }
  virtual byte* GetRawData      (void) { return (pData); }
  virtual long  GetObjectID     (void) { return (-1); }

  	/* Get the record header data */
  virtual dfsavetree_recordheader_t* GetRecordHeader (void) { return (NULL); }

	/* Get the real record size of the data */
  virtual long GetRawDataSize (void) { return (RecordSize - 1); }

	/* Determine if the header has been input yet */
  virtual boolean HasReadHeader (void) { return (HeaderInput); }

	/* Read in the raw record data */
  virtual boolean Read (FILE* pFileHandle);

	/* Set class members */
  virtual void SetReadHeader   (const boolean Value) { HeaderInput = Value; }
  virtual void SetRecordSize   (const long     Size) { RecordSize = Size; }
  virtual void SetRecordOffset (const long   Offset) { RecordOffset = Offset; }
  virtual void SetRecordType   (const byte     Type) { RecordType = Type; }
  virtual void SetTag	       (const Value = TRUE)  { Tagged = Value; }

 };
/*===========================================================================
 *		End of Class CDFSaveTreeRecord Definition
 *=========================================================================*/


/*===========================================================================
 *
 * Class CDFSaveTreeBase Definition
 *
 * Derived class for a regular SaveTree record.
 *
 *=========================================================================*/
class CDFSaveTreeBase : public CDFSaveTreeRecord { 

  /*---------- Begin Protected Class Members ---------------------*/
protected:
  dfsavetree_recordheader_t RecordHeader;


  /*---------- Begin Protected Class Methods ---------------------*/
protected:

	/* Copare the record header */
  boolean CompareRecordHeader (CDFSaveTreeRecord* pRecord);

	/* Read the record header data */
  boolean ReadRecordHeader (FILE* pFileHandle);


  /*---------- Begin Public Class Methods ------------------------*/
public:

	/* Class Constructor/Destructor */
  CDFSaveTreeBase();
  void Destroy (void);

	/* Get the real record size of the data */
  long GetRawDataSize (void) { return (RecordSize - 1 - DFSAVETREE_RECORDHEADER_SIZE); }
  long GetObjectID    (void) { return (RecordHeader.ObjectID); }

  	/* Get the record header data */
  dfsavetree_recordheader_t* GetRecordHeader (void) { return (&RecordHeader); }

 };
/*===========================================================================
 *		End of Class CDFSaveTreeBase Definition
 *=========================================================================*/


/*===========================================================================
 *
 * Class CDFSaveTreeNULL Definition
 *
 * Derived class for NULL, or 0 sized, SaveTree records.
 *
 *=========================================================================*/
class CDFSaveTreeNULL : public CDFSaveTreeRecord { 


  /*---------- Begin Protected Class Methods ---------------------*/
protected:

	/* No type or raw data for NULL records */
  boolean ReadType    (FILE* pFileHandle) { return (TRUE); }
  boolean ReadRawData (FILE* pFileHandle) { return (TRUE); }


  /*---------- Begin Public Class Methods ------------------------*/
public:

	/* No use comparing NULL records */
  boolean Compare (CDFSaveTreeRecord* pRecord) { return (TRUE); }

	/* Get the size of the raw town data */
  long GetRawDataSize (void) { return (0); }


 };
/*===========================================================================
 *		End of Class CDFSaveTreeNULL Definition
 *=========================================================================*/


/*===========================================================================
 *
 * Class CDFSaveTreeTown Definition
 *
 * Derived class for town SaveTree records.
 *
 *=========================================================================*/
class CDFSaveTreeTown : public CDFSaveTreeRecord { 

  /*---------- Begin Protected Class Methods ---------------------*/
protected:

	/* No type field for town records */
  boolean CompareRecordType (CDFSaveTreeRecord* pRecord) { return (TRUE); }

	/* No type field for town records */
  boolean ReadType (FILE* pFileHandle) { return (TRUE); }


  /*---------- Begin Public Class Methods ------------------------*/
public:

	/* Get the size of the raw town data */
  long GetRawDataSize (void) { return (RecordSize); }

 };
/*===========================================================================
 *		End of Class CDFSaveTreeTown Definition
 *=========================================================================*/


/*===========================================================================
 *
 * Class CDFSaveTreeDungeon Definition
 *
 * Derived class for dungeon SaveTree records.
 *
 *=========================================================================*/
class CDFSaveTreeDungeon : public CDFSaveTreeRecord { 

  /*---------- Begin Protected Class Methods ---------------------*/
protected:

	/* No type field for dungeon records */
  boolean CompareRecordType (CDFSaveTreeRecord* pRecord) { return (TRUE); }

	/* No type field for dungeon records */
  boolean ReadType (FILE* pFileHandle) { return (TRUE); }


  /*---------- Begin Public Class Methods --------------------------*/
public:

	/* Get the real record size of the dungeon data */
  long GetRawDataSize (void) { return (RecordSize*DFSAVETREE_DUNGEON_FACTOR); }

 };
/*===========================================================================
 *		End of Class CDFSaveTreeDungeon Definition
 *=========================================================================*/


/*===========================================================================
 *
 * Class CDFSaveTreeGuild Definition
 *
 *=========================================================================*/
class CDFSaveTreeGuild : public CDFSaveTreeBase {

  /*---------- Begin Protected Class Members ---------------------*/
protected:
  dfsavetree_guild_t GuildData;
  

  /*---------- Begin Protected Class Methods ---------------------*/
protected:

	/* Compare the guild data */
  boolean CompareRecordData  (CDFSaveTreeRecord* pRecord);

	/* Read the giuld and raw data */
  boolean ReadRawData (FILE* pFileHandle);


  /*---------- Begin Public Class Methods ------------------------*/
public:

	/* Class Constructor/Destructor */
  CDFSaveTreeGuild() { memset(&GuildData, 0, DFSAVETREE_GUILD_SIZE); }
  void Destroy (void) { memset(&GuildData, 0, DFSAVETREE_GUILD_SIZE);  CDFSaveTreeBase::Destroy(); }

	/* Get the real record size of the data */
  virtual long GetRawDataSize (void) { return (RecordSize - 1 - DFSAVETREE_RECORDHEADER_SIZE - DFSAVETREE_GUILD_SIZE); }

	/* Get the guild data pointer */
  virtual dfsavetree_guild_t* GetGuildData (void) { return (&GuildData); }

 };
/*===========================================================================
 *		End of Class CDFSaveTreeGuild
 *=========================================================================*/


/*===========================================================================
 *
 * Class CDFSaveTree Definition
 * 
 * Class for manipulating the SAVETREE.DAT file which contains character
 * savegame information.
 *
 *=========================================================================*/
class CDFSaveTree { 

  /*---------- Begin Protected Class Members ---------------------*/
protected:
  dfsavetree_header_t Header;		/* File header data */

	/* The raw record data */
  CDFSaveTreeRecord* pRecords[DFSAVETREE_MAX_RECORDS];
  int		     NumRecords;

	/* Record index of the dungeon data (even if zero-length) */
  int		     DungeonRecordIndex;		
  int		     NumZeroSizedRecords;


  /*---------- Begin Protected Class Methods ---------------------*/
protected:
	
	/* Create the next SaveTree record */
  virtual boolean CreateRecord (FILE* pFileHandle);

	/* Helper input functions */
  virtual boolean ReadHeader  (FILE* pFileHandle);
  virtual boolean ReadRecords (FILE* pFileHandle);


  /*---------- Begin Public Class Methods ------------------------*/
public:

	/* Class Constructor/Destructor */
  CDFSaveTree();
  virtual ~CDFSaveTree() { Destroy(); }
  virtual void Destroy (void);

	/* Compares two savetree files, record by record */
  virtual boolean Compare (CDFSaveTree& SaveTree);

	/* Outputs any untagged records to the SystemLog file */
  virtual boolean CompareOutputUntagged (void);

	/* Clears the tags of all the current records */
  virtual void ClearTags (void);

	/* Search for an ObjectID in the record array */
  virtual int FindObjectID (const long ObjectID);

	/* Get class members */
  virtual int GetNumRecords     (void) { return (NumRecords); }
  virtual int GetDungeonIndex   (void) { return (DungeonRecordIndex); }
  virtual int GetNumZeroRecords (void) { return (NumZeroSizedRecords); }
  
	/* Get a particular record pointer */
  virtual CDFSaveTreeRecord* GetRecord (const int Index) { if (IsValidIndex(Index)) return (pRecords[Index]); return (NULL); }

	/* Get specific records */
  virtual CDFSaveTreeTown*    GetTownRecord    (void) { if (NumRecords < 1) return (NULL); return ((CDFSaveTreeTown*) pRecords[0]); }
  virtual CDFSaveTreeDungeon* GetDungeonRecord (void) { if (DungeonRecordIndex < 0) return (NULL); return ((CDFSaveTreeDungeon*) pRecords[DungeonRecordIndex]); }

	/* Check a record index for validity */
  virtual boolean IsValidIndex (const int Index) { return ((Index >= 0 && Index < NumRecords) ? TRUE : FALSE); }

	/* Load a SaveTree.DAT file */
  virtual boolean Load (const char* pFilename);

 };
/*===========================================================================
 *		End of Class CDFSaveTree Definition
 *=========================================================================*/


/*===========================================================================
 *
 * Begin Function Prototypes
 *
 *=========================================================================*/

	/* Convert a record type to a string */
  char* DFGetSaveTreeType (const int Type);

/*===========================================================================
 *		End of Function Prototypes
 *=========================================================================*/


#endif
/*===========================================================================
 *		End of File DFSaveTree.H
 *=========================================================================*/
