/*===========================================================================
 *
 * DFTMapSurface.CPP - Dave Humphrey (uesp@m0use.net), 13 November 2000
 *
 *=========================================================================*/

	/* Include Files */
#include "dftmapddsurface.h"
#include "dfcommon_d3d.h"
#include "d3dapp.h"
#include "dfd3dapp.h"
#include "dflandbuilder.h"


#undef  __FUNC__
#define __FUNC__ "CDFTravelMapDDSM::CDFTravelMapDDSM()"
/*===========================================================================
 *
 * Class CDFTravelMapDDSM Constructor
 *
 *=========================================================================*/
CDFTravelMapDDSM::CDFTravelMapDDSM() {
 }
/*===========================================================================
 *		End of Class CDFTravelMapDDSM Constructor
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFTravelMapDDSM::Destroy()"
/*===========================================================================
 *
 * Class CDFTravelMapDDSM Destructor
 *
 *=========================================================================*/
void CDFTravelMapDDSM::Destroy (void) {

	/* Delete allocated images */
  DFMapImage.Destroy();
  DFRegionImage.Destroy();
 }
/*===========================================================================
 *		End of Class CDFTravelMapDDSM Destructor
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFTravelMapDDSM::InitSurfaceMode()"
/*===========================================================================
 *
 * Class CDFTravelMapDDSM Method - boolean InitSurfaceMode (hWindow, CD3DApp* pApp);
 *
 * Calls the base class member then sets up the DF portion.
 *
 *=========================================================================*/
