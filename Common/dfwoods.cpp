/*=========================================================================
 *
 * DFWoods.CPP - Dave Humphrey (uesp@m0use.net), 5 November 2000
 *
 *=======================================================================*/

	/* Include Files */
#include "dfwoods.h"
#include "dfcommon.h"
#include "profile.h"


#undef  __FUNC__
#define __FUNC__ "CDFWoodsPixel::CreateRandom()"
/*===========================================================================
 *
 * Class CDFWoodsPixel Method - void CreateRandom (X, Y);
 *
 * Create a random landscape record at the given pixel position.
 *
 *=========================================================================*/
void CDFWoodsPixel::CreateRandom (const int X, const int Y) {
  XPixel = X;
  YPixel = Y;
  RecordIndex = -1;
  RecordOffset = -1;
  Loaded = TRUE;
  Random = TRUE;

	/* Set to some random/preset values */
  SmallPixel = (byte) 3;
  memset(&PixelData, 3, sizeof(PixelData));
 }
/*===========================================================================
 *		End of Class Method CDFWoodsPixel::CreateRandom()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFWoodsPixel::Read()"
/*===========================================================================
 *
 * Class CDFWoodsPixel Method - boolean Read (pFileHandle);
 *
 * Read the pixel information from the open Woods.WLD file.
 * Returns FALSE on any error.
 *
 *=========================================================================*/
