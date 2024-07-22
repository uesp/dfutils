/*===========================================================================
 *
 * DFPixel_D3D.CPP - Dave Humphrey (uesp@m0use.net), 23 November 2000
 *
 *=========================================================================*/

	/* Include Files */
#include "dfpixel_d3d.h"
#include "dfcommon_d3d.h"
#include "profile.h"


#undef  __FUNC__
#define __FUNC__ "CDFPixelBuilderD3D::CDFPixelBuilderD3D()"
/*===========================================================================
 *
 * Class CDFPixelBuilderD3D Constructor
 * 
 *=========================================================================*/
CDFPixelBuilderD3D::CDFPixelBuilderD3D() {
  pBuilder = NULL;
  pFrame = NULL;
  pParentFrame = NULL;
 }
/*===========================================================================
 *		End of Class CDFPixelBuilderD3D Constructor
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFPixelBuilderD3D::CDFPixelBuilderD3D()"
/*===========================================================================
 *
 * Class CDFPixelBuilderD3D Destructor
 *
 *=========================================================================*/
void CDFPixelBuilderD3D::Destroy (void) {

	/* Release the Direct3D objects */
  if (pBuilder != NULL && pFrame != NULL) {
    pFrame->DeleteVisual(pBuilder);
    if (pParentFrame != NULL) pParentFrame->DeleteChild(pFrame);
   }

  RELEASE(pBuilder);
  RELEASE(pFrame);
  pParentFrame = NULL;

	/* Call the base class method */
  CDFPixelBuilder::Destroy();
 }
/*===========================================================================
 *		End of Class CDFPixelBuilderD3D Destructor
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFPixelBuilderD3D::CreateLandscape()"
/*===========================================================================
 *
 * Class CDFPixelBuilderD3D Method - boolean CreateLandscape (void)
 *
 * Creates the Direct3D landscape objects for the current land pixel.
 * Returns FALSE on any error.
 *
 *=========================================================================*/
boolean CDFPixelBuilderD3D::CreateLandscape (void) {
  HRESULT Result;

	/* Don't do anything if the builder object already exists */
  if (pBuilder != NULL) return (TRUE);

  PROFILE_INC();
  START_PROFILE(CreateArrays);

  	/* Generate the landscape vertex/face arrays */
  CreateArrays();

  END_PROFILE(CreateArrays);
  START_PROFILE(MakeBuilder);

	/* Create a mesh builder object from the D3D object as required */
  Result = MakeBuilder();
  if (FAILED(Result)) return (FALSE);

  END_PROFILE(MakeBuilder);
  START_PROFILE(AddBuilder);

      	/* Add the builder mesh to the frame */
  Result = pFrame->AddVisual(pBuilder);

  if (FAILED(Result)) {
    SET_D3D_ERROR2(Result, "Failed to add landscape object to the landscape frame!");
    return (FALSE);
   }

  END_PROFILE(AddBuilder);
  START_PROFILE(CreateLocation);

	/* Translate the frame to the desired world position (X-Z only).  The origin
	 * pixel always sits at (X=0, Z=0) */
  //XTranslation = D3DVAL(((float) (XCounter - DFLAND_PIXELS_ARRAYHALFSIZE)) * DFLAND_PIXEL_COORSIZE);
  //ZTranslation = D3DVAL(((float) (YCounter - DFLAND_PIXELS_ARRAYHALFSIZE)) * DFLAND_PIXEL_COORSIZE);
  //(*ppFramePtr)->SetPosition(pParentFrame, XTranslation, 0, ZTranslation);
	
	/* Create any locations in the land pixel */
  Result = CreateLocation();

  END_PROFILE(CreateLocation);
  PROFILE_DEC();

  return ((boolean) Result);
 }
/*===========================================================================
 *		End of Class Method CDFPixelBuilderD3D::CreateLandscape()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFPixelBuilderD3D::CreateLocation()"
/*===========================================================================
 *
 * Class CDFPixelBuilderD3D Method - boolean CreateLocation (void)
 *
 * Creates any locations in the land pixel. Returns FALSE on any error.
 * Protected class method.
 *
 *=========================================================================*/
