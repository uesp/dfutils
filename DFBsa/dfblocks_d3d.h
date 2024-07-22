/*===========================================================================
 *
 * DFBlocks_D3D.H - Dave Humphrey (uesp@m0use.net), 19 November 2000
 *
 *=========================================================================*/
#ifndef __DFBLOCKS_D3D_H
#define __DFBLOCKS_D3D_H

	/* Required Includes */
#include "dfblocks.h"
#include "d3dcom.h"


/*===========================================================================
 *
 * Class CDFRMBRecordD3D Definition
 *
 *=========================================================================*/
class CDFRMBRecordD3D : public CDFRMBRecord {

  /*---------- Begin Protected Class Members ----------------------*/
protected:


  /*---------- Begin Protected Class Methods ----------------------*/
protected:

	/* Helper functions to create a block record */
  boolean Create3DObjects (CD3DFrame* pDestFrame);
  boolean CreateFlats     (CD3DFrame* pDestFrame);
  boolean CreatePeople    (CD3DFrame* pDestFrame);
	

  /*---------- Begin Public Class Methods -------------------------*/
public:

	/* Class constructor/Destructor */
  CDFRMBRecordD3D() { }
  ~CDFRMBRecordD3D() { Destroy(); }
  virtual void Destroy (void) { CDFRMBRecord::Destroy();  }

	/* Adds 3D objects in the RMB record to a D3D frame */
  boolean CreateD3DBlock (const dfblk_flag_t Flags, CD3DFrame* pDestFrame);

 };
/*===========================================================================
 *		End of Class CDFRMBFileD3D Definition
 *=========================================================================*/



/*===========================================================================
 *
 * Class CDFRMBFileD3D Definition
 *
 *=========================================================================*/
class CDFRMBFileD3D : public CDFRMBFile {

  /*---------- Begin Protected Class Members ----------------------*/
protected:


  /*---------- Begin Protected Class Methods ----------------------*/
protected:

	/* Allocate the block record arrays */
  virtual void CreateBlockRecords (void);


  /*---------- Begin Public Class Methods -------------------------*/
public:

	/* Adds 3D objects in the RMB file to a D3D frame */
  boolean CreateD3DBlock (const dfblk_flag_t Flags, LPDIRECT3DRMFRAME2 pDestFrame);

 };
/*===========================================================================
 *		End of Class CDFRMBFileD3D Definition
 *=========================================================================*/


/*===========================================================================
 *
 * Class CDFBlocksFileD3D Definition
 *
 *=========================================================================*/
class CDFBlocksFileD3D : public CDFBlocksFile {

  /*---------- Begin Protected Class Members ----------------------*/
protected:


  /*---------- Begin Protected Class Methods ----------------------*/
protected:
  
	/* Create an RMB file object */
  void CreateRMBFile (const int Index);


  /*---------- Begin Public Class Methods -------------------------*/
public:

	/* Get RMB block record */
  virtual CDFRMBFileD3D* GetRMBFileD3D (const char* pFilename)  { return ((CDFRMBFileD3D*) GetRMBFile(pFilename)); }
  virtual CDFRMBFileD3D* GetRMBFileD3D (const int Index)	{ return ((CDFRMBFileD3D*) GetRMBFile(Index)); }
  virtual CDFRMBFileD3D* GetRMBFileD3D (const int FileIndex, const int CharIndex, const int Number) { return ((CDFRMBFileD3D*) GetRMBFile(FileIndex, CharIndex, Number)); }

 };
/*===========================================================================
 *		End of Class CDFRMBFileD3D Definition
 *=========================================================================*/

#endif
/*===========================================================================
 *		End of File DFBlocks_D3D.H
 *=========================================================================*/