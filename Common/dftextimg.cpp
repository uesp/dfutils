/*===========================================================================
 *
 * DFTextImg.CPP - Dave Humphrey (uesp@m0use.net), 3 November 2000
 *
 *
 *=========================================================================*/

	/* Include Files */
#include "dftextimg.H"
#include "dfcommon.h"


#undef  __FUNC__
#define __FUNC__ "CDFTextureImage::CDFTextureImage()"
/*===========================================================================
 *
 * Class CDFTextureImage Constructor
 *
 *=========================================================================*/
CDFTextureImage::CDFTextureImage() {
  HeaderOffset = 0;
  NumSubImagesAllocated = 0;
 }
/*===========================================================================
 *		End of Class CTextureImage Constructor
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFTextureImage::Destroy()"
/*===========================================================================
 *
 * Class CDFTextureImage Destructor
 *
 *=========================================================================*/
void CDFTextureImage::Destroy (void) {
  int LoopCounter;

	/* Delete all allocated subimages */
  for (LoopCounter = 0; LoopCounter < NumSubImagesAllocated; LoopCounter++) {
    DestroyPointer(pData[LoopCounter]);
   }

  NumSubImagesAllocated = 0;
  HeaderOffset = 0;
 }
/*===========================================================================
 *		End of Class CDFTextureImage Destructor
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFTextureImage::Dump()"
/*===========================================================================
 *
 * Class CDFTextureImage Method - void Dump (DumpMask, FILE* pFileHandle);
 *
 * Output object information to a file stream according to the DumpMask flags.
 *
 *=========================================================================*/
void CDFTextureImage::Dump (FILE* pFileHandle, const dftexture_dumpmask_t DumpMask) {

	/* Ensure valid input */
  if (pFileHandle == NULL) {
    SET_EXT_ERROR2(ERR_NULL, "Invalid NULL file handle received!");
    return;
   }

	/* Ensure we wish to output any information */
  if ((DumpMask & DFTEXTURE_DM_IMAGE) == 0) return;

	/* Output basic and header information */
  fprintf (pFileHandle, "\tTexture Image Object 0x%p (0x%08lX)\n", this, HeaderOffset);

  if (GetNumSubImages() > 1 && GetUnknown2() != 0) fprintf (pFileHandle, "\t\tSubImages = %d,  Unknown2 = 0x%04X\n", GetNumSubImages(), GetUnknown2()&0xFFFF);

	/* Output header information if required */
  if (DumpMask & DFTEXTURE_DM_IMAGEHEADER) {
    if (DumpMask & DFTEXTURE_DM_IMAGEOFFSETS) fprintf (pFileHandle, "\t\tOffsets = %d, %d\n", GetXOffset(), GetYOffset());
    fprintf (pFileHandle, "\t\tSize = %d x %d (%d, Offset = 0x%08lX), %d SubImages\n", GetWidth(), GetHeight(), GetImageSize(), GetDataOffset(), GetNumSubImages());
    if (DumpMask & DFTEXTURE_DM_IMAGEUNKNOWNS) fprintf (pFileHandle, "\t\tUnknowns = 0x%04X 0x%04X 0x%08lX 0x%04X\n", GetUnknown1() & 0xFFFF, GetUnknown2()&0xFFFF, GetUnknown3(), GetUnknown4()&0xFFFF);
   }
  
  fflush(pFileHandle);  
 }
/*===========================================================================
 *		End of Class Method CDFTextureImage::Dump()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFTextureImage::Read()"
/*===========================================================================
 *
 * Class CDFTextureImage Method - boolean Read (pFileHandle);
 *
 * Attempt to read in the image header and data starting from the current
 * position in the file stream. Returns FALSE on any error.
 *
 *=========================================================================*/