boolean CDFWoodsPixel::Read (FILE* pFileHandle) { 
  int  Result;

  PROFILE_INC();
  START_PROFILE(ReadPixel);
  
	/* Ensure valid input */
  if (pFileHandle == NULL) { 
    SET_EXT_ERROR2(ERR_NULL, "Invalid NULL file handle received!");
    return (FALSE);
   }

	/* Jump to the record data in file and read it */
  fseek (pFileHandle, RecordOffset, SEEK_SET);
  Result = fread (&PixelData, 1, DFWOODS_PIXELRECORD_SIZE, pFileHandle);

	/* Ensure a successful read */
  if (Result != DFWOODS_PIXELRECORD_SIZE) { 
    SET_EXT_ERROR4(ERR_READ, "Error reading pixel record (%d of %d bytes)!", Result, DFWOODS_PIXELRECORD_SIZE);
    return (FALSE);
   }

	/* Return success */
  Loaded = TRUE;
  Random = FALSE;

  END_PROFILE(ReadPixel);
  PROFILE_DEC();
  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFWoodsPixel::Read()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFWoodsFile::CDFWoodsFile"
/*===========================================================================
 *
 * Class CDFWoodsFile Constructor
 *
 *=========================================================================*/
CDFWoodsFile::CDFWoodsFile() { 
  Header.MapHeight = 0;
  Header.MapWidth = 0;
  Header.OffsetSize = 0;
  Header.Data1Offset = 0;
  Header.MapOffset = 0;
  HeaderRead = FALSE;
  pFileHandle = NULL;
  pWoodsFilename = NULL;
  SetFilename("arena2\\woods.wld");
 }
/*===========================================================================
 *		End of Class CDFWoodsFile Constructor
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFWoodsFile::Destroy"
/*===========================================================================
 *
 * Class CDFWoodsFile Destructor
 *
 *=========================================================================*/
void CDFWoodsFile::Destroy (void) { 

  Close();

  Header.MapHeight = 0;
  Header.MapWidth = 0;
  Header.OffsetSize = 0;
  Header.Data1Offset = 0;
  Header.MapOffset = 0;
  HeaderRead = FALSE;
 }
/*===========================================================================
 *		End of Class CDFWoodsFile Destructor
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFWoodsFile::ComputeRecordIndex()"
/*===========================================================================
 *
 * Class CDFWoodsFile Method - long ComputeRecordIndex (XPos, YPos);
 *
 * Compute the pixel record index for the given position.  Returns -1 on
 * any error.
 *
 *=========================================================================*/
long CDFWoodsFile::ComputeRecordIndex (const int XPos, const int YPos) {
  long Index;

  	/* Ensure a valid position */
  if (!IsValidXPos(XPos) || !IsValidYPos(YPos)) { 
    SET_EXT_ERROR4(ERR_INPUT, "Invalid position (%d, %d) received!", XPos, YPos);
    return (-1);
   }

	/* Compute the index */
  Index = (long) XPos + (long) YPos * (long) Header.MapWidth;

	/* Check for a valid index */
  if (!IsValidIndex(Index)) {
    SET_EXT_ERROR3(ERR_OVERFLOW, "Invalid record index %d!", Index);
    return (-1);
   }

  return (Index);
 }
/*===========================================================================
 *		End of Class Method CDFWoodsFile::ComputeRecordIndex()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFWoodsFile::Open()"
/*===========================================================================
 *
 * Class CDFWoodsFile Method - boolean Open (pFileName);
 *
 * Open the specified WOODS.WLD file, read the file header. Returns FALSE
 * on any error.
 *
 *=========================================================================*/
boolean CDFWoodsFile::Open (const char* pFilename) { 
  boolean Result;

	/* If already open return success */
  if (IsOpen()) return (TRUE);

	/* Delete any current information */
  Destroy();

	/* Attempt to open the file for binary input, use previous filename
	 * if none was specified. */
  if (pFilename == NULL) {
    pFileHandle = openfile (pWoodsFilename, "rb");
   }
  else {
    pFileHandle = openfile (pFilename, "rb");
    if (pFileHandle != NULL) SetFilename(pFilename);
   }

	/* Ensure the open operation was successfull */
  if (pFileHandle == NULL) {
    SET_EXT_ERROR3(ERR_FILE, "Failed to open file %s!", (pFilename == NULL) ? pWoodsFilename : pFilename);
    return (FALSE);
   }

  if (HeaderRead) return (TRUE);

	/* Attempt to read in the header information */
  Result = ReadHeader();
  if (!Result) Close();

  return (Result);
 }
/*===========================================================================
 *		End of Class Method CDFWoodsFile::Open()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFWoodsFile::ReadHeader()"
/*===========================================================================
 *
 * Class CDFWoodsFile Method - boolean ReadHeader (void);
 *
 * Read the file header information.  Returns FALSE on any error.
 *
 *=========================================================================*/
boolean CDFWoodsFile::ReadHeader (void) { 
  int Result;

	/* Ensure a valid file */
  if (!IsOpen()) {
    SET_EXT_ERROR2(ERR_FILE, "CDFWoodsFile::ReadHeader() - Woods file not open!");
    return (FALSE);
   }

	/* Jump to start of file */
  fseek (pFileHandle, 0, SEEK_SET);
	
	/* Attempt to read the header data all at once */
  Result = fread (&Header, 1, sizeof(dfwoods_header_t), pFileHandle);

  if (Result != sizeof(dfwoods_header_t)) {
    SET_EXT_ERROR4(ERR_READ, "CDFWoodsFile::ReadHeader() - Error reading header data (%d of %d bytes)!", Result, sizeof(dfwoods_header_t));
    return (FALSE);
   }

  HeaderRead = TRUE;  
  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFWoodsFile::ReadHeader()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFWoodsFile::ReadPixelRecord()"
/*===========================================================================
 *
 * Class CDFWoodsFile Method - boolean ReadPixelRecord (XPos, YPos, PixelRecord);
 *
 * Read the file header information.  Returns FALSE on any error.
 *
 *=========================================================================*/
boolean CDFWoodsFile::ReadPixelRecord (const int XPos, const int YPos, CDFWoodsPixel& PixelRecord) { 
  long RecordIndex;
  long RecordOffset;
  int  Result;

  PROFILE_INC();
  START_PROFILE(IsOpen);

	/* Ensure a valid file */
  if (!IsOpen()) {
    SET_EXT_ERROR2(ERR_FILE, "Woods file not open!");
    return (FALSE);
   }

  END_PROFILE(IsOpen);
  START_PROFILE(ComputeIndex);

	/* Compute which pixel record we wish to load */
  RecordIndex = ComputeRecordIndex(XPos, YPos);

  END_PROFILE(ComputeIndex);
  START_PROFILE(CheckRandom);

	/* Generate 'random' landscape data */
  if (RecordIndex < 0) {
    PixelRecord.CreateRandom (XPos, YPos);
    return (TRUE);
   }

  END_PROFILE(CheckRandom);
  START_PROFILE(ReadOffset);

  	/* Read the record offset in the file */
  RecordOffset = ReadRecordOffset(RecordIndex);
  if (RecordOffset < 0) return (FALSE);

  END_PROFILE(ReadOffset);
  START_PROFILE(SeekRecord);

	/* Jump to the small map offset to read single pixel data */
  fseek (pFileHandle, Header.MapOffset + RecordIndex, SEEK_SET);
  Result = fgetc(pFileHandle);

  if (Result < 0) {
    SET_EXT_ERROR2(ERR_READ, "Error reading single pixel data!");
    return (FALSE);
   }

  END_PROFILE(SeekRecord);
  START_PROFILE(SetRecord);

	/* Set the pixel record values */
  PixelRecord.SetSmallPixel ((byte)Result);
  PixelRecord.SetPosition (XPos, YPos);
  PixelRecord.SetIndex (RecordIndex);
  PixelRecord.SetOffset (RecordOffset);

  END_PROFILE(SetRecord);
  START_PROFILE(ReadPixelRecord);
  
	/* Read the pixel record and return result */
  Result = PixelRecord.Read(pFileHandle);

  END_PROFILE(ReadPixelRecord);
  PROFILE_DEC();
	
  return (Result);
 }
/*===========================================================================
 *		End of Class Method CDFWoodsFile::ReadPixelRecord()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFWoodsFile::ReadRecordOffset()"
/*===========================================================================
 *
 * Class CDFWoodsFile Method - long ReadRecordOffset (Index);
 *
 * Read the offset to the given record data.  Protected class method.
 * Returns -1 on any error.
 *
 *=========================================================================*/
long CDFWoodsFile::ReadRecordOffset (const long Index) { 
  long    Offset;
  boolean Result;

	/* Jump to the offset value in file */
  fseek (pFileHandle, DFWOODS_HEADER_SIZE + Index*4, SEEK_SET);

	/* Read in the offset value */
  Result = read_long(pFileHandle, Offset);

	/* Ensure a successful read */
  if (!Result) {
    SET_EXT_ERROR2(ERR_READ, "Error reading pixel record offset!");
    return (-1);
   }

  return (Offset);
 }
/*===========================================================================
 *		End of Class Method CDFWoodsFile::ReadPixelRecord()
 *=========================================================================*/