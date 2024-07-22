/*=========================================================================
 *
 * DFARCH3D.CPP - Dave Humphrey (uesp@m0use.net), 29 October 2000
 *
 *=======================================================================*/


	/* Include Files */
#include "dfarch3d.h"


#undef  __FUNC__
#define __FUNC__ "CDFArch3D::CDFArch3D()"
/*===========================================================================
 *
 * Class CDFArch3D Constructor
 * 
 *=========================================================================*/
CDFArch3D::CDFArch3D() {
  int LoopCounter;
  CreatePointerArray(p3DObjects, CDF3DObject*, DFARCH_MAX_3DOBJECTS);

  for (LoopCounter = 0; LoopCounter < DFARCH_MAX_3DOBJECTS; LoopCounter++) {
    p3DObjects[LoopCounter] = NULL;
   }

  SetFilename("arena2\\arch3d.bsa");
 }
/*===========================================================================
 *		End of Class CDFArch3D Constructor
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFArch3D::~CDFArch3D()"
/*===========================================================================
 *
 * Class CDFArch3D Destructor
 * 
 *=========================================================================*/
CDFArch3D::~CDFArch3D() {

  Destroy();
  DestroyPointerArray(p3DObjects);
 }
/*===========================================================================
 *		End of Class CDFArch3D Destructor
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFArch3D::Destroy()"
/*===========================================================================
 *
 * Class CDFArch3D Destructor
 * 
 *=========================================================================*/
void CDFArch3D::Destroy (void) {
  int LoopCounter;

  for (LoopCounter = 0; LoopCounter < DFARCH_MAX_3DOBJECTS; LoopCounter++) {
    DestroyPointer(p3DObjects[LoopCounter]);
   }

	/* Call base class method */
  CDFBSAFile::Destroy();
 }
/*===========================================================================
 *		End of Class CDFArch3D Destructor
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFArch3D::Create3DObject()"
/*===========================================================================
 *
 * Class CDFArch3D Method - void Create3DObject (RecordIndex);
 *
 * Creates the given object array.  Protected class method.
 *
 *=========================================================================*/
void CDFArch3D::Create3DObject (const int RecordIndex) {

	/* Delete the object if it exists */
  DestroyPointer(p3DObjects[RecordIndex]);

	/* Alloate the new object */
  CreatePointer(p3DObjects[RecordIndex], CDF3DObject);
 }
/*===========================================================================
 *		End of Class Method CDFArch3D::Create3DObject()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFArch3D::DebugCompare()"
/*===========================================================================
 *
 * Class CDFArch3D Method - int DebugCompare (ValueID, p3DObject);
 *
 * Compares the given 3D Object record with the 3D object with the given ValueID
 * in the opened ARCH3D.BSA file. Does a simple compare on a byte-by-byte 
 * basis.  Returns DFBSA_ERROR on any error, a positive value if the records 
 * do not match, and 0 if they match.  Assumes that the BSA header and 
 * directory have already been loaded. 
 * 
 *=========================================================================*/
int CDFArch3D::DebugCompare (const long ValueID, CDF3DObject* p3DObject) {
  int   RecordIndex;

	/* Find the record in the current directory and ensure it exists */
  RecordIndex = FindRecordIndex(ValueID);
  if (RecordIndex == DFBSA_ERROR) return (DFBSA_ERROR);

	/* Call the indexed version */
  return (DebugCompareIndex(ValueID, p3DObject));
 }
/*===========================================================================
 *		End of Class Method CDFArch3D::DebugCompare()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFArch3D::DebugCompareIndex()"
/*===========================================================================
 *
 * Class CDFArch3D Method - int DebugCompareIndex (RecordIndex, p3DObject);
 *
 * Compares the given 3D Object record with the 3D object with the given ValueID
 * in the opened ARCH3D.BSA file. Does a simple compare on a byte-by-byte 
 * basis.  Returns DFBSA_ERROR on any error, a positive value if the records 
 * do not match, and 0 if they match.  Assumes that the BSA header and 
 * directory have already been loaded. 
 * 
 *=========================================================================*/