boolean CDFPixelBuilderD3D::CreateLocation (void) {
  CD3DFrame*		pBlockFrame = NULL;
  CDFRMBFileD3D*	pRMBFile;
  CDFMapPItemRecord*    pPItem;
  dfmap_pitem_footer_t* pFooter;
  HRESULT               Result;
  d3dvalue_t		XPos;
  d3dvalue_t		ZPos;
  int			XCounter;
  int			YCounter;
  int			Counter;
  
	/* Ignore if no location in pixel or the location is already created */
  if (LocationIndex < 0 || CreatedLocation) return (TRUE);

  PROFILE_INC();
  START_PROFILE(ReadRegion);

	/* Read in the region data */
  Result = DFMaps.ReadRegion(RegionIndex, DFMAP_REGION_ALL);
  if (!Result) return (FALSE);

  END_PROFILE(ReadRegion);
  START_PROFILE(GetItems);

	/* Attempt to get the location PItem data */
  pPItem = DFMaps.GetMapPItem (RegionIndex, LocationIndex);
  if (pPItem == NULL) return (FALSE);
  pFooter = pPItem->GetFooter();

  END_PROFILE(GetItems);
  START_PROFILE(BlockLoop);
  PROFILE_INC();

	/* Load and add all valid blocks */
  for (YCounter = 0, Counter = 0; YCounter < pFooter->BlockHeight; YCounter++) {
    for (XCounter = 0; XCounter < pFooter->BlockWidth; XCounter++, Counter++) {

		/* Release the previous block frame, if any */
      RELEASE(pBlockFrame);

      START_PROFILE(CreateBlock);

		/* Create the temporary reference frame */
      Result = GetD3DRM()->CreateFrame(pFrame, &pBlockFrame);

      if (FAILED(Result)) {
        SET_D3D_ERROR2(Result, "Failed to create block record frame!");
        return (FALSE);
       }
    
      END_PROFILE(CreateBlock);
      START_PROFILE(GetRMBFile);

		/* Load the current block information */
      pRMBFile = DFBlocks.GetRMBFileD3D ((int) pFooter->BlockFileIndex[Counter], 
    				         (int) pFooter->BlockFileChar[Counter], 
				         (int) pFooter->BlockFileNumber[Counter]);

		/* TODO: Error loading block, just continue for now */
      if (pRMBFile == NULL) continue;

      END_PROFILE(GetRMBFile);
      START_PROFILE(CreateD3DBlock);

		/* Attempt to create and add the block to the frame */
      Result = pRMBFile->CreateD3DBlock(DFBLK_FLAG_DEFAULT, pBlockFrame);

      END_PROFILE(CreateD3DBlock);
      START_PROFILE(SetPosition);

		/* Move the frame to the appropiate position */
      XPos = D3DVAL(XCounter*2000.0 + (pPItem->GetXPosition() % (long) DFLAND_PIXEL_COORSIZE));
      ZPos = D3DVAL(YCounter*2000.0 + (pPItem->GetZPosition() % (long) DFLAND_PIXEL_COORSIZE));
      pBlockFrame->SetPosition(pFrame, XPos, -300.0, ZPos);

      END_PROFILE(SetPosition);
     }
   }

  PROFILE_DEC();
  END_PROFILE(BlockLoop);
  PROFILE_DEC();

	/* Update the location creation flag */
  SetCreatedLocation(TRUE);
  RELEASE(pBlockFrame);
  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFPixelBuilderD3D::CreateLocation()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFPixelBuilderD3D::MakeBuilder()"
/*===========================================================================
 *
 * Class CDFPixelBuilderD3D Method - boolean MakeBuilder (void);
 *
 * Makes a Direct3D builer object from the current land data.  Returns FALSE
 * on any error. Protected class member.
 *
 *=========================================================================*/
boolean CDFPixelBuilderD3D::MakeBuilder (void) {
  CD3DTexture*  pTexture = NULL;
  CD3DMaterial*	pMaterial = NULL;
  CD3DWrap*	pWrap = NULL;
  HRESULT	Result;
  float		FaceSize;

	/* Ensure valid objects */
  ASSERT(GetD3DRM()   != NULL);
  ASSERT(pParentFrame != NULL);

  PROFILE_INC();
  START_PROFILE(CreateFrame);

  	/* Create a destination frame from the D3D object */
  Result = GetD3DRM()->CreateFrame(pParentFrame, &pFrame);

  if (FAILED(Result)) {
    SET_D3D_ERROR2(Result, "Failed to create landscape frame!"); 
    return (FALSE);
   }

  END_PROFILE(CreateFrame);
  START_PROFILE(CreateBuilder);

	/* Create a mesh builder object from the D3D object */
  Result = GetD3DRM()->CreateMeshBuilder(&pBuilder);

  if (FAILED(Result)) {
    SET_D3D_ERROR2(Result, "Failed to create mesh builder object!"); 
    return (FALSE);
   }

  END_PROFILE(CreateBuilder);
  START_PROFILE(AddFaces);

	/* Attempt to build the object using the create vertex/face arrays */
  Result = pBuilder->AddFaces(NumVertices, (D3DVECTOR*)GetVertices(),  0, NULL, GetFaceData(), NULL);

  if (FAILED(Result)) {
    SET_D3D_ERROR2(Result, "Failed to add faces to the mesh builder!"); 
    return (FALSE);
   }

  END_PROFILE(AddFaces);
  START_PROFILE(GenerateNormals);

	/* Create the normals for the faces */
  Result = pBuilder->GenerateNormals();

  if (FAILED(Result)) {
    SET_D3D_ERROR2(Result, "Failed to generate landscape normals!");
    return (FALSE);
   }

  END_PROFILE(GenerateNormals);
  START_PROFILE(SetColor);

	/* Set the default color of the mesh */
  Result = pBuilder->SetColorRGB(D3DVAL(0.9), D3DVAL(0.9), D3DVAL(0.9));

  if (FAILED(Result)) {
    SET_D3D_ERROR2(Result, "Failed to set the builder mesh color!");
    return (FALSE);
   }

  END_PROFILE(SetColor);
  START_PROFILE(CreateMaterial);

  	/* Create a default material */
  Result = GetD3DRM()->CreateMaterial(D3DVAL(10.0), &pMaterial);

  if (FAILED(Result)) {
    SET_D3D_ERROR2(Result, "Failed to create the material!"); 
    return (FALSE);
   }

  END_PROFILE(CreateMaterial);
  START_PROFILE(SetMaterial);

	/* Set the mesh material */
  Result = pBuilder->SetMaterial(pMaterial);

  if (FAILED(Result)) {
    SET_D3D_ERROR2(Result, "Failed to set the mesh builder material!"); 
    RELEASE(pMaterial);
    return (FALSE);
   }

  END_PROFILE(SetMaterial);
  START_PROFILE(CreateWrap);

	/* Create a wrap  */
  FaceSize = (float) ((float) DFLAND_PIXEL_COORSIZE / (float) VertexSize);

  Result = GetD3DRM()->CreateWrap(D3DRMWRAP_FLAT, NULL, 
			      D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0),
			      D3DVAL(0.0), D3DVAL(1.0), D3DVAL(0.0),
			      D3DVAL(0.0), D3DVAL(0.0), D3DVAL(1.0), 
			      D3DVAL(0.0), D3DVAL(1.0), 
			      D3DVAL(1.0/FaceSize), D3DVAL(1.0/FaceSize), 
			      &pWrap);

  if (FAILED(Result)) {
    SET_D3D_ERROR2(Result, "Failed to create a Direct3D wrap object!"); 
    return (FALSE);
   }

  END_PROFILE(CreateWrap);
  START_PROFILE(GetTexture);

  if (GetSmallPixel() < 3)
    pTexture = DFGetD3DTexture(GetClimateType(), 0);
  else
    pTexture = DFGetD3DTexture(GetClimateType(), 2);

  END_PROFILE(GetTexture);

  if (pTexture != NULL) {
    START_PROFILE(ApplyWrap);

    Result = pWrap->Apply((CD3DObject*) pBuilder);

    if (FAILED(Result)) {
      SET_D3D_ERROR2(Result, "Failed to apply wrap to landscape mesh!");
      RELEASE(pMaterial);
      RELEASE(pWrap);
      return (FALSE);
     } 

    END_PROFILE(ApplyWrap);
    START_PROFILE(SetTexture);

    Result = pBuilder->SetTexture(pTexture);

    if (FAILED(Result)) {
      SET_D3D_ERROR2(Result, "Failed to apply texture to landscape mesh!");
      RELEASE(pMaterial);
      RELEASE(pWrap);
      return (FALSE);
     } 

    END_PROFILE(SetTexture);
   }

  PROFILE_DEC();

  	/* Return success */
  RELEASE(pMaterial);
  RELEASE(pWrap);
  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFPixelBuilderD3D::MakeBuilder()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFPixelBuilderD3D::SetFramePosition()"
/*===========================================================================
 *
 * Class CDFPixelBuilderD3D Method - boolean SetFramePosition (XPos, YPos, ZPos);
 *
 * Set the position of the land pixel Direct3D frame.  Returns FALSE on
 * any error.
 *
 *=========================================================================*/
boolean CDFPixelBuilderD3D::SetFramePosition (const float XPos, const float YPos, const float ZPos) {
  HRESULT Result;

	/* Ignore if no frames have been created yet */
  if (pFrame == NULL || pParentFrame == NULL) return (TRUE);

	/* Attempt to set the frame position */
  Result = pFrame->SetPosition(pParentFrame, XPos, YPos, ZPos);
 
  if (FAILED(Result)) { 
    SET_D3D_ERROR2(Result, "Failed to set the landscape pixel frame position!");
    return (FALSE);
   }

  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFPixelBuilderD3D::SetFramePosition()
 *=========================================================================*/
