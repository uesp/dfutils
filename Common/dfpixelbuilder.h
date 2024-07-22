/*===========================================================================
 *
 * DFPixelBuilder.H - Dave Humphrey (uesp@m0use.net), 21 November 2000
 *
 *=========================================================================*/
#ifndef __DFPIXELBUILDER_H
#define __DFPIXELBUILDER_H

	/* Required Includes */
#include "dfwoods.h"


/*===========================================================================
 *
 * Begin Defines
 *
 *=========================================================================*/

	/* Defines maximum resolution of landscapes */
  #define DFLAND_MAX_DETAILFACTOR 50

	/* Defines scaling of raw values to coordinates */
  #define DFLAND_ZPOS_FACTOR ((float)-10.0)

	/* Final coordinate size of a generated pixel record */
  #define DFLAND_PIXEL_COORSIZE   ((float)32768.0)
  #define DFLAND_PIXEL_COORHEIGHT ((float)28880.0)
  #define DFLAND_MIN_XPOS ((float)(0.0))
  #define DFLAND_MAX_XPOS ((float)(DFLAND_PIXEL_COORSIZE*(float)DFWOODS_MAP_WIDTH))
  #define DFLAND_MIN_ZPOS ((float)(0.0))
  #define DFLAND_MAX_ZPOS ((float)(DFLAND_PIXEL_COORSIZE*(float)DFWOODS_MAP_HEIGHT))

	/* Number of pixel records to load/buffer in each direction */
  #define DFLAND_PIXELS_ARRAYSIZE 5
  #define DFLAND_PIXELS_ARRAYHALFSIZE 2
  #define DFLAND_PIXELS_BUILDSIZE 3
  #define DFLAND_PIXELS_BUILDHALFSIZE 1

	/* Default randomness for a location land pixel */
  #define DFLAND_TOWN_RANDOMNESS ((float)5.0)

	/* Default detail factor */
  #define DFLAND_DEFAULT_DETAIL 10

/*===========================================================================
 *		End of Defines
 *=========================================================================*/


/*===========================================================================
 *
 * Begin Structure and Type Definitions
 *
 *=========================================================================*/

  	/* Simple point type */
  typedef struct {
    float X, Y, Z;
   } dfland_point_t;

	/* Type used for the face data */
  typedef unsigned long ulong;

	/* Types of faces we can build */
  typedef enum {
	DFLAND_FACE_SQUARES,
	DFLAND_FACE_TRIANGLES 
   } dfland_face_t;

/*===========================================================================
 *		End of Type and Structure Definitions
 *=========================================================================*/


/*===========================================================================
 *
 * Class CDFPixelBuilder Definition
 *
 * Class which builds one land pixel of Daggerfall landscape.
 *
 *=========================================================================*/
class CDFPixelBuilder : public CDFWoodsPixel { 

  /*---------- Begin Protected Class Members -----------------------*/
protected:
  dfland_point_t* pVertices;		/* The landscape vertex array */
  int		  VertexSize;		/* Size of the vertex array (width and height) */
  int		  NumVertices;		/* Total number of vertices */
  ulong*	  pFaceData;		/* Contains the face definition data */
  int		  NumFaceData;

  int		  DetailFactor;		/* The detail of the generated landscape */
  float		  RandomnessFactor;	/* Controls how random the landscape is */
  dfland_face_t	  FaceType;		/* Type of faces to create */

  dfland_point_t  Position;		/* Absolute position of the bottom-left of pixel */

  CDFPixelBuilder* pRightPixel;		/* Bordering pixel objects */
  CDFPixelBuilder* pBottomPixel; 
  CDFPixelBuilder* pTopPixel; 
  CDFPixelBuilder* pLeftPixel; 

  float		  CoorConversionFactor;	/* Used for faster coordinate conversions */
  

  /*---------- Begin Protected Class Methods -----------------------*/
protected:

