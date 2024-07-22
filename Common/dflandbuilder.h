/*===========================================================================
 *
 * DFLandBuilder.H - Dave Humphrey (uesp@m0use.net), 6 November 2000
 *
 *=========================================================================*/
#ifndef __DFLANDBUILDER_H
#define __DFLANDBUILDER_H

	/* Required Includes */
#include "dfpixelbuilder.h"


/*===========================================================================
 *
 * Begin Defines
 *
 *=========================================================================*/
/*===========================================================================
 *		End of Defines
 *=========================================================================*/


/*===========================================================================
 *
 * Begin Structure and Type Definitions
 *
 *=========================================================================*/
/*===========================================================================
 *		End of Type and Structure Definitions
 *=========================================================================*/


/*===========================================================================
 *
 * Class CDFLandBuilder Definition
 *
 *=========================================================================*/
class CDFLandBuilder {

  /*---------- Begin Protected Class Members ------------------------*/
protected:
  
	/* The map pixel record data to generate land from */
  CDFPixelBuilder* pPixelBuilders[DFLAND_PIXELS_ARRAYSIZE][DFLAND_PIXELS_ARRAYSIZE];
  CDFPixelBuilder* pOriginBuilder;
  boolean	   PixelBuildersAllocated;

	/* Last known location of the origin in the landscape */
  dfland_point_t OriginPosition;
  int		 OriginXPixel;
  int		 OriginYPixel;
  int		 LeftPixel;
  int		 TopPixel;


  /*---------- Begin Protected Class Methods ------------------------*/
protected:

	/* Allocate or initialize the pixel records array */
  void ClearPixelBuilders  (void);
  virtual void CreatePixelBuilders (void);

  	/* Delete any allocated pixel records */
  void DestroyPixelBuilders (void);

  	/* Generates the vertex/face arrays for the given pixel record */
  boolean GenerateLandPixel (const int XIndex, const int YIndex);

	/* Setup the reference pointers in the builder array */
  void SetBuilderReferences (void);

  	/* Attempt to reuse the current pixel records */
  void ShiftPixelBuilders (void);


  /*---------- Begin Public Class Methods ---------------------------*/
public:

	/* Class Constructor and Destructor */
  CDFLandBuilder(const boolean CreateBuilders = TRUE);
  ~CDFLandBuilder() { Destroy(); }
  void Destroy (void);

	/* Get class members */
  int    GetOriginXPixel (void) { return (OriginXPixel); }
  int    GetOriginYPixel (void) { return (OriginYPixel); }

	/* Checks to see if the origin pixel needs to be changed */
  boolean HasOriginPixelChanged (void) { if (pOriginBuilder == NULL) return (TRUE);  if (pOriginBuilder->GetXPixel() == OriginXPixel && pOriginBuilder->GetYPixel() == OriginYPixel) return (FALSE); return (TRUE);  }

	/* Load the pixel records required for the current point */
  boolean LoadPixelRecords (void);

	/* Set the position of the camera in the landscape */
  void SetOriginPosition (const float X, const float Y, const float Z);

 };
/*===========================================================================
 *		End of Class CDFLandBuilder 
 *=========================================================================*/


#endif
/*===========================================================================
 *		End of File DFLandBuilder.H
 *=========================================================================*/