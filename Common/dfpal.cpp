/*===========================================================================
 *
 * DFPal.CPP - Dave Humphrey (uesp@m0use.net), 9 November 2000
 *
 *=========================================================================*/

	/* Include Files */
#include "dfpal.h"
#include "dfcommon.h"


#undef  __FUNC__
#define __FUNC__ "CDFPalette::CDFPalette()"
/*===========================================================================
 *
 * Class CDFPalette Constructor
 *
 *=========================================================================*/
CDFPalette::CDFPalette() {
  COLFile = FALSE;
  NumPalEntries = 256;

	/* Copy the default palette data to initialize palette data array */
  memcpy (PalData, DFPalette, 768);
 }
/*===========================================================================
 *		End of Class CDFPalette Constructor
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFPalette::Load()"
/*===========================================================================
 *
 * Class CDFPalette Method - boolean Load (pFilename);
 *
 * Attempt to load the specified PAL or COL file.  Returns FALSE on any
 * error.  Attempts to determine the filetype by first the file extension
 * and then by the filesize. Use LoadPAL() or LoadCOL() to bypass this.
 *
 *=========================================================================*/
boolean CDFPalette::Load (const char* pFilename) {
  long  FileSize;

	/* Ensure a valid filename before we operate on it */
  if (pFilename == NULL) {
    SET_EXT_ERROR2(ERR_NULL, "Invalid NULL filename received!");
    return (FALSE);
   }

	/* Check for extension matches */
  if (CompareExtension(pFilename, "COL"))
    return LoadCOL(pFilename);
  else if (CompareExtension(pFilename, "PAL"))
    return LoadPAL(pFilename);

	/* Check for filesize matches */
  FileSize = GetFilesize(pFilename);

  if (FileSize == DF_PAL_FILESIZE)
    return LoadPAL(pFilename);
  else if (FileSize == DF_COL_FILESIZE)
    return LoadCOL(pFilename);
  
	/* Oh-oh, no idea what the filetype is, try COL */
  SystemLog.Printf ("No idea what file type is for DFPalette %s!", pFilename);
  return LoadCOL(pFilename);
 }
/*===========================================================================
 *		End of Class Method CDFPalette::Load()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFPalette::LoadCOL()"
/*===========================================================================
 *
 * Class CDFPalette Method - boolean LoadCOL (pFilename);
 *
 * Attempt to load the specified COL file.  Returns FALSE on any
 * error.
 *
 *=========================================================================*/
boolean CDFPalette::LoadCOL (const char* pFilename) {
  FILE* pFileHandle;
  int   Result;

	/* Attempt to open the file for input */
  pFileHandle = openfile(pFilename, "rb");

  if (pFileHandle == NULL) {
    SET_EXT_ERROR3(ERR_FILE, "Failed to open file %s!", pFilename);
    return (FALSE);
   }

	/* Read the header all at once */
  Result = fread (&COLHeader, 1, DF_COL_HEADERSIZE, pFileHandle);
  
  if (Result != DF_COL_HEADERSIZE) {
    SET_EXT_ERROR4(ERR_READ, "Error reading COL header (%d of %d bytes)!", Result, DF_COL_HEADERSIZE); 
    fclose (pFileHandle);
    return (FALSE);
   }

  COLFile = TRUE;
  NumPalEntries = (COLHeader.FileSize - 8)/3;

	/* Ensure a valid number of colors */
  if (NumPalEntries < 0 || NumPalEntries >= DF_MAX_PALENTRIES) {
    SET_EXT_ERROR3(ERR_INDEX, "Invalid number of palette colors received (%d)!", NumPalEntries); 
    NumPalEntries = 0;
    fclose (pFileHandle);
    return (FALSE);
   }

	/* Read the palette data all at once */
  Result = fread (PalData, 3, NumPalEntries, pFileHandle);

  if (Result != NumPalEntries) {
    SET_EXT_ERROR4(ERR_READ, "Failed to read the palette data (%d of %d colors)!", Result, NumPalEntries); 
    fclose (pFileHandle);
    NumPalEntries = 0;
    return (FALSE);
   }

  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFPalette::LoadCOL()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFPalette::LoadPAL()"
/*===========================================================================
 *
 * Class CDFPalette Method - boolean LoadPAL (pFilename);
 *
 * Attempt to load the specified PAL file.  Returns FALSE on any
 * error.
 *
 *=========================================================================*/
boolean CDFPalette::LoadPAL (const char* pFilename) {
  FILE* pFileHandle;
  int   Result;

	/* Attempt to open the file for input */
  pFileHandle = openfile(pFilename, "rb");

  if (pFileHandle == NULL) {
    SET_EXT_ERROR3(ERR_FILE, "Failed to open file %s!", pFilename);
    return (FALSE);
   }

	/* Read the data all at once */
  Result = fread (PalData, 1, 768, pFileHandle);

  if (Result != 768) {
    SET_EXT_ERROR4(ERR_READ, "Failed to read the palette data (%d of %d colors)!", Result, NumPalEntries); 
    fclose (pFileHandle);
    return (FALSE);
   }

  COLFile = FALSE;
  NumPalEntries = 256;

  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFPalette::LoadPAL()
 *=========================================================================*/