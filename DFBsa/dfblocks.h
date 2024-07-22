/*===========================================================================
 *
 * DFBlocks.H - Dave Humphrey (uesp@m0use.net), 16 November 2000
 *
 *=========================================================================*/
#ifndef __DFBLOCKS_H
#define __DFBLOCKS_H

	/* Required Includes */
#include "dfbsa.h"


/*===========================================================================
 *
 * Begin Defines
 *
 *=========================================================================*/
 
	/* Number of records allowed in a RMB file */
  #define DFBLK_MAX_RMBRECORDS 256

	/* Number of records in the Blocks.BSA file */
  #define DFBLK_MAX_RECORDS  1400

	/* For loading RMB files via data found in MAPS.BSA */
  #define DFBLK_MAX_FILEINDEX  45
  #define DFBLK_MAX_FILENUMBER 57

	/* Options for creating blocks */
  #define DFBLK_FLAG_OUTSIDE   1
  #define DFBLK_FLAG_INSIDE    2
  #define DFBLK_FLAG_EXTRA     4
  #define DFBLK_FLAG_3DOBJECTS 8
  #define DFBLK_FLAG_FLATS     16
  #define DFBLK_FLAG_PEOPLE    32
  #define DFBLK_FLAG_DOORS     64
  #define DFBLK_FLAG_DEBUG     128
  #define DFBLK_FLAG_ISINSIDE  256 
  #define DFBLK_FLAG_DEFAULT   (1|2|8|16|32|64|128)
  

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

	/* Flags used to indicate what objects to create */
  typedef int dfblk_flag_t;

	/* RMB Fixed Length Data Header record */
  typedef struct {
    long Width;
    long Height;
    long XPos;
    long ZPos;
    long YPos;
   } dfblk_rmbfld_position_t;


	/* RMB Fixed Length Data Section1 record */
  typedef struct {
    byte Unknowns[26];
   } dfblk_rmbfld_section1_t;

	/* RMB Fixed Length Data Section2 record */
  typedef struct {
    byte Unknowns[4];
   } dfblk_rmbfld_section2_t;

	/* RMB Fixed Length Data Block sizes record */
  typedef struct {
    long RecordSize;
   } dfblk_rmbfld_blocksize_t;

	/* Bit structure for the small map type in the FLD */
  typedef struct {
    byte ImageIndex	: 6;
    byte RotateImage	: 1;
    byte FlipImage	: 1;
   } dfblk_textureinfo_t;

   	/* RMB Fixed Length Data Small Map record */
  typedef struct {
    byte Header[8];

    union {
      dfblk_textureinfo_t Textures[256];
      dfblk_textureinfo_t TexturesXY[16][16];
     };
    union {
      byte Data2[256];
      byte Data2XY[16][16];
     };
   } dfblk_rmbfld_smallmap_t;

   	/* RMB Fixed Length Data AutoMap record */
  typedef struct {
    union {
      byte Map[4096];
      byte MapXY[64][64];
     };
   } dfblk_rmbfld_automap_t;

	/* RMB Fixed Length Data Filename record */
  typedef struct {
    char Filename[13];
   } dfblk_rmbfld_filename_t;

	/* Main RMB Fixed length data record */
  typedef struct {
    byte		     NumBlocks;
    byte		     Num3DObjects;
    byte		     NumFlatObjects;
    dfblk_rmbfld_position_t  PositionRecords[32];
    dfblk_rmbfld_section1_t  Section1Records[32];
    dfblk_rmbfld_section2_t  Section2Records[32];
    dfblk_rmbfld_blocksize_t BlockSizes[32];
    dfblk_rmbfld_smallmap_t  SmallMap;
    dfblk_rmbfld_automap_t   AutoMap;
    dfblk_rmbfld_filename_t  FilenameRecords[33];
   } dfblk_rmbfld_t;

	/* RMB Record Header */
  typedef struct {
    byte   Num3DObjectRecords;
    byte   NumFlatObjectRecords;
    byte   NumSection3Records;
    byte   NumPeopleRecords;
    byte   NumDoorRecords;
    ushort Unknowns[6];
   } dfblk_rmb_header_t;

	/* RMB 3D Object Type Record */
  typedef struct {
    ushort ObjectID1;
    byte   ObjectID2;
    byte   Unknown1;
    long   Unknown2;
    long   Unknown3;
    long   Unknown4;
    long   NullValue1;
    long   NullValue2;
    long   XPos1;
    long   YPos1;
    long   ZPos1;
    long   XPos2;
    long   YPos2;
    long   ZPos2;
    long   NullValue3;
    short  Angle;
    short  Unknown5;
    long   NullValue4;
    long   Unknown6;
    short  NullValue5;
   } dfblk_rmb_3dobject_t;

	/* RMB Flat Object Type Record */
  typedef struct {
    long   XPos;
    long   YPos;
    long   ZPos;
    ushort SubImageIndex	: 7;
    ushort TextureIndex		: 9;
    ushort FlatType;
    byte   Unknown3;
   } dfblk_rmb_flatobject_t;

	/* RMB Section3 Type Record */
  typedef struct {
    long   XPos;
    long   YPos;
    long   ZPos;
    byte   Unknown1;
    byte   Unknown2;
    ushort Unknown3;
   } dfblk_rmb_section3_t;

	/* RMB People Type Record */
  typedef struct {
    long   XPos;
    long   YPos;
    long   ZPos;
    ushort SubImageIndex	: 7;
    ushort TextureIndex		: 9;
    ushort NPCType;
    byte   Unknown3;
   } dfblk_rmb_people_t;

	/* RMB Door Type Record */
  typedef struct {
    long   XPos;
    long   YPos;
    long   ZPos;
    ushort Unknown1;
    ushort Unknown2;
    ushort Unknown3;
    byte   Unknown4;
   } dfblk_rmb_door_t;

