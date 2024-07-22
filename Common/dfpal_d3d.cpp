/*===========================================================================
 *
 * DFPal_D3D.CPP - Dave Humphrey (uesp@m0use.net), 9 November 2000
 *
 *=========================================================================*/

	/* Include Files */
#include "dfpal_d3d.h"


#undef  __FUNC__
#define __FUNC__ "CDFPaletteD3D::GenerateD3DPalette()"
/*===========================================================================
 *
 * Class CDFPaletteD3D Method - void GenerateD3DPalette (void);
 *
 * Creates the D3D palette data from the current raw palette.
 *
 *=========================================================================*/
void CDFPaletteD3D::GenerateD3DPalette (void) {
  int LoopCounter;

	/* Copy all the current defined palette entries */
  for (LoopCounter = 0; LoopCounter < NumPalEntries; LoopCounter++) {
    D3DPalData[LoopCounter].red   = PalData[LoopCounter].r * 4;
    D3DPalData[LoopCounter].blue  = PalData[LoopCounter].b * 4;
    D3DPalData[LoopCounter].green = PalData[LoopCounter].g * 4;
    D3DPalData[LoopCounter].flags = D3DRMPALETTE_FREE;
    DDPalData[LoopCounter].peBlue  = PalData[LoopCounter].b * 4;
    DDPalData[LoopCounter].peRed   = PalData[LoopCounter].r * 4;
    DDPalData[LoopCounter].peGreen = PalData[LoopCounter].g * 4;
    DDPalData[LoopCounter].peFlags = D3DRMPALETTE_FREE;
   }

 }
/*===========================================================================
 *		End of Class Method CDFPaletteD3D::GenerateD3DPalette()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFPaletteD3D::LoadCOL()"
/*===========================================================================
 *
 * Class CDFPaletteD3D Method - boolean LoadCOL (pFilename);
 *
 * Override simply to ensure the D3D palette data is properly regenerated
 * after a successfull load.
 *
 *=========================================================================*/
boolean CDFPaletteD3D::LoadCOL (const char* pFilename) {
  boolean Result;

	/* Call the base class method */
  Result = CDFPalette::LoadCOL(pFilename);

	/* Regenerate D3D palette on success */
  if (Result) GenerateD3DPalette();
  return (Result);
 }
/*===========================================================================
 *		End of Class Method CDFPaletteD3D::LoadCOL()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFPaletteD3D::LoadPAL()"
/*===========================================================================
 *
 * Class CDFPaletteD3D Method - boolean LoadPAL (pFilename);
 *
 * Override simply to ensure the D3D palette data is properly regenerated
 * after a successfull load.
 *
 *=========================================================================*/
boolean CDFPaletteD3D::LoadPAL (const char* pFilename) {
  boolean Result;

	/* Call the base class method */
  Result = CDFPalette::LoadPAL(pFilename);

	/* Regenerate D3D palette on success */
  if (Result) GenerateD3DPalette();
  return (Result);
 }
/*===========================================================================
 *		End of Class Method CDFPaletteD3D::LoadPAL()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFPaletteD3D::MakeRBG16Array()"
/*===========================================================================
 *
 * Class CDFPaletteD3D Method - ushort* MakeRGB16Array (RMask, GMask, BMask);
 *
 * Makes a pointer array of 16 BIT RGB triplets for each color in the
 * palette.  Returns NULL on any error.
 *
 *=========================================================================*/
ushort* CDFPaletteD3D::MakeRGB16Array (ulong RMask, ulong GMask, ulong BMask) {
  ushort* pArray;
  ushort* pArrayPtr;
  int	  Value;
  int     RShift = 8;
  int     GShift = 8;
  int     BShift = 8;
  int	  LoopCounter;
  
	/* Ensure at least one color in palette */
  if (NumPalEntries == 0) {
    SET_EXT_ERROR2(ERR_NULL, "No entries in palette!");
    return (NULL);
   }

  Value = RMask;

  while (Value != 0) {
    RShift--;
    Value >>= 1;
   }

  Value = GMask;

  while (Value != 0) {
    GShift--;
    Value >>= 1;
   }

  Value = BMask;

  while (Value != 0) {
    BShift--;
    Value >>= 1;
   }
 
	/* Allocate data for array */
  CreatePointerArray(pArray, ushort, NumPalEntries);
  pArrayPtr = pArray;

  for (LoopCounter = 0; LoopCounter < NumPalEntries; LoopCounter++) {

    if (RShift < 0) 
      *pArrayPtr = (ushort) ((D3DPalData[LoopCounter].red << -RShift) & RMask);
    else
      *pArrayPtr = (ushort) ((D3DPalData[LoopCounter].red >> RShift) & RMask);

    if (GShift < 0) 
      *pArrayPtr |= (ushort) ((D3DPalData[LoopCounter].green << -GShift) & GMask);
    else
      *pArrayPtr |= (ushort) ((D3DPalData[LoopCounter].green >> GShift) & GMask);
     
    if (BShift < 0) 
      *pArrayPtr |= (ushort) ((D3DPalData[LoopCounter].blue << -BShift) & BMask);
    else
      *pArrayPtr |= (ushort) ((D3DPalData[LoopCounter].blue >> BShift) & BMask);

    pArrayPtr++;
   }

  return (pArray);
 }
/*===========================================================================
 *		End of Class Method CDFPaletteD3D::MakeRGB16Array()
 *=========================================================================*/
