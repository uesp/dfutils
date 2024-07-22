/*===========================================================================
 *
 * DFImgD3D.CPP - Dave Humphrey (uesp@m0use.net), 12 November 2000
 *
 *=========================================================================*/

	/* Include Files */
#include "dfimgd3d.h"
#include "dfcommon_d3d.h"


#undef  __FUNC__
#define __FUNC__ "CDFImgFileD3D::MakeDDSurface()"
/*===========================================================================
 *
 * Class CDFImgFileD3D Method - boolean MakeDDSurface (void);
 *
 * Makes the DirectDraw surface associated with the image.  Returns FALSE 
 * on any error.
 *
 *=========================================================================*/
boolean CDFImgFileD3D::MakeDDSurface (void) { 
  ddsdesc_t LockDesc;
  HRESULT   Result;

  	/* Ensure the image data and DD object is valid */
  if (pData == NULL) {
    SET_EXT_ERROR2(ERR_NULL, "Image data is NULL!");
    return (FALSE);
   }

	/* Delete the current surface, if any */
  RELEASE(pSurface);

	/* Initialize the surface description */
  memset(&SurfaceDesc, 0, sizeof(SurfaceDesc));
  SurfaceDesc.dwSize = sizeof(SurfaceDesc);
  SurfaceDesc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
  SurfaceDesc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
  SurfaceDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
  SurfaceDesc.dwWidth = (int) GetWidth();
  SurfaceDesc.dwHeight = (int) GetHeight();
  SurfaceDesc.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
  SurfaceDesc.ddpfPixelFormat.dwRBitMask = 63488;
  SurfaceDesc.ddpfPixelFormat.dwGBitMask = 2016;
  SurfaceDesc.ddpfPixelFormat.dwBBitMask = 31;
  SurfaceDesc.ddpfPixelFormat.dwRGBBitCount = 16;
  SurfaceDesc.ddpfPixelFormat.dwRGBAlphaBitMask = 0;
  SurfaceDesc.ddpfPixelFormat.dwFlags = DDPF_PALETTEINDEXED8 | DDPF_PALETTEINDEXEDTO8;
//  SurfaceDesc.ddpfPixelFormat.dwFlags = DDPF_RGB;

 	/* Create the surface */
  Result = (GetDD())->CreateSurface(&SurfaceDesc, &pSurface, NULL);

  if (FAILED(Result)) {
    SET_D3D_ERROR2(Result, "Failed to create the DirectDraw surface!");
    return (FALSE);
   }

	/* Create the palette of the surface if required */
/*
  Result = GetDD()->CreatePalette(DDPCAPS_8BIT, 
				  LPPALETTEENTRY (DFStandardPaletteD3D.GetDDPalData()), 
				  &pDDPalette, NULL);

  if (FAILED(Result)) {
    SET_D3D_ERROR2(Result, "CDFImgFileD3D::MakeDDSurface() - Failed to create DirectDraw palette!");
    return (FALSE);
   }

  //Result = pSurface->SetPalette(pDDPalette);
  RELEASE(pDDPalette);

  if (FAILED(Result)) {
    SET_D3D_ERROR2(Result, "CDFImgFileD3D::MakeDDSurface() - Failed to set DirectDraw primary surface palette!");
    return (FALSE);
   } //*/

	/* Attempt to lock surface */
  LockDesc.dwSize = sizeof(LockDesc);
  Result = pSurface->Lock(NULL, &LockDesc, DDLOCK_WAIT, NULL);

  if (FAILED(Result)) {
    SET_D3D_ERROR2(Result, "Failed to lock the DirectDraw surface!");
    return (FALSE);
   }

  //SystemLog.Printf ("Lock Pitch = %d", LockDesc.lPitch);
  //SystemLog.Printf ("Size of Lock = %d x %d", LockDesc.dwWidth, LockDesc.dwHeight);
  //SystemLog.Printf ("Pixel Format = %d", LockDesc.dwFlags & DDSD_PIXELFORMAT);
  //SystemLog.Printf ("Pixel Size = %d", LockDesc.ddpfPixelFormat.dwSize);
  //SystemLog.Printf ("Pixel Flags = 0x%X", LockDesc.ddpfPixelFormat.dwFlags);
  //SystemLog.Printf ("Pixel RGB Bit Count = %d", LockDesc.ddpfPixelFormat.dwRGBBitCount);
  //SystemLog.Printf ("Pixel RGB Mask = %d / %d / %d", LockDesc.ddpfPixelFormat.dwRBitMask, LockDesc.ddpfPixelFormat.dwGBitMask, LockDesc.ddpfPixelFormat.dwBBitMask);
  //SystemLog.Printf ("Pixel RGB Alpha Mask = %d", LockDesc.ddpfPixelFormat.dwRGBAlphaBitMask);
  
  	/* Output image to the buffer depending on the BPP */
  if ((LockDesc.dwFlags & DDSD_PIXELFORMAT) == 0) {
    //MakePalDDSurface(LockDesc);
    SET_EXT_ERROR2(ERR_CUSTOM, "Unknown pixel format!");
   }
  else if ((LockDesc.ddpfPixelFormat.dwFlags & DDPF_RGB) != 0) {
    switch (LockDesc.ddpfPixelFormat.dwRGBBitCount) {
      case 16:
        MakeRGB16DDSurface(LockDesc);
    	break;
     }
   }

//  memset(LockDesc.lpSurface, 0, LockDesc.dwWidth*LockDesc.dwHeight);
//  ((byte *)LockDesc.lpSurface)[317] = 0x50;
//  ((byte *)LockDesc.lpSurface)[318] = 0x50;
//  ((byte *)LockDesc.lpSurface)[319] = 0x50;
//  ((byte *)LockDesc.lpSurface)[320] = 0x50;

  	/* Attempt to unlock surface */
  Result = pSurface->Unlock(NULL);

  if (FAILED(Result)) {
    SET_D3D_ERROR2(Result, "Failed to unlock the DirectDraw surface!");
    return (FALSE);
   }
  
  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFImgfileD3D::MakeDDSurface()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFImgFileD3D::MakeRGB16DDSurface()"
/*===========================================================================
 *
 * Class CDFImgFileD3D Method - boolean MakeRGB16DDSurface (Desc);
 *
 * Converts the image data to a 16 bit RGB type DirectDraw surface.  Returns
 * FALSE on any error.
 *
 *=========================================================================*/
boolean CDFImgFileD3D::MakeRGB16DDSurface (ddsdesc_t& Desc) {
  ushort* pPalette = NULL;
  ushort* pDestPtr;
  byte*	  pSourcePtr;
  int     RowCounter;
  int     ColCounter; 

  pDestPtr   = (ushort *) Desc.lpSurface;
  pPalette   = DFStandardPaletteD3D.MakeRGB16Array(Desc.ddpfPixelFormat.dwRBitMask, Desc.ddpfPixelFormat.dwGBitMask, Desc.ddpfPixelFormat.dwBBitMask);
  pSourcePtr = pData;

  for (RowCounter = 0; RowCounter < GetHeight(); RowCounter++) {

    for (ColCounter = 0; ColCounter < GetWidth(); ColCounter++) {
      *pDestPtr = pPalette[*pSourcePtr];
      pSourcePtr++;
      pDestPtr++;
     }

    //pDestPtr += Desc.lPitch - Desc.dwWidth;
   }

  DestroyPointer(pPalette);
  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFImgFileD3D::MakeRGB16DDSurface()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFImgFileD3D::Read()"
/*===========================================================================
 *
 * Class CDFImgFileD3D Method - boolean Read (pFileHandle);
 *
 * Calls the base class method and creates the DirectDraw surface on success.
 *
 *=========================================================================*/
boolean CDFImgFileD3D::Read (FILE* pFileHandle) {
  boolean Result;

	/* Ensure valid objects */
  ASSERT(pFileHandle != NULL);

	/* Call the base class method first */
  Result = CDFImgFile::Read(pFileHandle);
  if (!Result) return (FALSE);

	/* Attempt to make the DD surface object */
  return (MakeDDSurface());
 }
/*===========================================================================
 *		End of Class CDFImgFileD3D::Read()
 *=========================================================================*/