/*===========================================================================
 *
 * DFTexture.CPP - Dave Humphret (uesp@m0use.net), 2 November 2000
 *
 *=========================================================================*/

	/* Include Files */
#include "dftexture.h"
#include "dfcommon.h"


#undef  __FUNC__
#define __FUNC__ "CDFTexture::CDFTexture()"
/*===========================================================================
 *
 * Class CDFTexture Constructor
 *
 *=========================================================================*/
CDFTexture::CDFTexture() {
  int LoopCounter;

  NumImages = 0;
  NumImagesAllocated = 0;

	/* Allocate the image/offset arrays */
  CreatePointerArray(Offsets, dftexture_offset_t, DFTEXTURE_MAX_IMAGES);
  CreatePointerArray(pImages, CDFTextureImage*, DFTEXTURE_MAX_IMAGES);
  CreatePointerArray(Name, char, DFTEXTURE_NAME_SIZE);
  Name[0] = NULL_CHAR;

  for (LoopCounter = 0; LoopCounter < DFTEXTURE_MAX_IMAGES; LoopCounter++) {
    pImages[LoopCounter] = NULL;
   }
 }
/*===========================================================================
 *		End of Class CDFTexture Constructor
 *=========================================================================*/

#undef  __FUNC__
#define __FUNC__ "CDFTexture::~CDFTexture()"
/*===========================================================================
 *
 * Class CDFTexture Destructor
 *
 *=========================================================================*/
CDFTexture::~CDFTexture() {

  Destroy();
  
	/* Unallocate the image/offset arrays */
  delete[] Offsets;
  delete[] pImages;
  delete[] Name;
 }
/*===========================================================================
 *		End of Class CDFTexture Destructor
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFTexture::Destroy()"
/*===========================================================================
 *
 * Class CDFTexture Destructor
 *
 *=========================================================================*/
void CDFTexture::Destroy (void) {
  int LoopCounter;

  for (LoopCounter = 0; LoopCounter < NumImagesAllocated; LoopCounter++) {
    DestroyPointer(pImages[LoopCounter]);
   }

  Name[0] = NULL_CHAR;
  NumImages = 0;
  NumImagesAllocated = 0;
 }
/*===========================================================================
 *		End of Class CDFTexture Destructor
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFTexture::CreateImage()"
/*===========================================================================
 *
 * Class CDFTexture Method - void CreateImage (ImageIndex);
 *
 * Helper function which allocates the given image index.
 *
 *=========================================================================*/
void CDFTexture::CreateImage (const int ImageIndex) { 
  CreatePointer(pImages[ImageIndex], CDFTextureImage);
  NumImagesAllocated++;
 }
/*===========================================================================
 *		End of Class Method CDFTexture::CreateImage()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFTexture::Dump()"
/*===========================================================================
 *
 * Class CDFTexture Method - void Dump (DumpMask, FILE* pFileHandle);
 *
 * Output object information to a file stream according to the DumpMask flags.
 *
 *=========================================================================*/
void CDFTexture::Dump (FILE* pFileHandle, const dftexture_dumpmask_t DumpMask) {
  int LoopCounter;

	/* Ensure valid input */
  if (pFileHandle == NULL) {
    SET_EXT_ERROR2(ERR_NULL, "Invalid NULL file handle received!");
    return;
   }

	/* Output basic and header information */
  fprintf (pFileHandle, "Texture Object 0x%p\n", this);

  if (DumpMask & DFTEXTURE_DM_TEXTUREINFO) {
    fprintf (pFileHandle, "\tName     = '%s'\n", Name);
    fprintf (pFileHandle, "\tNumImages = %d\n", NumImages);
   }

  for (LoopCounter = 0; LoopCounter < NumImagesAllocated; LoopCounter++) {

  		/* Output offset table data if required */
    if (DumpMask & DFTEXTURE_DM_OFFSETLIST) {
      fprintf (pFileHandle, "\tOffset %3d: 0x%04X", LoopCounter+1, Offsets[LoopCounter].Type1&0xffff);
      fprintf (pFileHandle, " 0x%08lX 0x%04X 0x%08lX", Offsets[LoopCounter].HeaderOffset, Offsets[LoopCounter].Type2&0xffff, Offsets[LoopCounter].Unknown1);

		/* Output the confirmed NULL values if required */
      if (DumpMask & DFTEXTURE_DM_OFFSETNULLS) 
        fprintf (pFileHandle,  " 0x%08lX 0x%08lX",  Offsets[LoopCounter].NullValue1, Offsets[LoopCounter].NullValue2);

      fprintf (pFileHandle, " \n");
     }

     		/* Output image information if required */
     if (DumpMask & DFTEXTURE_DM_IMAGE) {
       pImages[LoopCounter]->Dump(pFileHandle, DumpMask);
      }
   }

  fflush(pFileHandle);  
 }
