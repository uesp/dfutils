/*===========================================================================
 *
 * DFLandBuilder.CPP - Dave Humphrey (uesp@m0use.net), 6 November 2000
 *
 *=========================================================================*/

	/* Include Files */
#include "dflandbuilder.h"
#include "dfmaps.h"
#include "dfcommon.h"
#include "math.h"
#include "profile.h"


#undef  __FUNC__
#define __FUNC__ "CDFLandBuilder::CDFLandBuilder()"
/*===========================================================================
 *
 * Class CDFLandBuilder Constructor
 *
 *=========================================================================*/
CDFLandBuilder::CDFLandBuilder(const boolean CreateBuilders) {
  PixelBuildersAllocated = FALSE;
  pOriginBuilder = NULL;
  OriginPosition.X = 0;
  OriginPosition.Y = 0;
  OriginPosition.Z = 0;
  OriginXPixel = -1;
  OriginYPixel = -1;
  LeftPixel = -1;
  TopPixel = -1;

	/* Create the array of pixel builders if required */
  if (CreateBuilders) CreatePixelBuilders();
 }
/*===========================================================================
 *		End of Class CDFLandBuilder Constructor
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFLandBuilder::Destroy()"
/*===========================================================================
 *
 * Class CDFLandBuilder Destructor
 *
 *=========================================================================*/
void CDFLandBuilder::Destroy (void) {
  DestroyPixelBuilders();
 }
/*===========================================================================
 *		End of Class CDFLandBuilder Destructor
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFLandBuilder::ClearPixelBuilders()"
/*===========================================================================
 *
 * Class CDFLandBuilder Method - void ClearPixelBuilders (void);
 *
 * Initialize the pixel builder array by destroying any information it
 * contains but keeping the allocated objects.  Protected class method.
 *
 *=========================================================================*/
void CDFLandBuilder::ClearPixelBuilders (void) { 
  int XCounter;
  int YCounter;

	/* We don't want to initialize an unallocated array */
  ASSERT(PixelBuildersAllocated == TRUE);

	/* Clear each pixel record */
  for (YCounter = 0; YCounter < DFLAND_PIXELS_ARRAYSIZE; YCounter++) { 
    for (XCounter = 0; XCounter < DFLAND_PIXELS_ARRAYSIZE; XCounter++) { 
      pPixelBuilders[YCounter][XCounter]->Destroy();
     }
   }

 }
/*===========================================================================
 *		End of Class Method CDFLandBuilder::ClearPixelBuilders()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFLandBuilder::CreatePixelBuilders()"
/*===========================================================================
 *
 * Class CDFLandBuilder Method - void CreatePixelBuilders (void);
 *
 * Allocate and initialize the pixel builder array. Protected class method.
 *
 *=========================================================================*/
void CDFLandBuilder::CreatePixelBuilders (void) { 
  int XCounter;
  int YCounter;

	/* We don't want to recreate the array over again */
  ASSERT(PixelBuildersAllocated != TRUE);

	/* Allocate each of the array elements */
  for (YCounter = 0; YCounter < DFLAND_PIXELS_ARRAYSIZE; YCounter++) { 
    for (XCounter = 0; XCounter < DFLAND_PIXELS_ARRAYSIZE; XCounter++) { 
      CreatePointer(pPixelBuilders[YCounter][XCounter], CDFPixelBuilder);
     }
   }

  PixelBuildersAllocated = TRUE;
  pOriginBuilder = pPixelBuilders[DFLAND_PIXELS_ARRAYHALFSIZE][DFLAND_PIXELS_ARRAYHALFSIZE];
 }
/*===========================================================================
 *		End of Class Method CDFLandBuilder::CreatePixelBuilders()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFLandBuilder::DestroyPixelBuilders()"
/*===========================================================================
 *
 * Class CDFLandBuilder Method - void DestroyPixelBuilders (void);
 *
 * Delete any allocated pixel builders.  Protected class method.
 *
 *=========================================================================*/
