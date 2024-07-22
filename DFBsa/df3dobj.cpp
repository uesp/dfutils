/*=========================================================================
 *
 * DF3DOBJ.CPP - Dave Humphrey (uesp@m0use.net), 30 October 2000
 *
 *=======================================================================*/


	/* Include Files */
#include "df3dobj.h"


#undef  __FUNC__
#define __FUNC__ "CDF3DObject::CDF3DObject()"
/*=========================================================================
 *
 * Class CDF3DObject Constructor
 *
 *=======================================================================*/
CDF3DObject::CDF3DObject() {
  SetVersion("v2.7");
  Header.NumPoints = 0;
  Header.NumPlanes = 0;
  Header.NumData2Records = 0;
  Header.NullValue1 = 0;
  Header.NullValue2 = 0;
  Header.NullValue3 = 0;
  Header.NullValue4 = 0;
  Header.NullValue5 = 0;
  NumPlanesAllocated = 0;
  NumData1Allocated = 0;
  NumData2Allocated = 0;

	/* Allocate the required arrays */
  CreatePointerArray(Points,  dfarch_point_t,	MAX_DFARCH_POINTS);
  CreatePointerArray(pPlanes, dfarch_plane_t*,  MAX_DFARCH_PLANES);
  CreatePointerArray(Normals, dfarch_point_t,   MAX_DFARCH_PLANES);
  CreatePointerArray(pData1,  dfarch_data1_t*,  MAX_DFARCH_PLANES);
  CreatePointerArray(pData2,  dfarch_data2_t*,  MAX_DFARCH_DATA2);
 }
/*=========================================================================
 *		End of Class CDF3DObject Constructor
 *=======================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDF3DObject::~CDF3DObject()"
/*=========================================================================
 *
 * Class CDF3DObject Destructor
 *
 *=======================================================================*/
CDF3DObject::~CDF3DObject () { 
  Destroy();

	/* Unallocate the arrays */
  delete[] Points;
  delete[] pPlanes;
  delete[] Normals;
  delete[] pData1;
  delete[] pData2;
 }
/*=========================================================================
 *		End of Class CDF3DObject Destructor
 *=======================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDF3DObject::Destroy()"
/*=========================================================================
 *
 * Class CDF3DObject Destructor
 *
 *=======================================================================*/
void CDF3DObject::Destroy (void) {
  int LoopCounter;

	/* Delete any allocated structures */
  for (LoopCounter = 0; LoopCounter < NumPlanesAllocated; LoopCounter++) {
    DestroyPointer(pPlanes[LoopCounter]);
   }

  for (LoopCounter = 0; LoopCounter < NumData1Allocated; LoopCounter++) {
    DestroyPointer(pData1[LoopCounter]);
   }

  for (LoopCounter = 0; LoopCounter < NumData2Allocated; LoopCounter++) {
    DestroyPointer(pData2[LoopCounter]);
   }

	/* Reset record counters */
  Header.NumPoints = 0;
  Header.NumPlanes = 0;
  Header.NumData2Records = 0;
  NumPlanesAllocated = 0;
  NumData1Allocated = 0;
  NumData2Allocated = 0;

	/* Reset version number */
  SetVersion("v2.7");
 }
/*=========================================================================
 *		End of Class CDF3DObject Destructor
 *=======================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDF3DObject::CountPlanePoints()"
/*===========================================================================
 *
 * Class CDF3DObject Method - long CountPlanePoints (void);
 *
 * Counts all the points defining all the allocated plane objects.
 *
 *=========================================================================*/
long CDF3DObject::CountPlanePoints (void) {
  int		  LoopCounter;
  long		  Counter = 0;

  for (LoopCounter = 0; LoopCounter < NumPlanesAllocated; LoopCounter++) { 
    Counter += pPlanes[LoopCounter]->PointCount;
   }

  return (Counter);
 }
/*===========================================================================
 *		End of Class Method CDF3DObject::CountPlanePoints()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDF3DObject::Dump()"
/*===========================================================================
 *
 * Class CDF3DObject Method - void Dump (pFileHandle);
 *
 * Outputs debug information about the object to the given file stream.
 *
 *=========================================================================*/
