/*===========================================================================
 *
 * DFBlocks_D3D.CPP - Dave Humphrey (uesp@m0use.net), 19 November 2000
 *
 *=========================================================================*/

	/* Include Files */
#include "dfblocks_d3d.h"
#include "dfcommon_d3d.h"
#include "profile.h"


#undef  __FUNC__
#define __FUNC__ "CDFRMBFileD3D::CreateBlockRecords()"
/*===========================================================================
 *
 * Class CDFRMBFileD3D Method - void CreateBlockRecords (void);
 *
 * Allocates the block records. A virtual and protected class method.
 *
 *=========================================================================*/
void CDFRMBFileD3D::CreateBlockRecords (void) {
  CreatePointerArray(pOutsideRecords, CDFRMBRecordD3D, FLDData.NumBlocks);
  CreatePointerArray(pInsideRecords,  CDFRMBRecordD3D, FLDData.NumBlocks);
 }
/*===========================================================================
 *		End of Class Method CDFRMBFile::CreateBlockRecords()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFRMBRecordD3D::Create3DObjects()"
/*===========================================================================
 *
 * Class CDFRMBRecordD3D Method - boolean Create3DObjects (pDestFrame)
 *
 * Creates and adds all the 3D Objects in the RMB record to the given 
 * reference frame.  Returns FALSE on any error.  Protected class method.
 *
 *=========================================================================*/