void CDFLandBuilder::DestroyPixelBuilders (void) { 
  int XCounter;
  int YCounter;

	/* Ensure there is something to delete */
  if (!PixelBuildersAllocated) return;

	/* Unallocate all records in the array */
  for (YCounter = 0; YCounter < DFLAND_PIXELS_ARRAYSIZE; YCounter++) { 
    for (XCounter = 0; XCounter < DFLAND_PIXELS_ARRAYSIZE; XCounter++) { 
      DestroyPointer(pPixelBuilders[YCounter][XCounter]);
     }
   }

  PixelBuildersAllocated = FALSE;
  pOriginBuilder = NULL;
 }
/*===========================================================================
 *		End of Class Method CDFLandBuilder::DestroyPixelBuilders()
 *=========================================================================*/

#undef  __FUNC__
#define __FUNC__ "CDFLandBuilder::GenerateLandPixel()"
/*===========================================================================
 *
 * Class CDFLandBuilder Method - boolean GenerateLandPixel (XIndex, YIndex);
 *
 * Creates the vertex/face arrays for the given pixel record.  Returns FALSE
 * on any error.  Protected class method.
 *
 *=========================================================================*/
boolean CDFLandBuilder::GenerateLandPixel (const int XIndex, const int YIndex) {
  int Result;

	/* Ensure valid indices and objects */
  ASSERT(XIndex >= 0 && XIndex < DFLAND_PIXELS_ARRAYSIZE);
  ASSERT(YIndex >= 0 && YIndex < DFLAND_PIXELS_ARRAYSIZE);
  ASSERT(PixelBuildersAllocated == TRUE);
    
	/* Create the pixel builder data */
  Result = pPixelBuilders[YIndex][XIndex]->CreateArrays();
  return (Result);
 }
/*===========================================================================
 *		End of Class Method CDFLandBuilder::GenerateLandPixel()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFLandBuilder::LoadPixelRecords()"
/*===========================================================================
 *
 * Class CDFLandBuilder Method - boolean LoadPixelRecords (void);
 *
 * Load the required pixel records to generate the landscape at the current
 * position from the WOODS file.  Returns FALSE on any error.
 *
 *=========================================================================*/
