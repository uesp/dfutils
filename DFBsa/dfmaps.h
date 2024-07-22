/*===========================================================================
 *
 * DFMaps.H - Dave Humphrey (uesp@m0use.net), 14 November 2000
 *
 *=========================================================================*/
#ifndef __DFMAPS_H
#define __DFMAPS_H

	/* Required Includes */
#include "dfbsa.h"


/*===========================================================================
 *
 * Begin Defines
 *
 *=========================================================================*/

	/* Number of records allowed */
  #define DFMAP_MAX_PITEM_PRERECORDS 40
  #define DFMAP_MAX_REGIONS          62

	/* Default string sizes */
  #define DFMAP_PITEM_NAMESIZE 32

	/* Number of 'extra' bytes at end of PItem/DItem records */
  #define DFMAP_PITEM_EXTRABYTES 380
  #define DFMAP_DITEM_EXTRABYTES 135

	/* Undefine to remove statistics code from module */
  #define DFMAP_DOSTATS

	/* Region flag types */
  #define DFMAP_REGION_NONE		0
  #define DFMAP_REGION_PITEM		1
  #define DFMAP_REGION_DITEM		2
  #define DFMAP_REGION_TABLE		4
  #define DFMAP_REGION_NAMES		8
  #define DFMAP_REGION_POFFSET		16
  #define DFMAP_REGION_DOFFSET		32
  #define DFMAP_REGION_NUMLOCATIONS	64
  #define DFMAP_REGION_NUMDUNGEONS	128
  #define DFMAP_REGION_ALL		255
  #define DFMAP_REGION_MINIMUM		(4|8|64|128)

	/* Dump flags */
  #define DFMAP_DUMP_NONE		0
  #define DFMAP_DUMP_MAPTABLE		1
  #define DFMAP_DUMP_MAPTABLE_UNKNOWNS  2
  #define DFMAP_DUMP_REGION		4
  #define DFMAP_DUMP_MAPNAMES		8
  #define DFMAP_DUMP_PITEMS		16
  #define DFMAP_DUMP_DITEMS		32
  #define DFMAP_DUMP_PITEMS_UNKNOWNS	64
  #define DFMAP_DUMP_ITEMINFO		128
  #define DFMAP_DUMP_UNKNOWNS           (2|64)
  #define DFMAP_DUMP_DEFAULT		(1|4|8|128)
  #define DFMAP_DUMP_ALL		(0xffff)

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

	/* Used for dumping object information */
  typedef int dfmap_dumpflags_t;

	/* Type defining which fields/records in a region have 
	 * been loaded and are valid. */
  typedef int dfmap_regionflags_t;

	/* PItem record offset type */
  typedef long dfmap_pitem_offset_t;

	/* DItem record offset type */
  typedef struct {
    long  Offset;
    short Unknown1;
    short Unknown2;
   } dfmap_ditem_offset_t;

	/* PItem pre-header record type */
  typedef struct {
    byte Data[6];
   } dfmap_pitem_prerecord_t;

   	/* PItem post record type, 26 bytes */
  typedef struct {
    ushort HouseName;
    byte   NullValues[16];
    ushort Unknown1;
    ushort Unknown2;
    ushort LocationID;
    byte   HouseType;
    byte   Unknown3;
   } dfmap_pitem_postrecord_t;

	/* DItem dungeon post record type, 4 bytes */
  typedef struct {
    byte Unknowns[4];
   } dfmap_ditem_postrecord_t;

	/* PItem record post-header information */
  typedef struct {
    byte		      Unknowns2[5];
    dfmap_pitem_postrecord_t* pPostRecords;
    char		      AnotherName[DFMAP_PITEM_NAMESIZE];
    long		      Unknown6;
    byte		      ExtraBytes[DFMAP_PITEM_EXTRABYTES];
   } dfmap_pitem_postinfo_t;

	/* DItem record post-header information */
  typedef struct {
    byte		      Unknowns2[5];
    dfmap_ditem_postrecord_t* pPostRecords;
    char		      AnotherName[DFMAP_PITEM_NAMESIZE];
    byte		      ExtraBytes[DFMAP_PITEM_EXTRABYTES];
   } dfmap_ditem_postinfo_t;

	/* PItem header information */
  typedef struct {
    long   OneValue1;
    short  NullValue1;
    char   NullValue2;
    long   XPosition;
    long   NullValue3;
    long   YPosition;
    long   Unknown1;
    long   Unknown2;
    long   Unknown3;
    short  OneValue2;
    ushort LocationID;
    long   NullValue4;
    short  Unknown4;
    long   Unknown5;
    byte   NullValues[26];
    char   Name[DFMAP_PITEM_NAMESIZE];
   } dfmap_pitem_header_t;

	/* PItem extra bytes at end  */
  typedef struct {
    char AnotherName[32];
    long UnknownID;
    byte Unknowns1[4];
    byte BlockWidth;
    byte BlockHeight;
    byte Unknowns2[7];
    byte BlockFileIndex[64];
    byte BlockFileNumber[64];
    byte BlockFileChar[64];
    byte Unknowns3[32];
    byte Unknown4;
    byte Unknown5;
    byte NullValues1[9];
    long Unknowns6[22];
    byte NullValues2[40];
    long Unknown7;
   } dfmap_pitem_footer_t;


	/* Map table record structure */
  typedef struct {
    byte   Unknown1;
    long   Unknown2;
    uint   XPosition 	 : 17;
    uint   LocationType  : 15;
    ushort YPosition;
    short  Unknown3;
    long   Unknown4;
   } dfmap_table_t;

	/* Map name record */
  typedef struct {
    char Name[DFMAP_PITEM_NAMESIZE];
   } dfmap_name_t;

