/*===========================================================================
 *
 * DFIMG.H - Dave Humphrey (4 November 2000), uesp@m0use.net
 *
 *=========================================================================*/
#ifndef __DFIMG_H
#define __DFIMG_H


	/* Required Includes */
#include "genutil.h"


/*===========================================================================
 *
 * Begin Defines
 *
 *=========================================================================*/

	/* Need for loading of CIF files */
  #define DFCIF_OFFSETLIST_SIZE 64

/*===========================================================================
 *		End of Defines
 *=========================================================================*/


/*===========================================================================
 *
 * Class CDFImgFile Definition
 *
 *=========================================================================*/
class CDFImgFile {

  /*---------- Begin Protected Class Members -----------------------*/
protected:
  short		 XOffset;	/* Image position offset */
  short		 YOffset;
  short		 Width;
  short		 Height;	/* Image size */
  short		 Unknown;	/* Unknown value */
  int            ImageSize;	/* Size of the image data */
  byte*		 pData;		/* The raw image data */

  boolean	 DestroyOnRead;	/* Reset image contents in the Read() method? */
  boolean	 HasOffsetList; /* For loading certain CIF images */
  boolean	 HasNoHeader;	/* For special CIF and some image files */
  boolean	 HasCIFWeaponHeader;	/* Indicates a special CIF header type */
  int		 CIFGroupIndex; /* The weapon cif group index number */


  /*---------- Begin Protected Class Methods  ----------------------*/
protected:

	/* Helper input routines */
  boolean ReadHeader         (FILE* pFileHandle);
  boolean ReadData           (FILE* pFileHandle);
  boolean ReadCompressedData (FILE* pFileHandle);

  /*---------- Begin Public Class Members --------------------------*/
public:

	/* Class Constructor/Destructor */
  CDFImgFile();
  virtual ~CDFImgFile() { Destroy(); }
  virtual void Destroy (void);

	/* Class get members */
  short		 GetXOffset   (void) { return (XOffset); }
  short		 GetYOffset   (void) { return (YOffset); }
  short		 GetWidth     (void) { return (Width); }
  short		 GetHeight    (void) { return (Height); }
  short		 GetUnknown   (void) { return (Unknown); }
  int		 GetImageSize (void) { return (ImageSize); }
  byte*		 GetImageData (void) { return (pData); }
  byte*		 GetData      (void) { return (pData); }

	/* Is the image data compressed? */
  boolean IsCompressed (void) { if (Unknown == 0x0002 || ImageSize != Width*Height) return (TRUE); return (FALSE); }

	/* Read in a image file */
  boolean Load (const char* pFilename);

  	/* Read in image data from a file stream */
  virtual boolean Read (FILE* pFileHandle);

	/* Class set members */
  void SetHasOffsetList (const boolean Value) { HasOffsetList = Value; }
  void SetHasNoHeader   (const boolean Value) { HasNoHeader   = Value; }
  void SetCIFGroupIndex (const int Index)     { CIFGroupIndex = Index; }
  void SetHeight        (const short Value)   { Height = Value; }
  void SetWidth         (const short Value)   { Width = Value; }
  void SetImageSize     (const int   Value)   { ImageSize = Value; }
  void SetHasCIFWeaponHeader (const boolean Value)     { HasCIFWeaponHeader = Value; }

 };
/*===========================================================================
 *		End of Class CDFImgFile Definition
 *=========================================================================*/


#endif
/*===========================================================================
 *		End of File DFImg.H
 *=========================================================================*/