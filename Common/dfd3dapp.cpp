/*===========================================================================
 *
 * DFD3DApp.CPP - Dave Humphrey (uesp@m0use.net), 8 November 2000
 *
 *=========================================================================*/

	/* Include Files */
#include "dfd3dapp.h"
#include "dfcommon_d3d.h"


BEGIN_COMMAND_FUNCTION(CDFD3DApp)
END_COMMAND_FUNCTION()


#undef  __FUNC__
#define __FUNC__ "CDFD3DApp::CDFD3DApp()"
/*===========================================================================
 *
 * Class CDFD3DApp Constructor
 *
 *=========================================================================*/
CDFD3DApp::CDFD3DApp() {

	/* Set DF object default parameters */
  //LandBuilder.SetDetailFactor(DF_DEFAULT_LANDDETAILFACTOR);

	/* Initialize D3D objects */
  pSunFrame = NULL;
  pMainFrame = NULL;
  pSunLight = NULL;
  pAmbientLight = NULL;

	/* Set default parameters */
  MovementSpeed  = DF_DEFAULT_MOVEMENTSPEED;
  ViewBackLength = DF_DEFAULT_VIEWBACKLENGTH;
  FogColor       = D3DRGB(1.0,1.0,1.0);
  FogMode	 = D3DRMFOG_LINEAR;
  FogEnable      = TRUE;
  FogFrontPlane  = DF_DEFAULT_FOGFRONTPLANE;
  FogDensity     = DF_DEFAULT_FOGDENSITY;

  OriginPosition.x = D3DVAL(0.0);
  OriginPosition.y = D3DVAL(0.0);
  OriginPosition.z = D3DVAL(0.0);
  RelativeOriginPosition.x = D3DVAL(0.0);
  RelativeOriginPosition.y = D3DVAL(0.0);
  RelativeOriginPosition.z = D3DVAL(0.0);

  CameraHeight = DF_DEFAULT_CAMERAHEIGHT;
 }
/*===========================================================================
 *		End of Class CDFD3DApp Constructor
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFD3DApp::Destroy()"
/*===========================================================================
 *
 * Class CDFD3DApp Destructor
 *
 *=========================================================================*/
void CDFD3DApp::Destroy (void) {

	/* Destroy the land builder object first */
  LandBuilder.Destroy();

	/* Unallocate D3D objects */
  RELEASE(pSunFrame);
  RELEASE(pMainFrame);
  RELEASE(pSunLight);
  RELEASE(pAmbientLight);

	/* Call the base class method */
  CD3DApp::Destroy();  
 }
/*===========================================================================
 *		End of Class CDFD3DApp Destructor
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFD3DApp::AbsToRelMapPosition()"
/*===========================================================================
 *
 * Class CDFD3DApp Method - void AbsToRelMapPosition (Vector);
 *
 * Converts a absolute map position vector to an relative map position in 
 * DF map coordinates.
 *
 *=========================================================================*/
void CDFD3DApp::AbsToRelMapPosition (d3dvector_t& Vector) {
  Vector.x -= DFPixelToMapX(GetOriginXPixel());
  Vector.z = DFPixelToMapZ(GetOriginYPixel()) - Vector.z;
 }
/*===========================================================================
 *		End of Class Method CDFD3DApp::AbsToRelMapPosition()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFD3DApp::CreateFog()"
/*===========================================================================
 *
 * Class CDFD3DApp Method - boolean CreateFog (void);
 *
 * Creates and initializes the fog settings for the scene.  Returns
 * TRUE on success or FALSE on any error.
 *
 *=========================================================================*/
