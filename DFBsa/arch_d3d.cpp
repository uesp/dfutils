/*===========================================================================
 *
 * ARCH_D3D.CPP - Dave Humphrey (uesp@m0use.net), 31 October 2000
 *
 *=========================================================================*/

	/* Include Files */
#include "arch_d3d.h"
#include "dfcommon_d3d.h"
#include "profile.h"


	/* Initialize class static members */
boolean    CDF3DObjectD3D::LoadObjectNormals = TRUE;	
d3dvalue_t CDF3DObjectD3D::PointScale = DF_DEFAULT_3DOBJECT_SCALE;	


#undef  __FUNC__
#define __FUNC__ "CDF3DObjectD3D::CDF3DObjectD3D()"
/*===========================================================================
 *
 * Class CDF3DObjectD3D Constructor
 *
 *=========================================================================*/
CDF3DObjectD3D::CDF3DObjectD3D() {
  pD3DBuilder = NULL;
 }
/*===========================================================================
 *		End of Class CDF3DObjectD3D Constructor
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDF3DObjectD3D::Destroy()"
/*===========================================================================
 *
 * Class CDF3DObjectD3D Destructor
 *
 *=========================================================================*/
void CDF3DObjectD3D::Destroy () {

	/* Release allocated objects */
  RELEASE(pD3DBuilder);

	/* Call base class method */
  CDF3DObject::Destroy();
 }
/*===========================================================================
 *		End of Class CDF3DObjectD3D Destructor
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDF3DObjectD3D::GetD3DBuilder()"
/*===========================================================================
 *
 * Class CDF3DObjectD3D Method - CD3DMeshBuilder* GetD3DBuilder (void);
 *
 * Retrieves the 3D objects D3D builder.  Creates it if it doesn't currently
 * exist.  Returns NULL on any error.
 *
 *=========================================================================*/
CD3DMeshBuilder* CDF3DObjectD3D::GetD3DBuilder (void) {
  boolean Result;

	/* Attempt to make the builder if it doesn't exist */
  if (pD3DBuilder == NULL) {
    Result = MakeMeshBuilder();
    if (!Result) return (NULL);
   }

  return (pD3DBuilder);
 }
/*===========================================================================
 *		End of Class Method CDF3DObjectD3D::GetD3DBuilder()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDF3DObjectD3D::MakeBuildData()"
/*===========================================================================
 *
 * Class CDF3DObjectD3D Method - DWORD* MakeBuildData (void);
 *
 * Creates the build data array associated with the 3d object.  Always returns
 * a valid pointer unless there are no planes in the object. Protected
 * class method.
 *
 *=========================================================================*/
DWORD* CDF3DObjectD3D::MakeBuildData (void) {
  dfarch_plane_t* pPlaneData;
  int		  LoopCounter;
  int		  PointCounter;
  int		  IndexDivide = 12;
  long		  DataIndex = 0;
  long		  NumPlanePoints;
  long		  BuildDataSize;
  DWORD*	  pBuildData;

  START_PROFILE(MakeBuildData);

	/* Count the number of plane points to ensure there's at least one */
  NumPlanePoints = CountPlanePoints();
  if (NumPlanePoints <= 0) return (NULL);

	/* Special case for version v2.5 objects */
  if (IsVersion("v2.5")) IndexDivide = 36;

	/* Determine the size of the build array */
  if (LoadObjectNormals)
    BuildDataSize = GetNumPlanesAllocated() + NumPlanePoints*2 + 2;
  else
    BuildDataSize = GetNumPlanesAllocated() + NumPlanePoints + 2;

	/* Allocate the build data array */
  CreatePointerArray(pBuildData, DWORD, BuildDataSize);

	/* Add all the allocated planes in the object */
  for (LoopCounter = 0, DataIndex = 0; LoopCounter < GetNumPlanesAllocated(); LoopCounter++) { 
    pPlaneData = GetPlane(LoopCounter);

		/* Set the number of points/normals indices which follow */
    pBuildData[DataIndex] = pPlaneData->PointCount;
    DataIndex++;

		/* Add each point in the currnt plane */
    for (PointCounter = 0; PointCounter < pPlaneData->PointCount; PointCounter++) {
      pBuildData[DataIndex] = (pPlaneData->SubRecords[PointCounter].PointOffset)/IndexDivide;
      DataIndex++;

		/* Don't interlace the normals if this option is off */
      if (LoadObjectNormals) {
        pBuildData[DataIndex] = LoopCounter;
        DataIndex++;
       }
     }
   }

	/* Terminate the build data array */
  pBuildData[DataIndex] = 0;
  DataIndex++;

	 /* Check the array index */
  if (DataIndex > BuildDataSize) {
    SET_EXT_ERROR3(ERR_OVERFLOW, "Build data array index exceeded by %ld!", DataIndex - BuildDataSize);
   }

  END_PROFILE(MakeBuildData);

  return (pBuildData);
 }
