/*===========================================================================
 *
 * Arch_D3D.H - Dave Humphrey (uesp@m0use.net), 31 October 2000
 *
 *=========================================================================*/
#ifndef __ARCH_D3D_H
#define __ARCH_D3D_H

	/* Required Includes */
#include "df3dobj.h"
#include "d3dcom.h"
#include "dfarch3d.h"


/*===========================================================================
 *
 * Begin Defines
 *
 *=========================================================================*/

	/* Default scaling factor for DF objects */
  #define DF_DEFAULT_3DOBJECT_SCALE D3DVAL(256.0)

/*===========================================================================
 *		End of Defines
 *=========================================================================*/


/*===========================================================================
 *
 * Class CDF3DObjectD3D Definition
 *
 *=========================================================================*/
class CDF3DObjectD3D : public CDF3DObject {

  /*---------- Begin Protected Class Members -----------------------*/
protected:
  static boolean    LoadObjectNormals;	/* Do we load normals or create them with the builder? */
  static d3dvalue_t PointScale;		/* How to scale points from DF to D3D coordinates */

  CD3DMeshBuilder*  pD3DBuilder;	/* The builder object for this 3DdObject */


  /*---------- Begin Protected Class Methods -----------------------*/
protected:

	/* Helper functions to make D3D builder object */
  d3dvector_t* MakeVectors     (void);
  d3dvector_t* MakeNormals     (void);
  DWORD*       MakeBuildData   (void);
  boolean      MakeTextures    (void);
  boolean      MakeMeshBuilder (void);


  /*---------- Begin Public Class Methods --------------------------*/
public:

	/* Class constructor/destructor */
  CDF3DObjectD3D ();
  ~CDF3DObjectD3D() { Destroy(); }
  void Destroy (void);
  
    	/* Quick conversion functions */
  inline d3dvector_t PointToVector       (const dfarch_point_t& Point) { return (D3DVECTOR(D3DVAL(Point.X), D3DVAL(Point.Y), D3DVAL(Point.Z))); }
  inline d3dvector_t PointToScaledVector (const dfarch_point_t& Point) { return (D3DVECTOR(D3DVAL(Point.X), D3DVAL(Point.Y), D3DVAL(Point.Z))/PointScale); }
  inline d3dvector_t PointToNormal       (const dfarch_point_t& Point) { return Normalize(D3DVECTOR(D3DVAL(Point.X), D3DVAL(Point.Y), D3DVAL(Point.Z))); }

  	/* Get/Make a mesh builder from the object */
  CD3DMeshBuilder* GetD3DBuilder (void);
  
	/* Class get methods */
  static boolean    GetLoadObjectNormals (void) { return (LoadObjectNormals); }
  static d3dvalue_t GetPointScale        (void) { return (PointScale); }

  	/* Class set methods */
  static void SetLoadObjectNormals (const boolean  Value) { LoadObjectNormals = Value; }
  static void SetPointScale        (const D3DVALUE Scale) { PointScale = Scale; }

 };
/*===========================================================================
 *		End of Class CDF3DObjectD3D Definition
 *=========================================================================*/


/*=========================================================================
 *
 * Class CDFArch3D Definition
 *
 * Contains methods needed to manipulate Daggerfall's ARCH3D.BSA file.
 * Based on the CDFBSAFile class.
 *
 *=======================================================================*/
class CDFArchD3D : public CDFArch3D {

  /*---------- Begin Protected Class Members ---------------------*/
protected:


  /*---------- Begin Protected Class Methods ---------------------*/
protected:

	/* Creates a 3D Object pointer */
  void Create3DObject (const int RecordIndex);

  
  /*---------- Begin Public Class Methods ------------------------*/
public:

	/* Class Constructor/Destructor */
  //CDFArchD3D();
  //~CDFArchD3D() { Destroy(); }
  //void Destroy (void);

	/* Get/load a D3DObject */
  CDF3DObjectD3D* GetD3DObject (const long ValueID)     { return (CDF3DObjectD3D*) Get3DObject(ValueID); } 
  CDF3DObjectD3D* GetD3DObject (const int  RecordIndex) { return (CDF3DObjectD3D*) Get3DObject(RecordIndex); }

  CD3DMeshBuilder* GetD3DBuilder (const long ValueID);
  CD3DMeshBuilder* GetD3DBuilder (const int  Index);

 };
/*=========================================================================
 *		End of Class CDFArchD3D Definition
 *=======================================================================*/


#endif
/*===========================================================================
 *		End of File ARCH_D3D.H
 *=========================================================================*/