int CDFArch3D::DebugCompareIndex (const int RecordIndex, CDF3DObject* p3DObject) {
  int   Result;
  int   Result1;
  int   Result2;
  long  RecordSize;
  char* pDataBuffer1;
  char* pDataBuffer2;
  FILE* pTempHandle;

	/* Ensure valid input */
  if (!IsOpen() || p3DObject == NULL) {
    SET_EXT_ERROR2(ERR_NULL, "Invalid object or BSA file not open!");
    return (DFBSA_ERROR);
   }

	/* Ensure a valid record index */
  if (!IsValidRecordIndex(RecordIndex)) {
    SET_EXT_ERROR3(ERR_INDEX, "Invalid BSA record index %d!", RecordIndex);
    return (DFBSA_ERROR);
   }

	/*  Check the record sizes first for equality */
  RecordSize = pDirectory[RecordIndex]->RecordSize;

  if (p3DObject->GetRecordSize() != RecordSize) {
    SET_EXT_ERROR2(ERR_CUSTOM, "Record sizes are different!");
    return (DFBSA_ERROR);
   }

  	/* Create temp file for input/output */
  pTempHandle = openfile("temp.3d", "w+b");

  if (pTempHandle == NULL) {
    SET_EXT_ERROR2(ERR_FILE, "Failed to open temporary file 'temp.3d'!");
    return (DFBSA_ERROR);
   }

	/* Output 3D Object data to temp file */
  Result = p3DObject->Write(pTempHandle);
  if (!Result) return (DFBSA_ERROR);
  fflush (pTempHandle);

	/* Allocate a temp buffers for the data */
  CreatePointerArray(pDataBuffer1, char, RecordSize);
  CreatePointerArray(pDataBuffer2, char, RecordSize);

	/* Jump to record position in BSA file */
  fseek (pFileHandle, pDirectory[RecordIndex]->RecordOffset, SEEK_SET);

	/* Read in data from BSA and ensure success */
  Result1 = fread(pDataBuffer1, 1, RecordSize, pFileHandle);
  
  	/* Read in data from temp file */
  fseek (pTempHandle, 0, SEEK_SET);
  Result2 = fread (pDataBuffer2, 1, RecordSize, pTempHandle);
  fclose (pTempHandle);
 
  if (Result1 != RecordSize || Result2 != RecordSize) {
    DestroyPointer(pDataBuffer1);
    DestroyPointer(pDataBuffer2);
    SET_EXT_ERROR2(ERR_READ, "Failed to read from temp or BSA files!");
    return (DFBSA_ERROR);
   }

	/* Compare data */
  Result = memcmp(pDataBuffer1, pDataBuffer2, RecordSize);

	/* Unallocate temp data buffers */
  DestroyPointer(pDataBuffer1);
  DestroyPointer(pDataBuffer2);
  
	/* Return result */
  if (Result == 0) return (0);
  return (1);
 }
/*===========================================================================
 *		End of Class Method CDFArch3D::DebugCompareIndex()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFArch3D::Get3DObject()"
/*===========================================================================
 *
 * Class CDFArch3D Method - CDF3DObject* Get3DObject (ValueID);
 *
 * Attempts to retrieve a 3d object given its value ID, loading the data if
 * required.  Returns NULL on any error.
 *
 *=========================================================================*/
CDF3DObject* CDFArch3D::Get3DObject (const long ValueID) {
  int RecordIndex;

	/* Try and find the record index */
  RecordIndex = FindRecordIndex(ValueID);
  if (RecordIndex == DFBSA_ERROR) return (NULL);

	/* Load the object with the given record index */
  return (Get3DObject(RecordIndex));
 }
/*===========================================================================
 *		End of Class Method CDFArch3D::Get3DObject()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFArch3D::Get3DObject()"
/*===========================================================================
 *
 * Class CDFArch3D Method - CDF3DObject* Get3DObject (RecordIndex);
 *
 * Attempts to retrieve a 3d object given its index, loading the data if
 * required.  Returns NULL on any error.
 *
 *=========================================================================*/
