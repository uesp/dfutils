/*===========================================================================
 *
 * DFCif.H - Dave Humphrey (uesp@m0use.net), 4 November 2000
 *
 *=========================================================================*/
#ifndef __DFCIF_H
#define __DFCIF_H

	/* Required Includes */
#include "dfimg.h"


/*===========================================================================
 *
 * Begin Defines
 *
 *=========================================================================*/

	/* Number of images allowed per CIF file */
  #define DFCIF_MAX_IMAGES 64

	/* Minimum number of bytes left to consider data another image record */
  #define DFCIF_MINBYTES_LEFT 12

	/* Number of subimage offsets in a weapon CIF */
  #define DFCIF_NUM_OFFSETS     32
  
/*===========================================================================
 *		End of Defines
 *=========================================================================*/


/*===========================================================================
 *
 * Class CDFCifImage
 *
 *=========================================================================*/
class CDFCifImage : public virtual CDFImgFile {

  /*---------- Begin Protected Class Members ----------------------*/
protected:
  short CIFUnknowns[33];	/* Unknown CIF information */


  /*---------- Begin Public Class Methods -------------------------*/
public:

	/* Class constructor/destructor */
  CDFCifImage() { memset(CIFUnknowns, 0x00, 2*33); }
  virtual ~CDFCifImage() { Destroy(); }
  virtual void Destroy (void) { memset(CIFUnknowns, 0x00, 2*33); CDFImgFile::Destroy(); }

	/* The load function is no longer valid */
  virtual boolean Load (const char* ) { SystemLog.Printf("CDFCifImage::Load() is no longer valid!"); return (FALSE); }

 };
/*===========================================================================
 *		End of Class CDFCifImage
 *=========================================================================*/


/*===========================================================================
 *
 * Class CDFCifFile
 *
 *=========================================================================*/
class CDFCifFile {

  /*---------- Begin Protected Class Members -----------------------*/
protected:
  CDFCifImage* pImages[DFCIF_MAX_IMAGES];
  int          NumImages;


  /*---------- Begin Protected Class Methods -----------------------*/
protected:

	/* Input helper functions */
  boolean ReadCIF       (FILE* pFileHandle);
  boolean ReadWeaponCIF (FILE* pFileHandle);


  /*---------- Begin Public Class Methods --------------------------*/
public:

	/* Class Constructor and Destructor */
  CDFCifFile();
  ~CDFCifFile() { Destroy(); }
  void Destroy (void);

	/* Class get members */
  int          GetNumImages (void) { return (NumImages); }
  CDFCifImage* GetImage     (const int Index) { if (IsValidIndex(Index)) return (pImages[Index]); return (NULL); }
  short	       GetWidth     (const int Index) { if (IsValidIndex(Index)) return (pImages[Index]->GetWidth()); return (NULL); }
  short	       GetHeight    (const int Index) { if (IsValidIndex(Index)) return (pImages[Index]->GetHeight()); return (NULL); }
  byte*	       GetImageData (const int Index) { if (IsValidIndex(Index)) return (pImages[Index]->GetData()); return (NULL); }

	/* Check for a weapon filename */
  boolean IsWeaponCIFFilename (const char* pFilename) { if (strnicmp(pFilename, "weap", 4) == 0) return (TRUE); return(FALSE); }

	/* Check the validity of a record index */
  boolean IsValidIndex (const int Index) { return ((Index >= 0 && Index < DFCIF_MAX_IMAGES) ? TRUE : FALSE); }

	/* Read in a CIF file */
  boolean Load (const char* pFilename);

 };
/*===========================================================================
 *		End of Class CDFCifFile
 *=========================================================================*/


#endif
/*===========================================================================
 *		End of File DFCif.H
 *=========================================================================*/