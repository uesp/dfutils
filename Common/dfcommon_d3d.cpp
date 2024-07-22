/*===========================================================================
 *
 * DFCommon_D3D.Cpp - Dave Humphrey (uesp@m0use.net), 9 November 2000
 *
 *=========================================================================*/

	/* Include Files */
#include "dfcommon_d3d.h"
#include "dftexture_d3d.h"


	/* The default daggerfall palette */
CDFPaletteD3D DFStandardPaletteD3D;

	/* The texture file information */
CDFTextureD3D DFTexturesD3D[DF_MAX_TEXTURES];

	/* 3D Objects File */
CDFArchD3D    DFArch3D;

	/* 3D Object Block Information */
CDFBlocksFileD3D DFBlocks;


#undef  __FUNC__
#define __FUNC__ "DFGetD3DTexture()"
/*===========================================================================
 *
 * Function - CD3DTexture* DFGetD3DTexture (File, Index, SubIndex);
 *
 * Get the specified texture image.  Returns NULL on any error.
 *
 *=========================================================================*/
CD3DTexture* DFGetD3DTexture (const int File, const int Index, const int SubIndex) {
  CD3DTexture* pTexture;
  char	       FileBuffer[1024];
  boolean      Result;

	/* Ensure a valid texture index */
  if (File < 0 || File >= DF_MAX_TEXTURES) {
    SET_EXT_ERROR3(ERR_NULL, "Invalid file index %d received!", File);
    return (NULL);
   }

	/* Check to see if the texture already exists */
  Result = DFTexturesD3D[File].IsValidIndex(Index);

	/* Load the texture if it doesn't exist */
  if (!Result) {

		/* Make the texture filename and ensure it exists */
    sprintf (FileBuffer, "%stexture.%03d", GetDFArena2Path(), File);

		/* Try loading the texture from the CD */
    if (!FileExists(FileBuffer)) {
      sprintf (FileBuffer, "%stexture.%03d", GetDFArena2CDPath(), File);

		/* Couldn't find the file */
      if (!FileExists(FileBuffer)) {
        SET_EXT_ERROR3(ERR_FILE, "Failed to find texture file 'Texture.%03d' in DF Arena2 directory on disk or CD!", File);
        return (NULL);
       }
     }

		/* Attempt to load the file */
    Result = DFTexturesD3D[File].Load(FileBuffer);
    if (!Result) return (FALSE);
   }

	/* Get and return the D3D texture */
  pTexture = DFTexturesD3D[File].GetTexture(Index, SubIndex);
  return (pTexture);
 }
/*===========================================================================
 *		End of Function DFGetD3DTexture()
 *=========================================================================*/

	
/*===========================================================================
 *
 * Function - boolean DFGetImageFile (pFilename, Image);
 *
 * Attempts to load the image file into the given object.  Returns FALSE
 * on any error.
 *
 *=========================================================================*/
boolean DFGetImageFile (const char* pFilename, CDFImgFileD3D& Image) {
  char	   FileBuffer[1024];

	/* Make the image filename and ensure it exists */
  sprintf (FileBuffer, "%s%s", GetDFArena2Path(), pFilename);

	/* Doesn't exist on disk, try on the CD */
  if (!FileExists(FileBuffer)) {
    sprintf (FileBuffer, "%s%s", GetDFArena2CDPath(), pFilename);
	
		/* Couldn't find image on disk or CD */
    if (!FileExists(FileBuffer)) {
      SET_EXT_ERROR3(ERR_FILE, "Failed to find image file '%s' in DF Arena2 directory on disk or CD!", pFilename);
      return (NULL);
     }
   }

	/* Load the image and return result */
  return Image.Load(FileBuffer);
 }
/*===========================================================================
 *		End of Function DFGetImageFile()
 *=========================================================================*/