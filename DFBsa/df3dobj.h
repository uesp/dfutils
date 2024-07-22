/*=========================================================================
 *
 * DF3DOBJ.H - Dave Humphrey (uesp@m0use.net), 30 October 2000
 *
 *=======================================================================*/
#ifndef __DF3DOBJ_H
#define __DF3DOBJ_H


	/* Required Includes */
#include "genutil.h"


/*=========================================================================
 *
 * Begin Defines
 *
 *=======================================================================*/

	/* Maximum allowed number of points/planes.  These values
	 * are code specific, not BSA imposed limits. */
  #define MAX_DFARCH_PLANES 1000
  #define MAX_DFARCH_POINTS 2000
  #define MAX_DFARCH_PLANEPOINTS 40
  #define MAX_DFARCH_DATA2  300

	/* Standard record sizes */
  #define DFARCH_POINT_RECORDSIZE    12
  #define DFARCH_NORMAL_RECORDSIZE   12
  #define DFARCH_DATA1_RECORDSIZE    24
  #define DFARCH_HEADER_RECORDSIZE   64
  #define DFARCH_DATA2_SUBRECORDSIZE 6

/*=========================================================================
 *		End of Defines
 *=======================================================================*/


/*=========================================================================
 *
 * Begin Type and Structure Definitions
 *
 *=======================================================================*/

	/* To ensure proper size of structures */
#ifdef _WIN32
  #pragma pack(push, save_pack)
  #pragma pack(1)
#endif

	/* Basic point type */
  typedef struct {
    long X, Y, Z;
   } dfarch_point_t;

	/* Plane sub-record type */
  typedef struct {
    long PointOffset;
    long Unknown;
   } dfarch_plane_points_t;

	/* Basic plane type */
  typedef struct {
    byte   PointCount;
    byte   Unknown1;
    ushort SubImageIndex	: 7;
    ushort TextureIndex		: 9;
    long   Unknown2;
    dfarch_plane_points_t SubRecords[MAX_DFARCH_PLANEPOINTS];
   } dfarch_plane_t;

	/* Data1 record type */
  typedef struct {
    unsigned char Unknown[24];
   } dfarch_data1_t;

	/* Data2 record subtype */
  typedef struct {
    char Unknown[6];
   } dfarch_data2_subrecord_t;

	/* Data2 record type */
  typedef struct {
    long  Unknown[4];
    short NumSubRecords;
    dfarch_data2_subrecord_t SubRecords[MAX_DFARCH_DATA2];
   } dfarch_data2_t;


#ifdef _WIN32
  #pragma pack(pop, save_pack)
#endif

/*=========================================================================
 *		End of Type and Structure Definitions
 *=======================================================================*/


/*=========================================================================
 *
 * Structure DF3DObjectHeader_t Definition
 *
 * The header information of a 3D object in ARCH3D. 64 bytes total.
 *
 *=======================================================================*/
typedef struct {
  char  Version[4];	/* Object version string */
  long  NumPoints;	/* Number of points in object */
  long  NumPlanes;	/* Number of planes in object */
  long  Unknown1;
  long  NullValue1;
  long  NullValue2;
  long  Data1Offset;
  long  Data2Offset;
  short NumData2Records;
  short NullValue3;
  short Unknown3;
  short Unknown4;
  long  NullValue4;
  long  NullValue5;
  long  PointOffset;
  long  NormalOffset;
  long  Unknown6;
  long  PlaneOffset;
 } dfarch_header_t;
/*=========================================================================
 *		End of struct DF3DObjectHeader_t Definition
 *=======================================================================*/


/*=========================================================================
 *
 * Class CDF3DObject Definition
 *
 *=======================================================================*/
class CDF3DObject {

  /*---------- Begin Protected Class Members ---------------------*/
protected:

  dfarch_header_t    Header;	/* Object header information */

	/* Point data */
  dfarch_point_t*     Points;

	/* Plane data */
  dfarch_plane_t**     pPlanes;

	/* Normals? data */
  dfarch_point_t*      Normals;

	/* Data1 section data (unknown) */
  dfarch_data1_t**     pData1;

	/* Data2 section data (unknown) */
  dfarch_data2_t**     pData2;

	/* Used to keep track of allocate arrays */
  int NumPlanesAllocated;
  int NumData1Allocated;
  int NumData2Allocated;

	/* Used to help input/output objects in a BSA file */
  long BSARecordOffset;
  long BSARecordSize;
  long BSARecordID;
  int  BSARecordIndex;


  /*---------- Begin Protected Class Methods ---------------------*/
protected:


	/* Helper functions for reading in the object data */
  boolean ReadData1   (FILE* pFileHandle);
  boolean ReadData2   (FILE* pFileHandle);
  boolean ReadHeader  (FILE* pFileHandle);
  boolean ReadNormals (FILE* pFileHandle);
  boolean ReadPlanes  (FILE* pFileHandle);
  boolean ReadPoints  (FILE* pFileHandle);

	/* Helper functions for outputting object data */
  boolean WriteData1   (FILE* pFileHandle);
  boolean WriteData2   (FILE* pFileHandle);
  boolean WriteHeader  (FILE* pFileHandle);
  boolean WriteNormals (FILE* pFileHandle);
  boolean WritePlanes  (FILE* pFileHandle);
  boolean WritePoints  (FILE* pFileHandle);


  /*---------- Begin Public Class Methods ------------------------*/
public:

	/* Class Constructor/Destructor */
  CDF3DObject();
  virtual ~CDF3DObject();
  virtual void Destroy (void);

	/* Count the number of points defining the planes in all planes */
  long CountPlanePoints (void);

	/* Outputs object information to a file stream */
  void Dump	  (FILE* pFileHandle);
  void DumpHeader (FILE* pFileHandle);

	/* Data member access functions */
  long GetNumPoints 	  (void) { return (Header.NumPoints); }
  long GetNumPlanes 	  (void) { return (Header.NumPlanes); }
  long GetNumData2Records (void) { return (Header.NumData2Records); }
  long GetPointOffset 	  (void) { return (Header.PointOffset); }
  long GetPlaneOffset 	  (void) { return (Header.PlaneOffset); }
  long GetNormalOffset    (void) { return (Header.NormalOffset); }
  long GetData1Offset     (void) { return (Header.Data1Offset); }
  long GetData2Offset     (void) { return (Header.Data2Offset); }
  long GetBSARecordOffset (void) { return (BSARecordOffset); }
  long GetBSARecordSize   (void) { return (BSARecordOffset); }
  long GetBSARecordID     (void) { return (BSARecordID); }
  int  GetBSARecordIndex  (void) { return (BSARecordIndex); }
  long GetNumPlanesAllocated (void) { return (NumPlanesAllocated); }

	/* Get data pointers */
  dfarch_point_t* GetPoints  (void) { return (Points); }
  dfarch_point_t* GetNormals (void) { return (Normals); }
  dfarch_plane_t* GetPlane   (const int PlaneIndex) { if (PlaneIndex >= 0 && PlaneIndex < NumPlanesAllocated) return (pPlanes[PlaneIndex]); return (NULL); }

	/* Fills a string with plane information */
  boolean GetPlaneInfo (const int Index, char* pBuffer, const int MaxLength);

	/* Returns the size of the 3D Object record in bytes */
  long GetRecordSize (void);

	/* Check indices */ 
  boolean IsValidPointIndex  (const int Index) { return ((Index >= 0 && Index < Header.NumPoints ) ? TRUE : FALSE); }
  boolean IsValidNormalIndex (const int Index) { return ((Index >= 0 && Index < Header.NumPlanes ) ? TRUE : FALSE); }
  boolean IsValidPlaneIndex  (const int Index) { return ((Index >= 0 && Index < NumPlanesAllocated ) ? TRUE : FALSE); }
  boolean IsValidData1Index  (const int Index) { return ((Index >= 0 && Index < NumData1Allocated ) ? TRUE : FALSE); }
  boolean IsValidData2Index  (const int Index) { return ((Index >= 0 && Index < NumData2Allocated ) ? TRUE : FALSE); }

	/* Compares version numbers */
  boolean IsVersion (const char* pString) { if (pString == NULL) return (FALSE); if (strnicmp(Header.Version, pString, 4) == 0) return (TRUE); return (FALSE); }

	/* Reads in a record from the current position in the file */
  boolean Read 	     (FILE* pFileHandle);

	/* Data member set functions */
  void SetNumPoints       (const long  Count) { Header.NumPoints = Count; }
  void SetNumPlanes       (const long  Count) { Header.NumPlanes = Count; }
  void SetNumData2Records (const short Count) { Header.NumData2Records = Count; }
  void SetVersion         (const char* pString = "v2.7");
  void SetBSARecordOffset (const long  Offset) { BSARecordOffset = Offset; }
  void SetBSARecordSize   (const long  Size)   { BSARecordSize   = Size; }
  void SetBSARecordID     (const long  ID)     { BSARecordID     = ID; }
  void SetBSARecordIndex  (const int   Index)  { BSARecordIndex  = Index; }

	/* Writes a record to the current position in the file */
  boolean Write (FILE* pFileHandle);

 };
/*=========================================================================
 *		End of Class  CDF3DObject Definition
 *=======================================================================*/


#endif
/*=========================================================================
 *		End of File DF3DObj.H
 *=======================================================================*/