#ifdef _WIN32
  #pragma pack(pop, save_pack)
#endif
/*===========================================================================
 *		End of Type and Structure Definitions
 *=========================================================================*/


/*===========================================================================
 *
 * Class CDFMapItemRecord
 *
 *=========================================================================*/
class CDFMapItemRecord {

  /*---------- Begin Protected Class Members ----------------------*/
protected:
  long			    RecordOffset;	/* Position of record in file */
  long			    RecordSize;		/* Size of record in file */
  boolean		    Dungeon;		/* Indicates a P/DItem record */
  dfmap_pitem_prerecord_t*  pPreRecords;	/* Pre-header records */
  long			    NumPreRecords;
  dfmap_pitem_header_t	    Header;		/* Header information */
  byte 			    Unknowns1[9];
  ushort		    NumPostRecords;

  
  /*---------- Begin Protected Class Methods ----------------------*/
protected:

	/* Internal creation functions */
  virtual void CreatePostRecords (void) { return; }
  virtual void CreatePreRecords (void);

	/* Input helper functions */
  virtual boolean ReadExtra       (FILE* pFileHandle) { return (FALSE); }
  virtual boolean ReadHeader      (FILE* pFileHandle);
  virtual boolean ReadPostHeader  (FILE* pFileHandle);
  virtual boolean ReadPreRecords  (FILE* pFileHandle);
  virtual boolean ReadPostRecords (FILE* pFileHandle) { return (FALSE); }


  /*---------- Begin Public Class Methods --------------------------*/
public:

	/* Class Constructor/Destructor */
  CDFMapItemRecord();
  virtual~CDFMapItemRecord() { Destroy(); }
  virtual void Destroy (void);
  virtual void Clear (void);

	/* Output object information to file stream */
  virtual void Dump (FILE* pFileHandle, const dfmap_dumpflags_t DumpFlags);

	/* Get class members */
  long   GetXPosition  (void) { return (Header.XPosition); }
  long   GetZPosition  (void) { return (Header.YPosition); }
  ushort GetLocationID (void) { return (Header.LocationID); }
  char*  GetName       (void) { return (Header.Name); }

	/* Read PItem record from a file stream */
  boolean Read (FILE* pFileHandle);

	/* Set class members */
  void SetRecordSize (const long Size) { RecordSize = Size; }

 };
/*===========================================================================
 *		End of Class CDFMapItemRecord
 *=========================================================================*/


/*===========================================================================
 *
 * Class CDFMapPItemRecord
 *
 *=========================================================================*/
class CDFMapPItemRecord : public CDFMapItemRecord {

  /*---------- Begin Protected Class Members ----------------------*/
protected:
  byte			    Unknowns2[5];
  dfmap_pitem_postrecord_t* pPostRecords;
  dfmap_pitem_footer_t      Footer;

  
  /*---------- Begin Protected Class Methods ----------------------*/
protected:

	/* Internal creation functions */
  void CreatePostRecords (void);

	/* Input helper functions */
  boolean ReadExtra       (FILE* pFileHandle);
  boolean ReadPostHeader  (FILE* pFileHandle);
  boolean ReadPostRecords (FILE* pFileHandle);


  /*---------- Begin Public Class Methods --------------------------*/
public:

	/* Class Constructor/Destructor */
  CDFMapPItemRecord();
  ~CDFMapPItemRecord() { Destroy(); }
  void Destroy (void);
  void Clear (void);

