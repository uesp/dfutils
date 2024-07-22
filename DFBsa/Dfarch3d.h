/*=========================================================================
 *
 * DFARCH3D.H - Dave Humphrey (uesp@m0use.net), 29 October 2000
 *
 *=======================================================================*/
#ifndef __DFARCH3D_H
#define __DFARCH3D_H


	/* Required Includes */
#include "dfbsa.h"
#include "df3dobj.h"


/*=========================================================================
 *
 * Begin Defines
 *
 *=======================================================================*/

	/* Number of objects allowed in the arch3d file */
  #define DFARCH_MAX_3DOBJECTS 11000

/*=========================================================================
 *		End of Defines
 *=======================================================================*/


/*=========================================================================
 *
 * Begin Type and Structure Definitions
 *
 *=======================================================================*/

/*=========================================================================
 *		End of Type and Structure Definitions
 *=======================================================================*/


/*=========================================================================
 *
 * Class CDFArch3D Definition
 *
 * Contains methods needed to manipulate Daggerfall's ARCH3D.BSA file.
 * Based on the CDFBSAFile class.
 *
 *=======================================================================*/
class CDFArch3D : public virtual CDFBSAFile {

  /*---------- Begin Protected Class Members ---------------------*/
protected:
  CDF3DObject** p3DObjects;	/* Pointer to an array of pointer objects */


  /*---------- Begin Protected Class Methods ---------------------*/
protected:

	/* Creates a 3D Object pointer */
  virtual void Create3DObject (const int RecordIndex);

  
  /*---------- Begin Public Class Methods ------------------------*/
public:

	/* Class Constructor/Destructor */
  CDFArch3D();
  ~CDFArch3D();
  void Destroy (void);

	/* Compares two 3D object records */
  int DebugCompare      (const long ValueID,     CDF3DObject* p3DObject);
  int DebugCompareIndex (const int  RecordIndex, CDF3DObject* p3DObject);

	/* Get/load a 3DObject */
  CDF3DObject* Get3DObject (const long ValueID);
  CDF3DObject* Get3DObject (const int  RecordIndex);

	/* Load one 3d object from the file */
  boolean Load3DObject      (const long ValueID);
  boolean Load3DObjectIndex (const int  RecordIndex);

 };
/*=========================================================================
 *		End of Class CDFArch3D Definition
 *=======================================================================*/



#endif
/*=========================================================================
 *		End of File DFARCH3D.H
 *=======================================================================*/

