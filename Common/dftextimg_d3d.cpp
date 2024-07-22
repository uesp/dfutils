/*===========================================================================
 *
 * DFTextImg_D3D.CPP - Dave Humphrey (uesp@m0use.net), 9 November 2000
 *
 *=========================================================================*/

	/* Include Files */
#include "dftextimg_d3d.h"
#include "dfcommon_d3d.h"


#undef  __FUNC__
#define __FUNC__ "CDFTextureImageD3D::CDFTextureImageD3D()"
/*===========================================================================
 *
 * Class CDFTextureImageD3D Constructor
 *
 *=========================================================================*/
CDFTextureImageD3D::CDFTextureImageD3D() {
  int LoopCounter;

  for (LoopCounter = 0; LoopCounter < DFTEXTURE_MAX_SUBIMAGES; LoopCounter++) {
    pTexture[LoopCounter] = NULL;
    pImageInfo[LoopCounter] = NULL;
   }

 }
/*===========================================================================
 *		End of Class CDFTextureImageD3D Constructor
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFTextureImageD3D::Destroy()"
/*===========================================================================
 *
 * Class CDFTextureImageD3D Denstructor
 *
 *=========================================================================*/
void CDFTextureImageD3D::Destroy (void) {
  int LoopCounter;

	/* Call the base class method first */
  CDFTextureImage::Destroy();

	/* Release the D3D texture objects */
  for (LoopCounter = 0; LoopCounter < DFTEXTURE_MAX_SUBIMAGES; LoopCounter++) {
    RELEASE(pTexture[LoopCounter]);
    DestroyPointer(pImageInfo[LoopCounter]);
   }

 }
/*===========================================================================
 *		End of Class CDFTextureImageD3D Destructor
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFTextureImageD3D::CreateTexture()"
/*===========================================================================
 *
 * Class CDFTextureImageD3D Method - boolean CreateTexture (ImageIndex);
 *
 * Creates a D3DRM texture from the given sub-image.  Returns FALSE on any error.
 *
 *=========================================================================*/
boolean CDFTextureImageD3D::CreateTexture (const int ImageIndex) {
   HRESULT  Result;
 
	/* Ensure valid D3DRM object */
  ASSERT(GetD3DRM() != NULL) 

	/* Ensure a valid image index */
  if (!IsValidImageIndex(ImageIndex)) { 
    SET_EXT_ERROR3(ERR_INDEX, "Invalid texture subimage index %d!", ImageIndex);
    return (FALSE);
   }

	/* Does the texture information already exist? */
  if (pTexture[ImageIndex] != NULL) return (TRUE);

	/* Allocate the image info structure */
  CreatePointer(pImageInfo[ImageIndex], D3DRMIMAGE);

	/* Setup the image information structure */
  pImageInfo[ImageIndex]->alpha_mask = 0xFF;	
  pImageInfo[ImageIndex]->blue_mask = 0xFF;	
  pImageInfo[ImageIndex]->red_mask = 0xFF;	
  pImageInfo[ImageIndex]->green_mask = 0xFF;	
  pImageInfo[ImageIndex]->aspectx = 1;
  pImageInfo[ImageIndex]->aspecty = 1;
  pImageInfo[ImageIndex]->depth = 8;
  pImageInfo[ImageIndex]->palette = DFStandardPaletteD3D.GetD3DPalData();
  pImageInfo[ImageIndex]->height = GetHeight(ImageIndex);
  pImageInfo[ImageIndex]->width = GetWidth(ImageIndex);
  pImageInfo[ImageIndex]->bytes_per_line = GetWidth(ImageIndex);
  pImageInfo[ImageIndex]->rgb = FALSE;
  pImageInfo[ImageIndex]->palette_size = DFStandardPaletteD3D.GetNumPalEntries();
  pImageInfo[ImageIndex]->buffer1 = (void *) GetData(ImageIndex);
  pImageInfo[ImageIndex]->buffer2 = NULL;

	/* Attempt to create the texture */
  Result = GetD3DRM()->CreateTexture(pImageInfo[ImageIndex], &pTexture[ImageIndex]);
  
  if (FAILED(Result)) {
    SET_D3D_ERROR2(Result, "Failed to create Direct3D texture object!");
    return (FALSE);
   }

  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFTextureImageD3D::CreateTexture()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFTextureImageD3D::CountTextureRefs()"
/*===========================================================================
 *
 * Class CDFTextureImageD3D Method - ULONG CountTextureRefs (ImageIndex);
 *
 * Counts the number of texture references for the given texture. Returns
 * 0 on any error.  Protected class method.
 *
 *=========================================================================*/
ULONG CDFTextureImageD3D::CountTextureRefs (const int ImageIndex) {
  ULONG Result;

	/* Ensure a valid image index */
  if (!IsValidImageIndex(ImageIndex)) {
    SET_EXT_ERROR3(ERR_INDEX, "Invalid texture sub-image index %d received!", ImageIndex);
    return (0);
   }

	/* Add and then release to get the count */
  Result = pTexture[ImageIndex]->AddRef();
  Result = pTexture[ImageIndex]->Release();
  return (Result);
 }
/*===========================================================================
 *		End of Class Method CDFTextureImageD3D::CountTextureRefs()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFTextureImageD3D::Read()"
/*===========================================================================
 *
 * Class CDFTextureImageD3D Method - boolean Read (pFileHandle);
 *
 * Calls the base class Read() method and, on success, creates the D3D texture
 * objects for each subimage
 *
 *=========================================================================*/
boolean CDFTextureImageD3D::Read (FILE* pFileHandle) {
  boolean Result;
  int     LoopCounter;

	/* Call the base class method */
  Result = CDFTextureImage::Read(pFileHandle);
  if (!Result) return (FALSE);

	/* Create all the textures */
  for (LoopCounter = 0; LoopCounter < GetNumSubImages(); LoopCounter++) {
    Result = CreateTexture(LoopCounter);
    if (!Result) return (FALSE);
   }

	/* Return success */
  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFTextureImageD3D::Read()
 *=========================================================================*/