/*=========================================================================
 *
 * DFWoods.H - Dave Humphrey (uesp@m0use.net), 5 November 2000
 *
 *=======================================================================*/
#ifndef __DFWOODS_H
#define __DFWOODS_H


	/* Required Includes */
#include "genutil.h"


/*===========================================================================
 *
 * Begin Defines
 *
 *=========================================================================*/

	/* Size of the woods header in bytes */
  #define DFWOODS_HEADER_SIZE 144

	/* Size of the pixel records in bytes */
  #define DFWOODS_PIXELRECORD_SIZE 47

	/* Hardcoded values which shouldn't change */
  #define DFWOODS_MAP_WIDTH 1000
  #define DFWOODS_MAP_HEIGHT 500

	/* Size of the map data array for each land pixel */
  #define DFWOODS_PIXEL_SIZE 5

/*===========================================================================
 *		End of Defines
 *=========================================================================*/


/*===========================================================================
 *
 * Begin Type and Structure Definitions
 *
 *======================================================================*/

/* To ensure proper size of structures */
#ifdef _WIN32
  #pragma pack(push, save_pack)
  #pragma pack(2)
#endif

	/* Woods file header information */
  typedef struct {
    long OffsetSize;
    long MapWidth;
    long MapHeight;
    long NullValue1;
    long Data1Offset;
    long Unknown1;
    long Unknown2;
    long MapOffset;
    long NullValues[28];
   } dfwoods_header_t;


	/* Pixel record type */
  typedef struct { 
    union {
      short Unknown1;
      short SeedNumber;
     };
    long  NullValue1;
    short ClimateType;
    byte  LandType;
    byte  LandHeight;
    long  NullValues[3];
    union {
      byte  MapXYData[5][5];
      byte  MapData[25];
     };
   } dfwoods_pixel_t;

#ifdef _WIN32
  #pragma pack(pop, save_pack)
#endif

/*===========================================================================
 *		End of Type and Structure Definitions
 *=========================================================================*/


/*===========================================================================
 *
 * Class CDFWoodsPixel Definition
 *
 *=========================================================================*/
class CDFWoodsPixel {

  /*---------- Begin Protected Class Members ------------------------*/
protected:
  dfwoods_pixel_t PixelData;	/* Main pixel data */

  int	          XPixel;	/* X-Y location of the pixel record */
  int		  YPixel;	/* relative to the 1000x500 small map */
  int		  RecordIndex;	/* Pixel record index */
  long		  RecordOffset;	/* Offset to the pixel data in file */
  byte		  SmallPixel;	/* Value of the pixel in the small map */

  boolean	  Loaded;	/* Has the pixel data been loaded yet? */
  boolean	  Random;	/* Is the pixel data generated randomly */

  int		  RegionIndex;	/* Used when building the landscapes */
  int		  LocationIndex;
  boolean	  CreatedLocation;


  /*---------- Begin Public Class Methods ---------------------------*/
public:
  
	/* Class constructor/Destructor */
  CDFWoodsPixel()	      { memset(&PixelData, 0, sizeof(PixelData)); XPixel = -1; YPixel = -1; SmallPixel = 0; RecordIndex = -1; RecordOffset = 0; Loaded = FALSE; Random = FALSE; RegionIndex = -1; LocationIndex = -1; CreatedLocation = FALSE; }
  virtual void Destroy (void) { memset(&PixelData, 0, sizeof(PixelData)); XPixel = -1; YPixel = -1; SmallPixel = 0; RecordIndex = -1; RecordOffset = 0; Loaded = FALSE; Random = FALSE; RegionIndex = -1; LocationIndex = -1; CreatedLocation = FALSE; }

	/* Creates a 'random' landscape record */
  void CreateRandom (const int X, const int Y);

	/* Get class members */
  int   GetXPixel     (void) { return (XPixel); }
  int   GetYPixel     (void) { return (YPixel); }
  int   GetIndex      (void) { return (RecordIndex); }
  int   GetSmallPixel (void) { return ((int) (unsigned int)SmallPixel); }
  int   GetUnknown1   (void) { return ((int)PixelData.SeedNumber); }
  int   GetSeedNumber (void) { return ((int)PixelData.SeedNumber); }
  int   GetClimateType(void) { return ((int)PixelData.ClimateType); }
  int   GetLandType   (void) { return ((int)PixelData.LandType); }
  int   GetLandHeight (void) { return ((int)PixelData.LandHeight); }
  int   GetRegionIndex(void) { return (RegionIndex); }
  int   GetLocationIndex(void) { return (LocationIndex); }
  byte  GetMapData    (const int X, const int Y) { return (PixelData.MapXYData[Y][X]); }
  byte* GetMapPointer (void) { return (&PixelData.MapData[0]); }
  dfwoods_pixel_t* GetPixelData       (void) { return (&PixelData); }
  boolean	   GetCreatedLocation (void) { return (CreatedLocation); }

