/*===========================================================================
 *
 * DFIMG.CPP - Dave Humphrey (4 November 2000), uesp@m0use.net
 *
 *=========================================================================*/

	/* Include Files */
#include "dfimg.h"
#include "dfcommon.h"


#undef  __FUNC__
#define __FUNC__ "CDFImgFile::CDFImgFile()"
/*===========================================================================
 *
 * Class CDFImgFile Constructor
 *
 *=========================================================================*/
CDFImgFile::CDFImgFile() {
  pData = NULL;
  Width = 0;
  Height = 0;
  XOffset = 0;
  YOffset = 0;
  Unknown = 0;
  ImageSize = 0;
  CIFGroupIndex = 0;
  HasOffsetList = FALSE;
  HasNoHeader = FALSE;
  HasCIFWeaponHeader = FALSE;
  DestroyOnRead = FALSE;
 }
/*===========================================================================
 *		End of Class CDFImgFile Constructor
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFImgFile::Destroy()"
/*===========================================================================
 *
 * Class CDFImgFile Destructor
 *
 *=========================================================================*/
void CDFImgFile::Destroy (void) {
  DestroyPointer(pData);

  Width = 0;
  Height = 0;
  XOffset = 0;
  YOffset = 0;
  Unknown = 0;
  ImageSize = 0;
  HasOffsetList = FALSE;
  HasNoHeader = FALSE;
  HasCIFWeaponHeader = FALSE;
  CIFGroupIndex = 0;
 }
/*===========================================================================
 *		End of Class CDFImgFile Destructor
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFImgFile::Load()"
/*===========================================================================
 *
 * Class CDFImgFile Method - boolean Load (pFilename);
 *
 * Open and read a IMG file. Returns FALSE on any error.
 *
 *=========================================================================*/
boolean CDFImgFile::Load (const char* pFilename) {
  FILE*   pFileHandle;
  boolean Result;
  long    FileSize;

	/* Attempt to open file for input */
  pFileHandle = openfile (pFilename, "rb");

  if (pFileHandle == NULL) {
    SET_EXT_ERROR3(ERR_FILE, "Failed to open file %s!", pFilename);
    return (FALSE);
   }

	/* Delete the current contents */
  Destroy();

	/* Check special IMG files */
  FileSize = GetFilesize(pFileHandle);

  if (FileSize == 720) {
    HasNoHeader = TRUE;
    Width = 9;
    Height = 80;
    ImageSize = 720;
   }
  else if (FileSize == 990) {
    HasNoHeader = TRUE;
    Width = 45;
    Height = 22;
    ImageSize = 990;
   }
  else if (FileSize == 1720) {
    HasNoHeader = TRUE;
    Width = 43;
    Height = 40;
    ImageSize = 1720;
   }
  else if (FileSize == 2140) {
    HasNoHeader = TRUE;
    Width = 107;
    Height = 20;
    ImageSize = 2140;
   }
  else if (FileSize == 2916) { 
    HasNoHeader = TRUE;
    Width = 81;
    Height = 36;
    ImageSize = 2916;
   }
  else if (FileSize == 3200) { 
    HasNoHeader = TRUE;
    Width = 40;
    Height = 80;
    ImageSize = 3200;
   }
  else if (FileSize == 3938) {
    HasNoHeader = TRUE;
    Width = 179;
    Height = 22;
    ImageSize = 3938;
   }
  else if (FileSize == 4280) {
    HasNoHeader = TRUE;
    Width = 107;
    Height = 40;
    ImageSize = 4280;
   }
  else if (FileSize == 20480) { 
    HasNoHeader = TRUE;
    Width = 320;
    Height = 64;
    ImageSize = 20480;
   }
  else if (FileSize == 26496) { 
    HasNoHeader = TRUE;
    Width = 207;
    Height = 128;
    ImageSize = 26496;
   }
  else if (FileSize == 64000) {
    HasNoHeader = TRUE;
    Width = 320;
    Height = 200;
    ImageSize = 64000;
   }
  else if (FileSize == 64768) {
    HasNoHeader = TRUE;
    Width = 320;
    Height = 200;
    ImageSize = 64000;
   }
  else if (FileSize == 68800) {
    HasNoHeader = TRUE;
    Width = 320;
    Height = 215;
    ImageSize = 68800;
   }
  else if (FileSize == 112128) {
    HasNoHeader = TRUE;
    Width = 512;
    Height = 219;
    ImageSize = 112128;
   }

	/* Read the image file */
  Result = Read(pFileHandle);

  fclose (pFileHandle);
  return (Result);
 }
/*===========================================================================
 *		End of Class Method CDFImgFile::Load()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFImgFile::Read()"
/*===========================================================================
 *
 * Class CDFImgFile Method - boolean Read (pFileHandle);
 *
 * Read a IMG file from the current position in the given file stream.
 * Returns FALSE on any error.
 *
 *=========================================================================*/
