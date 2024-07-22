/*===========================================================================
 *
 * DFTextImg.H - Dave Humphrey (uesp@m0use.net), 3 November 2000
 *
 * To only be included from DFTexture.H
 *
 *=========================================================================*/
#ifndef __DFTEXTIMG_H
#define __DFTEXTIMG_H

	/* Required Includes */
#include "genutil.h"


/*===========================================================================
 *
 * Begin Defines
 *
 *=========================================================================*/

	/* Number of sub-images allowed per image */
  #define DFTEXTURE_MAX_SUBIMAGES 24

  	/* Standard file offsets to data */
  #define DFTEXTURE_HEADER_OFFSET 0
  #define DFTEXTURE_OFFSET_OFFSET 26
	
	/* Maximum image sizes */
  #define DFTEXTURE_MAXIMAGE_WIDTH  400
  #define DFTEXTURE_MAXIMAGE_HEIGHT 300

	/* Used to decode RLE type images, Width*4 */
  #define DFTEXTURE_MAXRLEROW_SIZE  1600

	/* Standard texture record sizes */
  #define DFTEXTURE_OFFSET_RECORDSIZE 20
  #define DFTEXTURE_HEADER_RECORDSIZE 26
  #define DFTEXTURE_IMAGEHEADER_SIZE  28

	/* Standard size of texture image rows */
  #define DFTEXTURE_IMAGEROW_WIDTH 256

	/* Dump mask values */
  #define DFTEXTURE_DM_TEXTUREINFO   1
  #define DFTEXTURE_DM_OFFSETLIST    2
  #define DFTEXTURE_DM_OFFSETNULLS   4
  #define DFTEXTURE_DM_IMAGE	     8
  #define DFTEXTURE_DM_IMAGEHEADER   16
  #define DFTEXTURE_DM_IMAGEOFFSETS  32
  #define DFTEXTURE_DM_IMAGEUNKNOWNS 64
  #define DFTEXTURE_DM_ALL	     0xFFFF

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

	/* Flags for dumping object information */
  typedef unsigned int dftexture_dumpmask_t;

	/* Texture image header description */
  typedef struct {
    short XOffset;
    short YOffset;
    short Width;
    short Height;
    short Unknown1;
    long  ImageSize;
    long  DataOffset;
    short Unknown2;
    short NumSubImages;
    long  Unknown3;
    short Unknown4;
   } dftexture_imageheader_t;

	/* Information needed for subimage data */
  typedef struct {
    long  Offset;
    short Width;
    short Height;
   } dftexture_subimage_t;


	/* Used for RLE type image data */
  typedef struct {
    short	   Offset;
    unsigned short Unknown;
   } dftexture_rleoffset_t;

#ifdef _WIN32
  #pragma pack(pop, save_pack)
#endif

/*===========================================================================
 *		End of Type and Structure Definitions
 *=========================================================================*/


/*===========================================================================
 *
 * Class CDFTextureImage Definition
 *
 *=========================================================================*/
class CDFTextureImage {

  /*---------- Begin Protected Class Members ----------------------*/
protected:
  dftexture_imageheader_t Header;	/* Image header information */
  long			  HeaderOffset;	/* Location of header in file */

	/* The image data */
  byte*			  pData[DFTEXTURE_MAX_SUBIMAGES];
  int			  NumSubImagesAllocated;

	/* Subimage data, if more than 1 */
  dftexture_subimage_t	  SubImageInfo[DFTEXTURE_MAX_SUBIMAGES];


  /*---------- Begin Protected Class Methods ----------------------*/
protected:

	/* Input helper functions */
  boolean ReadData    (FILE* pFileHandle);
  boolean ReadHeader  (FILE* pFileHandle);
  boolean ReadRLEData (FILE* pFileHandle);
  boolean ReadRLESubImageData (FILE* pFileHandle);
  boolean ReadSubImageData    (FILE* pFileHandle);
  boolean ReadSubImageInfo    (FILE* pFileHandle);

	/* Helper function to uncompress a RLE encoded row of image data */
  boolean UncompressRLERow      (FILE* pFileHandle, dftexture_rleoffset_t* pRowOffset, byte** ppDataPtr);
  boolean UncompressSubImageRow (FILE* pFileHandle, const int SubImageIndex, byte** ppDataPtr);


  /*---------- Begin Public Class Methods -------------------------*/
public:

	/* Class Constructor and Destructor */
  CDFTextureImage();
  ~CDFTextureImage() { Destroy(); }
  void Destroy (void);

	/* Output object information to a file stream */
  void Dump (FILE* pFileHandle, const dftexture_dumpmask_t DumpMask = DFTEXTURE_DM_ALL);

	/* Class get members */
  short GetXOffset        (void) { return (Header.XOffset); }
  short GetYOffset        (void) { return (Header.YOffset); }
  short GetWidth          (const int Index) { if (GetNumSubImages() <= 1) return (Header.Width);  if (IsValidImageIndex(Index)) return (SubImageInfo[Index].Width);  return (0); }
  short GetHeight         (const int Index) { if (GetNumSubImages() <= 1) return (Header.Height);  if (IsValidImageIndex(Index)) return (SubImageInfo[Index].Height);  return (0); }
  short GetWidth          (void) { return (Header.Width); }
  short GetHeight         (void) { return (Header.Height); }
  short GetUnknown1       (void) { return (Header.Unknown1); } 
  long  GetImageSize      (void) { return (Header.ImageSize); }
  long  GetDataOffset     (void) { return (Header.DataOffset); }
  short GetNumSubImages   (void) { return (Header.NumSubImages); }
  short GetUnknown2       (void) { return (Header.Unknown2); }
  long  GetUnknown3       (void) { return (Header.Unknown3); }
  short GetUnknown4       (void) { return (Header.Unknown4); }
  long  GetHeaderOffset   (void) { return (HeaderOffset); }
  long  GetAbsDataOffset  (void) { return (HeaderOffset + Header.DataOffset); }
  long  GetDataSize	  (void) { return (Header.Width * Header.Height); }

	/* Get the given image data */
  byte* GetData	          (const int Index = 0) { if (IsValidImageIndex(Index)) return (pData[Index]); return (NULL); }

	/* Checks to see if any image is currently in use */
  virtual boolean IsImageUsed (const int Index = 0) { if (!IsValidImageIndex(Index)) return (FALSE); return (TRUE); }

	/* Is the image data RLE compressed */
  boolean IsRLECompressed (void) { if (GetUnknown1() == 0x1108 || GetUnknown1() == 0x0108) return (TRUE); return (FALSE); }

	/* Checks the validity of a subimage index */
  boolean IsValidImageIndex (const int Index) { return ((Index >= 0 && Index < GetNumSubImages()) ? TRUE : FALSE); }

	/* Attempt to read in the image data from the given file stream */
  virtual boolean Read (FILE* pFileHandle);

 };
/*===========================================================================
 *		End of Class CDFTextureImage Definition
 *=========================================================================*/


#endif
/*===========================================================================
 *		End of File DFTextImg.H
 *=========================================================================*/