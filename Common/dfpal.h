/*===========================================================================
 *
 * DFPal.H - Dave Humphrey (uesp@m0use.net), 9 November 2000
 *
 *=========================================================================*/
#ifndef __DFPAL_H
#define __DFPAL_H

	/* Required Includes */
#include "genutil.h"


/*===========================================================================
 *
 * Begin Defines
 *
 *=========================================================================*/

	/* Maximum number of entries in the palette */
  #define DF_MAX_PALENTRIES 256

	/* Default file sizes */
  #define DF_PAL_FILESIZE 768
  #define DF_COL_FILESIZE 776

	/* Default record sizes */
  #define DF_COL_HEADERSIZE 8

/*===========================================================================
 *		End of Defines
 *=========================================================================*/


/*===========================================================================
 *
 * Begin Type and Structure Definitions
 *
 *=========================================================================*/

	/* To ensure proper size of structures */
#ifdef _WIN32
  #pragma pack(push, save_pack)
  #pragma pack(1)
#endif

	/* Standard palette type */
  typedef struct {
    union { byte r, R, Red; };
    union { byte g, G, Green; };
    union { byte b, B, Blue; };
   } df_palentry_t;

	/* Header information contained in COL files */
  typedef struct {
    long FileSize;
    long Unknown;
   } df_colheader_t;

#ifdef _WIN32
  #pragma pack(pop, save_pack)
#endif

/*===========================================================================
 *		End of Type and Structure Definitions
 *=========================================================================*/


/*===========================================================================
 *
 * Class CDFPalette Definition
 *
 *=========================================================================*/
class CDFPalette {

  /*---------- Begin Protected Class Members ---------------------*/
protected:	
  df_colheader_t COLHeader;			/* Header information for COL files */
  df_palentry_t  PalData[DF_MAX_PALENTRIES];	/* The raw DF palette data */
  int		 NumPalEntries;			/* Number of colors currently defined */
  boolean	 COLFile;			/* Is the file a COL or PAL? */


  /*---------- Begin Public Class Methods ------------------------*/
public:

	/* Class constructor */
  CDFPalette();

	/* Class get members */
  int		 GetNumPalEntries (void) { return (NumPalEntries); }
  df_palentry_t* GetPalData	  (void) { return (&PalData[0]); }
  df_palentry_t* GetPalette	  (void) { return (&PalData[0]); }
  byte*		 GetBytePalData   (void) { return (&PalData[0].r); }
  byte*		 GetBytePalette	  (void) { return (&PalData[0].r); }
  df_palentry_t* GetPalEntry	  (const int Index) { if (Index < 0 || Index >= NumPalEntries) return (NULL); return (&PalData[Index]); }

	/* Get the type of palette data */
  boolean IsCOLFile (void) { return (COLFile); }
  boolean IsPALFile (void) { return ((boolean) !COLFile); }

	/* Attempt to load a COL/PAL file */
	  boolean Load    (const char* pFilename);
  virtual boolean LoadCOL (const char* pFilename);
  virtual boolean LoadPAL (const char* pFilename);

 };
/*===========================================================================
 *		End of Class CDFPalette Definition
 *=========================================================================*/

#endif
/*===========================================================================
 *		End of File DFPal.H	
 *=========================================================================*/