/*===========================================================================
 *		End of Class Method CDF3DObjectD3D::MakeBuildData()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDF3DObjectD3D::MakeNormals()"
/*===========================================================================
 *
 * Class CDF3DObjectD3D Method - d3dvector_t* MakeNormals (void);
 *
 * Creates the normals array associated with the 3d object.  Always returns
 * a valid pointer unless there are no planes in the object or if the 
 * LoadObjectNormals is FALSE. Protected class method.
 *
 *=========================================================================*/
d3dvector_t* CDF3DObjectD3D::MakeNormals (void) {
  dfarch_point_t* pPoints;
  int		  LoopCounter;
  d3dvector_t*	  pNormals;

  START_PROFILE(MakeNormals);

	/* Ensure a valid number of points */
  if (GetNumPlanesAllocated() <= 0 || !LoadObjectNormals) {
    SET_EXT_ERROR(ERR_NULL);
    return (NULL);
   }

	/* Allocate the normals array */
  CreatePointerArray(pNormals, d3dvector_t, GetNumPlanesAllocated());
  pPoints = GetNormals();

	/* Setup the normals array */
  for (LoopCounter = 0; LoopCounter < GetNumPlanesAllocated(); LoopCounter++) {
    pNormals[LoopCounter] = PointToNormal(*pPoints);
    pPoints++;
   } 

  END_PROFILE(MakeNormals);

  return (pNormals);
 }
/*===========================================================================
 *		End of Class Method CDF3DObjectD3D::MakeNormals()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDF3DObjectD3D::MakeTextures()"
/*===========================================================================
 *
 * Class CDF3DObjectD3D Method - boolean MakeTextures (void);
 *
 * Assigns textures to the builder object.  Protected
 * class method.  Returns FALSE on any error.
 *
 *=========================================================================*/