	/* Create the location for the current land pixel */
  virtual boolean CreateLocation  (void) { return (FALSE); }

  	/* Create the face data array */
  boolean CreateFaceData      (void);
  void    CreateSquareFaces   (void);
  void    CreateTriangleFaces (void);

	/* Create the vertex array */
  void    CreateVertexArray (void);
  boolean CreateVertices    (void);

  	/* Compute any of the required conversion factors */
  void ComputeConversionFactors (void);

	/* Compute the current land randomness */
  void ComputeRandomness (void);

  	/* Compute the width/height of the vertex array */
  void ComputeVertexSize (void) { VertexSize = DetailFactor*5 + 6; NumVertices = VertexSize * VertexSize; ComputeConversionFactors(); }

  	/* Retrieve the Y-pos from the vertex array */
  float GetYPos (const int XIndex, const int YIndex) { return (pVertices[XIndex + YIndex*VertexSize].Y); }

	/* Creates the type specific builder object for the landscape pixel */
  boolean MakeBuilder (void) { return (FALSE); }

  	/* Ensure adajacent landscape pixels match at their borders */
  void MatchBottomBorder (void);
  void MatchLeftBorder   (void);
  void MatchRightBorder  (void);
  void MatchTopBorder    (void);

	/* Seed the initial vertex array */
  void SeedVertexArray (void);

	/* Set the random seed number */
  void SetRandomSeed (void);

	/* Set vertex values */
  void SetRawVertex (const int XIndex, const int YIndex, const byte  MapPixel);
  void SetVertex    (const int XIndex, const int YIndex, const float YPos);

  	/* Recursive landscape generation */
  void SubdivideVertices (int Left, int Top, int Right, int Bottom);

  
  /*---------- Begin Public Class Methods --------------------------*/
public:

	/* Class Constructor/Destructor */
  CDFPixelBuilder();
  virtual ~CDFPixelBuilder() { Destroy(); }
  virtual void Destroy (void);

  	/* Create the required vertex and face data arrays */
  virtual boolean CreateArrays (void);

	/* Create the landscape for the current land pixel */
  virtual boolean CreateLandscape (void) { return (FALSE); }

	/* Get the bordering landscape pixels */
  CDFPixelBuilder* GetBottomPixel (void) { return (pBottomPixel); }  
  CDFPixelBuilder* GetRightPixel  (void) { return (pRightPixel); }
  CDFPixelBuilder* GetLeftPixel   (void) { return (pLeftPixel); }
  CDFPixelBuilder* GetTopPixel    (void) { return (pTopPixel); }

	/* Get the face and pixel data */
  ulong* GetFaceData     (void) { return (pFaceData); }
  float* GetVertices     (void) { if (pVertices == NULL) return (NULL); return (&pVertices->X); }
  int    GetNumVertices  (void) { return (NumVertices); }
  int    GetNumFaceData  (void) { return (NumFaceData); }

  	/* Attempt to load the pixel record from the Woods.WLD file */
  boolean LoadPixelRecord (void);

	/* Set the bordering landscape pixels */
  void SetBottomPixel (CDFPixelBuilder* pBuilder) { pBottomPixel = pBuilder; }
  void SetLeftPixel   (CDFPixelBuilder* pBuilder) { pLeftPixel   = pBuilder; }
  void SetRightPixel  (CDFPixelBuilder* pBuilder) { pRightPixel  = pBuilder; }
  void SetTopPixel    (CDFPixelBuilder* pBuilder) { pTopPixel    = pBuilder; }

	/* Set the position of the land pixel reference frame */
  virtual boolean SetFramePosition (const float XPos, const float YPos, const float ZPos) { return (FALSE); }

 }; 
/*===========================================================================
 *		End of Class CDFPixelBuilder Definition
 *=========================================================================*/


#endif
/*===========================================================================
 *		End of File DFPixelBuilder.H
 *=========================================================================*/