boolean CDFTextureImage::Read (FILE* pFileHandle) {
  boolean Result;

	/* Ensure valid input */
  if (pFileHandle == NULL) { 
    SET_EXT_ERROR2(ERR_NULL, "Invalid NULL file handle received!");
    return (FALSE);
   }

	/* Delete the current contents */
  Destroy();

	/* Set the offset of the image header */
  HeaderOffset = ftell(pFileHandle);

  if (HeaderOffset <= 0) {
    SET_EXT_ERROR2(ERR_READ, "Invalid header offset received!");
    return (FALSE);
   }

	/* Attempt to input header information */
  Result = ReadHeader(pFileHandle);
  if (!Result) return (FALSE);
   
	/* Read the data information depending on the type of image
	 * (single or multiple subimages) */
  if (GetNumSubImages() > 1 && IsRLECompressed()) {
    Result = ReadRLESubImageData(pFileHandle);
   }
  else if (GetNumSubImages() > 1) {
    Result =  ReadSubImageInfo(pFileHandle);
    if (Result) Result = ReadSubImageData(pFileHandle);
   }
	/* Read in single subimage data, RLE encoded */
  else if (IsRLECompressed()) 
    Result = ReadRLEData(pFileHandle);
  else 
    Result = ReadData(pFileHandle);

  return (Result);
 }
/*===========================================================================
 *		End of Class Method CDFTextureImage::Read()
 *=========================================================================*/
 

#undef  __FUNC__
#define __FUNC__ "CDFTextureImage::ReadHeader()"
/*===========================================================================
 *
 * Class CDFTextureImage Method - boolean ReadHeader (pFileHandle);
 *
 * Attempt to read in the image header from the given file stream.
 * Returns FALSE on any error.  Protected class method.
 *
 *=========================================================================*/
boolean CDFTextureImage::ReadHeader (FILE* pFileHandle) {
  int Result;

	/* Assume we are at the header position currently and read in the
	 * header data all at once.*/
  Result = fread (&Header, 1, DFTEXTURE_IMAGEHEADER_SIZE, pFileHandle);

  if (Result != DFTEXTURE_IMAGEHEADER_SIZE) {
    SET_EXT_ERROR4(ERR_READ, "Error reading header information (%d of %d bytes)!", Result, DFTEXTURE_IMAGEHEADER_SIZE);
    return (FALSE);
   }

   	/* Ensure a valid number of sub-images */
  if (GetNumSubImages() >= DFTEXTURE_MAX_SUBIMAGES) {
    SET_EXT_ERROR3(ERR_READ, "Invalid number of sub-images received (%d)!", GetNumSubImages());
    return (FALSE);
   }

  return (Result);
 }
/*===========================================================================
 *		End of Class Method CDFTextureImage::ReadHeader()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFTextureImage::ReadData()"
/*===========================================================================
 *
 * Class CDFTextureImage Method - boolean ReadData (pFileHandle);
 *
 * Attempt to read in the image data from the given file stream.
 * Returns FALSE on any error.  Protected class method.
 *
 *=========================================================================*/
