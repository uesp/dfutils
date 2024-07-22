/*===========================================================================
 *
 * DFTMapDDSurface.H - Dave Humphrey (uesp@m0use.net), 13 November 2000
 *
 *=========================================================================*/
#ifndef __DFTMAPDDSURFACE_H
#define __DFTMAPDDSURFACE_H


	/* Required Includes */
#include "ddsurfmode.h"
#include "dfimgd3d.h"
#include "dfpal_d3d.h"


/*===========================================================================
 *
 * Class CDFTravelMapDDSM Definition
 *
 *=========================================================================*/
class CDFTravelMapDDSM : public CDDSurfaceMode {

  /*---------- Begin Protected Class Members ------------------------*/
protected:
  CDFImgFileD3D DFMapImage;		/* Overall travel map of daggerfall */
  CDFImgFileD3D DFRegionImage;		/* Individual travel maps */



  /*---------- Begin Protected Class Methods ------------------------*/
protected:

	/* Event handlers */
  virtual boolean OnKeyUp	(LRESULT& Result, int KeyCode);
  virtual boolean OnLButtonDown	(LRESULT& Result, int KeyFlags, int XPos, int YPos);


  /*---------- Begin Public Class Methods ---------------------------*/
public:

	/* Class Constructor/Destructor */
  CDFTravelMapDDSM();
  virtual void Destroy (void);

	/* Initialize the surface mode and DF parameters */
  virtual boolean InitSurfaceMode (HWND hWindow, CD3DApp* pApp);

	/* Update the display */
  virtual void Update (void);

 };
/*===========================================================================
 *		End of Class CDFTravelMapDDSM Definition
 *=========================================================================*/


#endif
/*===========================================================================
 *		End of File DFTMapDDSurface.H
 *=========================================================================*/