CDF3DObject* CDFArch3D::Get3DObject (const int RecordIndex) {
  boolean HadToOpenFile = FALSE;
  boolean Result;

  	/* Open file if required to compare directory */
  if (!DirectoryRead && !IsOpen()) {
    if (!Open()) return (FALSE);
    HadToOpenFile = TRUE;
   }

	/* Ensure a valid record index */
  if (!IsValidRecordIndex(RecordIndex)) { 
    SET_EXT_ERROR3(ERR_INDEX, "Invalid 3DObject record index %d received!", RecordIndex);
    if (HadToOpenFile) Close();
    return (NULL);
   }

	/* Return the object if it is already loaded */
  if (p3DObjects[RecordIndex] != NULL) {
    if (HadToOpenFile) Close();
    //SystemLog.Printf ("\t\tNum Faces = %d", p3DObjects[RecordIndex]->GetNumPlanes());
    return (p3DObjects[RecordIndex]);
   }

	/* Open file if required to read object */
  if (!IsOpen()) {
    if (!Open()) return (FALSE);
    HadToOpenFile = TRUE;
   }

	/* Attempt to load the object */
  Result = Load3DObjectIndex (RecordIndex);
  if (HadToOpenFile) Close();

  //SystemLog.Printf ("\t\tNum Faces = %d", p3DObjects[RecordIndex]->GetNumPlanes());

	/* Return appropiate value */
  if (!Result) return (NULL);
  return (p3DObjects[RecordIndex]);
 }
/*===========================================================================
 *		End of Class Method CDFArch3D::Get3DObject()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFArch3D::Load3DObject()"
/*===========================================================================
 *
 * Class CDFArch3D Method - boolean Load3DObject (ValueID);
 *
 * Attempts to load a 3d object given its value ID.  Returns FALSE 
 * on any error.
 *
 *=========================================================================*/
boolean CDFArch3D::Load3DObject (const long ValueID) {
  int RecordIndex;

	/* Try and find the record index */
  RecordIndex = FindRecordIndex(ValueID);
  if (RecordIndex == DFBSA_ERROR) return (FALSE);

	/* Load the object with the given record index */
  return (Load3DObjectIndex(RecordIndex));
 }
/*===========================================================================
 *		End of Class Method CDFArch3D::Load3DObject()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFArch3D::Load3DObjectIndex()"
/*===========================================================================
 *
 * Class CDFArch3D Method - boolean Load3DObjectIndex (ValueID);
 *
 * Attempts to load a 3d object given its record index.  Returns FALSE on any
 * error.
 *
 *=========================================================================*/
boolean CDFArch3D::Load3DObjectIndex (const int RecordIndex) {
  boolean Result;

	/* Is the given record index valid? */
  if (!IsValidRecordIndex(RecordIndex)) {
    SET_EXT_ERROR3(ERR_INDEX, "Invalid BSA record index %d!", RecordIndex);
    return (FALSE);
   }

	/* Create the required object */
  Create3DObject(RecordIndex);

	/* Set the BSA object data */
  p3DObjects[RecordIndex]->SetBSARecordID(pDirectory[RecordIndex]->Value);
  p3DObjects[RecordIndex]->SetBSARecordOffset(pDirectory[RecordIndex]->RecordOffset);
  p3DObjects[RecordIndex]->SetBSARecordSize(pDirectory[RecordIndex]->RecordSize);
  p3DObjects[RecordIndex]->SetBSARecordIndex(RecordIndex);

	/* Move file pointer to appropiate spot */
  fseek (pFileHandle, pDirectory[RecordIndex]->RecordOffset, SEEK_SET);

	/* Attempt to load item */
  Result = p3DObjects[RecordIndex]->Read(pFileHandle);

  if (!Result) {
    DestroyPointer(p3DObjects[RecordIndex]);
    return (FALSE);
   }
 
  return (TRUE);  
 }
/*===========================================================================
 *		End of Class Method CDFArch3D::Load3DObjectIndex()
 *=========================================================================*/