/*===========================================================================
 *
 * DFPixelBuilder.CPP - Dave Humphrey (uesp@m0use.net), 21 November 2000
 *
 *=========================================================================*/

	/* Include Files */
#include "dfpixelbuilder.h"
#include "dfcommon.h"
#include "profile.h"
#include "math.h"


#undef  __FUNC__
#define __FUNC__ "CDFPixelBuilder::CDFPixelBuilder()"
/*===========================================================================
 *
 * Class CDFPixelBuilder Constructor
 *
 *=========================================================================*/
CDFPixelBuilder::CDFPixelBuilder() {
  pVertices = NULL;
  pFaceData = NULL;
  pBottomPixel = NULL;
  pLeftPixel = NULL;
  pRightPixel = NULL;
  pTopPixel = NULL;
  DetailFactor = DFLAND_DEFAULT_DETAIL;
  NumFaceData = 0;
  NumVertices = 0;
  VertexSize = 0;
  CoorConversionFactor = (float) 1.0;
  RandomnessFactor = (float) 2.0;
  FaceType = DFLAND_FACE_TRIANGLES;
 }
/*===========================================================================
 *		End of Class CDFPixelBuilder Constructor
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFPixelBuilder::Destroy()"
/*===========================================================================
 *
 * Class CDFPixelBuilder Destructor
 *
 *=========================================================================*/
void CDFPixelBuilder::Destroy (void) {

	/* Delete the allocate vertex and face arrays */
  DestroyPointer(pVertices);
  DestroyPointer(pFaceData);

  pBottomPixel = NULL;
  pLeftPixel = NULL;
  pRightPixel = NULL;
  pTopPixel = NULL;

  CoorConversionFactor = (float) 1.0;
  NumVertices = 0;
  NumFaceData = 0;
  VertexSize = 0;

	/* Call the base class method */
  CDFWoodsPixel::Destroy();
 }
/*===========================================================================
 *		End of Class CDFPixelBuilder Destructor
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFPixelBuilder::CreateArrays()"
/*===========================================================================
 *
 * Class CDFPixelBuilder Method - boolean CreateArrays (void)
 *
 * Creates the face and vertex arrays.  Returns FALSE on any error.
 *
 *=========================================================================*/
boolean CDFPixelBuilder::CreateArrays (void) {
  boolean Result;

//  Result = LoadPixelRecord();  
  Result = CreateVertices();
  if (Result) Result = CreateFaceData();

  return (Result);
 }
/*===========================================================================
 *		End of Class Method CDFPixelBuilder::CreateArrays()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFPixelBuilder::CreateFaceData()"
/*===========================================================================
 *
 * Class CDFPixelBuilder Method - boolean CreateFaceData (void);
 *
 * Attempts to create the array of faces defining the landscape as given
 * by the current class members. Returns FALSE on any error.
 *
 *=========================================================================*/