void CDF3DObject::Dump (FILE* pFileHandle) {
  
	/* Ignore any invalid handles */
  if (pFileHandle == NULL) return;

	/* Dump basic info */
  fprintf (pFileHandle, "CDF3DObject Object (0x%08p)\n", this);
  fprintf (pFileHandle, "\tBSARecordIndex  = %d (0x%04X)\n", BSARecordIndex, BSARecordIndex);
  fprintf (pFileHandle, "\tBSARecordOffset = 0x%08lX\n", BSARecordOffset);
  fprintf (pFileHandle, "\tBSARecordSize   = 0x%08lX\n", BSARecordSize);
  fprintf (pFileHandle, "\tBSARecordID     = 0x%08lX\n", BSARecordID);
  DumpHeader(pFileHandle);
 }
/*===========================================================================
 *		End of Class Method CDF3DObject::Dump()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDF3DObject::DumpHeader()"
/*===========================================================================
 *
 * Class CDF3DObject Method - void DumpHeader (pFileHandle);
 *
 * Outputs debug information about the object to the given file stream.
  *
 *=========================================================================*/
void CDF3DObject::DumpHeader (FILE* pFileHandle) {
  
	/* Ignore any invalid handles */
  if (pFileHandle == NULL) return;

  fprintf (pFileHandle, "\tCDF3DObject Header (0x%08p)\n", &Header);
  fprintf (pFileHandle, "\t\tVersion = '%04s'\n", Header.Version);
  fprintf (pFileHandle, "\t\tNumPoints = %ld\n", Header.NumPoints);
  fprintf (pFileHandle, "\t\tNumPlanes = %ld\n", Header.NumPlanes);
  fprintf (pFileHandle, "\t\tUnknown1 = 0x%08lX\n", Header.Unknown1);
  fprintf (pFileHandle, "\t\tNullValue1 = 0x%08lX\n", Header.NullValue1);
  fprintf (pFileHandle, "\t\tNullValue2 = 0x%08lX\n", Header.NullValue2);
  fprintf (pFileHandle, "\t\tData1Offset = 0x%08lX\n", Header.Data1Offset);
  fprintf (pFileHandle, "\t\tData2Offset = 0x%08lX\n", Header.Data2Offset);
  fprintf (pFileHandle, "\t\tNumData1Records = %d\n", Header.NumData2Records);
  fprintf (pFileHandle, "\t\tNullValue3 = 0x%04X\n", Header.NullValue3);
  fprintf (pFileHandle, "\t\tUnknown3 = 0x%04X\n", Header.Unknown3);  
  fprintf (pFileHandle, "\t\tUnknown4 = 0x%04X\n", Header.Unknown4);
  fprintf (pFileHandle, "\t\tNullValue4 = 0x%08lX\n", Header.NullValue4);
  fprintf (pFileHandle, "\t\tNullValue5 = 0x%08lX\n", Header.NullValue5);
  fprintf (pFileHandle, "\t\tPointOffset = 0x%08lX\n", Header.PointOffset);
  fprintf (pFileHandle, "\t\tNormalOffset = 0x%08lX\n", Header.NormalOffset);
  fprintf (pFileHandle, "\t\tUnknown6 = 0x%08lX\n", Header.Unknown6);
  fprintf (pFileHandle, "\t\tPlaneOffset = 0x%08lX\n", Header.PlaneOffset);
  fprintf (pFileHandle, "\tEnd of Header\n");

 }
/*===========================================================================
 *		End of Class Method CDF3DObject::Dump()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDF3DObject::GetPlaneInfo()"
/*===========================================================================
 *
 * Class CDF3DObject Method - boolean GetPlaneInfo (Index, pBuffer, MaxLength);
 *
 * Fills a string buffer with information about the given plane.  Returns
 * FALSE on any error.
 *
 *=========================================================================*/
