/*===========================================================================
 *
 * DFWoods_D3D.H - Dave Humphrey (uesp@m0use.net), 7 November 2000
 *
 *=========================================================================*/
#ifndef __DFWOODS_D3D_H
#define __DFWOODS_D3D_H

	/* Requird Includes */
#include "dflandbuilder.h"
#include "dfpixel_d3d.h"
#include "d3dcom.h"
#include "dftexture_d3d.h"


/*===========================================================================
 *
 * Class CDFLandD3D Definitions
 *
 *=========================================================================*/
class CDFLandD3D : virtual public CDFLandBuilder {

  /*---------- Begin Protected Class Members ---------------------*/
protected:
  CD3DFrame* pParentFrame;


  /*---------- Begin Protected Class Methods ---------------------*/
protected:

	/* Allocate the pixel builder array */
  virtual void CreatePixelBuilders (void);


  /*---------- Begin Public Class Methods ------------------------*/
public:

	/* Class Constructor/Destructor */
  CDFLandD3D();
  virtual ~CDFLandD3D (void) { Destroy(); }
  virtual void Destroy (void);

	/* Create the landscape objects */
  boolean CreateLandscape (void);

	/* Get class members */
  CD3DFrame* GetOriginFrame (void);

	/* Set class members */
  void SetParentFrame (CD3DFrame* pFrame) { pParentFrame = pFrame; }

 };
/*===========================================================================
 *		End of Class CDFLandD3D Definition
 *=========================================================================*/


#endif
/*===========================================================================
 *		End of File DFWoods_D3D.H
 *=========================================================================*/