#ifdef _WIN32
  #pragma pack(pop, save_pack)
#endif

/*===========================================================================
 *		End of Structure and Type Definitions
 *=========================================================================*/


/*===========================================================================
 *
 * Class CDFRMBRecord Definition
 *
 * Defines one variable sized record in a RMB file.
 *
 *=========================================================================*/
class CDFRMBRecord { 

  /*---------- Begin Protected Class Members --------------------*/
protected:
  dfblk_rmb_header_t	  Header;		/* Header data for variable length records */
  dfblk_rmb_3dobject_t*   p3DObjectRecords;	
  dfblk_rmb_flatobject_t* pFlatObjectRecords;	
  dfblk_rmb_section3_t*   pSection3Records;	
  dfblk_rmb_people_t*     pPeopleRecords;	
  dfblk_rmb_door_t*       pDoorRecords;		
  boolean		  HasExtraByte;		/* Extra 1 byte at end of data */
  byte			  ExtraByte;


  /*---------- Begin Protected Class Members --------------------*/
protected:

	/* Input helper functions */
  boolean ReadHeader      (FILE* pFileHandle);
  boolean Read3DObjects   (FILE* pFileHandle);
  boolean ReadFlatObjects (FILE* pFileHandle);
  boolean ReadSection3    (FILE* pFileHandle);
  boolean ReadPeople      (FILE* pFileHandle);
  boolean ReadDoors       (FILE* pFileHandle);


  /*---------- Begin Public Class Methods -----------------------*/
public:

	/* Class Constructor/Destructor */
  CDFRMBRecord();
  virtual ~CDFRMBRecord() { Destroy(); }
  virtual void Destroy (void);

	/* Read in the record data from the current position in a file stream */
  virtual boolean Read (FILE* pFileHandle);

	/* Set class members */
  void SetExtraByte (const byte Byte) { HasExtraByte = TRUE; ExtraByte = Byte; }

 };
/*===========================================================================
 *		End of Class CDFRMBRecord Definition
 *=========================================================================*/


/*===========================================================================
 *
 * Class CDFRMBFile Definition
 *
 *=========================================================================*/
class CDFRMBFile { 

  /*---------- Begin Protected Class Members --------------------*/
protected:
  dfblk_rmbfld_t  FLDData;	/* The Fixed Length Data */
  long		  RecordOffset;	/* Offset of record in file */
  long		  RecordSize;	/* Size of record in file */

		/* Variable Records Data */
  CDFRMBRecord*		  pOutsideRecords;
  CDFRMBRecord*		  pInsideRecords;
  dfblk_rmb_3dobject_t*   p3DObjects;
  dfblk_rmb_flatobject_t* pFlatObjects;
  

  /*---------- Begin Protected Class Members --------------------*/
protected:

	/* Allocate the block record arrays */
  virtual void CreateBlockRecords (void);

	/* Input helper functions */
  boolean ReadFLD         (FILE * pFileHandle);
  boolean ReadBlocks      (FILE * pFileHandle);
  boolean Read3DObjects   (FILE * pFileHandle);
  boolean ReadFlatObjects (FILE * pFileHandle); 


  /*---------- Begin Public Class Methods -----------------------*/
public:

	/* Class CDFRMBFile Constructor */
  CDFRMBFile();
  virtual ~CDFRMBFile() { Destroy(); }
  virtual void Destroy (void);

	/* Attempt to read in the file from the current position in the file stream */
  virtual boolean Read (FILE* pFileHandle, const long RecordSize);

	/* Set class members */
  void SetRecordOffset (const long Offset) { RecordOffset = Offset; }
  void SetRecordSize   (const long Size)   { RecordSize   = Size; }

 };
/*===========================================================================
 *		End of Class CDFRMBFile Definition
 *=========================================================================*/


/*===========================================================================
 *
 * Class CDFBlocksFile Definition
 *
 *=========================================================================*/
class CDFBlocksFile : public CDFBSAFile {

  /*---------- Begin Protected Class Members --------------------*/
protected:
  CDFRMBFile** pRecords;


  /*---------- Begin Protected Class Methods --------------------*/
protected:

	/* Create an RMB file object */
  void CreateRMBFile (const int Index);


  /*---------- Begin Public Class Methods -----------------------*/
public:

	/* Class constructor/destructor */
  CDFBlocksFile();
  ~CDFBlocksFile();
  void Destroy (void);

	/* Checks a directory entry for filename types */
  boolean IsRMBFile (const int Index);
  boolean IsRDIFile (const int Index);
  boolean IsRDBFile (const int Index);
 
  virtual CDFRMBFile* GetRMBFile (const char* pFilename);
  virtual CDFRMBFile* GetRMBFile (const int Index);
  virtual CDFRMBFile* GetRMBFile (const int FileIndex, const int CharIndex, const int Number);

	/* Attempt to input the specified RMB file */
  virtual boolean ReadRMB (const char* pFilename);
  virtual boolean ReadRMB (const int Index);

 };
/*===========================================================================
 *		End of Class CDFBlocksFile Definition
 *=========================================================================*/


/*===========================================================================
 *
 * Begin Function Prototypes
 *
 *=========================================================================*/

	/* Statistics mode exported functions */
  #ifdef DOSTATS
    void DFBlocksOutputStats (void);
  #endif

/*===========================================================================
 *		End of Function Prototypes
 *=========================================================================*/


#endif
/*===========================================================================
 *		End of File DFBlocks.H
 *=========================================================================*/