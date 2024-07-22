/*===========================================================================
 *
 * DFPal_D3D.H - Dave Humphrey (uesp@m0use.net), 9 November 2000
 *
 *=========================================================================*/
#ifndef __DFPAL_D3D_H
#define __DFPAL_D3D_H

	/* Required Includes */
#include "d3dcom.h"
#include "dfpal.h"



/*===========================================================================
 *
 * Class CDFPaletteD3D Definition
 *
 *=========================================================================*/
class CDFPaletteD3D : public virtual CDFPalette {

  /*---------- Begin Protected Class Members ----------------------*/
protected:
  d3dpalentry_t D3DPalData[DF_MAX_PALENTRIES];	/* Palette data for D3DRM */
  ddpalentry_t  DDPalData[DF_MAX_PALENTRIES];	/* Palette data for DD */


  /*---------- Begin Public Class Methods -------------------------*/
public:

	/* Class Constructor */
  CDFPaletteD3D() { GenerateD3DPalette(); }

	/* Generate the D3DPalette from the current raw palette */
  void GenerateD3DPalette (void);

	/* Get class members */
  d3dpalentry_t* GetD3DPalette  (void) { return &D3DPalData[0]; }
  d3dpalentry_t* GetD3DPalData  (void) { return &D3DPalData[0]; }
  d3dpalentry_t* GetD3DPalEntry (const int Index) { if (Index < 0 || Index >= NumPalEntries) return (NULL); return (&D3DPalData[Index]); }
  ddpalentry_t*  GetDDPalette   (void) { return &DDPalData[0]; }
  ddpalentry_t*  GetDDPalData   (void) { return &DDPalData[0]; }
  ddpalentry_t*  GetDDPalEntry  (const int Index) { if (Index < 0 || Index >= NumPalEntries) return (NULL); return (&DDPalData[Index]); }
  

	/* Override these to generate D3D palette */
  virtual boolean LoadCOL (const char* pFilename); 
  virtual boolean LoadPAL (const char* pFilename);

	/* Make conversion arrays */
  //virtual byte*   MakeRGB8Array  (DDSURFACEDESC& SurfaceDesc);
  virtual ushort* MakeRGB16Array (ulong RMask, ulong GMask, ulong BMask);
  //virtual uint*   MakeRGB24Array (DDSURFACEDESC& SurfaceDesc);

 };
/*===========================================================================
 *		End of Class CDFPaletteD3D Definition
 *=========================================================================*/


#endif
/*===========================================================================
 *		End of File DFPal_D3D.H
 *=========================================================================*/