boolean CDF3DObjectD3D::MakeTextures (void) {
  CD3DTexture*    pTexture;
  CD3DFaceArray*  pFaceArray = NULL;
  CD3DFace*	  pFace = NULL;
  CD3DWrap*       pWrap = NULL;
  d3dpalentry_t*  pPalEntry;
  d3dvector_t	  FaceNormal;
  d3dvector_t 	  FaceVertex;
  d3dvalue_t	  UTexture;
  d3dvalue_t	  VTexture;
  HRESULT	  Result;
  uint		  LoopCounter;
  int		  VertexCounter;
  dfarch_plane_t* pPlane;
  int		  ColorIndex;

  START_PROFILE(MakeTextures);

  	/* Get the D3D face array object */
  Result = pD3DBuilder->GetFaces(&pFaceArray);

  if (FAILED(Result)) {
    SET_D3D_ERROR2(Result, "Failed to get face array for Direct3D builder object!");
    return (FALSE);
   }

  Result = GetD3DRM()->CreateWrap(D3DRMWRAP_SPHERE, NULL, 
			      D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0),
			      D3DVAL(1.0), D3DVAL(0.0), D3DVAL(0.0),
			      D3DVAL(0.0), D3DVAL(1.0), D3DVAL(0.0), 
			      D3DVAL(0.0), D3DVAL(1.0), 
			      D3DVAL(1.0), D3DVAL(1.0), 
			      &pWrap);

  if (FAILED(Result)) {
    SET_D3D_ERROR2(Result, "Failed to create wrap");
   }
  else
    pWrap->Apply(pD3DBuilder);          

  	/* Set the texture for each face */
  for (LoopCounter = 0; LoopCounter < pFaceArray->GetSize() && LoopCounter < (uint) NumPlanesAllocated; LoopCounter++) {
    pPlane = GetPlane(LoopCounter);
    if (pPlane == NULL) continue;
       
		/* Get the current face object */
    Result = pFaceArray->GetElement(LoopCounter, &pFace);

    if (FAILED(Result)) {
      SET_D3D_ERROR3(Result, "Failed to get face %d from face array!", LoopCounter);
      goto DF3D_MAKETEXTURE_ERROR;
     }

     		/* Special case for textures 0/1 for now */
    if (pPlane->TextureIndex <= 1) { 

			/* Compute color index and attempt to retrieve D3D palette entry for it*/
      ColorIndex = pPlane->TextureIndex*128 + pPlane->SubImageIndex;
      pPalEntry = DFStandardPaletteD3D.GetD3DPalEntry(ColorIndex);

      if (pPalEntry == NULL) {
        SET_D3D_ERROR3(Result, "Failed to get D3D palette entry %d from standard palette!", ColorIndex);
        RELEASE(pFace);
        continue;
       }

			/* Set the face color */
      pFace->SetColorRGB(D3DVAL(pPalEntry->red)/256, D3DVAL(pPalEntry->green)/256, D3DVAL(pPalEntry->blue)/256);
      RELEASE(pFace);
      continue;
     }

		/* Load/retreive the appropiate texture image */
    pTexture = DFGetD3DTexture(pPlane->TextureIndex, pPlane->SubImageIndex);		

    if (pTexture == NULL) {
      RELEASE(pFace);
      continue;
     }

    if (pWrap) {
      Result = pWrap->Apply(pFace);
      if (FAILED(Result)) SET_D3D_ERROR2(Result, "Failed to apply wrap to face");
     }

    //SystemLog.Printf ("Texture Coordinates for Face %d", LoopCounter);
   
		/* Set the face texture */
    Result = pFace->SetTexture(pTexture);
    pWrap->Apply(pFace);

    if (FAILED(Result)) {
      SET_D3D_ERROR4(Result, "Failed to set the face texture (%d, %d)!", pPlane->TextureIndex, pPlane->SubImageIndex);
     }
/*
   for (VertexCounter = 0; VertexCounter < pFace->GetVertexCount(); VertexCounter++) {
      Result = pFace->GetVertex(VertexCounter, &FaceVertex, &FaceNormal);
      if (FAILED(Result)) SET_D3D_ERROR2(Result, "Failed to get vertex");
      FaceNormal.y = (float)rand()/(float)RAND_MAX;
      FaceNormal.z = (float)rand()/(float)RAND_MAX;
      Result = pFace->SetTextureCoordinates(VertexCounter, FaceNormal.y, FaceNormal.z);
      if (FAILED(Result)) SET_D3D_ERROR2(Result, "Failed to set texture coordinates");
      pFace->GetTextureCoordinates(VertexCounter, &UTexture, &VTexture);
      SystemLog.Printf ("\tVertex %d: %f, %f", VertexCounter, UTexture, VTexture);
     } //*/

    RELEASE(pFace);
   } 

  END_PROFILE(MakeTextures);

	/* Cleanup and return success */
  RELEASE(pFaceArray);
  RELEASE(pFace);
  RELEASE(pWrap);
  return (TRUE);
  
DF3D_MAKETEXTURE_ERROR:
  RELEASE(pFaceArray);
  RELEASE(pFace);
  RELEASE(pWrap);

  return (FALSE);
 }
/*===========================================================================
 *		End of Class Method CDF3DObjectD3D::MakeTextures()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDF3DObjectD3D::MakeVectors()"
/*===========================================================================
 *
 * Class CDF3DObjectD3D Method - d3dvector_t* MakeVectors (void);
 *
 * Creates the vector array associated with the 3d object.  Always returns
 * a valid pointer unless there are no points in the object.  Protected
 * class method.
 *
 *=========================================================================*/
d3dvector_t* CDF3DObjectD3D::MakeVectors (void) {
  dfarch_point_t* pPoints;
  int		  LoopCounter;
  d3dvector_t*	  pVertices;

  START_PROFILE(MakeVectors);

	/* Ensure a valid number of points */
  if (GetNumPoints() <= 0) {
    SET_EXT_ERROR(ERR_NULL);
    return (NULL);
   }

	/* Allocate the vertices array */
  CreatePointerArray(pVertices, d3dvector_t, GetNumPoints());
  pPoints = GetPoints();

	/* Setup the vertex array */
  for (LoopCounter = 0; LoopCounter < GetNumPoints(); LoopCounter++) {
    pVertices[LoopCounter] = PointToScaledVector(*pPoints);
    pPoints++;
   } 

  END_PROFILE(MakeVectors);

  return (pVertices);
 }
