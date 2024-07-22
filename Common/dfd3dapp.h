/*===========================================================================
 *
 * DFD3DApp.H - Dave Humphrey (uesp@m0use.net), 8 November 2000
 *
 *=========================================================================*/
#ifndef __DFD3DAPP_H
#define __DFD3DAPP_H

	/* Required Includes */
#include "d3dapp.h"
#include "arch_d3d.h"
#include "dfwoods_d3d.h"
#include "dftmapddsurface.h"
#include "command.h"


/*===========================================================================
 *
 * Begin Defines
 *
 *=========================================================================*/

	/* Rate of the sun's rotation in radians/sec */
  #define DF_SUN_ROTATION_RATE D3DVAL(0.002)

	/* Default parameters and options */
  #define DF_DEFAULT_MOVEMENTSPEED    D3DVAL(250.0)
  #define DF_DEFAULT_VIEWBACKLENGTH   D3DVAL(30000.0)
  #define DF_DEFAULT_LANDDETAILFACTOR 10
  #define DF_DEFAULT_FOGFRONTPLANE    D3DVAL(500.0)
  #define DF_DEFAULT_FOGDENSITY	      D3DVAL(0.2)
  #define DF_DEFAULT_CAMERAHEIGHT     D3DVAL(50.0)

/*===========================================================================
 *		End of Defines
 *=========================================================================*/


/*===========================================================================
 *
 * Class CDFD3DApp Definition
 *
 *=========================================================================*/
class CDFD3DApp : public CD3DApp {

  /*---------- Begin Protected Class Members ---------------------------*/
protected:
  CD3DFrame*	pMainFrame;	/* Main world frame */
  CD3DFrame*	pSunFrame;	/* Sun reference frame */
	
  CD3DLight*	pSunLight;	/* The sun light */
  CD3DLight*	pAmbientLight;	/* Ambient light */
	
  d3dcolor_t	FogColor;	/* Color of the fog */
  d3dvalue_t	FogFrontPlane;	/* Start distance of the fog */
  d3dvalue_t	FogDensity;	/* Fog density for exponential fog */
  d3dfogmode_t  FogMode;	/* Type of fog */
  boolean	FogEnable;	/* Is fog visible or not */
	
  d3dvector_t	OriginPosition;		/* Absolute position of the origin, or PC */
  d3dvector_t	RelativeOriginPosition;	/* Relative PC position */

		/* Surface mode objects */
  CDFTravelMapDDSM	DFTravelMapDDSM;
  

  /*---------- Begin Public Class Members -------------------------------*/
public:

		/* Land generation class */
  CDFLandD3D		LandBuilder;



  /*---------- Begin Protected Class Methods ---------------------------*/
protected:

	/* Initializes the DF D3D/base portion of the app */
  boolean DFD3DInitialization (void);
  boolean DFInitialization (void);

	/* Event handlers */
  boolean OnKeyDown		(LRESULT& Result, int KeyCode);
  boolean OnKeyUp		(LRESULT& Result, int KeyCode);


  /*---------- Begin Public Class Methods ------------------------------*/
public:

	/* Class Constructor/Destructor */
  CDFD3DApp();
  virtual ~CDFD3DApp() { Destroy(); }
  virtual void Destroy (void);

	/* Convert an absolute position vector to a relative vector */
  void AbsToRelMapPosition (d3dvector_t& Vector);

	/* Create and initialize the fog settings for the scene */
  boolean CreateFog (void);

	/* Create the main frame for objects */
  boolean CreateMainFrame (void);

	/* Create the sunlight frame and light source */
  boolean CreateSun (void);

	/* Get class members */
  CD3DFrame* GetMainFrame    (void) { return (pMainFrame); }
  CD3DFrame* GetOriginFrame  (void) { return (LandBuilder.GetOriginFrame()); }
  int	     GetOriginXPixel (void) { return (LandBuilder.GetOriginXPixel()); }
  int	     GetOriginYPixel (void) { return (LandBuilder.GetOriginYPixel()); }
  

	/* Handles window messages as per WindowProc() */
  virtual boolean HandleMessages (LRESULT& Result, HWND hWindow, UINT Message, WPARAM wParam, LPARAM lParam);

	/* Initialize the app and D3D interfaces */
  virtual boolean Initialize (HINSTANCE ThisInstance, int CommandShow);

	/* Convert a relative position vector to an absolute in DF map coordinates */
  void RelToAbsMapPosition (d3dvector_t& Vector);

	/* Render the scene to the viewport */
  virtual boolean Render (void);

	/* Change the position of the origin */
  boolean SetOriginPosition (d3dvalue_t X, d3dvalue_t Y, d3dvalue_t Z);


  DECLARE_COMMAND_FUNCMAP()

 };
/*===========================================================================
 *		End of Class CDFD3DApp Definition
 *=========================================================================*/


#endif
/*===========================================================================
 *		End of File DFD3DApp.H
 *=========================================================================*/