/*===========================================================================
 *		End of Class Method CDFTexture::Dump()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFTexture::CDFTexture()"
/*===========================================================================
 *
 * Class CDFTexture Method - boolean IsValidIndex (Index);
 *
 * Ensures the given texture index is a valid array object.    Returns FALSE
 * if it is not.
 *
 *=========================================================================*/
boolean CDFTexture::IsValidIndex (const int Index) { 
  return ((Index >= 0 && Index < NumImages && pImages[Index] != NULL) ? TRUE : FALSE); 
 }
/*===========================================================================
 *		End of Class Method CDFTexture::IsValidIndex()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFTexture::Load()"
/*===========================================================================
 *
 * Class CDFTexture Method - boolean Load (pFilename);
 *
 * Attempt to load the entire texture file.  Returns FALSE on any error.
 *
 *=========================================================================*/
boolean CDFTexture::Load (const char* pFilename) {
  FILE*   pFileHandle;
  boolean Result;

	/* Attempt to open file for input */
  pFileHandle = openfile(pFilename, "rb");

  if (!pFileHandle) {
    SET_EXT_ERROR3(ERR_FILE, "Failed to open file %s!", pFilename);
    return (FALSE);
   }

	/* Delete the current image information */
  Destroy();

	/* Read in the header and offset information */
  Result = ReadHeader(pFileHandle);
  if (Result) Result = ReadOffsets(pFileHandle);
  if (Result) Result = ReadImages(pFileHandle);

	/* Close file and return result */
  fclose (pFileHandle);
  return (Result);
 }
/*===========================================================================
 *		End of Class Method CDFTexture::Load()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFTexture::ReadHeader()"
/*===========================================================================
 *
 * Class CDFTexture Method - boolean ReadHeader (pFileHandle);
 *
 * Attempt to read the texture header data from the given file stream. 
 * Returns FALSE on any error.
 *
 *=========================================================================*/
boolean CDFTexture::ReadHeader (FILE* pFileHandle) {
  int Result;

	/* Move to the start of the header data */
  fseek (pFileHandle, DFTEXTURE_HEADER_OFFSET, SEEK_SET);

	/* Input the number of images in the texture file */
  NumImages = read_short(pFileHandle);

	/* Ensure a valid number of images */
  if (NumImages < 0 || NumImages >= DFTEXTURE_MAX_IMAGES) { 
    SET_EXT_ERROR3(ERR_INDEX, "Invalid number of images received (%d)!", NumImages);
    return (FALSE);
   }

	/* Input the texture name string */
  Result = fread (Name, 1, DFTEXTURE_NAME_SIZE, pFileHandle);

	/* Ensure the read has been successful */
  if (Result != DFTEXTURE_NAME_SIZE) {
    SET_EXT_ERROR4(ERR_READ, "Error reading texture name (%d of %d bytes)!", Result, DFTEXTURE_NAME_SIZE);
    return (FALSE);
   }

  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFTexture::ReadHeader()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFTexture::ReadImages()"
/*===========================================================================
 *
 * Class CDFTexture Method - boolean ReadImages (pFileHandle);
 *
 * Attempt to read the image data from the given file stream. 
 * Returns FALSE on any error. Protected class member.
 *
 *=========================================================================*/
boolean CDFTexture::ReadImages (FILE* pFileHandle) {
  int Result;
  int LoopCounter;

	/* Ensure a valid number of images */
  if (NumImages >= DFTEXTURE_MAX_IMAGES) { 
    SET_EXT_ERROR3(ERR_MAXINDEX, "Invalid number of images (%d)!", NumImages);
    return (FALSE);
   }

	/* Input all image data */
  for (LoopCounter = 0; LoopCounter < NumImages; LoopCounter++) { 
	
		/* Attempt to allocate the image */
    CreateImage(LoopCounter);

		/* Jump to image header position */
    fseek (pFileHandle, Offsets[LoopCounter].HeaderOffset, SEEK_SET);

		/* Attempt to read image data */
    Result = pImages[LoopCounter]->Read(pFileHandle);
    if (!Result) return (FALSE);
   }

  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFTexture::ReadImages()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFTexture::ReadOffsets()"
/*===========================================================================
 *
 * Class CDFTexture Method - boolean ReadOffsets (pFileHandle);
 *
 * Attempt to read the texture offset data from the given file stream. 
 * Returns FALSE on any error.
 *
 *=========================================================================*/
boolean CDFTexture::ReadOffsets (FILE* pFileHandle) {
  int Result;

	/* Move to the start of the offset records */
  fseek (pFileHandle, DFTEXTURE_OFFSET_OFFSET, SEEK_SET);
	
	/* Read in the offset data all at once */
  Result = fread (Offsets, DFTEXTURE_OFFSET_RECORDSIZE, NumImages, pFileHandle);

	/* Ensure the read has been successful */
  if (Result != NumImages) {
    SET_EXT_ERROR4(ERR_READ, "Error reading offset data (%d of %d offsets)!", Result, NumImages);
    return (FALSE);
   }

  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFTexture::ReadOffsets()
 *=========================================================================*/