/*===========================================================================
 *		End of Class Method CDF3DObjectD3D::MakeVectors()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDF3DObjectD3D::MakeMeshBuilder()"
/*===========================================================================
 *
 * Class CDF3DObjectD3D Method - boolean MakeMeshBuilder (void);
 *
 * Make a D3DRM Builder object from the 3D object. Returns the D3D error
 * code on error, or D3DRM_OK on success.  Protected class method.
 *
 *=========================================================================*/
boolean CDF3DObjectD3D::MakeMeshBuilder (void) { 
  d3dvector_t* pVertices = NULL;
  d3dvector_t* pNormals = NULL;
  DWORD*       pBuildData = NULL;
  HRESULT      Result;

  START_PROFILE(MakeMeshBuilder);
  PROFILE_INC();

	/* Ensure a valid input objects */
  ASSERT(GetD3DRM() != NULL);
  	
	/* Output debug information on object to log file */
#ifdef _DEBUG
  //Dump(SystemLog.GetFileHandle());
#endif

	/* Create a mesh builder object */
  Result = GetD3DRM()->CreateMeshBuilder(&pD3DBuilder);

  if (FAILED(Result)) {  
    SET_D3D_ERROR2(Result, "Failed to create the Builder2 object!");
    return (FALSE);
   }

	/* Create the vertices, normals, and build data arrays */
  pVertices = MakeVectors();
  pNormals = MakeNormals();
  pBuildData = MakeBuildData();

	/* Add the faces to the mesh builder */
  if (LoadObjectNormals)
    Result = pD3DBuilder->AddFaces(GetNumPoints(), pVertices, GetNumPlanesAllocated(), pNormals, pBuildData, NULL);
  else
    Result = pD3DBuilder->AddFaces(GetNumPoints(), pVertices, 0, NULL, pBuildData, NULL);
    
  if (FAILED(Result)) {
    SET_D3D_ERROR2(Result, "Failed to add faces to the Builder2 object!");
    RELEASE(pD3DBuilder);  
    return (FALSE);
   }

	/* Make the textures for the object */
  Result = MakeTextures();

	/* Unallocate the temporary array data */
  DestroyPointer(pVertices);
  DestroyPointer(pNormals);
  DestroyPointer(pBuildData);

  PROFILE_DEC();
  END_PROFILE(MakeMeshBuilder);

  return (boolean) (Result);
 }
/*===========================================================================
 *		End of Class Method CDF3DObjectD3D::MakeMeshBuilder()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDF3DObjectD3D::Create3DObject()"
/*===========================================================================
 *
 * Class CDFArchD3D Method - void Create3DObject (RecordIndex);
 *
 * Creates the given object array.  Protected class method.
 *
 *=========================================================================*/
void CDFArchD3D::Create3DObject (const int RecordIndex) {

	/* Delete the object if it exists */
  DestroyPointer(p3DObjects[RecordIndex]);

	/* Alloate the new object */
  CreatePointer(p3DObjects[RecordIndex], CDF3DObjectD3D);
 }
/*===========================================================================
 *		End of Class Method CDFArchD3D::Create3DObject()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDF3DObjectD3D::GetD3DBuilder()"
/*===========================================================================
 *
 * Class CDFArchD3D Method - CD3DMeshBuilder* GetD3DBuilder(ValueID);
 *
 * Returns the specified Direct3D builder object or NULL on error.  
 *
 *=========================================================================*/
CD3DMeshBuilder* CDFArchD3D::GetD3DBuilder (const long ValueID) {
  return GetD3DBuilder(FindRecordIndex(ValueID));
 }
/*===========================================================================
 *		End of Class Method CDFArchD3D::Create3DObject()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDF3DObjectD3D::GetD3DBuilder()"
/*===========================================================================
 *
 * Class CDFArchD3D Method - CD3DMeshBuilder* GetD3DBuilder(Index);
 *
 * Returns the specified Direct3D builder object or NULL on error.  
 *
 *=========================================================================*/
CD3DMeshBuilder* CDFArchD3D::GetD3DBuilder (const int Index) {
  CDF3DObjectD3D* p3DObject;

	/* Ensure the object is loaded */
  p3DObject = GetD3DObject(Index);
  if (p3DObject == NULL) return (NULL);

	/* Return the builder object */
  return (p3DObject->GetD3DBuilder());
 }
/*===========================================================================
 *		End of Class Method CDFArchD3D::Create3DObject()
 *=========================================================================*/