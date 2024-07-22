/*===========================================================================
 *
 * DFCommon_D3D.H - Dave Humphrey (uesp@m0use.net), 9 November 2000
 *
 *=========================================================================*/
#ifndef __DFCOMMON_D3D_H
#define __DFCOMMON_D3D_H

	/* Required Includes */
#include "dfcommon.h"
#include "d3dcom.h"
#include "dfpal_d3d.h"
#include "dfimgd3d.h"
#include "dftexture_d3d.h"
#include "arch_d3d.h"
#include "dfmaps.h"
#include "dfblocks_d3d.h"


/*===========================================================================
 *
 * Begin Defines
 *
 *=========================================================================*/

	/* Default number of texture files */
  #define DF_MAX_TEXTURES 512

	/* Define the scaling of pixels to coordinates for texture decals */
  #define DFD3D_DECAL_SCALE(Value) ((float)Value)

/*===========================================================================
 *		End of Defines
 *=========================================================================*/


/*===========================================================================
 *
 * Begin Function Definitions
 *
 *=========================================================================*/

	/* Get a DF D3D texture object */
   CD3DTexture* DFGetD3DTexture (const int File, const int Index, const int SubIndex = 0);

	/* Load a Image File */
   boolean DFGetImageFile (const char* pFilename, CDFImgFileD3D& Image);

/*===========================================================================
 *		End of Function Definitions
 *=========================================================================*/


/*===========================================================================
 *
 * Begin External Variables
 *
 *=========================================================================*/

  extern CDFPaletteD3D    DFStandardPaletteD3D;
  extern CDFBlocksFileD3D DFBlocks;
  extern CDFArchD3D       DFArch3D;

/*===========================================================================
 *		End of External Variables
 *=========================================================================*/


#endif
/*===========================================================================
 *		End of File DFCommon_D3D.H
 *=========================================================================*/
