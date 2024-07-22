/*===========================================================================/*===========================================================================
 *
 * DFPixel_D3D.H - Dave Humphrey (uesp@m0use.net), Thursday, November 23, 2000
 *
 *=========================================================================*/
#ifndef __DFPIXEL_D3D_H
#define __DFPIXEL_D3D_H

	/* Required Includes */
#include "dfpixelbuilder.h"
#include "d3dcom.h"
#include "dfblocks_d3d.h"


/*===========================================================================
 *
 * Class CDFPixelBuilderD3D Definition
 *
 *=========================================================================*/
class CDFPixelBuilderD3D : public CDFPixelBuilder {

  /*---------- Begin Protected Class Members -------------------------*/
protected:
  CD3DMeshBuilder* pBuilder;		/* The D3D builder object */
  CD3DFrame*	   pFrame;		/* The frame of the object */
  CD3DFrame*	   pParentFrame;	/* The parent frame of the object */


  /*---------- Begin Protected Class Methods -------------------------*/
protected:

	/* Create the location for the current land pixel */
  boolean CreateLocation  (void);

	/* Creates the Direct3D builder object for the landscape pixel */
  boolean MakeBuilder (void);


  /*---------- Begin Public Class Methods ----------------------------*/
public:

	/* Class Constructor/Destructor */
  CDFPixelBuilderD3D();
  virtual ~CDFPixelBuilderD3D() { Destroy(); }
  virtual void Destroy (void);

   	/* Create the landscape for the current land pixel */
  virtual boolean CreateLandscape (void);

	/* Get the class members */
  CD3DFrame*       GetFrame   (void) { return (pFrame); }
  CD3DMeshBuilder* GetBuilder (void) { return (pBuilder); }

  	/* Set the position of the land pixel reference frame */
  boolean SetFramePosition (const float XPos, const float YPos, const float ZPos);

	/* Set the parent frame of the landscape pixel */
  void SetParentFrame (CD3DFrame* pNewFrame) { pParentFrame = pNewFrame; }

 };
/*===========================================================================
 *		End of Class CDFPixelBuilderD3D definition
 *=========================================================================*/


#endif
/*===========================================================================
 *		End of File DFPixel_D3D.H
 *=========================================================================*/
