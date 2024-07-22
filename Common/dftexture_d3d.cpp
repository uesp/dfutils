/*===========================================================================
 *
 * DFTexture_D3D.CPP - Dave Humphrey (uesp@m0use.net), 9 November 2000
 *
 *=========================================================================*/

	/* Include Files */
#include "dftexture_d3d.h"


#undef  __FUNC__
#define __FUNC__ "CDFTextureD3D::CreateImage()"
/*===========================================================================
 *
 * Class CDFTextureD3D Method - void CreateImage (ImageIndex);
 *
 * Helper function which allocates the given image index.
 *
 *=========================================================================*/
void CDFTextureD3D::CreateImage (const int ImageIndex) { 
  CreatePointer(pImages[ImageIndex], CDFTextureImageD3D);
  NumImagesAllocated++;
 }
/*===========================================================================
 *		End of Class Method CDFTextureD3D::CreateImage()
 *=========================================================================*/