boolean CDFD3DApp::CreateFog (void) {

	/* Ensure a valid scene object */
  ASSERT(pScene    != NULL);

  pScene->SetSceneFogMode(FogMode);
  pScene->SetSceneFogColor(FogColor);
  pScene->SetSceneFogEnable(FogEnable);
  pScene->SetSceneFogParams(FogFrontPlane, ViewBackLength, FogDensity);

  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFD3DApp::CreateFog();
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFD3DApp::CreateMainFrame()"
/*===========================================================================
 *
 * Class CDFD3DApp Method - boolean CreateMainFrame (void);
 *
 * Creates and initializes the main frame.  Returns
 * TRUE on success or FALSE on any error.
 *
 *=========================================================================*/
boolean CDFD3DApp::CreateMainFrame (void) {
  HRESULT Result;

	/* Ensure valid objects */
  ASSERT(pDirect3D  != NULL);
  ASSERT(pScene     != NULL);
  ASSERT(pMainFrame == NULL);
  
	/* Create the frame object */
  Result = pDirect3D->CreateFrame(pScene, &pMainFrame);

  if (FAILED(Result)) {
    SET_D3D_ERROR2(Result, "Error creating the main reference frame!");
    return (FALSE); 
   }

	/* Rotate 180 degrees about the Y-axis */
  Result = pMainFrame->SetOrientation(pScene, D3DVAL(0.0), D3DVAL(0.0), D3DVAL(1.0), 
					      D3DVAL(0.0), D3DVAL(-1.0), D3DVAL(0.0));

  if (FAILED(Result)) {
    SET_D3D_ERROR2(Result, "Error setting the main frame orientation!");
    return (FALSE); 
   } 

	/* Set the position of the sub-frame relative to the scene */
  Result = pMainFrame->SetPosition(pScene, D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0));

  if (FAILED(Result)) {
    SET_D3D_ERROR2(Result, "Error setting main frame position!");
    return (FALSE); 
   }

	/* Set default scene background color to something 'nice' */
  pScene->SetSceneBackgroundRGB(D3DVAL(0.4), D3DVAL(0.4), D3DVAL(0.7));

  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFD3DApp::CreateMainFrame()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFD3DApp::CreateSun()"
/*===========================================================================
 *
 * Class CDFD3DApp Method - boolean CreateSun (void);
 *
 * Creates and initializes the sunlight frame and sun light source.  Returns
 * TRUE on success or FALSE on any error.
 *
 *=========================================================================*/
boolean CDFD3DApp::CreateSun (void) {
  HRESULT Result;

	/* Ensure valid objects */
  ASSERT(pDirect3D != NULL);
  ASSERT(pScene    != NULL);
  ASSERT(pSunFrame == NULL);

  	/* Create a sub-frame under the main scene */	
  Result = pDirect3D->CreateFrame(pScene, &pSunFrame);

  if (FAILED(Result)) {
    SET_D3D_ERROR2(Result, "Error creating the Sun reference frame!");
    return (FALSE); 
   }

    	/* Set the sub-frame rotation */
  Result = pSunFrame->SetRotation(pScene, D3DVAL(1.0), D3DVAL(0.0), D3DVAL(0.0), DF_SUN_ROTATION_RATE*10);

  if (FAILED(Result)) {
    SET_D3D_ERROR2(Result, "Error setting the Sun frame rotation!");
    return (FALSE); 
   }
   
   	/* Create a directional light source (the sun) */
  Result = pDirect3D->CreateLightRGB(D3DRMLIGHT_DIRECTIONAL, D3DVAL(1.0), D3DVAL(1.0), D3DVAL(1.0), &pSunLight);

  if (FAILED(Result)) {
    SET_D3D_ERROR2(Result, "Error creating the SunLight object!");
    return (FALSE); 
   }

	/* Create an ambient light source */
  Result = pDirect3D->CreateLightRGB(D3DRMLIGHT_AMBIENT, D3DVAL(0.5), D3DVAL(0.5), D3DVAL(0.5), &pAmbientLight);

  if (FAILED(Result)) {
    SET_D3D_ERROR2(Result, "Error creating the AmbientLight object!");
    return (FALSE); 
   }

	/* Add the sunlight to the light light frame */
  Result = pSunFrame->AddLight(pSunLight);

  if (FAILED(Result)) {
    SET_D3D_ERROR2(Result, "Error adding Sunlight to the sun frame!");
    return (FALSE); 
   }

	/* Add the ambient light to the light sub-frame frame */
  Result = pSunFrame->AddLight(pAmbientLight);

  if (FAILED(Result)) {
    SET_D3D_ERROR2(Result, "Error adding the AmbientLight to the sun frame!");
    return (FALSE); 
   }

  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFD3DApp::CreateSun()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFD3DApp::DFInitialization()"
/*===========================================================================
 *
 * Class CDFD3DApp Method - boolean DFInitialization (void);
 *
 * Initializes the required DF components of the application.  Returns
 * FALSE on any error.
 *
 *=========================================================================*/
boolean CDFD3DApp::DFInitialization (void) {
  HRESULT   Result;
  D3DVECTOR CameraVector;

	/* Create the main DF object frame */
  Result = CreateMainFrame();
  if (Result) Result = CreateSun();
  if (Result) Result = CreateFog();
  if (!Result) return (FALSE);

	/* Preload the map tables */
  Result = DFMaps.Read(DFMAP_REGION_MINIMUM);
  if (!Result) return (FALSE);

	/* Set the object references for the land builder */
  LandBuilder.SetParentFrame(pMainFrame);

	/* Retrieve the camera position */
  Result = pCamera->GetPosition(pMainFrame, &CameraVector);

  if (FAILED(Result)) {
    SET_D3D_ERROR2(Result, "Failed to get the camera position!");
    return (FALSE);
   }

	/* Set the position of the camera */
  //Result = SetOriginPosition (D3DVAL(547.0*32400.0 + 120.0), D3DVAL(-350.0), D3DVAL((500-48)*32400.0 + 56));
  //Result = SetOriginPosition (D3DVAL(19866762), D3DVAL(-350.0), D3DVAL(3796973));
  Result = SetOriginPosition (D3DVAL(20101777), D3DVAL(-350.0), D3DVAL(3850125));
  if (!Result) return (FALSE);

  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFD3DApp::DFInitialization()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFD3DApp::HandleMessages()"
/*===========================================================================
 *
 * Class CDFD3DApp Method - long HandleMessages (&lResult, hWindow, Message, wParam, lParam);
 *
 * Handles any general window messages.
 *
 *=========================================================================*/
boolean CDFD3DApp::HandleMessages (LRESULT& Result, HWND hWindow, UINT Message, WPARAM wParam, LPARAM lParam) {
  boolean bResult;

	/* Call the base class first */
  bResult = CD3DApp::HandleMessages(Result, hWindow, Message, wParam, lParam);
  if (bResult) return (TRUE);

  return (FALSE);
 }
/*===========================================================================
 *		End of Class Method CDFD3DApp::HandleMessage()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFD3DApp::Initialize()"
/*===========================================================================
 *
 * Class CDFD3DApp Method - boolean Initialize (ThisInstance, CommandShow);
 *
 * Standard intialization function.  Performs all the initialization
 * needed to get the app running.
 *
 *=========================================================================*/
boolean CDFD3DApp::Initialize (HINSTANCE ThisInstance, int CommandShow) {
  boolean Result;

	/* Call the base class method first */
  Result = CD3DApp::Initialize(ThisInstance, CommandShow);
  if (!Result) return (FALSE);

	/* Perform the Daggerfall specific Direct3D initialization */
  Result = DFInitialization();
  Initialized = TRUE;

  return (Result);
 }
/*===========================================================================
 *		End of Class Method CdfD3DApp::Initialize()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFD3DApp::OnKeyDown()"
/*===========================================================================
 *
 * Class CD3DApp Event - boolean OnKeyDown (lResult, KeyDown);
 *
 * Called when a key is pressed
 *
 *=========================================================================*/
boolean CDFD3DApp::OnKeyDown (LRESULT& lResult, int KeyCode) {

 
	/* Call the base class */
  return (CD3DApp::OnKeyDown(lResult, KeyCode));
 }      
/*===========================================================================
 *		End of Class Event CDFD3DApp::OnKeyDown()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFD3DApp::OnKeyUp()"
/*===========================================================================
 *
 * Class CD3DApp Event - boolean OnKeyUp (lResult, KeyCode);
 *
 * Called when a key is pressed
 *
 *=========================================================================*/
boolean CDFD3DApp::OnKeyUp (LRESULT& lResult, int KeyCode) {
  lResult = 0;
  
  switch (KeyCode) {
    case VK_ESCAPE:
      if (pCurrentSurfaceMode == NULL) break;
      RemoveSurfaceMode();
      return (TRUE);
    case 'M':
      SetSurfaceMode(&DFTravelMapDDSM);
      return (TRUE);
   }
        
	/* Call the base class */
  return (CD3DApp::OnKeyUp(lResult, KeyCode));
 }      
/*===========================================================================
 *		End of Class Event CDFD3DApp::OnKeyUp()
 *=========================================================================*/


/*===========================================================================
 *
 * Class CDFD3DApp Method - void RelToAbsMapPosition (Vector);
 *
 * Converts a relative map position vector to an absolute map position in 
 * DF map coordinates.
 *
 *=========================================================================*/
void CDFD3DApp::RelToAbsMapPosition (d3dvector_t& Vector) {
  Vector.x += DFPixelToMapX(GetOriginXPixel());
  Vector.z = DFPixelToMapZ(GetOriginYPixel()) - Vector.z;
 }
/*===========================================================================
 *		End of Class Method CDFD3DApp::RelToAbsMapPosition()
 *=========================================================================*/


/*===========================================================================
 *
 * Class CDFD3DApp Method - boolean Render (void);
 *
 * Updates the window with one frame.
 *
 *=========================================================================*/
boolean CDFD3DApp::Render (void) {
  HRESULT   Result;
  D3DVECTOR CameraVector;

	/* Ensure we should be rendering now */
  if (!CanRenderScene()) return (FALSE);

	/* Call the base class member first */
  Result = CD3DApp::Render();
  if (!Result) return (FALSE);

	/* Retrieve the camera position */
  Result = pCamera->GetPosition(pMainFrame, &CameraVector);

  if (FAILED(Result)) {
    SET_D3D_ERROR2(Result, "Failed to get the camera position!");
    return (FALSE);
   }

	/* Set the origin position for the land builder */
  RelToAbsMapPosition(CameraVector);
  Result = SetOriginPosition (CameraVector.x, CameraVector.y, CameraVector.z);
  if (!Result) return (FALSE);

  HDC	   hDC;
  char     Buffer[64];
  int      BufferSize;

  hDC = GetDC(hMainWindow);

  if (hDC) { 
    BufferSize = sprintf (Buffer, "%03d, %03d (%.0f, %.0f, %.0f)", LandBuilder.GetOriginXPixel(), LandBuilder.GetOriginYPixel(), CameraVector.x, CameraVector.y, CameraVector.z);
    TextOut(hDC, 10, 10, Buffer, BufferSize);
    ReleaseDC(hMainWindow, hDC);
   }

  return (TRUE);
 }
/*===========================================================================
 *		End of Class CDFD3DApp::Render()
 *=========================================================================*/


/*===========================================================================
 *
 * Class CDFD3DApp Method - boolean SetOriginPosition (X, Y, Z);
 *
 * Sets the position of the origin in absolute DF coordinates.
 *
 *=========================================================================*/
boolean CDFD3DApp::SetOriginPosition (d3dvalue_t X, d3dvalue_t Y, d3dvalue_t Z) {
  HRESULT Result;

	/* Ensure valid objects */
  if (pCamera == NULL) {
    SET_EXT_ERROR(ERR_NULL);
    return (FALSE);
   }

	/* Set the absolute position */
  OriginPosition.x = X;
  OriginPosition.y = Y;
  OriginPosition.z = Z;
  RelativeOriginPosition = OriginPosition;

	/* Compute the relative position */
  AbsToRelMapPosition(RelativeOriginPosition);

	/* Change the landbuilder origin position */
  LandBuilder.SetOriginPosition (X, Y, Z);

	/* Do we need to update the landscape? */
  if (LandBuilder.HasOriginPixelChanged()) {

		/* Recreate the landscape */
    ResetLastRender();
    Result = LandBuilder.CreateLandscape();
    if (!Result) return (FALSE);

		/* Compute the relative position */
    RelativeOriginPosition = OriginPosition;
    AbsToRelMapPosition(RelativeOriginPosition);

		/* Change the camera postion */
    Result = pCamera->SetPosition (pMainFrame, RelativeOriginPosition.x, RelativeOriginPosition.y, RelativeOriginPosition.z);

    if (FAILED(Result)) {
      SET_D3D_ERROR2(Result, "Failed to set the camera position!");
      return (FALSE);
     }
   }

  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFD3DApp::SetOriginPosition()
 *=========================================================================*/