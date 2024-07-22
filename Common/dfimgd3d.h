/*===========================================================================
 *
 * DFImgD3D.H - Dave Humphrey (uesp@m0use.net), 12 November 2000
 *
 *=========================================================================*/
#ifndef __DFIMGD3D_H
#define __DFIMGD3D_H

	/* Required Includes */
#include "dfimg.h"
#include "d3dcom.h"


/*===========================================================================
 *
 * Class CDFImgFileD3D Definition
 *
 *=========================================================================*/
class CDFImgFileD3D : public virtual CDFImgFile {

  /*---------- Begin Protected Class Members -----------------------*/
protected:
  CDDSurface* pSurface;		/* DirectDraw surface object */
  ddsdesc_t   SurfaceDesc;	/* Surface description */


  /*---------- Begin Protected Class Methods -----------------------*/
protected:

	/* Internal image conversion routines */
  boolean MakeRGB16DDSurface (ddsdesc_t& Desc);


  /*---------- Begin Public Class Methods --------------------------*/
public:

	/* Class Constructor/Destructor */
  CDFImgFileD3D() { pSurface = NULL; }
  virtual ~CDFImgFileD3D() { Destroy(); }
  virtual void Destroy (void) { RELEASE(pSurface); CDFImgFile::Destroy(); }

	/* Return the image's DirectDraw surface */
  CDDSurface* GetDDSurface (void) { return (pSurface); }

	/* Make the image's DD surface object */
  boolean MakeDDSurface (void);

	/* Read in image data from a file stream */
  boolean Read (FILE* pFileHandle);

 };
/*===========================================================================
 *		End of Class CDFImgFileD3D
 *=========================================================================*/


#endif
/*===========================================================================
 *		End of File DFImgD3D.H
 *=========================================================================*/