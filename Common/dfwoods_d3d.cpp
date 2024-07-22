/*===========================================================================
 *
 * DFWoods_D3D.CPP - Dave Humphrey (uesp@m0use.net), 7 November 2000
 *
 *=========================================================================*/

	/* Include Files */
#include "dfwoods_d3d.h"
#include "dfcommon_d3d.h"
#include "dfcommon.h"
#include "dfblocks_d3d.h"
#include "profile.h"


#undef  __FUNC__
#define __FUNC__ "CDFLandD3D::CDFLandD3D()"
/*===========================================================================
 *
 * Class CDFLandD3d Constructor
 *
 *=========================================================================*/
CDFLandD3D::CDFLandD3D() : CDFLandBuilder(FALSE) {
  pParentFrame = NULL;

  CreatePixelBuilders();
 }
/*===========================================================================
 *		End of Class CDFLandD3D Constructor
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFLandD3D::Destroy()"
/*===========================================================================
 *
 * Class CDFLandD3d Destructor
 *
 *=========================================================================*/
void CDFLandD3D::Destroy (void) {
  pParentFrame = NULL;
  
	/* Call the base class destructor */
  CDFLandBuilder::Destroy();
 }
/*===========================================================================
 *		End of Class CDFLandD3D Destructor
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFLandD3D::CreateLandscape()"
/*===========================================================================
 *
 * Class CDFLandD3d Method - boolean CreateLandscape (void);
 *
 * Creates the entire build array with the landscape objects as nessecary.
 * Returns D3DRM_OK on success.
 *
 *=========================================================================*/
boolean CDFLandD3D::CreateLandscape (void) {
  int	     XCounter;
  int	     YCounter;
  d3dvalue_t XTranslation;
  d3dvalue_t ZTranslation;
  HRESULT    Result;
  
	/* Ensure there is a destination frame and valid Direct3D objects */
  ASSERT(pParentFrame != NULL);
  ASSERT(GetD3DRM()   != NULL);

  START_PROFILE(LoadPixelRecords);
  
  	/* Load the required pixel records from the woods file */
  Result = LoadPixelRecords();
  if (!Result) return (FALSE);

  END_PROFILE(LoadPixelRecords);

	/* Initialize and create the build array */
  for (YCounter = 1; YCounter < DFLAND_PIXELS_ARRAYSIZE - 1; YCounter++) {
    for (XCounter = 1; XCounter < DFLAND_PIXELS_ARRAYSIZE - 1; XCounter++) {

		/* Set the parent frame for the land pixel */
      ((CDFPixelBuilderD3D *)(pPixelBuilders[YCounter][XCounter]))->SetParentFrame(pParentFrame);

      START_PROFILE(CreateLandscape);

		/* Create the landscape/location for the pixel  */
      Result = pPixelBuilders[YCounter][XCounter]->CreateLandscape();
      if (!Result) return (FALSE);

      END_PROFILE(CreateLandscape);

		/* Translate the frame to the desired world position (X-Z only).  The origin
	 * pixel always sits at (X=0, Z=0) */
      XTranslation = D3DVAL(((float) (XCounter - DFLAND_PIXELS_ARRAYHALFSIZE)) * DFLAND_PIXEL_COORSIZE);
      ZTranslation = D3DVAL(((float) (YCounter - DFLAND_PIXELS_ARRAYHALFSIZE)) * DFLAND_PIXEL_COORSIZE);

      Result = pPixelBuilders[YCounter][XCounter]->SetFramePosition(XTranslation, 0, ZTranslation);
      if (!Result) return (FALSE);
     }
   }

	/* Return success */
  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFLandD3D::CreateLandscape()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFLandD3D::CreatePixelBuilders()"
/*===========================================================================
 *
 * Class CDFLandD3D Method - void CreatePixelBuilders (void);
 *
 * Allocate and initialize the pixel builder array. Protected class method.
 *
 *=========================================================================*/
void CDFLandD3D::CreatePixelBuilders (void) { 
  int XCounter;
  int YCounter;

	/* We don't want to recreate the array over again */
  ASSERT(PixelBuildersAllocated != TRUE);

	/* Allocate each of the array elements */
  for (YCounter = 0; YCounter < DFLAND_PIXELS_ARRAYSIZE; YCounter++) { 
    for (XCounter = 0; XCounter < DFLAND_PIXELS_ARRAYSIZE; XCounter++) { 
      CreatePointer(pPixelBuilders[YCounter][XCounter], CDFPixelBuilderD3D);
     }
   }

  PixelBuildersAllocated = TRUE;
  pOriginBuilder = pPixelBuilders[DFLAND_PIXELS_ARRAYHALFSIZE][DFLAND_PIXELS_ARRAYHALFSIZE];
 }
/*===========================================================================
 *		End of Class Method CDFLandD3D::CreatePixelBuilders()
 *=========================================================================*/



#undef  __FUNC__
#define __FUNC__ "CDFLandD3D::GetOriginFrame()"
/*===========================================================================
 *
 * Class CDFLandD3D Method - CD3DFrame* GetOriginFrame (void)
 *
 * Retrieve the reference frame of the origin land pixel.  Returns NULL
 * on any error.
 *
 *=========================================================================*/
CD3DFrame* CDFLandD3D::GetOriginFrame (void) {

	/* Ensure valid objects */
  ASSERT(PixelBuildersAllocated == TRUE);

  return ( ((CDFPixelBuilderD3D*)pPixelBuilders[DFLAND_PIXELS_ARRAYHALFSIZE][DFLAND_PIXELS_ARRAYHALFSIZE])->GetFrame());
 }
/*===========================================================================
 *		End of Class Method CDFLandD3D::GetOriginFrame()
 *=========================================================================*/