	/* Output object information to file stream */
  void Dump (FILE* pFileHandle, const dfmap_dumpflags_t DumpFlags);

	/* Get class members */
  dfmap_pitem_footer_t*     GetFooter     (void) { return (&Footer); }
  dfmap_pitem_postrecord_t* GetPostRecord (const int Index) { if (Index < 0 || Index >= NumPostRecords || pPostRecords == NULL) return (NULL); return (&pPostRecords[Index]); }

 };
/*===========================================================================
 *		End of Class CDFMapPItemRecord
 *=========================================================================*/


/*===========================================================================
 *
 * Class CDFMapDItemRecord
 *
 *=========================================================================*/
class CDFMapDItemRecord : public CDFMapItemRecord {

  /*---------- Begin Protected Class Members ----------------------*/
protected:
  long			    Unknown6;
  long			    Unknown7;
  dfmap_ditem_postrecord_t* pPostRecords;
  ushort		    NumDungeonPostRecords;			
  ushort		    Unknown8;
  ushort		    Unknown9;
  ushort		    Unknown10;
  byte			    ExtraBytes[DFMAP_DITEM_EXTRABYTES];
  int			    ExtraBytesSize;

  
  /*---------- Begin Protected Class Methods ----------------------*/
protected:

	/* Internal creation functions */
  void CreatePostRecords (void);

	/* Input helper functions */
  boolean ReadExtra       (FILE* pFileHandle);
  boolean ReadPostHeader  (FILE* pFileHandle);
  boolean ReadPostRecords (FILE* pFileHandle);


  /*---------- Begin Public Class Methods --------------------------*/
public:

	/* Class Constructor/Destructor */
  CDFMapDItemRecord();
  ~CDFMapDItemRecord() { Destroy(); }
  void Destroy (void);
  void Clear (void);

	/* Output object information to file stream */
  void Dump (FILE* pFileHandle, const dfmap_dumpflags_t DumpFlags);

 };
/*===========================================================================
 *		End of Class CDFMapDItemRecord
 *=========================================================================*/


/*===========================================================================
 *
 * Class CDFMapTableRecord Definition
 *
 *=========================================================================*/
class CDFMapTableRecord {

  /*---------- Begin Protected Class Members ----------------------*/
protected:
  long		RecordOffset;	/* Position of record in file */
  dfmap_table_t TableData;	/* The table record data */


  /*---------- Begin Public Class Methods --------------------------*/
public:

	/* Class Constructor/Destructor */
  CDFMapTableRecord();
  ~CDFMapTableRecord() { Destroy(); }
  void Destroy (void);

	/* Output object information to a file stream */
  void Dump (FILE* pFileHandle, const dfmap_dumpflags_t DumpFlags);

	/* Get class members */
  long GetXPosition    (void) { return (TableData.XPosition); }
  long GetYPosition    (void) { return (TableData.YPosition); }
  int  GetLocationType (void) { return (TableData.LocationType); }

	/* Read table record from a file stream */
  boolean Read (FILE* pFileHandle);

 };
/*===========================================================================
 *		End of Class CDFMapTableRecord
 *=========================================================================*/


/*===========================================================================
 *
 * Class CDFMapRegion Definition
 *
 *=========================================================================*/
class CDFMapRegion {

  /*---------- Begin Protected Class Members ----------------------*/
protected:
  int RegionNumber;		/* The unique region number (00-61) */
  int NumLocations;		/* Total number of locations in this region */
  int NumDungeons;		/* Total number of dungeons in this region */
  
	/* The region data */
  dfmap_pitem_offset_t* pPItemOffsets;
  dfmap_ditem_offset_t* pDItemOffsets;
  CDFMapPItemRecord*    pPItemRecords;
  CDFMapDItemRecord*    pDItemRecords;
  CDFMapTableRecord*    pMapTables;
  dfmap_name_t*	        pMapNames;

  dfmap_regionflags_t   RegionFlags; 

	/* File offsets */
  long MapPItemOffset;
  long MapDItemOffset;
  long MapTableOffset;
  long MapNamesOffset;


  /*---------- Begin Protected Class Methods ----------------------*/
protected:

	/* Helper input functions */
  boolean ReadPItemOffsets    (FILE* pFileHandle);
  boolean ReadPItemRecords    (FILE* pFileHandle);
  boolean ReadDItemOffsets    (FILE* pFileHandle);
  boolean ReadDItemRecords    (FILE* pFileHandle);
  boolean ReadMapTableRecords (FILE* pFileHandle);
  boolean ReadMapNamesRecords (FILE* pFileHandle);
  boolean ReadNumLocations    (FILE* pFileHandle);
  boolean ReadNumDungeons     (FILE* pFileHandle);