boolean CDF3DObject::GetPlaneInfo (const int Index, char* pBuffer, const int MaxLength) {
  char TempBuffer[256];
  int  BufferLength;
  int  Length;
  int  LoopCounter;

	/* Ensure valid input */ 
  if (pBuffer == NULL || MaxLength <= 0) {
    SET_EXT_ERROR(ERR_INPUT);
    return (FALSE);
   }

	/* Ensure a valid index */
  if (!IsValidPlaneIndex(Index)) {
    SET_EXT_ERROR3(ERR_INDEX, "Invalid face index %d!", Index);
    sprintf (pBuffer, "Invalid plane index %d!", Index);
    return (FALSE);
   }

	/* Start output information */
  BufferLength = sprintf (pBuffer, "Plane %d\n\r", Index);

  Length = sprintf (TempBuffer, "\tPointCount = %d\n\r", (int)pPlanes[Index]->PointCount);
  if (BufferLength + Length >= MaxLength) return (FALSE);
  strcat (pBuffer, TempBuffer);
  BufferLength += Length;

  Length = sprintf (TempBuffer, "\tTexture = %d (%d)\n\r", pPlanes[Index]->TextureIndex, pPlanes[Index]->SubImageIndex);
  if (BufferLength + Length >= MaxLength) return (FALSE);
  strcat (pBuffer, TempBuffer);
  BufferLength += Length;

  Length = sprintf (TempBuffer, "\tUnknowns = 0x%02X 0x%08lX\n\r", 
		   (unsigned int)pPlanes[Index]->Unknown1&0xff, pPlanes[Index]->Unknown2);
  if (BufferLength + Length >= MaxLength) return (FALSE);
  strcat (pBuffer, TempBuffer);
  BufferLength += Length;

  for (LoopCounter = 0; LoopCounter < (int)pPlanes[Index]->PointCount; LoopCounter++) {
    Length = sprintf (TempBuffer, "\tPointRecord %d: PointOffset %ld,  Unknown = 0x%08lX\n\r", 
		      LoopCounter, pPlanes[Index]->SubRecords[LoopCounter].PointOffset, 
				   pPlanes[Index]->SubRecords[LoopCounter].Unknown);

		/* Ensure the maximum string length has not been exceeded */
    if (BufferLength + Length >= MaxLength) {
      SET_EXT_ERROR3(ERR_OVERFLOW, "Maximum buffer string length of %d was exceeded!", MaxLength);
      return (FALSE);
     }

    strcat (pBuffer, TempBuffer);
    BufferLength += Length;
   }
  
  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDF3DObject::GetPlaneInfo()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDF3DObject::GetRecordSize()"
/*===========================================================================
 *
 * Class CDF3DObject Method - long GetRecordSize (void);
 *
 * Computes and returns the size of the 3D Object record in bytes. If the
 * record is not completely loaded or defined, an improper size may be
 * returned.
 *
 *=========================================================================*/
long CDF3DObject::GetRecordSize (void) {
  long RecordSize;
  int  LoopCounter;

	/* Add header and point array record sizes */
  RecordSize = sizeof(dfarch_header_t);
  RecordSize += GetNumPoints() * sizeof(dfarch_point_t);

	/* Add record size of any allocated planes */
  for (LoopCounter = 0; LoopCounter < NumPlanesAllocated; LoopCounter++)
    RecordSize += 8 + 8*pPlanes[LoopCounter]->PointCount;
   
	/* And normal and data1 record sizes */
  RecordSize += GetNumPlanes() * sizeof(dfarch_point_t);
  RecordSize += GetNumPlanes() * sizeof(dfarch_data1_t);

	/* Add data2 record size for allocated indices */
  for (LoopCounter = 0; LoopCounter < NumData2Allocated; LoopCounter++)
    RecordSize += 18 + 6*pData2[LoopCounter]->NumSubRecords;

  return (RecordSize);
 }
/*===========================================================================
 *		End of Class Method CDF3DObject::GetRecordSize()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDF3DObject::Read()"
/*=========================================================================
 *
 * Class CDF3DObject Method - boolean Read (pFileHandle);
 *
 * Attempts to read in a 3D object from the current position in the
 * given file stream.  Returns FALSE on any error.
 *
 *=======================================================================*/
boolean CDF3DObject::Read (FILE* pFileHandle) {
  boolean Result;
  long    RecordOffset;

	/* Ensure valid input */
  if (pFileHandle == NULL) {
    SET_EXT_ERROR2(ERR_NULL, "Received a NULL filehandle!");
    return (FALSE);
   }

	/* Delete the current contents if any */
  Destroy();

	/* Save the record offset */
  RecordOffset = ftell(pFileHandle);

  if (RecordOffset < 0) {
    SET_EXT_ERROR3(ERR_READ, "Error reading record offset (%ld)!", RecordOffset);
    return (FALSE);
   }

	/* Read the header information */
  Result = ReadHeader(pFileHandle);
  if (!Result) return (FALSE);

	/* Jump to the point data and read points */
  fseek (pFileHandle, RecordOffset + GetPointOffset(), SEEK_SET);
  Result = ReadPoints (pFileHandle);
  if (!Result) return (FALSE);

	/* Jump to the plane data and read planes */
  fseek (pFileHandle, RecordOffset + GetPlaneOffset(), SEEK_SET);
  Result = ReadPlanes (pFileHandle);
  if (!Result) return (FALSE);

  	/* Jump to the normal data and read normals */
  fseek (pFileHandle, RecordOffset + GetNormalOffset(), SEEK_SET);
  Result = ReadNormals (pFileHandle);
  if (!Result) return (FALSE);

	/* Jump to the data1 section and read it */
  fseek (pFileHandle, RecordOffset + GetData1Offset(), SEEK_SET);
  Result = ReadData1 (pFileHandle);
  if (!Result) return (FALSE);

	/* Jump to the data2 section and read it */
  fseek (pFileHandle, RecordOffset + GetData2Offset(), SEEK_SET);
  Result = ReadData2 (pFileHandle);
  if (!Result) return (FALSE);

  return (TRUE);
 }
/*=========================================================================
 *		End of Class Method CDF3DObject::Read()
 *=======================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDF3DObject::ReadData1()"
/*=========================================================================
 *
 * Class CDF3DObject Method - boolean ReadData1 (pFileHandle);
 *
 * Attempts to read in the 3D object data1 information from the current
 * position in the given file stream.  Returns FALSE on any error.
 * Protected class method.
 *
 *=======================================================================*/
boolean CDF3DObject::ReadData1 (FILE* pFileHandle) {
  int Result;
  int LoopCounter;

	/* Must read records in one at a time */
  for (LoopCounter = 0; LoopCounter < GetNumPlanes(); LoopCounter++) {

		/* Allocate the plane information */
    CreatePointer(pData1[LoopCounter], dfarch_data1_t);
    NumData1Allocated++;

		/* Read in the data all at once */
    Result = fread (pData1[LoopCounter], 1, DFARCH_DATA1_RECORDSIZE, pFileHandle);

		/* Ensure the data was successfully read */
    if (Result != DFARCH_DATA1_RECORDSIZE) {
      SET_EXT_ERROR4(ERR_READ, "Failed to read all the Data1 section information (%d of %d bytes)!", Result, DFARCH_DATA1_RECORDSIZE);
      return (FALSE);
     }
   }

  return (TRUE);
 }
/*=========================================================================
 *		End of Class Method CDF3DObject::ReadData1()
 *=======================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDF3DObject::ReadData2()"
/*=========================================================================
 *
 * Class CDF3DObject Method - boolean ReadData2 (pFileHandle);
 *
 * Attempts to read in the 3D object data1 information from the current
 * position in the given file stream.  Returns FALSE on any error.
 * Protected class method.
 *
 *=======================================================================*/
boolean CDF3DObject::ReadData2 (FILE* pFileHandle) {
  int Result;
  int LoopCounter;
  int Data2Size;

	/* Must read records in one at a time */
  for (LoopCounter = 0; LoopCounter < GetNumData2Records(); LoopCounter++) {

		/* Allocate the plane information */
    CreatePointer(pData2[LoopCounter], dfarch_data2_t);
    NumData2Allocated++;

		/* Read in the first 18 bytes */
    Result = fread (pData2[LoopCounter], 1, 18, pFileHandle);

		/* Ensure the data was successfully read */
    if (Result != 18) {
      SET_EXT_ERROR2(ERR_READ, "Failed to read the first 18 bytes of the Data2 section!");
      return (FALSE);
     }

		/* Ensure a valid read */
    if (pData2[LoopCounter]->NumSubRecords < 0 || pData2[LoopCounter]->NumSubRecords >= MAX_DFARCH_DATA2) {
      SET_EXT_ERROR4(ERR_READ, "Invalid number of Data2 subrecords read!\n(%d, Offset  = 0x%08lX)", pData2[LoopCounter]->NumSubRecords, ftell(pFileHandle)-2);
      //Dump(SystemLog.GetFileHandle());
      return (FALSE);
     }

		/* Compute the size of the sub-record data */
    Data2Size = DFARCH_DATA2_SUBRECORDSIZE * pData2[LoopCounter]->NumSubRecords;

		/* Read in the sub-record data all at once */
    Result = fread (pData2[LoopCounter]->SubRecords, 1, Data2Size, pFileHandle);

		/* Ensure the data was successfully read */
    if (Result != Data2Size) {
      SET_EXT_ERROR4(ERR_READ, "Failed to read all the Data2 subrecords (%d of %d bytes)!", Result, Data2Size);
      return (FALSE);
     }
   }

  return (TRUE);
 }
/*=========================================================================
 *		End of Class Method CDF3DObject::ReadData1()
 *=======================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDF3DObject::ReadHeader()"
/*=========================================================================
 *
 * Class CDF3DObject Method - boolean ReadHeader (pFileHandle);
 *
 * Attempts to read in the 3D object header information from the current
 * position in the given file stream.  Returns FALSE on any error.
 * Protected class method.
 *
 *=======================================================================*/
boolean CDF3DObject::ReadHeader (FILE* pFileHandle) {
  int Result;

	/* Attempt to read the header in at once */
  Result = fread (&Header, 1, DFARCH_HEADER_RECORDSIZE, pFileHandle);

	/* Ensure the read was successful */
  if (Result != DFARCH_HEADER_RECORDSIZE) {
    SET_EXT_ERROR4(ERR_READ, "Failed to read the header information (%d of %d bytes)!", Result, DFARCH_HEADER_RECORDSIZE);
    return (FALSE);
   }

  return (TRUE);
 }
/*=========================================================================
 *		End of Class Method CDF3DObject::ReadHeader()
 *=======================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDF3DObject::ReadNormals()"
/*=========================================================================
 *
 * Class CDF3DObject Method - boolean ReadNormals (pFileHandle);
 *
 * Attempts to read in the 3D object normals information from the current
 * position in the given file stream.  Returns FALSE on any error.
 * Protected class method.
 *
 *=======================================================================*/
boolean CDF3DObject::ReadNormals (FILE* pFileHandle) {
  int Result;

	/* Read all the points at once */
  Result = fread (Normals, DFARCH_NORMAL_RECORDSIZE, (int)GetNumPlanes(), pFileHandle);

	/* Ensure the read was successful */
  if (Result != GetNumPlanes()) {
    SET_EXT_ERROR4(ERR_READ, "Failed to read the normals data (%d of %d normals)!", Result, GetNumPlanes());
    return (FALSE);
   }

  return (TRUE);
 }
/*=========================================================================
 *		End of Class Method CDF3DObject::ReadNormals()
 *=======================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDF3DObject::ReadPlanes()"
/*=========================================================================
 *
 * Class CDF3DObject Method - boolean ReadPlanes (pFileHandle);
 *
 * Attempts to read in the 3D object plane information from the current
 * position in the given file stream.  Returns FALSE on any error.
 * Protected class method.
 *
 *=======================================================================*/
boolean CDF3DObject::ReadPlanes (FILE* pFileHandle) {
  int Result;
  int LoopCounter;
  int PlaneSize;
  int NumPlanePoints;

	/* Must read planes in one at a time */
  for (LoopCounter = 0; LoopCounter < GetNumPlanes(); LoopCounter++) {

		/* Allocate the plane information */
    CreatePointer(pPlanes[LoopCounter], dfarch_plane_t);
    NumPlanesAllocated++;

		/* Read in the number of points in the plane */
    NumPlanePoints = fgetc(pFileHandle);

    if (NumPlanePoints < 0) {
      SET_EXT_ERROR3(ERR_READ, "Invalid number of face-points received, %d!", NumPlanePoints);
      return (FALSE);
     }

		/* Move file position back one */
    fseek (pFileHandle, -1, SEEK_CUR);

		/* Compute the plane size */
    PlaneSize = 8 + NumPlanePoints*8;

		/* Read in the plane data all at once */
    Result = fread (pPlanes[LoopCounter], 1, PlaneSize, pFileHandle);

		/* Ensure the plane was successfully read */
    if (Result != PlaneSize) {
      SET_EXT_ERROR4(ERR_READ, "Failed to read the face data (%d of %d bytes)!", Result, PlaneSize);
      return (FALSE);
     }
   }

  return (TRUE);
 }
/*=========================================================================
 *		End of Class Method CDF3DObject::ReadPlanes()
 *=======================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDF3DObject::ReadPoints()"
/*=========================================================================
 *
 * Class CDF3DObject Method - boolean ReadPoints (pFileHandle);
 *
 * Attempts to read in the 3D object point information from the current
 * position in the given file stream.  Returns FALSE on any error.
 * Protected class method.
 *
 *=======================================================================*/
boolean CDF3DObject::ReadPoints (FILE* pFileHandle) {
  int Result;

	/* Read all the points at once */
  Result = fread (Points, DFARCH_POINT_RECORDSIZE, (int)GetNumPoints(), pFileHandle);

	/* Ensure the read was successful */
  if (Result != GetNumPoints()) {
    SET_EXT_ERROR4(ERR_READ, "Failed to read all the points data (%d of %d points)!", Result, GetNumPoints());
    return (FALSE);
   }

  return (TRUE);
 }
/*=========================================================================
 *		End of Class Method CDF3DObject::ReadPoints()
 *=======================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDF3DObject::SetVersion()"
/*=========================================================================
 *
 * Class CDF3DObject Method - void SetVersion (pString);
 *
 * Sets the version code of the 3DObject (default is 'v2.7'). Accepts
 * up to the first four characters from the input string.
 *
 *=======================================================================*/
void CDF3DObject::SetVersion (const char* pString) {
  int Count = 0;

	/* Ignore any invalid input */
  if (pString == NULL) return;

	/* Copy up to first 4 characters or NULL is reached */
  while (pString[Count] != NULL_CHAR && Count < 4) {
    Header.Version[Count] = pString[Count];
    Count++;
   }

	/* Clear remaining characters */
  while (Count < 4) {
    Header.Version[Count] = NULL_CHAR;
    Count++;
   }
 }
/*=========================================================================
 *		End of Class Method CDF3DObject::SetVersion();
 *=======================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDF3DObject::Write()"
/*=========================================================================
 *
 * Class CDF3DObject Method - boolean Write (pFileHandle);
 *
 * Attempts to write the 3D object to the current position in the
 * given file stream.  Returns FALSE on any error.  Only outputs 
 * any allocated data so file will not be complete if not all
 * data has been allocated and previously read or assigned.
 *
 *=======================================================================*/
boolean CDF3DObject::Write (FILE* pFileHandle) {
  boolean Result;
  long    RecordOffset;

	/* Ensure valid input */
  if (pFileHandle == NULL) {
    SET_EXT_ERROR2(ERR_NULL, "Invalid NULL file handle received!");
    return (FALSE);
   }
	
	/* Save the starting record offset */
  RecordOffset = ftell(pFileHandle);

  if (RecordOffset < 0) {
    SET_EXT_ERROR3(ERR_READ, "Invalid starting offset received (%ld)!", RecordOffset);
    return (FALSE);
   }

	/* Write the header information */
  Result = WriteHeader(pFileHandle);
  if (!Result) return (FALSE);

	/* Jump to the point data and write points */
  fseek (pFileHandle, RecordOffset + GetPointOffset(), SEEK_SET);
  Result = WritePoints (pFileHandle);
  if (!Result) return (FALSE);

	/* Jump to the plane data and write planes */
  fseek (pFileHandle, RecordOffset + GetPlaneOffset(), SEEK_SET);
  Result = WritePlanes (pFileHandle);
  if (!Result) return (FALSE);

  	/* Jump to the normal data and write normals */
  fseek (pFileHandle, RecordOffset + GetNormalOffset(), SEEK_SET);
  Result = WriteNormals (pFileHandle);
  if (!Result) return (FALSE);

	/* Jump to the data1 section and write it */
  fseek (pFileHandle, RecordOffset + GetData1Offset(), SEEK_SET);
  Result = WriteData1 (pFileHandle);
  if (!Result) return (FALSE);

	/* Jump to the data2 section and write it */
  fseek (pFileHandle, RecordOffset + GetData2Offset(), SEEK_SET);
  Result = WriteData2 (pFileHandle);
  if (!Result) return (FALSE);

  return (TRUE);
 }
/*=========================================================================
 *		End of Class Method CDF3DObject::Write()
 *=======================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDF3DObject::WriteData1()"
/*=========================================================================
 *
 * Class CDF3DObject Method - boolean WriteData1 (pFileHandle);
 *
 * Attempts to write the 3D object data1 information to the current
 * position in the given file stream.  Returns FALSE on any error.
 * Protected class method.
 *
 *=======================================================================*/
boolean CDF3DObject::WriteData1 (FILE* pFileHandle) {
  int Result;
  int LoopCounter;

	/* Must write records in one at a time */
  for (LoopCounter = 0; LoopCounter < GetNumPlanes(); LoopCounter++) {

		/* Write the data all at once */
    Result = fwrite (pData1[LoopCounter], 1, DFARCH_DATA1_RECORDSIZE, pFileHandle);

		/* Ensure the data was successfully written */
    if (Result != DFARCH_DATA1_RECORDSIZE) {
      SET_EXT_ERROR4(ERR_WRITE, "Failed to write Data1 section information (%d of %d bytes)!", Result, DFARCH_DATA1_RECORDSIZE);
      return (FALSE);
     }
   }

  return (TRUE);
 }
/*=========================================================================
 *		End of Class Method CDF3DObject::WriteData1()
 *=======================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDF3DObject::WriteData2()"
/*=========================================================================
 *
 * Class CDF3DObject Method - boolean WriteData2 (pFileHandle);
 *
 * Attempts to write the 3D object data1 information to the current
 * position in the given file stream.  Returns FALSE on any error.
 * Protected class method.
 *
 *=======================================================================*/
boolean CDF3DObject::WriteData2 (FILE* pFileHandle) {
  int Result;
  int LoopCounter;
  int Data2Size;

	/* Must write records in one at a time */
  for (LoopCounter = 0; LoopCounter < NumData2Allocated; LoopCounter++) {

		/* Compute the size of the record data */
    Data2Size = 18 + DFARCH_DATA2_SUBRECORDSIZE * pData2[LoopCounter]->NumSubRecords;

		/* Write the data2 record data all at once */
    Result = fwrite (pData2[LoopCounter], 1, Data2Size, pFileHandle);

		/* Ensure the data was successfully read */
    if (Result != Data2Size) {
      SET_EXT_ERROR4(ERR_WRITE, "Failed to write Data2 section information (%d of %d bytes)!", Result, Data2Size);
      return (FALSE);
     }
   }

  return (TRUE);
 }
/*=========================================================================
 *		End of Class Method CDF3DObject::WriteData2()
 *=======================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDF3DObject::WriteHeader()"
/*=========================================================================
 *
 * Class CDF3DObject Method - boolean WriteHeader (pFileHandle);
 *
 * Attempts to write the 3D object header information to the current
 * position in the given file stream.  Returns FALSE on any error.
 * Protected class method.
 *
 *=======================================================================*/
boolean CDF3DObject::WriteHeader (FILE* pFileHandle) {
  int Result;

	/* Attempt to write the header at once */
  Result = fwrite (&Header, 1, DFARCH_HEADER_RECORDSIZE, pFileHandle);

	/* Ensure the write was successful */
  if (Result != DFARCH_HEADER_RECORDSIZE) {
    SET_EXT_ERROR4(ERR_WRITE," Failed to write header section information (%d of %d bytes)!", Result, DFARCH_HEADER_RECORDSIZE);
    return (FALSE);
   }

  return (TRUE);
 }
/*=========================================================================
 *		End of Class Method CDF3DObject::WriteHeader()
 *=======================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDF3DObject::WriteNormals()"
/*=========================================================================
 *
 * Class CDF3DObject Method - boolean WriteNormals (pFileHandle);
 *
 * Attempts to write the 3D object normals information to the current
 * position in the given file stream.  Returns FALSE on any error.
 * Protected class method.
 *
 *=======================================================================*/
boolean CDF3DObject::WriteNormals (FILE* pFileHandle) {
  int Result;

	/* Write all the points at once */
  Result = fwrite (Normals, DFARCH_NORMAL_RECORDSIZE, (int)GetNumPlanes(), pFileHandle);

	/* Ensure the read was successful */
  if (Result != GetNumPlanes()) {
    SET_EXT_ERROR4(ERR_WRITE, "Failed to write the normals data (%d of %d normals)!", Result, GetNumPlanes());
    return (FALSE);
   }

  return (TRUE);
 }
/*=========================================================================
 *		End of Class Method CDF3DObject::WriteNormals()
 *=======================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDF3DObject::WritePlanes()"
/*=========================================================================
 *
 * Class CDF3DObject Method - boolean WritePlanes (pFileHandle);
 *
 * Attempts to write the 3D object plane information to the current
 * position in the given file stream.  Returns FALSE on any error.
 * Protected class method.
 *
 *=======================================================================*/
boolean CDF3DObject::WritePlanes (FILE* pFileHandle) {
  int Result;
  int LoopCounter;
  int PlaneSize;

	/* Must write planes one at a time */
  for (LoopCounter = 0; LoopCounter < NumPlanesAllocated; LoopCounter++) {

		/* Compute the plane size */
    PlaneSize = 8 + pPlanes[LoopCounter]->PointCount*8;

		/* Write the plane data all at once */
    Result = fwrite (pPlanes[LoopCounter], 1, PlaneSize, pFileHandle);

		/* Ensure the plane was successfully written */
    if (Result != PlaneSize) {
      SET_EXT_ERROR4(ERR_WRITE, "Failed to write the face data (%d of %d bytes)!", Result, PlaneSize);
      return (FALSE);
     }
   }

  return (TRUE);
 }
/*=========================================================================
 *		End of Class Method CDF3DObject::WritePlanes()
 *=======================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDF3DObject::WritePoints()"
/*=========================================================================
 *
 * Class CDF3DObject Method - boolean WritePoints (pFileHandle);
 *
 * Attempts to write the 3D object point information to the current
 * position in the given file stream.  Returns FALSE on any error.
 * Protected class method.
 *
 *=======================================================================*/
boolean CDF3DObject::WritePoints (FILE* pFileHandle) {
  int Result;

	/* Write all the points at once */
  Result = fwrite (Points, DFARCH_POINT_RECORDSIZE, (int)GetNumPoints(), pFileHandle);

	/* Ensure the write was successful */
  if (Result != GetNumPoints()) {
    SET_EXT_ERROR4(ERR_WRITE, "Failed to write the points data (%d of %d bytes)!", Result, GetNumPoints());
    return (FALSE);
   }

  return (TRUE);
 }
/*=========================================================================
 *		End of Class Method CDF3DObject::WritePoints()
 *=======================================================================*/