	/* Get various states of the pixel data */
  boolean IsRandom (void) { return (Random); }
  boolean IsLoaded (void) { return (Loaded); }

	/* Read in the pixel data from a file stream */
  boolean Read (FILE* pFileHandle);  

	/* Set class members */
  void SetPosition   (const int X, const int Y) { XPixel = X; YPixel = Y; }
  void SetIndex      (const int Value)	        { RecordIndex = Value; }
  void SetOffset     (const int Offset)	        { RecordOffset = Offset; }
  void SetSmallPixel (const byte Value)		{ SmallPixel = Value; }
  void SetRandom     (const boolean Value)      { Random = Value; }
  void SetRegionIndex     (const int Value)	{ RegionIndex   = Value; }
  void SetLocationIndex   (const int Value)	{ LocationIndex = Value; }
  void SetCreatedLocation (const boolean Value) { CreatedLocation = Value; }

 };
/*===========================================================================
 *		End of Class CDFWoodsPixel Definition
 *=========================================================================*/


/*===========================================================================
 *
 * Class CDFWoodsFile Definition
 *
 *=========================================================================*/
class CDFWoodsFile {

  /*---------- Begin Protected Class Members ---------------------*/
protected:
  dfwoods_header_t Header;	/* File header information */
  boolean	   HeaderRead;	/* Has the header been input yet? */

  FILE*	   pFileHandle;		/* The handle to the current woods file stream */
  char*	   pWoodsFilename;	/* The current filename, if available */  		 


  /*---------- Begin Protected Class Methods ---------------------*/
protected:

	/* Helper input functions */
  boolean ReadHeader (void);
 
	/* Read in the pixel record file offset */
  long ReadRecordOffset (const long RecordIndex);

 
  /*---------- Begin Public Class Methods ------------------------*/
public:

	/* Class Constructor/Destructor */
  CDFWoodsFile();
  virtual ~CDFWoodsFile() {  Destroy(); DestroyPointer(pWoodsFilename); }
  virtual void Destroy (void);

	/* Closes an open woods file */
  void Close (void) { if (pFileHandle) { fclose (pFileHandle); pFileHandle = NULL; } }
	
	/* Get the pixel record index for a given position */
  long ComputeRecordIndex (const int XPos, const int YPos);

	/* Creates an array of vertices of the given record according to the
	 * given parameters. */
  float* CreateVertexArray (const dfwoods_pixel_t* pRecord, const int DetailFactor);

	/* Get class members */
  char* GetFilename (void) { return (pWoodsFilename); }

	/* Is the file currently open? */
  boolean IsOpen (void) { return ((pFileHandle == NULL) ? FALSE : TRUE); }

	/* Check class values for validity */
  boolean IsValidXPos  (const int  XPos)  { return ((XPos >= 0 && XPos < Header.MapWidth) ? TRUE : FALSE); }
  boolean IsValidYPos  (const int  YPos)  { return ((YPos >= 0 && YPos < Header.MapWidth) ? TRUE : FALSE); }
  boolean IsValidIndex (const long Index) { return ((Index >= 0 && Index < Header.OffsetSize/4) ? TRUE : FALSE); }

	/* Open the specified Woods.WLD file and read header information */
  boolean Open (const char* pFileName = NULL);

	/* Read in one pixel record given by the X-Y coordinates */
  boolean ReadPixelRecord (const int XPos, const int YPos, CDFWoodsPixel& PixelRecord);

	/* Set class members */
  void SetFilename (const char* pString) { DestroyPointer(pWoodsFilename); pWoodsFilename = CreateString(pString); }

 };
/*===========================================================================
 *		End of Class CDFWoodsFile Definition
 *=========================================================================*/


#endif
/*===========================================================================
 *		End of File DFWoods.H
 *=========================================================================*/