boolean CDFTravelMapDDSM::InitSurfaceMode (HWND hWindow, CD3DApp* pApp) {
  HRESULT Result;

	/* Call the base class member first */
  Result = CDDSurfaceMode::InitSurfaceMode(hWindow, pApp);
  if (!Result) return (FALSE);

	/* Attempt to load the required images */
  Result = DFGetImageFile("trav0i00.img", DFMapImage); 	
  if (!Result) return (FALSE);
  
  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFTravelMapDDSM::InitSurfaceMode()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFTravelMapDDSM::OnKeyUp()"
/*===========================================================================
 *
 * Class CDFTravelMapDDSM Event - boolean OnKeyUp (Result, KeyCode);
 *
 *=========================================================================*/
boolean CDFTravelMapDDSM::OnKeyUp (LRESULT& Result, int KeyCode) {

	/* Ignore if not currently active */
  if (!IsActive()) return (FALSE);

  switch (KeyCode) {
    case VK_ESCAPE:
      pParentApp->RemoveSurfaceMode();
      Result = 0;
      return (TRUE);
   };

  return (FALSE);
 }
/*===========================================================================
 *		End of Class Event CDFTravelMapDDSM::OnKeyUp()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFTravelMapDDSM::OnLButtonDown()"
/*===========================================================================
 *
 * Class CDFTravelMapDDSM Event - boolean OnLButtonDown (Result, KeyFlags, XPos, YPos);
 *
 *=========================================================================*/
boolean CDFTravelMapDDSM::OnLButtonDown (LRESULT& Result, int KeyFlags, int XPos, int YPos) {
  RECT  Rect;
  float TamXPos;
  float TamYPos;

	/* Ignore if not currently active */
  if (!IsActive()) return (FALSE);

  GetClientRect(hMainWindow, &Rect);

	/* Convert to tamriel coordinates */
  TamXPos = (float) ( ((float)XPos) / ((float)Rect.right)*(float)DFLAND_PIXEL_COORSIZE*1000.0);
  TamYPos = (float) (( ((float)YPos)) / ((float)Rect.bottom - 40.0/200.0)*(float)DFLAND_PIXEL_COORSIZE*500.0);
  TamYPos = (float) ( ( ((float)YPos) / ((float)Rect.bottom - 40.0*(float)Rect.bottom/200.0) - (float) 12.0/200.0) * ((float)DFLAND_PIXEL_COORSIZE*500.0) );
  TamYPos = DFLAND_MAX_ZPOS - TamYPos;
  SystemLog.Printf ("Tam Position = %.0f, %.0f", TamXPos, TamYPos);

	/* Ensure valid coordinates */
  if (TamXPos <  DFLAND_MIN_XPOS) return (TRUE);
  if (TamXPos >= DFLAND_MAX_XPOS) return (TRUE);
  if (TamYPos <  DFLAND_MIN_ZPOS) return (TRUE);
  if (TamYPos >= DFLAND_MAX_ZPOS) return (TRUE);

	/* Change the origin position */
  ((CDFD3DApp*)pParentApp)->SetOriginPosition(TamXPos, 0, TamYPos);
  pParentApp->RemoveSurfaceMode();
  return (TRUE);
 }
/*===========================================================================
 *		End of Class Event CDFTravelMapDDSM::OnLButtonDown()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFTravelMapDDSM::Update()"
/*===========================================================================
 *
 * Class CDFTravelMapDDSM Method - void Update (void);
 *
 * Updates the surface display.
 *
 *=========================================================================*/
void CDFTravelMapDDSM::Update (void) { 
  HDC		hDC;
  HRESULT	Result;
  RECT		DestRect;
  RECT		SourceRect;
  POINT		OffsetPoint = { 0, 0 };
  int		OriginX;
  int		OriginY;
  int		RectWidth;
  int		RectHeight;

	/* Ignore if not currently active */
  if (!IsActive()) return;

	/* Get the client area of the main window */
  Result = GetClientRect(hMainWindow, &DestRect);

  if (!Result) {
    SET_WIN_ERROR2("CDFTravelMapDDSM::Update() - Failed to retrieve window rectangle!"); 
    return;
   }

  RectWidth = DestRect.right;
  RectHeight = DestRect.bottom;
  ClientToScreen(hMainWindow, &OffsetPoint);
  OffsetRect(&DestRect, OffsetPoint.x, OffsetPoint.y);
  SourceRect.left = 0;
  SourceRect.top = 0;
  SourceRect.right = DFMapImage.GetWidth();
  SourceRect.bottom = DFMapImage.GetHeight();
  SystemLog.Printf ("MapWidth = %d", DFMapImage.GetWidth());

	/* Attempt to copy main map image */
  Result = pPrimarySurface->Blt(&DestRect, DFMapImage.GetDDSurface(), &SourceRect, 0, NULL);

  if (FAILED(Result)) {
    SET_D3D_ERROR2(Result, "CDFTravelMapDDSM::Update() - Failed to blt DF map image!"); 
    return;
   }

	/* Update the current origin position */
  OriginX = ((CDFD3DApp*)pParentApp)->GetOriginXPixel();
  OriginY = ((CDFD3DApp*)pParentApp)->GetOriginYPixel();

	/* Convert to window coordinates */
  OriginX = (int) ( ((float)OriginX)/1000.0 * (float) RectWidth) + OffsetPoint.x;
  OriginY = (int) ( ( ((float)OriginY) / 500.0 + 12.0/200.0) * ( ((float)RectHeight) - 40.0/200.0*(float)RectHeight) );
  OriginY += OffsetPoint.y;

	/* Get DC for surface */  
  Result = pPrimarySurface->GetDC(&hDC);
  if (FAILED(Result)) return;

	/* Select a red pen object */
  HPEN    hPen = CreatePen(PS_SOLID, 1, RGB(255,0,0));
  HGDIOBJ hLastPen = SelectObject(hDC, hPen);

	/* Draw current position */
  MoveToEx(hDC, DestRect.left, OriginY, NULL);
  LineTo(hDC, DestRect.right, OriginY);
  MoveToEx(hDC, OriginX, DestRect.top, NULL);
  LineTo(hDC, OriginX, DestRect.bottom);

	/* Release DC */
  SelectObject(hDC, hLastPen);
  pPrimarySurface->ReleaseDC(hDC);
 }
/*===========================================================================
 *		End of Class Method CDFTravelMapDDSM::Update()
 *=========================================================================*/