boolean CDFRMBRecordD3D::Create3DObjects (CD3DFrame* pDestFrame) {
  CD3DFrame*       pFrame = NULL;
  CD3DMeshBuilder* pBuilder;
  d3dbox_t	   Box;
  HRESULT	   Result;
  int		   LoopCounter;
  long		   ObjectID;
  float		   YPos;

  for (LoopCounter = 0; LoopCounter < Header.Num3DObjectRecords; LoopCounter++) {
    RELEASE(pFrame);

		/* Create the temporary reference frame */
    Result = GetD3DRM()->CreateFrame(pDestFrame, &pFrame);

    if (FAILED(Result)) {
      SET_D3D_ERROR2(Result, "Failed to create block record frame!");
      return (FALSE);
     }

    START_PROFILE(GetBuilder);

		/* Ensure the given Arch3D object ID is valid */
    ObjectID = (long) (p3DObjectRecords[LoopCounter].ObjectID1*100 + p3DObjectRecords[LoopCounter].ObjectID2);
    pBuilder = DFArch3D.GetD3DBuilder(ObjectID);
    if (pBuilder == NULL) continue;

    END_PROFILE(GetBuilder);
    START_PROFILE(AddVisual);

	/* Add the visual mesh builder */
    Result = pFrame->AddVisual(pBuilder);

    if (FAILED(Result)) {
      SET_D3D_ERROR2(Result, "Failed to add block record visual to frame!");
      RELEASE(pFrame);
      return (FALSE);
     }

    END_PROFILE(AddVisual);

    pBuilder->GetBox(&Box);
    
		/* Compute the proper Y-Position for the object */
    switch(p3DObjectRecords[LoopCounter].Unknown1) {
      case 3:
        YPos = -D3DVAL(p3DObjectRecords[LoopCounter].YPos2) - (Box.max.y - Box.min.y)/2;
	break;
      default:
        YPos = D3DVAL(p3DObjectRecords[LoopCounter].YPos2);
        break;
     }

		/* Set the position of object's frame */
    Result = pFrame->SetPosition(pDestFrame, D3DVAL(p3DObjectRecords[LoopCounter].XPos2), 
					     YPos,
					     D3DVAL(p3DObjectRecords[LoopCounter].ZPos2));

		/* Set the rotation of the object's frame */
    Result = pFrame->AddRotation(D3DRMCOMBINE_BEFORE, D3DVAL(0.0), D3DVAL(-1.0), D3DVAL(0.0), D3DVAL(p3DObjectRecords[LoopCounter].Angle)/1024.0*M_PI);

    SystemLog.Printf ("Item %d of %d:", LoopCounter, Header.Num3DObjectRecords);
    SystemLog.Printf ("\t\tUnknowns  = 0x%02X 0x%08lX 0x%08lX 0x%08lX 0x%08lX 0x%08lX", p3DObjectRecords[LoopCounter].Unknown1, p3DObjectRecords[LoopCounter].Unknown2, p3DObjectRecords[LoopCounter].Unknown3, p3DObjectRecords[LoopCounter].Unknown4, p3DObjectRecords[LoopCounter].Unknown5, p3DObjectRecords[LoopCounter].Unknown6);
    SystemLog.Printf ("\t\tPosition  = (%ld, %ld, %ld)", p3DObjectRecords[LoopCounter].XPos2, p3DObjectRecords[LoopCounter].YPos2, p3DObjectRecords[LoopCounter].ZPos2);
    SystemLog.Printf ("\t\tPosition1 = (%ld, %ld, %ld)", p3DObjectRecords[LoopCounter].XPos1, p3DObjectRecords[LoopCounter].YPos1, p3DObjectRecords[LoopCounter].ZPos1);
   }

  RELEASE(pFrame);
  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFRMBRecordD3D::Create3DObjects()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFRMBRecordD3D::CreateFlats()"
/*===========================================================================
 *
 * Class CDFRMBRecordD3D Method - boolean CreateFlats (pDestFrame)
 *
 * Creates and adds all the Flat Objects in the RMB record to the given 
 * reference frame.  Returns FALSE on any error.  Protected class method.
 *
 *=========================================================================*/
boolean CDFRMBRecordD3D::CreateFlats (CD3DFrame* pDestFrame) {
  CD3DFrame*       pFrame = NULL;
  CD3DTexture*	   pTexture;
  d3dimage_t*	   pImageInfo;
  HRESULT	   Result;
  int		   LoopCounter;

	/* Add all flats to the destination frame */
  for (LoopCounter = 0; LoopCounter < Header.NumFlatObjectRecords; LoopCounter++) {
    RELEASE(pFrame);

		/* Create the temporary reference frame */
    Result = GetD3DRM()->CreateFrame(pDestFrame, &pFrame);

    if (FAILED(Result)) {
      SET_D3D_ERROR2(Result, "Failed to create flat object record frame!");
      return (FALSE);
     }

		/* Get the decal texture */
    pTexture = DFGetD3DTexture(pFlatObjectRecords[LoopCounter].TextureIndex, pFlatObjectRecords[LoopCounter].SubImageIndex);
    if (pTexture == NULL) continue;

		/* Set the decal options */
    pTexture->SetDecalScale(TRUE);
    pTexture->SetDecalTransparency(TRUE);
    pTexture->SetDecalTransparentColor(D3DRGB(0,0,0));
    pImageInfo = pTexture->GetImage();
    ASSERT(pImageInfo != NULL);
    pTexture->SetDecalOrigin(pImageInfo->width/2, pImageInfo->height);
    pTexture->SetDecalSize(DFD3D_DECAL_SCALE(pImageInfo->width), DFD3D_DECAL_SCALE(pImageInfo->height));
     
		/* Add the visual texture decal */
    Result = pFrame->AddVisual(pTexture);

    if (FAILED(Result)) {
      SET_D3D_ERROR2(Result, "Failed to add flat record visual to frame!");
      RELEASE(pFrame);
      return (FALSE);
     }

    		/* Translate texture decal frame */
    Result = pFrame->SetPosition(pDestFrame, D3DVAL(pFlatObjectRecords[LoopCounter].XPos), D3DVAL(pFlatObjectRecords[LoopCounter].YPos), D3DVAL(pFlatObjectRecords[LoopCounter].ZPos));
   }

  RELEASE(pFrame);
  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFRMBRecordD3D::CreateFlats()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFRMBRecordD3D::CreatePeople()"
/*===========================================================================
 *
 * Class CDFRMBRecordD3D Method - boolean CreatePeople (pDestFrame)
 *
 * Creates and adds all the People Objects in the RMB record to the given 
 * reference frame.  Returns FALSE on any error.  Protected class method.
 *
 *=========================================================================*/
boolean CDFRMBRecordD3D::CreatePeople (CD3DFrame* pDestFrame) {
  CD3DFrame*       pFrame = NULL;
  CD3DTexture*	   pTexture;
  d3dimage_t*	   pImageInfo;
  HRESULT	   Result;
  int		   LoopCounter;

	/* Add all flats to the destination frame */
  for (LoopCounter = 0; LoopCounter < Header.NumPeopleRecords; LoopCounter++) {
    RELEASE(pFrame);

		/* Create the temporary reference frame */
    Result = GetD3DRM()->CreateFrame(pDestFrame, &pFrame);

    if (FAILED(Result)) {
      SET_D3D_ERROR2(Result, "Failed to create people object frame!");
      return (FALSE);
     }

		/* Get the decal texture */
    pTexture = DFGetD3DTexture(pPeopleRecords[LoopCounter].TextureIndex, pPeopleRecords[LoopCounter].SubImageIndex);
    if (pTexture == NULL) continue;

		/* Set the decal options */
    pTexture->SetDecalScale(TRUE);
    pTexture->SetDecalTransparency(TRUE);
    pTexture->SetDecalTransparentColor(D3DRGB(0,0,0));
    pImageInfo = pTexture->GetImage();
    ASSERT(pImageInfo != NULL);
    pTexture->SetDecalOrigin(pImageInfo->width/2, pImageInfo->height);
    pTexture->SetDecalSize(DFD3D_DECAL_SCALE(pImageInfo->width), DFD3D_DECAL_SCALE(pImageInfo->height));
     
		/* Add the visual texture decal */
    Result = pFrame->AddVisual(pTexture);

    if (FAILED(Result)) {
      SET_D3D_ERROR2(Result, "Failed to add people record visual to frame!");
      RELEASE(pFrame);
      return (FALSE);
     }

    		/* Translate texture decal frame */
    Result = pFrame->SetPosition(pDestFrame, D3DVAL(pPeopleRecords[LoopCounter].XPos), D3DVAL(pPeopleRecords[LoopCounter].YPos), D3DVAL(pPeopleRecords[LoopCounter].ZPos));
   }

  RELEASE(pFrame);
  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFRMBRecordD3D::CreatePeople()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFRMBFileD3D::CreateD3DBlock()"
/*===========================================================================
 *
 * Class CDFRMBRecordD3D Method - boolean CreateD3DBlock (Flags, pDestFrame);
 *
 * Attempts to add 3D objects in the RMB record to the given frame according
 * to the given creation options. Returns FALSE on any error.
 *
 *=========================================================================*/
boolean CDFRMBRecordD3D::CreateD3DBlock (const dfblk_flag_t Flags, CD3DFrame* pDestFrame) {
  boolean Result;

	/* Ensure valid objects */
  ASSERT(pDestFrame != NULL);
  ASSERT(GetD3DRM() != NULL);

  PROFILE_INC();
  
	/* Add all 3D objects to the frame if required */
  if (Flags & DFBLK_FLAG_3DOBJECTS) {
    Result = Create3DObjects(pDestFrame);
    if (!Result) return (FALSE);
   }

	/* Add all flat objects to the frame if required */
  if (Flags & DFBLK_FLAG_FLATS) {
    Result = CreateFlats(pDestFrame);
    if (!Result) return (FALSE);
   }

	/* Add all people objects to the frame if required */
  if (Flags & DFBLK_FLAG_PEOPLE) {
    Result = CreatePeople(pDestFrame);
    if (!Result) return (FALSE);
   }

  PROFILE_DEC();

  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFRMBRecordD3D::CreateD3DBlock()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFRMBFileD3D::CreateD3DBlock()"
/*===========================================================================
 *
 * Class CDFRMBFileD3D Method - boolean CreateD3DBlock (Flags, pDestFrame);
 *
 * Attempts to add 3D objects in the RMB file to the given frame according
 * to the given options. Returns FALSE on any error.
 *
 *=========================================================================*/
boolean CDFRMBFileD3D::CreateD3DBlock (const dfblk_flag_t Flags, CD3DFrame* pDestFrame) {
  CD3DFrame* pFrame = NULL;
  HRESULT    Result = TRUE;
  int	     LoopCounter;

	/* Ensure valid input */
  ASSERT(pDestFrame != NULL);

  PROFILE_INC();

	/* Add all blocks to the frame */
  for (LoopCounter = 0; LoopCounter < FLDData.NumBlocks; LoopCounter++) {
    RELEASE(pFrame);

    START_PROFILE(CreateFrame);

		/* Create the temporary reference frame */
    Result = GetD3DRM()->CreateFrame(pDestFrame, &pFrame);

    if (FAILED(Result)) {
      SET_D3D_ERROR2(Result, "Failed to create block record frame!");
      return (FALSE);
     }

    END_PROFILE(CreateFrame);
    START_PROFILE(CreateOutsideBlock);

		/* Add the outside blocks */
    if (Flags & DFBLK_FLAG_OUTSIDE) {
      Result = ((CDFRMBRecordD3D *)pOutsideRecords)[LoopCounter].CreateD3DBlock(Flags, pFrame);
      if (!Result) break;
     }

    END_PROFILE(CreateOutsideBlock);
    START_PROFILE(CreateInsideBlock);

		/* Add the inside blocks */
    if (Flags & DFBLK_FLAG_INSIDE) {
      Result = ((CDFRMBRecordD3D *)pInsideRecords)[LoopCounter].CreateD3DBlock(Flags, pFrame);
      if (!Result) break;
     }

    END_PROFILE(CreateInsideBlock);

		/* Set the appropriate frame position */
    Result = pFrame->SetPosition(pDestFrame, D3DVAL(FLDData.PositionRecords[LoopCounter].XPos), D3DVAL(-200.0), D3DVAL(FLDData.PositionRecords[LoopCounter].ZPos));
   }

  PROFILE_DEC();
  RELEASE(pFrame);
  return ((boolean)Result);
 }
/*===========================================================================
 *		End of Class Method CDFRMBFileD3D::CreateD3DBlock()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFRMBFileD3D::CreateRMBFile()"
/*===========================================================================
 *
 * Class CDFBlocksFileD3D Method - void CreateRMBFile (Index);
 *
 * Attempts to create a new RMBFile object in the given array index.  Protected
 * class method.
 *
 *=========================================================================*/
void CDFBlocksFileD3D::CreateRMBFile (const int Index) {

	/* Unallocate if already allocated */
  DestroyPointer(pRecords[Index]);

	/* Create new pointer */
  CreatePointer(pRecords[Index], CDFRMBFileD3D);
 }
/*===========================================================================
 *		End of Class Method CDFBlocksFileD3D::CreateRMBFile()
 *=========================================================================*/