boolean CDFPixelBuilder::CreateFaceData (void) {
  
	/* Delete the previous face array, if any */
  DestroyPointer(pFaceData);

	/* Determine the type of faces to create */
  if (FaceType == DFLAND_FACE_TRIANGLES) 
    CreateTriangleFaces();
  else
    CreateSquareFaces();

  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFPixelBuilder::CreateFaceData()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFPixelBuilder::CreateSquareFaces()"
/*===========================================================================
 *
 * Class CDFPixelBuilder Method - void CreateSquareFaces (void);
 *
 * Fills the face array with the information for creating square faces.
 * Protected class method.
 *
 *=========================================================================*/
void CDFPixelBuilder::CreateSquareFaces (void) {
  ulong* pFacePtr;
  int    XIndex;
  int    YIndex;
  int    PointIndex;
  
	/* Allocate the face array for square planes data */
  NumFaceData = (VertexSize - 1)*(VertexSize - 1) * 5 + 1;
  CreatePointerArray(pFaceData, ulong, NumFaceData);
  pFacePtr = pFaceData;

	/* Build the array */
  for (YIndex = 0, PointIndex = 0; YIndex < VertexSize-1; YIndex++) {
    for (XIndex = 0; XIndex < VertexSize-1; XIndex++, PointIndex++) {
      *pFacePtr = 4;
      pFacePtr++;
      *pFacePtr = PointIndex;
      pFacePtr++;
      *pFacePtr = PointIndex + 1;
      pFacePtr++;
      *pFacePtr = PointIndex + VertexSize + 1;
      pFacePtr++;
      *pFacePtr = PointIndex + VertexSize;
      pFacePtr++;
     }
    PointIndex++;
   }

	/* Terminate the face array */
  *pFacePtr = 0;
 }
/*===========================================================================
 *		End of Class Method CDFPixelBuilder::CreateSquareFaces()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFPixelBuilder::CreateTriangleFaces()"
/*===========================================================================
 *
 * Class CDFPixelBuilder Method - void CreateTriangleFaces (void);
 *
 * Fills the face array with the information for creating square faces.
 * Protected class method.
 *
 *=========================================================================*/
void CDFPixelBuilder::CreateTriangleFaces (void) {
  ulong* pFacePtr;
  int    XIndex;
  int    YIndex;
  int    PointIndex;
  
	/* Compute the size of the face array and allocate it */
  NumFaceData = (VertexSize - 1)*(VertexSize - 1) * 8 + 1;
  CreatePointerArray(pFaceData, ulong, NumFaceData);
  pFacePtr = pFaceData;

	/* Build the array for triangle shaped faces */
  for (YIndex = 0, PointIndex = 0; YIndex < VertexSize-1; YIndex++) {
    for (XIndex = 0; XIndex < VertexSize-1; XIndex++, PointIndex++) {
      *pFacePtr = 3;
      pFacePtr++;
      *pFacePtr = PointIndex;
      pFacePtr++;
      *pFacePtr = PointIndex + 1;
      pFacePtr++;
      *pFacePtr = PointIndex + VertexSize + 1;
      pFacePtr++;
      *pFacePtr = 3;
      pFacePtr++;
      *pFacePtr = PointIndex;
      pFacePtr++;
      *pFacePtr = PointIndex + VertexSize + 1;
      pFacePtr++;
      *pFacePtr = PointIndex + VertexSize;
      pFacePtr++;
     }
    PointIndex++;
   }	

	/* Terminate the face array */
  *pFacePtr = 0;
 }
/*===========================================================================
 *		End of Class Method CDFPixelBuilder::CreateTriangleFaces()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFPixelBuilder::CreateVertexArray()"
/*===========================================================================
 *
 * Class CDFPixelBuilder Method - void CreateVertexArray (void)
 *
 * Allocates a new vertex array using the current DetailLevel.  Clears
 * the array to initially zero.
 *
 *=========================================================================*/
void CDFPixelBuilder::CreateVertexArray (void) {

	/* Delete the previous vertex array, if any */
  DestroyPointer(pVertices);
  
	/* Compute the number of vertices to create */
  ComputeVertexSize();
  
	/* Allocate and clear the vertex array */
  CreatePointerArray(pVertices, dfland_point_t, NumVertices);
  memset(pVertices, 0, sizeof(dfland_point_t)*NumVertices);
 }
/*===========================================================================
 *		End of Class Method CDFPixelBuilder::CreateVertexArray()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFPixelBuilder::CreateVertices()"
/*===========================================================================
 *
 * Class CDFPixelBuilder Method - boolean CreateVertices (void)
 *
 * Description.
 *
 *=========================================================================*/
boolean CDFPixelBuilder::CreateVertices (void) {
  int XIndex;
  int YIndex;
  int XCounter;
  int YCounter;

	/* Create a new vertex array */
  CreateVertexArray();

	/* Look for a location in the current land pixel */
  DFMaps.FindLandPixel(XPixel, YPixel, RegionIndex, LocationIndex);

	/* Set the randomess seed and land randomness */
  SetRandomSeed();
  ComputeRandomness();

	/* Setup the initial vertex array seeds */
  SeedVertexArray();

	/* Subdivide all the areas to generate the landscape recursively */
  for (YIndex = 0, YCounter = 0; YCounter < 5; YCounter++, YIndex += DetailFactor + 1) { 
    for (XCounter = 0, XIndex = 0; XCounter < 5; XCounter++, XIndex += DetailFactor + 1) { 
      SubdivideVertices(XIndex, YIndex, XIndex + DetailFactor + 1, YIndex + DetailFactor + 1);
     }
   }
  
  return (TRUE);  
 }
/*===========================================================================
 *		End of Class Method CDFPixelBuilder::CreateVertices()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFPixelBuilder::ComputeConversionFactors()"
/*===========================================================================
 *
 * Class CDFPixelBuilder Method - void ComputeConversionFactors (void)
 *
 * Updates the conversion factor from the current vertex array size.
 *
 *=========================================================================*/
void CDFPixelBuilder::ComputeConversionFactors (void) {

  if (VertexSize > 1) 
    CoorConversionFactor = DFLAND_PIXEL_COORSIZE / (float) (VertexSize - 1); 
  else 
    CoorConversionFactor = DFLAND_PIXEL_COORSIZE;

 }
/*===========================================================================
 *		End of Class Method CDFPixelBuilder::ComputeConversionFactors()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFPixelBuilder::ComputeRandomness()"
/*===========================================================================
 *
 * Class CDFPixelBuilder Method - void ComputeRandomness (void)
 *
 * Computes the land randomness by using the current pixel data.
 *
 *=========================================================================*/
void CDFPixelBuilder::ComputeRandomness (void) {

	/* Water should be absolutely flat */
  if (SmallPixel < 3)
    RandomnessFactor = (float) 0.0;

	/* Set the randomess for a town pixel */
  else if (LocationIndex >= 0)
    RandomnessFactor = DFLAND_TOWN_RANDOMNESS;

	/* Select normal randomess based on landscape 'color' */
  else
    RandomnessFactor = (float) (SmallPixel*2);

 }
/*===========================================================================
 *		End of Class Method CDFPixelBuilder::ComputeRandomness()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFPixelBuilder::LoadPixelRecord()"
/*===========================================================================
 *
 * Class CDFPixelBuilder Method - boolean LoadPixelRecord (void);
 *
 * Load the pixel record to generate the landscape at the current
 * position from the WOODS file.  Returns FALSE on any error.
 *
 *=========================================================================*/
boolean CDFPixelBuilder::LoadPixelRecord (void) {
  boolean Result;

  PROFILE_INC();
  START_PROFILE(IsLoaded);

	/* Ignore if data is already loaded */
  if (IsLoaded()) return (TRUE);

  END_PROFILE(IsLoaded);
  START_PROFILE(ReadPixelRecord);
 
	/* Ensure the woods file is open */
  //Result = DFWoodsFile.Open();
  //if (!Result) return (FALSE);
	
	/* Read the pixel record */
  //SystemLog.Printf ("Reading pixel record %d,%d", XPixel, YPixel);
  Result = DFWoodsFile.ReadPixelRecord(XPixel, YPixel, *this);
  //DFWoodsFile.Close();

  END_PROFILE(ReadPixelRecord);
  PROFILE_DEC();

  return (Result);
 }
/*===========================================================================
 *		End of Class Method CDFPixelBuilder::LoadPixelRecord()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFPixelBuilder::MatchBottomBorder()"
/*===========================================================================
 *
 * Class CDFPixelBuilder Method - void MatchBottomBorder (void)
 *
 * Ensures the bottom side of the landscape matches up with the top side
 * of the adjacent landscape pixel.
 *
 *=========================================================================*/
void CDFPixelBuilder::MatchBottomBorder (void) {
  int   XIndex;
  int   XCounter;
  int   LoopCounter;
  byte* pBottomMapData;
  byte* pBottomRightMapData;
  byte  FirstPixel;
  byte  LastPixel;
  float PixelValue;
  float DeltaPixel;

	/* Ensure valid border objects */
  ASSERT(pBottomPixel != NULL);
  ASSERT(pBottomPixel->GetRightPixel() != NULL);

	/* Get the required adjacent map data */
  pBottomMapData = pBottomPixel->GetMapPointer();
  pBottomRightMapData = pBottomPixel->GetRightPixel()->GetMapPointer();

	/* Define all the vertices on the bottom (South) side */
  for (XCounter = 0, XIndex = 0; XCounter < DFWOODS_PIXEL_SIZE; XCounter++) {
    FirstPixel = *pBottomMapData;

		/* Get the last pixel value for this set */
    if (XCounter == DFWOODS_PIXEL_SIZE - 1)
      LastPixel = *pBottomRightMapData;
    else {
      pBottomMapData += DFWOODS_PIXEL_SIZE;
      LastPixel = *pBottomMapData;
     }

		/* Initialize the line parameters */
    PixelValue = (float) FirstPixel;
    DeltaPixel = (float) (((float) (LastPixel - FirstPixel)) / (float)(DetailFactor+1));

		/* Fill in the pixels in a linear fashion for this section */
    for (LoopCounter = 0; LoopCounter < DetailFactor+1; LoopCounter++, XIndex++) {
      SetRawVertex (XIndex, VertexSize-1, (byte) PixelValue);
      PixelValue += DeltaPixel;
     }
   }

 }
/*===========================================================================
 *		End of Class Method CDFPixelBuilder::MatchBottomBorder()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFPixelBuilder::MatchLeftBorder()"
/*===========================================================================
 *
 * Class CDFPixelBuilder Method - void MatchLeftBorder (void)
 *
 * Ensures the left side of the landscape matches up with the right side
 * of the adjacent landscape pixel.
 *
 *=========================================================================*/
void CDFPixelBuilder::MatchLeftBorder (void) {
  int   YIndex;
  int   YCounter;
  int   LoopCounter;
  byte* pMapData;
  byte* pBottomMapData;
  byte  FirstPixel;
  byte  LastPixel;
  float PixelValue;
  float DeltaPixel;

	/* Ensure valid border objects */
  ASSERT(pBottomPixel != NULL);

	/* Get the required adjacent map data */
  pMapData = GetMapPointer();
  pBottomMapData = pBottomPixel->GetMapPointer();

	/* Define all the vertices on the left (West) side */
  for (YCounter = 0, YIndex = 0; YCounter < DFWOODS_PIXEL_SIZE; YCounter++) {
    FirstPixel = *pMapData;

		/* Get the last pixel value for this set */
    if (YCounter == DFWOODS_PIXEL_SIZE - 1)
      LastPixel = *pBottomMapData;
    else {
      pMapData += DFWOODS_PIXEL_SIZE;
      LastPixel = *pMapData;
     }

		/* Initialize the line parameters */
    PixelValue = (float) FirstPixel;
    DeltaPixel = (float) (((float) (LastPixel - FirstPixel)) / (float)(DetailFactor+1));

		/* Fill in the pixels in a linear fashion for this section */
    for (LoopCounter = 0; LoopCounter < DetailFactor+1; LoopCounter++, YIndex++) {
      SetRawVertex (0, YIndex, (byte) PixelValue);
      PixelValue += DeltaPixel;
     }
   }

 }
/*===========================================================================
 *		End of Class Method CDFPixelBuilder::MatchLeftBorder()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFPixelBuilder::MatchRightBorder()"
/*===========================================================================
 *
 * Class CDFPixelBuilder Method - void MatchRightBorder (void)
 *
 * Ensures the right side of the landscape matches up with the left side
 * of the adjacent landscape pixel.
 *
 *=========================================================================*/
void CDFPixelBuilder::MatchRightBorder (void) {
  int   YIndex;
  int   YCounter;
  int   LoopCounter;
  byte* pRightMapData;
  byte* pBottomRightMapData;
  byte  FirstPixel;
  byte  LastPixel;
  float PixelValue;
  float DeltaPixel;

	/* Ensure valid border objects */
  ASSERT(pRightPixel  != NULL);
  ASSERT(pBottomPixel != NULL);
  ASSERT(pBottomPixel->GetRightPixel() != NULL);

	/* Get the required adjacent map data */
  pRightMapData = pRightPixel->GetMapPointer();
  pBottomRightMapData = pBottomPixel->GetRightPixel()->GetMapPointer();

	/* Define all the vertices on the right (East) side */
  for (YCounter = 0, YIndex = 0; YCounter < DFWOODS_PIXEL_SIZE; YCounter++) {
    FirstPixel = *pRightMapData;

		/* Get the last pixel value for this set */
    if (YCounter == DFWOODS_PIXEL_SIZE - 1)
      LastPixel = *pBottomRightMapData;
    else {
      pRightMapData += DFWOODS_PIXEL_SIZE;
      LastPixel = *pRightMapData;
     }

		/* Initialize the line parameters */
    PixelValue = (float) FirstPixel;
    DeltaPixel = (float) (((float) (LastPixel - FirstPixel)) / (float)(DetailFactor+1));

		/* Fill in the pixels in a linear fashion for this section */
    for (LoopCounter = 0; LoopCounter < DetailFactor+1; LoopCounter++, YIndex++) {
      SetRawVertex (VertexSize-1, YIndex, (byte) PixelValue);
      PixelValue += DeltaPixel;
     }
   }

	/* Set the bottom-right pixel value */
  SetRawVertex(VertexSize-1, VertexSize-1, *pBottomRightMapData);
 }
/*===========================================================================
 *		End of Class Method CDFPixelBuilder::MatchRightBorder()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFPixelBuilder::MatchTopBorder()"
/*===========================================================================
 *
 * Class CDFPixelBuilder Method - void MatchTopBorder (void)
 *
 * Ensures the top side of the landscape matches up with the bottom side
 * of the adjacent landscape pixel.
 *
 *=========================================================================*/
void CDFPixelBuilder::MatchTopBorder (void) {
  int   XIndex;
  int   XCounter;
  int   LoopCounter;
  byte* pMapData;
  byte* pRightMapData;
  byte  FirstPixel;
  byte  LastPixel;
  float PixelValue;
  float DeltaPixel;

	/* Ensure valid border objects */
  ASSERT(pRightPixel != NULL);

	/* Get the required adjacent map data */
  pMapData = GetMapPointer();
  pRightMapData = pRightPixel->GetMapPointer();

	/* Define all the vertices on the bottom (South) side */
  for (XCounter = 0, XIndex = 0; XCounter < DFWOODS_PIXEL_SIZE; XCounter++) {
    FirstPixel = *pMapData;

		/* Get the last pixel value for this set */
    if (XCounter == DFWOODS_PIXEL_SIZE - 1)
      LastPixel = *pRightMapData;
    else {
      pMapData += DFWOODS_PIXEL_SIZE;
      LastPixel = *pMapData;
     }

		/* Initialize the line parameters */
    PixelValue = (float) FirstPixel;
    DeltaPixel = (float) (((float) (LastPixel - FirstPixel)) / (float)(DetailFactor+1));

		/* Fill in the pixels in a linear fashion for this section */
    for (LoopCounter = 0; LoopCounter < DetailFactor+1; LoopCounter++, XIndex++) {
      SetRawVertex (XIndex, 0, (byte) PixelValue);
      PixelValue += DeltaPixel;
     }
   }

 }
/*===========================================================================
 *		End of Class Method CDFPixelBuilder::MatchTopBorder()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFPixelBuilder::SetRandomSeed()"
/*===========================================================================
 *
 * Class CDFPixelBuilder Method - void SetRandomSeed (void)
 *
 * Sets the random seed for the current landscape pixel.  Uses the random
 * number contained in the current land pixel record.
 *
 *=========================================================================*/
void CDFPixelBuilder::SetRandomSeed (void) {
  srand(GetSeedNumber());
 }
/*===========================================================================
 *		End of Class Method CDFPixelBuilder::SetRandomSeed()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFPixelBuilder::SeedVertexArray()"
/*===========================================================================
 *
 * Class CDFPixelBuilder Method - void SeedVertexArray (void)
 *
 * Creates the initial vertex array with the appropiate 'seed' values in 
 * the correct locations.  Protected class method.
 *
 *=========================================================================*/
void CDFPixelBuilder::SeedVertexArray (void) {
  int   IndexDelta = DetailFactor + 1;
  int   YIndex;
  int   XIndex;
  byte* pMapData; 

	/* Get a pointer to the raw map data 5x5 array */
  pMapData = GetMapPointer();

	/* Use the map data to set the seeds throughout the vertex array */
  for (YIndex = 0; YIndex < VertexSize-1; YIndex += IndexDelta) {
    for (XIndex = 0; XIndex < VertexSize-1; XIndex += IndexDelta) {
      SetRawVertex(XIndex, YIndex, *pMapData);
      pMapData++;
     }
   }
   
	/* Match the adjacent pixel borders */
  MatchRightBorder();	
  MatchLeftBorder();
  MatchTopBorder();
  MatchBottomBorder();
 }
/*===========================================================================
 *		End of Class Method CDFPixelBuilder::SeedVertexArray()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFPixelBuilder::SetRawVertex()"
/*===========================================================================
 *
 * Class CDFPixelBuilder Method - void SetRawVertex (XIndex, YIndex, MapPixel);
 *
 * Sets the XYZ position of the given element in the vertex array using the
 * given raw MapPixel value from the WOODS.WLD map record. Protected Class Method.
 *
 *=========================================================================*/
void CDFPixelBuilder::SetRawVertex (const int XIndex, const int YIndex, const byte MapPixel) { 
  SetVertex (XIndex, YIndex, (float)MapPixel * DFLAND_ZPOS_FACTOR);
 }
/*===========================================================================
 *		End of Class Method CDFPixelBuilder::SetRawVertex()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFPixelBuilder::SetVertex()"
/*===========================================================================
 *
 * Class CDFPixelBuilder Method - void SetVertex (XIndex, YIndex, YPos);
 *
 * Sets the XYZ position of the given element in the vertex array using the
 * given Y-Coordinate.  Protected Class Method.
 *
 *=========================================================================*/
void CDFPixelBuilder::SetVertex (const int XIndex, const int YIndex, const float YPos) { 
  dfland_point_t* pVertex = &pVertices[XIndex + YIndex * VertexSize];

	/* Add a small value to the Y-position for proper landscape generation
	 * using the plasma type fractal. */
  pVertex->X = ((float) XIndex) * CoorConversionFactor;
  pVertex->Y = YPos + (float) 0.0001;
  pVertex->Z = ((float) YIndex) * CoorConversionFactor;
 }
/*===========================================================================
 *		End of Class Method CDFPixelBuilder::SetRawVertex()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFPixelBuilder::SubdivideVertices()"
/*===========================================================================
 *
 * Class CDFPixelBuilder Method - SubdivideVertices (Left, Top, Right, Bottom);
 *
 * Recursive algorithim to generate the landscape pixel. Protected class method.
 *
 *=========================================================================*/
void CDFPixelBuilder::SubdivideVertices (int Left, int Top, int Right, int Bottom) {
  int   MidX;
  int   MidY;
  float yTopLeft;
  float yTopRight;
  float yBottomLeft;
  float yBottomRight;
  float YPos;
  float yValue;
  unsigned Spread;

	/* Compute the middle of area */
  MidX = (Right + Left) / 2;
  MidY = (Bottom + Top) / 2;

        /* Find the four corner z-coordinates*/
  yTopLeft     = GetYPos(Left,  Top);
  yTopRight    = GetYPos(Right, Top);
  yBottomLeft  = GetYPos(Left,  Bottom);
  yBottomRight = GetYPos(Right, Bottom);
      
	/* Compute the x-spread value, controlling randomness of landscape */
  Spread = (unsigned)((Right - Left) * RandomnessFactor);

        /* Generate the top point */
  YPos = GetYPos(MidX, Top);

  if (YPos == 0.0) {
    yValue = (float) (rand() % (Spread * 2 + 1)) - Spread;
    yValue += (yTopLeft + yTopRight) / 2;
    SetVertex(MidX, Top, yValue);
   }

        /* Generate the bottom point */
  YPos = GetYPos(MidX, Bottom);

  if (YPos == 0.0) {
    yValue = (float) (rand() % (Spread * 2 + 1)) - Spread;
    yValue += (yBottomLeft + yBottomRight) / 2;
    SetVertex (MidX, Bottom, yValue);
   }

	/* Compute the y-spread value, controlling randomness of landscape */
  Spread = (unsigned)((Bottom - Top) * RandomnessFactor);

        /* Generate the left point */
  YPos = GetYPos(Left, MidY);

  if (YPos == 0.0) {
    yValue = (float) (rand() % (Spread * 2 + 1)) - Spread;
    yValue += (yTopLeft + yBottomLeft) / 2;
    SetVertex (Left, MidY, yValue);
   }

        /* Generate the right point */
  YPos = GetYPos(Right, MidY);

  if (YPos == 0.0) {
    yValue = (float) (rand() % (Spread * 2 + 1)) - Spread;
    yValue += (yTopRight + yBottomRight) / 2;
    SetVertex (Right, MidY, yValue);
   }

	/* Generate the center point */
  YPos = GetYPos(MidX, MidY);

  if (YPos == 0.0) {
    Spread = (unsigned)((sqrt(((float)(Right - Left) * (Right - Left) + (Bottom - Top) * (Bottom - Top))) / 2) * RandomnessFactor);
    yValue = (float) (rand() % (Spread * 2 + 1)) - Spread;
    yValue += (yTopLeft + yBottomRight + yTopRight + yBottomLeft) / 4;
    SetVertex (MidX, MidY, yValue);
   }

        /* Recurse if we need to */
  if (Right - Left >= 2 || Top - Bottom >= 2) {
    SubdivideVertices(Left, Top,  MidX,  MidY);
    SubdivideVertices(MidX, Top,  Right, MidY);
    SubdivideVertices(Left, MidY, MidX,  Bottom);
    SubdivideVertices(MidX, MidY, Right, Bottom);
   }

 }
/*===========================================================================
 *		End of Class Method CDFPixelBuilder::SubdivideVertices()
 *=========================================================================*/