  /*---------- Begin Public Class Methods --------------------------*/
public:

	/* Class Constructor/Destructor */
  CDFMapRegion();
  ~CDFMapRegion() { Destroy(); }
  void Destroy (void);

  	/* Output object information to a file stream */
  void Dump (FILE* pFileHandle, const dfmap_dumpflags_t DumpFlags);

	/* Find a location by its maptable coordinates */
  boolean FindLocation (const int PixelX, const int PixelY, int& RegionIndex, int& LocationIndex);

	  /* Retrieve location data */
  CDFMapPItemRecord* GetMapPItem (const int LocationIndex);
  CDFMapDItemRecord* GetMapDItem (const int LocationIndex);
  CDFMapTableRecord* GetMapTable (const int LocationIndex);
  char*		     GetMapName  (const int LocationIndex);

	/* Is a particular part of the region loaded? */
  boolean IsLoaded        (const dfmap_regionflags_t Flags) { return (( (Flags & RegionFlags) == 0) ? FALSE : TRUE);}
  boolean IsValidLocation (const int Index)		    { return ((Index >= 0 && Index < NumLocations) ? TRUE : FALSE); }
  boolean IsValidDungeon  (const int Index)		    { return ((Index >= 0 && Index < NumDungeons)  ? TRUE : FALSE); }

	/* Read all or a portion of the region */
  boolean Read (FILE* pFileHandle, const dfmap_regionflags_t Flags);

  	/* Set class members */
  void SetMapPItemOffset (const long Offset) { MapPItemOffset = Offset; }
  void SetMapDItemOffset (const long Offset) { MapDItemOffset = Offset; }
  void SetMapTableOffset (const long Offset) { MapTableOffset = Offset; }
  void SetMapNamesOffset (const long Offset) { MapNamesOffset = Offset; }
  void SetRegionNumber   (const int  Number) { RegionNumber   = Number; }

 };
/*===========================================================================
 *		End of Class CDFMapRegion Definition
 *=========================================================================*/	


/*===========================================================================
 *
 * Class CDFMapsFile Definition
 *
 *=========================================================================*/
class CDFMapsFile : public CDFBSAFile {

  /*---------- Begin Protected Class Members -----------------------*/
protected:
  CDFMapRegion Regions[DFMAP_MAX_REGIONS];


  /*---------- Begin Protected Class Members -----------------------*/
protected:

  	/* Reads the BSA directory of a file */
  boolean ReadDirectory (void);


  /*---------- Begin Public Class Methods --------------------------*/
public:

	/* Class Constructor/Destructor */
  CDFMapsFile();
  ~CDFMapsFile() { Destroy(); }
  void Destroy (void);

  	/* Output object information to a file stream */
  void Dump (FILE* pFileHandle, const dfmap_dumpflags_t DumpFlags);

	/* Attempt to find a location in the given land pixel */
  boolean FindLandPixel (const int PixelX, const int PixelY, int& RegionIndex, int& LocationIndex);

	/* Retrieve location data */
  CDFMapPItemRecord* GetMapPItem (const int RegionIndex, const int LocationIndex);
  CDFMapDItemRecord* GetMapDItem (const int RegionIndex, const int LocationIndex);
  CDFMapTableRecord* GetMapTable (const int RegionIndex, const int LocationIndex);
  char*		     GetMapName  (const int RegionIndex, const int LocationIndex);

	/* Check a region index for validity */
  boolean IsValidRegion (const int Index) { if (Index >= 0 && Index < DFMAP_MAX_REGIONS) return (TRUE); return (FALSE); }

	/* Read all or a portion of a region */
  boolean ReadRegion (const int RegionIndex, const dfmap_regionflags_t Flags);
  boolean Read       (const dfmap_regionflags_t Flags);

	/* Update the region file offset values */
  void SetRegionOffsets (void);

 };
/*===========================================================================
 *		End of Class CDFMapsFile Definition
 *=========================================================================*/


/*===========================================================================
 *
 * External Variables
 *
 *=========================================================================*/

  #ifdef DFMAP_DOSTATS
    extern long MinPreRecordCount;
    extern long MaxPreRecordCount;

		/* Function to output stats to system log file */
    void OutputStats (void);
  #endif

/*===========================================================================
 *		End of External Variables
 *=========================================================================*/

#endif
/*===========================================================================
 *		End of File DFMaps.H
 *=========================================================================*/