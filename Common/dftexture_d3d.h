/*===========================================================================
 *
 * DFTexture_D3D.H - Dave Humphrey (uesp@m0use.net), 9 November 2000
 *
 *=========================================================================*/
#ifndef __DFTEXTURE_D3D_H
#define __DFTEXTURE_D3D_H


	/* Required Includes */
#include "dftexture.h"
#include "dftextimg_d3d.h"


/*===========================================================================
 *
 * Begin Class CDFTextureD3D Definition
 *
 *=========================================================================*/
class CDFTextureD3D : public virtual CDFTexture {

  /*---------- Begin Protected Class Members --------------------*/
protected:


  /*---------- Begin Protected Methods --------------------------*/
protected:

	/* Helper function to create texture image objects */
  virtual void CreateImage (const int ImageIndex);


  /*---------- Begin Public Methods -----------------------------*/
public:

	/* Get an image texture object */
  CD3DTexture* GetTexture (const int Index, const int SubIndex = 0) { if (IsValidIndex(Index)) return ((CDFTextureImageD3D *)(pImages[Index]))->GetTexture(SubIndex); return (NULL); }
	
 };
/*===========================================================================
 *		End of Class CDFTextureD3D Definition
 *=========================================================================*/

#endif
/*===========================================================================
 *		End of File DFTexture_D3D.H
 *=========================================================================*/