boolean CDFImgFile::Read (FILE* pFileHandle) {
  int Result;
  
	/* Ensure valid input */
  if (pFileHandle == NULL) {
    SET_EXT_ERROR2(ERR_NULL, "Invalid NULL file handle received!");
    return (FALSE);
   }

	/* Delete the current image data (only for CIF files) */
  if (DestroyOnRead) Destroy();

	/* Read in the image header */
  Result = ReadHeader(pFileHandle);

	/* Skip the weapon CIF offset list, if present */
  if (HasOffsetList) {
    fseek (pFileHandle, DFCIF_OFFSETLIST_SIZE, SEEK_CUR);
   }

  if (Result) {
    if (IsCompressed())
      Result = ReadCompressedData(pFileHandle);
    else
      Result = ReadData(pFileHandle);
   }

  return (Result);
 }
/*===========================================================================
 *		End of Class Method CDFImgFile::Read()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFImgFile::ReadHeader()"
/*===========================================================================
 *
 * Class CDFImgFile Method - boolean ReadHeader (pFileHandle);
 *
 * Protected class method which reads in the img file header from the
 * current position in the file stream. Returns FALSE on any error.
 *
 *=========================================================================*/
boolean CDFImgFile::ReadHeader (FILE* pFileHandle) {

	/* Ensure valid parameters if not loading a header */
  if (HasNoHeader) {
    if (Width == 0 || Height == 0) {
      SET_EXT_ERROR2(ERR_INPUT, "Invalid image width/height with no header!");
      return (FALSE);
     }

    return (TRUE);
   }

	/* Read in the image header variables */
  clearerr(pFileHandle);

  if (HasCIFWeaponHeader) {
    Width     = read_short(pFileHandle);
    Height    = read_short(pFileHandle);
    XOffset   = read_short(pFileHandle);
    YOffset   = read_short(pFileHandle);
    Unknown   = read_short(pFileHandle);
    ImageSize = (int) (unsigned short) read_short(pFileHandle);
   }
  else {
    XOffset   = read_short(pFileHandle);
    YOffset   = read_short(pFileHandle);
    Width     = read_short(pFileHandle);
    Height    = read_short(pFileHandle);
    Unknown   = read_short(pFileHandle);
    ImageSize = (int) (unsigned short) read_short(pFileHandle);
   }

  	/* Check the success of the input */
  if (ferror(pFileHandle) || feof(pFileHandle)) {
    SET_EXT_ERROR2(ERR_READ, "Failed to read header information!");
    return (FALSE);
   }

  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFImgFile::ReadHeader()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFImgFile::ReadData()"
/*===========================================================================
 *
 * Class CDFImgFile Method - boolean ReadData (pFileHandle);
 *
 * Protected class method which reads in the uncompressed image data from the
 * current position in the file stream. Returns FALSE on any error.
 *
 *=========================================================================*/
boolean CDFImgFile::ReadData (FILE* pFileHandle) {
  int Result;
  int Size = Width*Height;
  
	/* Allocate the image data */
  CreatePointerArray(pData, byte, Size);

	/* Read in image data all at once */
  Result = fread (pData, 1, Size, pFileHandle);

  if (Result != (int) Size) {
    SET_EXT_ERROR4(ERR_READ, "Error reading image data (%d of %d bytes)!", Result, Size);
    return (FALSE);
   }

  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFImgFile::ReadData()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFImgFile::ReadCompressedData()"
/*===========================================================================
 *
 * Class CDFImgFile Method - boolean ReadCompressedData (pFileHandle);
 *
 * Protected class method which reads in the compressed image data from the
 * current position in the file stream. Returns FALSE on any error.
 *
 *=========================================================================*/
boolean CDFImgFile::ReadCompressedData (FILE* pFileHandle) {
  int   Size = Width*Height;
  int   BytesWritten = 0;
  int   LoopCounter;
  int   ByteCount;
  byte* pImagePtr;
  int   Data;
  
	/* Allocate the image data */
  CreatePointerArray(pData, byte, Size);
  pImagePtr = pData;

	/* Uncompress the data to image buffer */
  while (BytesWritten < Size) { 
    clearerr(pFileHandle);
    Data = fgetc(pFileHandle);

		/* RLE type byte indicator */
    if (Data > 0x7f) {
      ByteCount = (Data & 0xFF) - 0x7f;
      BytesWritten += ByteCount;
      Data = fgetc(pFileHandle);

      for (LoopCounter = 0; LoopCounter < ByteCount; LoopCounter++) {
        *pImagePtr = Data & 0xFF;
	pImagePtr++;
       }

     }
		/* Unencoded pixel data */
    else {
      ByteCount = (Data & 0xFF) + 1;
      BytesWritten += ByteCount;

		/* Copy pixel values */
      for (LoopCounter = 0; LoopCounter < ByteCount; LoopCounter++) {
        *pImagePtr = fgetc(pFileHandle) & 0xFF;
	pImagePtr++;
       }
     }

		/* Check for input errors */
    if (ferror(pFileHandle) || feof(pFileHandle)) { 
      SET_EXT_ERROR2(ERR_READ, "Error reading image data!");
      return (FALSE);
     }
   } /* End of while uncompressed data */ 


  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFImgFile::ReadCompressedData()
 *=========================================================================*/