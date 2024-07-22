/*===========================================================================
 *
 * DFCif.CPP - Dave Humphrey (uesp@m0use.net), 4 November 2000
 *
 *=========================================================================*/	

	/* Includes */
#include "dfcif.h"
#include "dfcommon.h"



#undef  __FUNC__
#define __FUNC__ "CDFCifFile::CDFCifFile()"
/*===========================================================================
 *
 * Class CDFCifFile Constructor
 *
 *=========================================================================*/
CDFCifFile::CDFCifFile() {
  NumImages = 0;
 }
/*===========================================================================
 *		End of Class CDFCifFile Constructor
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFCifFile::Destroy()"
/*===========================================================================
 *
 * Class CDFCifFile Destructor
 *
 *=========================================================================*/
void CDFCifFile::Destroy (void) {
  int LoopCounter;

  for (LoopCounter = 0; LoopCounter < NumImages; LoopCounter++) {
    DestroyPointer(pImages[LoopCounter]);
   }

  NumImages = 0;
 }
/*===========================================================================
 *		End of Class CDFCifFile Destructor
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFCifFile::Load()"
/*===========================================================================
 *
 * Class CDFCifFile Method - boolean Load (pFilename);
 *
 * Attempts to read in the given CIF file.  Returns FALSE on any error.
 *
 *=========================================================================*/
boolean CDFCifFile::Load (const char* pFilename) {
  FILE*   pFileHandle;
  boolean Result = TRUE;

	/* Attempt to open file for input */
  pFileHandle = openfile(pFilename, "rb");

  if (pFileHandle == NULL) {
    SET_EXT_ERROR3(ERR_FILE, "Failed to open file %s!", pFilename);
    return (FALSE);
   }

	/* Read a regular or weapon CIF file */
  if (IsWeaponCIFFilename(pFilename))
    Result = ReadWeaponCIF(pFileHandle);
  else
    Result = ReadCIF(pFileHandle);

  fclose (pFileHandle);
  return (Result);
 }
/*===========================================================================
 *		End of Class Method CDFCifFile::Load()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFCifFile::ReadCIF()"
/*===========================================================================
 *
 * Class CDFCifFile Method - boolean ReadCIF (pFileHandle);
 *
 * Attempts to read in the regular CIF file.  Returns FALSE on any error.
 *
 *=========================================================================*/
boolean CDFCifFile::ReadCIF (FILE* pFileHandle) {
  long    FileSize;
  boolean Result = TRUE;

	/* Get the filesize */
  FileSize = GetFilesize(pFileHandle);

	/* Check for errors and the special FACES.CIF file */
  if (FileSize == INVALID_FILESIZE) {
    SET_EXT_ERROR2(ERR_READ, "Invalid filesize received!");
    return (FALSE);
   }
  
	/* Destroy the current image information */
  Destroy();

	/* Read in image data until no more left */
  while (FileSize - ftell(pFileHandle) >= DFCIF_MINBYTES_LEFT) {

		/* Ensure we don't load too many images to overflow array */
    if (NumImages >= DFCIF_MAX_IMAGES) {
      SET_EXT_ERROR3(ERR_MAXINDEX, "Maximum number of images exceeded (%d)!", DFCIF_MAX_IMAGES);
      Result = FALSE;
      break;
     }

		/* Allocate the next image */
    CreatePointer(pImages[NumImages], CDFCifImage);
    NumImages++;

		/* Special case for FACES.CIF */
    if (FileSize == 249856l) {
      pImages[NumImages-1]->SetHasNoHeader(TRUE);
      pImages[NumImages-1]->SetWidth(64);
      pImages[NumImages-1]->SetHeight(64);
      pImages[NumImages-1]->SetImageSize(64*64);
     }

		/* Read in the next image file */
    Result = pImages[NumImages-1]->Read(pFileHandle);
    if (!Result) break;
   }

	/* Output debug message */
  return (Result);
 }
/*===========================================================================
 *		End of Class Method CDFCifFile::ReadCIF()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFCifFile::ReadWeaponCIF()"
/*===========================================================================
 *
 * Class CDFCifFile Method - boolean ReadWeaponCIF (pFileHandle);
 *
 * Attempts to read in the given Weapon CIF file.  Returns FALSE on any error.
 *
 *=========================================================================*/