boolean CDFLandBuilder::LoadPixelRecords (void) {
  CDFPixelBuilder** ppPixelPtr;
  int		    Result;
  int		    XCounter;
  int		    YCounter;

  	/* Do we have to do anything? */
  if (!HasOriginPixelChanged()) return (TRUE);

  PROFILE_INC();
  START_PROFILE(ShiftPixelBuilders);

	/* Do we need to load all pixel records, or can we shift some around? */
  ShiftPixelBuilders();

  END_PROFILE(ShiftPixelBuilders);
  START_PROFILE(OpenWoodsFile);

	/* Ensure the woods file is open */
  Result = DFWoodsFile.Open();
  if (!Result) return (FALSE);

  END_PROFILE(OpenWoodsFile);

  ppPixelPtr = &pPixelBuilders[0][0];

	/* Load all the pixel records as required */
  for (YCounter = 0; YCounter < DFLAND_PIXELS_ARRAYSIZE; YCounter++) { 
    for (XCounter = 0; XCounter < DFLAND_PIXELS_ARRAYSIZE; XCounter++, ppPixelPtr++) { 

      START_PROFILE(IsLoaded);

		/* Load the record only if required */
      //if (pPixelBuilders[YCounter][XCounter]->IsLoaded()) continue;
      if ((*ppPixelPtr)->IsLoaded()) continue;

      END_PROFILE(IsLoaded);
      START_PROFILE(ReadPixelRecord);
 
      //Result = pPixelBuilders[YCounter][XCounter]->LoadPixelRecord();
      //Result = (*ppPixelPtr)->LoadPixelRecord();
      Result = DFWoodsFile.ReadPixelRecord((*ppPixelPtr)->GetXPixel(), (*ppPixelPtr)->GetYPixel(), **ppPixelPtr);
      
      END_PROFILE(ReadPixelRecord);
      START_PROFILE(CheckResult);

      if (!Result) {
        DFWoodsFile.Close();
        return (FALSE);
       }

      END_PROFILE(CheckResult);
     }
   }

  START_PROFILE(WoodsClose);
  DFWoodsFile.Close();
  END_PROFILE(WoodsClose);

  PROFILE_DEC();
  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFLandBuilder::LoadPixelRecords()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFLandBuilder::SetBuilderReferences()"
/*===========================================================================
 *
 * Class CDFLandBuilder Method - void SetBuilderReferences (void)
 *
 * Sets the neighbour references in the builder array.
 *
 *=========================================================================*/
void CDFLandBuilder::SetBuilderReferences (void) {
  int XCounter;
  int YCounter;

  for (YCounter = 0; YCounter < DFWOODS_PIXEL_SIZE; YCounter++) {
    for (XCounter = 0; XCounter < DFWOODS_PIXEL_SIZE; XCounter++) {

      if (XCounter == 0)
        pPixelBuilders[YCounter][XCounter]->SetLeftPixel(NULL);
      else
        pPixelBuilders[YCounter][XCounter]->SetLeftPixel(pPixelBuilders[YCounter][XCounter-1]);

      if (XCounter == DFWOODS_PIXEL_SIZE - 1)
        pPixelBuilders[YCounter][XCounter]->SetRightPixel(NULL);
      else
        pPixelBuilders[YCounter][XCounter]->SetRightPixel(pPixelBuilders[YCounter][XCounter+1]);

      if (YCounter == 0) 
        pPixelBuilders[YCounter][XCounter]->SetTopPixel(NULL);
      else
        pPixelBuilders[YCounter][XCounter]->SetTopPixel(pPixelBuilders[YCounter-1][XCounter]);

      if (YCounter == DFWOODS_PIXEL_SIZE - 1)
        pPixelBuilders[YCounter][XCounter]->SetBottomPixel(NULL);
      else
        pPixelBuilders[YCounter][XCounter]->SetBottomPixel(pPixelBuilders[YCounter+1][XCounter]);

      pPixelBuilders[YCounter][XCounter]->SetPosition(XCounter + LeftPixel, YCounter + TopPixel);
     }
   }

 }
/*===========================================================================
 *		End of Class Method CDFLandBuilder::SetBuilderReferences()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFLandBuilder::SetOriginPosition()"
/*===========================================================================
 *
 * Class CDFLandBuilder Method - void SetOriginPosition (X, Y, Z);
 *
 * Changes the origin of the landscape where the camera is located.  Adjusts
 * all the required variables.
 *
 *=========================================================================*/
void CDFLandBuilder::SetOriginPosition (const float X, const float Y, const float Z) { 
  OriginPosition.X = X;  
  OriginPosition.Y = Y; 
  OriginPosition.Z = Z; 
  OriginXPixel = (int) ((X)/DFLAND_PIXEL_COORSIZE); 
  OriginYPixel = (int) ((DFLAND_MAX_ZPOS - Z)/DFLAND_PIXEL_COORSIZE); 
  LeftPixel = OriginXPixel - DFLAND_PIXELS_ARRAYHALFSIZE;  
  TopPixel  = OriginYPixel - DFLAND_PIXELS_ARRAYHALFSIZE; 
 }
/*===========================================================================
 *		End of Class Method CDFLandBuilder::SetOriginPosition()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFLandBuilder::ShiftPixelBuilders()"
/*===========================================================================
 *
 * Class CDFLandBuilder Method - void ShiftPixelBuilders (void);
 *
 * Attempt to shift records in the pixel builder array around to reuse 
 * already loaded items.
 *
 *=========================================================================*/
void CDFLandBuilder::ShiftPixelBuilders (void) {
  CDFPixelBuilder*  pCopyPixels[DFLAND_PIXELS_ARRAYSIZE][DFLAND_PIXELS_ARRAYSIZE];
  int		    XPixelDiff;
  int		    YPixelDiff;
  int		    XCounter;
  int		    YCounter;
  int		    TargetX;
  int		    TargetY;
  float		    XFramePos;
  float		    ZFramePos;
  boolean	    ClearYRecord = FALSE;
  boolean	    ClearXRecord = FALSE;
  
	/* Must load all pixel records */
  if (pOriginBuilder == NULL || !pOriginBuilder->IsLoaded()) {
    ClearPixelBuilders();
    SetBuilderReferences();
    return;
  }
	
	/* Check the previous/current pixel coordinates */
  XPixelDiff = pOriginBuilder->GetXPixel() - OriginXPixel;
  YPixelDiff = pOriginBuilder->GetYPixel() - OriginYPixel;

	/* Do we have to load all new records? */
  if (abs(XPixelDiff) >= DFLAND_PIXELS_ARRAYSIZE || abs(YPixelDiff) >= DFLAND_PIXELS_ARRAYSIZE) {
    ClearPixelBuilders();
    SetBuilderReferences();
    return;
   }

	/* Create a copy of the pixel record array */
  memcpy(&pCopyPixels, &pPixelBuilders, sizeof(CDFPixelBuilder*) * DFLAND_PIXELS_ARRAYSIZE * DFLAND_PIXELS_ARRAYSIZE);
  
	/* Shift array row by row */
  for (YCounter = 0, TargetY = YPixelDiff; YCounter < DFLAND_PIXELS_ARRAYSIZE; YCounter++, TargetY++) {

	/* Ensure the targety array index properly wraps */
    if (TargetY < 0) {
      TargetY += DFLAND_PIXELS_ARRAYSIZE;
      ClearYRecord = (boolean) !ClearYRecord;
     }
    else if (TargetY >= DFLAND_PIXELS_ARRAYSIZE) {
      TargetY -= DFLAND_PIXELS_ARRAYSIZE;
      ClearYRecord = (boolean) !ClearYRecord;
     }

		/* Shift each column of array */
    for (XCounter = 0, ClearXRecord = FALSE, TargetX = XPixelDiff; XCounter < DFLAND_PIXELS_ARRAYSIZE; XCounter++, TargetX++) {
    
		/* Ensure the targetX array index properly wraps */
      if (TargetX < 0) {
        TargetX += DFLAND_PIXELS_ARRAYSIZE;
	ClearXRecord = (boolean) !ClearXRecord;
       }
      else if (TargetX >= DFLAND_PIXELS_ARRAYSIZE) {
        TargetX -= DFLAND_PIXELS_ARRAYSIZE;
	ClearXRecord = (boolean) !ClearXRecord;
       }

		/* Shift the pixel record */
      pPixelBuilders[TargetY][TargetX] = pCopyPixels[YCounter][XCounter];

		/* Clear the pixel builder if required */
      if (ClearXRecord || ClearYRecord) {
        pPixelBuilders[TargetY][TargetX]->Destroy();
       }
		/* Shift the position of the builder frame */
      else {
        XFramePos = (float)(((float) (TargetX - DFLAND_PIXELS_ARRAYHALFSIZE)) * DFLAND_PIXEL_COORSIZE);
        ZFramePos = (float)(((float) (TargetY - DFLAND_PIXELS_ARRAYHALFSIZE)) * DFLAND_PIXEL_COORSIZE);
        pPixelBuilders[TargetY][TargetX]->SetFramePosition(XFramePos, 0, ZFramePos);
       }
     }
   }
  
	/* Reset the orgin pixel pointer and builder references */
  pOriginBuilder = pPixelBuilders[DFLAND_PIXELS_ARRAYHALFSIZE][DFLAND_PIXELS_ARRAYHALFSIZE];
  SetBuilderReferences();
 }
/*===========================================================================
 *		End of Class Method CDFLandBuilder::ShiftPixelBuilders()
 *=========================================================================*/