boolean CDFTextureImage::ReadData (FILE* pFileHandle) {
  int   Result;
  int   LoopCounter;
  byte* pDataPtr;

	/* Allocate the data pointer */
  CreatePointerArray(pData[0], byte, GetDataSize());
  NumSubImagesAllocated = 1;

	/* Move to the start of the image data */
  fseek (pFileHandle, GetAbsDataOffset(), SEEK_SET);
  pDataPtr = pData[0];

	/* Must read data row by row */
  for (LoopCounter = 0; LoopCounter < GetHeight(); LoopCounter++) {

		/* Read in one row of image data */
    Result = fread (pDataPtr, 1, GetWidth(), pFileHandle);

		/* Ensure the read was successful */
    if (Result != GetWidth()) {
      SET_EXT_ERROR4(ERR_READ, "Error reading image data row (%d of %d bytes)!", Result, GetWidth());
      return (FALSE);
     }

		/* Move to next row */
    fseek (pFileHandle, DFTEXTURE_IMAGEROW_WIDTH - GetWidth(), SEEK_CUR);
    pDataPtr += GetWidth();
   }

  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFTextureImage::ReadData()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFTextureImage::ReadRLEData()"
/*===========================================================================
 *
 * Class CDFTextureImage Method - boolean ReadRLEData (pFileHandle);
 *
 * Attempt to read in the RLE image data from the given file stream.
 * Returns FALSE on any error.  Protected class method.
 *
 *=========================================================================*/
boolean CDFTextureImage::ReadRLEData (FILE* pFileHandle) {
  int			Result;
  int			LoopCounter;
  byte*			pDataPtr;
  dftexture_rleoffset_t RowOffsets[DFTEXTURE_MAXIMAGE_HEIGHT+1];

	/* Ensure a valid height */
  if (GetHeight() >= DFTEXTURE_MAXIMAGE_HEIGHT) {
    SET_EXT_ERROR3(ERR_INPUT, "Invalid image height (%d)!", GetHeight());
    return (FALSE);
   }

	/* Allocate the data pointer */
  CreatePointerArray(pData[0], byte, GetDataSize());
  NumSubImagesAllocated = 1;

	/* Move to the start of the image data */
  fseek (pFileHandle, GetAbsDataOffset(), SEEK_SET);
  pDataPtr = pData[0];

	/* Read in the row offset data */
  Result = fread (RowOffsets, sizeof(dftexture_rleoffset_t), GetHeight(), pFileHandle);

	/* Ensure the input was a success */
  if (Result != GetHeight()) {
    SET_EXT_ERROR4(ERR_READ, "Error readin the row offsets (%d of %d offsets)!", Result, GetHeight());
    return (FALSE);
   }

	/* Set the last offset value */
  RowOffsets[GetHeight()].Offset = (short) GetImageSize();

	/* Must read data row by row */
  for (LoopCounter = 0; LoopCounter < GetHeight(); LoopCounter++) {
    Result = UncompressRLERow(pFileHandle, &RowOffsets[LoopCounter], &pDataPtr);
    if (!Result) return (FALSE);
   }

  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFTextureImage::ReadRLEData()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFTextureImage::ReadRLESubImageData()"
/*===========================================================================
 *
 * Class CDFTextureImage Method - boolean ReadRLESubImageData (pFileHandle);
 *
 * Attempt to read in the RLE subimage data from the given file stream.
 * Returns FALSE on any error.  Protected class method.
 *
 *=========================================================================*/
boolean CDFTextureImage::ReadRLESubImageData (FILE* pFileHandle) {
  int			 Result;
  int			 LoopCounter;
  int			 RowCounter;
  int			 NumRowOffsets;
  byte*			 pDataPtr;
  dftexture_rleoffset_t* pRowOffsets;

	/* Allocate the row offset structure */
  NumRowOffsets = GetHeight() * GetNumSubImages();
  CreatePointerArray(pRowOffsets, dftexture_rleoffset_t,  NumRowOffsets + 1);

	/* Read in the row offset data */
  Result = fread (pRowOffsets, sizeof(dftexture_rleoffset_t), NumRowOffsets, pFileHandle);

  if (Result != NumRowOffsets) {
    DestroyPointer(pRowOffsets);
    SET_EXT_ERROR4(ERR_READ, "Error reading the row offsets (%d of %d offsets)!", Result, NumRowOffsets);
    return (FALSE);
   }

	/* Read in each subimage */
  for (LoopCounter = 0; LoopCounter < GetNumSubImages(); LoopCounter++) {

		/* Allocate the data pointer */
    CreatePointerArray(pData[LoopCounter], byte, GetDataSize());
    pDataPtr = pData[LoopCounter];
    NumSubImagesAllocated++;

		/* Set the subimage info */
    SubImageInfo[LoopCounter].Width  = GetWidth();
    SubImageInfo[LoopCounter].Height = GetHeight();
    SubImageInfo[LoopCounter].Offset = -1;

		/* Must read data row by row */
    for (RowCounter = 0; RowCounter < GetHeight(); RowCounter++) {
      Result = UncompressRLERow(pFileHandle, &pRowOffsets[RowCounter], &pDataPtr);

      if (!Result) { 
        DestroyPointer(pRowOffsets);
	return (FALSE);
       }
     }
   }

  DestroyPointer(pRowOffsets);
  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFTextureImage::ReadRLESubImageData()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFTextureImage::ReadSubImageData()"
/*===========================================================================
 *
 * Class CDFTextureImage Method - boolean ReadSubImageData (pFileHandle);
 *
 * Attempt to read in the subimage data from the given file stream.
 * Returns FALSE on any error.  Protected class method.
 *
 *=========================================================================*/
boolean CDFTextureImage::ReadSubImageData (FILE* pFileHandle) {
  int   Result;
  int   LoopCounter;
  int   RowCounter;
  byte* pDataPtr;

	/* Read in each subimage */
  for (LoopCounter = 0; LoopCounter < GetNumSubImages(); LoopCounter++) {

		/* Allocate the data pointer */
    CreatePointerArray(pData[LoopCounter], byte, GetDataSize());
    NumSubImagesAllocated++;

		/* Move to the start of the subimage data */
    fseek (pFileHandle, SubImageInfo[LoopCounter].Offset + GetAbsDataOffset(), SEEK_SET);

		/* Read the subimage width and height  */
    SubImageInfo[LoopCounter].Width  = read_short(pFileHandle);
    SubImageInfo[LoopCounter].Height = read_short(pFileHandle);
    pDataPtr = pData[LoopCounter];

		/* Must read data row by row */
    for (RowCounter = 0; RowCounter < SubImageInfo[LoopCounter].Height; RowCounter++) {
      Result = UncompressSubImageRow(pFileHandle, LoopCounter, &pDataPtr);
      if (!Result) return (FALSE);
     }
   }

  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFTextureImage::ReadSubImageData()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFTextureImage::ReadSubImageInfo()"
/*===========================================================================
 *
 * Class CDFTextureImage Method - boolean ReadSubImageInfo (pFileHandle);
 *
 * Attempt to read in the subimage offsets from the given file stream.
 * Returns FALSE on any error.  Protected class method.
 *
 *=========================================================================*/
boolean CDFTextureImage::ReadSubImageInfo (FILE* pFileHandle) {
  int  Result;
  long Offsets[DFTEXTURE_MAX_SUBIMAGES];
  int  LoopCounter;

	/* Are there are subimage offsets to read? */
  if (GetNumSubImages() <= 1) return (TRUE);

	/* Jump to the position of the subimage offsets */ 
  fseek (pFileHandle, GetAbsDataOffset(), SEEK_SET);

	/* Read in the offsets all at once */
  Result = fread (Offsets, 4, GetNumSubImages(), pFileHandle);

  if (Result != GetNumSubImages()) {
    SET_EXT_ERROR4(ERR_READ, "Error reading subimage offsets (%d of %d offsets)!", Result, GetNumSubImages());
    return (FALSE);
   }

	/* Save offsets to record data */
  for (LoopCounter = 0; LoopCounter < GetNumSubImages(); LoopCounter++) {
    SubImageInfo[LoopCounter].Offset = Offsets[LoopCounter];
   }

  return (Result);
 }
/*===========================================================================
 *		End of Class Method CDFTextureImage::ReadSubImageInfo()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFTextureImage::UncompressRLERow()"
/*===========================================================================
 *
 * Class CDFTextureImage Method - boolean UnCompressRLERow (pFileHandle, pRowOffsets, ppDataPtr);
 *
 * Protected class method to uncompress one row of RLE image data.  Returns
 * FALSE on any error.
 *
 *=========================================================================*/
boolean CDFTextureImage::UncompressRLERow (FILE* pFileHandle, dftexture_rleoffset_t* pRowOffset, byte** ppDataPtr) {
  int  Result;
  int  BytesWritten;
  int  BytesDecoded;
  int  RLECounter;
  int  RowWidth;
  int  ByteCount;
  byte RLEData;

	/* Jump to the start of the row data */
  fseek (pFileHandle, pRowOffset->Offset + HeaderOffset, SEEK_SET);

	/* Is the row compressed or not? */
  if (pRowOffset->Unknown == 0x0000) {

		/* Read in the row data all at once and ensure success */
    Result = fread (*ppDataPtr, 1, GetWidth(), pFileHandle);

    if (Result != GetWidth()) {
      SET_EXT_ERROR4(ERR_READ, "Error reading row data (%d of %d bytes)!", Result, GetWidth());
      return (FALSE);
     }
      
    (*ppDataPtr) += GetWidth();
    return (TRUE);
   }

	/* Input the row width value */
  RowWidth = read_short(pFileHandle);
		
	/* Initialize the RLE variables */
  BytesWritten = 0;
  BytesDecoded = 0;
		
	/* Uncompress the row data */
  while (BytesWritten < RowWidth) {
    ByteCount = read_short(pFileHandle);

		/* Write out pixel values explicitly */
    if (ByteCount > 0) {
        
      for (RLECounter = 0; RLECounter < ByteCount; RLECounter++) {
        **ppDataPtr = fgetc(pFileHandle);

	if (**ppDataPtr < 0)  {
	  SET_EXT_ERROR2(ERR_READ, "Failed to read data from file!");
	  return (FALSE);
	 }

        (*ppDataPtr)++;
       }

      BytesDecoded += ByteCount + 2;
      BytesWritten += ByteCount;
     }	/* Decode RLE bytes */
    else {
      RLEData = fgetc(pFileHandle);
      ByteCount = -ByteCount;

      if (RLEData < 0)  {
        SET_EXT_ERROR2(ERR_READ, "Failed to read data from file!");
        return (FALSE);
       }      

      for (RLECounter = 0; RLECounter < ByteCount; RLECounter++) {
        **ppDataPtr = RLEData;
        (*ppDataPtr)++;
       }

      BytesDecoded += 3;
      BytesWritten += ByteCount;
     }

   } /* End of while decoding */

  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFTextureImage::UnCompressRLERow()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFTextureImage::UncompressSubImageRow()"
/*===========================================================================
 *
 * Class CDFTextureImage Method - boolean UnCompressSubImageRow (pFileHandle, SubImageIndex, ppDataPtr);
 *
 * Protected class method to uncompress one row of regular subimage data.
 * Returns FALSE on any error.
 *
 *=========================================================================*/
boolean CDFTextureImage::UncompressSubImageRow (FILE* pFileHandle, const int SubImageIndex, byte** ppDataPtr) {
  int   Result;
  int   RowSkipBytes;
  int   RowBytes;
  int   RowWidth = SubImageInfo[SubImageIndex].Width;
  int   BytesWritten = 0;

  while (BytesWritten < RowWidth) {

		/* Read the two compressed rowinfo bytes */
    RowSkipBytes = fgetc(pFileHandle);
    RowBytes     = fgetc(pFileHandle);

		/* Ensure valid input */
    if (RowSkipBytes < 0 || RowBytes < 0) {
      SET_EXT_ERROR4(ERR_READ, "Invalid compressed image data received (%d/%d)", RowSkipBytes, RowBytes);
      return (FALSE);
     }

		/* Fill the first section of the row data */
    memset(*ppDataPtr, 0x00, RowSkipBytes);
    (*ppDataPtr) += RowSkipBytes;
    BytesWritten += RowSkipBytes;
      	
		/* Move to next byte sequence if no bytes to read */
    if (!RowBytes) continue;

    		/* Read in the row data */
    Result = fread (*ppDataPtr, 1, RowBytes, pFileHandle);
    (*ppDataPtr) += RowBytes;
    BytesWritten += RowBytes;

		/* Ensure the read was successful */
    if (Result != RowBytes) {
      SET_EXT_ERROR4(ERR_READ, "Failed to read image data (%d of %d bytes)!", Result, RowBytes);
      return (FALSE);
     }

   } /* End of while loop */

  
  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFTextureImage::UnCompressSubImageRow()
 *=========================================================================*/