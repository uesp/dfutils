/*===========================================================================
 *
 * DFTextImg_D3D.H - Dave Humphrey (uesp@m0use.net), 9 November 2000
 *
 *=========================================================================*/
#ifndef __DFTEXTIMG_D3D_H
#define __DFTEXTIMG_D3D_H

	/* Required Includes */
#include "dfcommon.h"
#include "d3dcom.h"
#include "dfpal_d3d.h"
#include "dftextimg.h"


/*===========================================================================
 *
 * Class CDFTextureImageD3D Definition
 *
 *=========================================================================*/
class CDFTextureImageD3D : public CDFTextureImage {

  /*---------- Begin Protected Class Members ------------------------*/
protected:
  CD3DTexture* pTexture[DFTEXTURE_MAX_SUBIMAGES];	/* The D3D texture object */
  d3dimage_t*  pImageInfo[DFTEXTURE_MAX_SUBIMAGES];	/* D3D texture info */


  /*---------- Begin Protected Class Methods ------------------------*/
protected:

	/* Create the D3D texture object from the current image data */
  virtual boolean CreateTexture (const int ImageIndex = 0);

	/* Count the number of references of the D3D texture */
  virtual ULONG CountTextureRefs (const int ImageIndex = 0);


  /*---------- Begin Public Class Methods ----------------------------*/
public:

	/* Class Constructor and Destructor */
  CDFTextureImageD3D();
  virtual ~CDFTextureImageD3D() { Destroy(); }
  virtual void Destroy (void);

	/* Get class members */
  CD3DTexture* GetTexture (const int Index = 0) { if (IsValidImageIndex(Index)) return (pTexture[Index]); else return (NULL); }

	/* Checks to see if the texture is currently in use */
  boolean IsImageUsed (const int Index = 0) { if (CountTextureRefs(Index) == 0) return (FALSE); return (TRUE); }

  	/* Attempt to read in the image data from the given file stream */
  virtual boolean Read (FILE* pFileHandle);

 };
/*===========================================================================
 *		End of Class CDFTextureImageD3D Definition
 *=========================================================================*/


#endif
/*===========================================================================
 *		End of File DFTextImg_D3D.H
 *=========================================================================*/