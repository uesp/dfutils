/*===========================================================================
 *
 * DFTexture.H - Dave Humphret (uesp@m0use.net), 2 November 2000
 *
 *=========================================================================*/
#ifndef __DFTEXTURE_H
#define __DFTEXTURE_H


	/* Required Includes */
#include "dftextimg.h"


/*===========================================================================
 *
 * Begin Defines
 *
 *=========================================================================*/

 	/* Number of images allowed per texture file. Code introduced limit,
	 * not a DF texture file limit */
  #define DFTEXTURE_MAX_IMAGES 256

  	/* Size in bytes of the texture name string */
  #define DFTEXTURE_NAME_SIZE 24

/*===========================================================================
 *		End of Defines
 *=========================================================================*/


/*===========================================================================
 *
 * Type and Structure Definitions
 *
 *=========================================================================*/

	/* To ensure proper size of structures */
#ifdef _WIN32
  #pragma pack(push, save_pack)
  #pragma pack(2)
#endif
	/* Texture offset record description */
  typedef struct {
    short Type1;
    long  HeaderOffset;
    short Type2;
    long  Unknown1;
    long  NullValue1;
    long  NullValue2;
   } dftexture_offset_t;

#ifdef _WIN32
  #pragma pack(pop, save_pack)
#endif

/*===========================================================================
 *		End of Type and Structure Definitions
 *=========================================================================*/


	/* Another required include */
#include "dftextimg.H"


/*===========================================================================
 *
 * Class CDFTexture Definition
 *
 *=========================================================================*/
class CDFTexture { 

  /*---------- Begin Protected Class Members ----------------------*/
protected:
  char*			Name;			/* Texture identifying string */
  dftexture_offset_t*	Offsets;		/* Offset record information */
  CDFTextureImage**	pImages;		/* Texture image data */
  int			NumImages;		/* Total number of images */
  int			NumImagesAllocated;	/* Images currently loaded */


  /*---------- Begin Protected Class Methods ----------------------*/
protected:

	/* Creates an image object */
  virtual void CreateImage (const int ImageIndex);

	/* Load method helper functions */
  boolean ReadHeader  (FILE* pFileHandle);
  boolean ReadImages  (FILE* pFileHandle);
  boolean ReadOffsets (FILE* pFileHandle);


  /*---------- Begin Public Class Methods -------------------------*/
public:

	/* Class Constructor and Destructor */
  CDFTexture();
  ~CDFTexture();
  void Destroy (void);

	/* Output object information to the file stream */
  void Dump (FILE* pFileHandle, const dftexture_dumpmask_t DumpMask = DFTEXTURE_DM_ALL);

	/* Class get member functions */
  char* GetName         (void) { return (Name); }
  int   GetNumImages    (void) { return (NumImages); }
  int   GetNumSubImages (const int Index) { if (IsValidIndex(Index)) return (pImages[Index]->GetNumSubImages()); return(0); }
  int   GetWidth        (const int Index, const int SubIndex = 0) { if (IsValidIndex(Index)) return (pImages[Index]->GetWidth(SubIndex)); return(0); }
  int   GetHeight       (const int Index, const int SubIndex = 0) { if (IsValidIndex(Index)) return (pImages[Index]->GetHeight(SubIndex)); return(0); }
  byte* GetImageData    (const int Index, const int SubIndex = 0) { if (IsValidIndex(Index)) return (pImages[Index]->GetData(SubIndex)); return(NULL); }

	/* Get the offset record data */
  dftexture_offset_t* GetOffsetData (const int Index) { if (IsValidIndex(Index)) return (&Offsets[Index]); return (NULL); }

	/* Determines if the part/entire texture file is loaded */
  boolean IsLoaded (void) { return ((NumImages == NumImagesAllocated) ? TRUE : FALSE); }

	/* Checks an image index for validity */
  boolean IsValidIndex (const int Index);

	/* Attempt to load the given texture file */
  virtual boolean Load (const char* pFilename);

	/* Class member set functions */
  void SetName (const char* pString) { if (pString == NULL) { Name[0] = NULL_CHAR; } else { strnncpy(Name, pString, DFTEXTURE_NAME_SIZE); } }

 };
/*===========================================================================
 *		End of Class CDFTexture Definition
 *=========================================================================*/


#endif
/*===========================================================================
 *		End of File DFTexture.h
 *=========================================================================*/