boolean CDFCifFile::ReadWeaponCIF (FILE* pFileHandle) {
  long    FileSize;
  boolean Result = TRUE;
  boolean HasNoFirstImage = FALSE;
  ushort  OffsetList[DFCIF_NUM_OFFSETS];
  int     GroupCounter = 0;
  long    GroupOffset;
  int     LoopCounter;
  int     NumSubImages;

	/* Get the filesize */
  FileSize = GetFilesize(pFileHandle);

	/* Check for errors in filesize */
  if (FileSize == INVALID_FILESIZE) {
    SET_EXT_ERROR2(ERR_READ, "Invalid filesize received!");
    fclose (pFileHandle);
    return (FALSE);
   }

	/* Destroy the current image information */
  Destroy();

	/* Check to see if there is a first, regular, image */
  fseek(pFileHandle, 8, SEEK_SET);
  if (fgetc(pFileHandle) == 0x15) HasNoFirstImage = TRUE;
  fseek (pFileHandle, 0, SEEK_SET);

	/* Read in the first, regular, image data */
  if (!HasNoFirstImage) {
    CreatePointer(pImages[NumImages], CDFCifImage);
    pImages[NumImages]->SetCIFGroupIndex(GroupCounter);
    GroupCounter++;
    NumImages++;
    Result = pImages[NumImages-1]->Read(pFileHandle);
    if (!Result) return (FALSE);
   }

	/* Read in weapon CIF image data until no more left */
  while (FileSize - ftell(pFileHandle) >= DFCIF_MINBYTES_LEFT) {

  		/* Ensure we don't load too many images to overflow array */
    if (NumImages >= DFCIF_MAX_IMAGES) {
      SET_EXT_ERROR3(ERR_MAXINDEX, "Maximum number of images exceeded (%d)!", DFCIF_MAX_IMAGES);
      Result = FALSE;
      break;
     }

		/* Save the group file offset */
    GroupOffset = ftell(pFileHandle);

		/* Read in the first image in a group */
    CreatePointer(pImages[NumImages], CDFCifImage);
    pImages[NumImages]->SetHasOffsetList(TRUE);
    pImages[NumImages]->SetHasCIFWeaponHeader(TRUE);
    pImages[NumImages]->SetCIFGroupIndex(GroupCounter);
    NumImages++;
    
    Result = pImages[NumImages-1]->Read(pFileHandle);
    if (!Result) break;

		/* Read in the group offset list */
    fseek (pFileHandle, GroupOffset + 12, SEEK_SET);
    Result = fread (OffsetList, sizeof(short), DFCIF_NUM_OFFSETS, pFileHandle);

    if (Result != DFCIF_NUM_OFFSETS) {
      SET_EXT_ERROR4(ERR_READ, "Error reading offset list (%d of %d offsets)!", Result, DFCIF_NUM_OFFSETS); 
      break;
     }

    		/* Count the number of subimages */
    for (NumSubImages = 0; NumSubImages < DFCIF_NUM_OFFSETS; NumSubImages++) {
      if (OffsetList[NumSubImages] == 0) break;
     }

		/* Read in each sub-image in group */
    for (LoopCounter = 1; LoopCounter < NumSubImages; LoopCounter++) {

    		/* Ensure we don't create too many images to overflow image array */
      if (NumImages >= DFCIF_MAX_IMAGES) {
        SET_EXT_ERROR3(ERR_MAXINDEX, "Maximum number of images exceeded (%d)!", DFCIF_MAX_IMAGES);
        return (FALSE);
       }

		/* Allocate and setup the image */
      CreatePointer(pImages[NumImages], CDFCifImage);
      pImages[NumImages]->SetHasNoHeader(TRUE);
      pImages[NumImages]->SetCIFGroupIndex(GroupCounter);
      pImages[NumImages]->SetWidth(pImages[NumImages-1]->GetWidth());
      pImages[NumImages]->SetHeight(pImages[NumImages-1]->GetHeight());
      NumImages++;

		/* Jump to the appropiate file offset and read image data */
      fseek (pFileHandle, GroupOffset + OffsetList[LoopCounter], SEEK_SET);
      Result = pImages[NumImages-1]->Read(pFileHandle);
      if (!Result) return (FALSE);
     }

		/* Move to start of next image group */  
    fseek (pFileHandle, OffsetList[DFCIF_NUM_OFFSETS-1] + GroupOffset, SEEK_SET);

		/* Increment CIF group index counter */		
    GroupCounter++;
   }

	/* Output debug message */
  return (Result);
 }
/*===========================================================================
 *		End of Class Method CDFCifFile::ReadWeaponCIF()
 *=========================================================================*/