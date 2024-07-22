/*===========================================================================
 *
 * DFConfig.CPP - Dave Humphrey (uesp@m0use.net), 9 November 2000
 *
 *=========================================================================*/

	/* Include Files */
#include "dfconfig.h"


	/* Local shortcut for defining the lookup table */
#define DF_CONFIG_LOOKUP(Name, Type) { #Name, DF_CFGID_##Name, DF_CONFIG_VALUE_##Type },


	/* Contains the known config file parameter and types */
df_config_lookup_t DFLookupTable[] = {
	DF_CONFIG_LOOKUP(TYPE, STRING)
	DF_CONFIG_LOOKUP(PATH, STRING)
	DF_CONFIG_LOOKUP(PATHCD, STRING)
	DF_CONFIG_LOOKUP(FADECOLOR, INTEGER)
	DF_CONFIG_LOOKUP(MAPFILE, STRING)
	DF_CONFIG_LOOKUP(RENDERGAME, INTEGER)
	DF_CONFIG_LOOKUP(USER, INTEGER)
	DF_CONFIG_LOOKUP(STARTMAP, INTEGER)
	DF_CONFIG_LOOKUP(REGION, INTEGER)
	DF_CONFIG_LOOKUP(HELMET, INTEGER)
	DF_CONFIG_LOOKUP(MAXSPEED, INTEGER)
	DF_CONFIG_LOOKUP(CONTROLS, STRING)
	DF_CONFIG_LOOKUP(OBJMEMSIZE, INTEGER)
	DF_CONFIG_LOOKUP(MAPS, STRING)
	DF_CONFIG_LOOKUP(CHEATMODE, INTEGER)
	DF_CONFIG_LOOKUP(MAGICREPAIR, INTEGER)
	DF_CONFIG_LOOKUP(ROOTPATH, STRING)		/* Custom parameter */
	{ NULL, DF_CFGID_NONE, DF_CONFIG_VALUE_STRING }	/* Must be last record */
 };


#undef  __FUNC__
#define __FUNC__ "CDFConfigRecord::Destroy()"
/*===========================================================================
 *
 * Class CDFConfigRecord Destructor
 *
 *=========================================================================*/
void CDFConfigRecord::Destroy (void) {

	/* Delete the parameter value only if its a string */
  if (Type == DF_CONFIG_VALUE_STRING) { 
    DestroyPointer(pValue); 
   }
   
  DestroyPointer(pName);
  Type = DF_CONFIG_VALUE_INTEGER;
  ID = DF_CFGID_NONE;
 }
/*===========================================================================
 *		End of Class CDFConfigRecord Destructor
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFConfigRecord::Read()"
/*===========================================================================
 *
 * Class CDFConfigRecord Method - int Read (pFileHandle);
 *
 * Attempt to load one line from the specified config file.  
 * Returns FALSE on any error. Return values are
 *	DF_CFGREAD_NOTHING - Nothing was read.
 *	DF_CFGREAD_RECORD  - A record was read
 *	DF_CFGREAD_ERROR   - Something bad happened.
 *
 *=========================================================================*/
int CDFConfigRecord::Read (FILE* pFileHandle) {
  char  InputBuffer[DF_CONFIG_MAXLINELENGTH+1];
  char* pParse;
  char* pParameter;
  char* pValue;
  int   Result;
  df_config_lookup_t* pLookup;

	/* Delete the current record information */
  Destroy();

	/* Input one line from file */
  Result = read_eol(pFileHandle, InputBuffer, DF_CONFIG_MAXLINELENGTH);
  if (Result == READ_MSL) read_eol(pFileHandle);

	/* Trim leading/trailing whitespace and ensure something is left over */
  pParse = trim(InputBuffer);
  if(*pParse == NULL_CHAR) return (DF_CFGREAD_NOTHING);

	/* Find the parameter/value pair */
  Result = SeperateVarValue(InputBuffer, NULL_CHAR, ' ', &pParameter, &pValue);
  SetName(pParameter);

	/* Attempt to find the parameter name in the lookup table */
  pLookup = DFLookupConfig(pParameter);

	/* Was nothing found? Add parameter as a general string type */
  if (pLookup == NULL) {
    ID = DF_CFGID_NONE;
    SetString(pValue);
    return (DF_CFGREAD_RECORD);
   }

  ID = pLookup->ID;

	/* Set the value according to the type recorded in the lookup table */
  if (pLookup->Type == DF_CONFIG_VALUE_INTEGER)
    SetNumber(atoi(pValue));
  else
    SetString(pValue);

  return (DF_CFGREAD_RECORD);
 }
/*===========================================================================
 *		End of Class Method CDFConfigRecord::Load()
 *========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFConfigRecord::AddString()"
/*===========================================================================
 *
 * Class CDFConfig Method - boolean AddString (pName, pValue, ID);
 *
 * Adds a string value record to the config file.  Returns FALSE on any error.
 *
 *=========================================================================*/
boolean CDFConfig::AddString (const char* pName, const char* pValue, const int ID) {

	/* Ensure valid input */
  if (pName == NULL || pValue == NULL) {
    SET_EXT_ERROR2(ERR_NULL, "Invalid input received!");
    return (FALSE);
   }

	/* Ensure a valid number of records */
  if (NumRecords >= DF_CONFIG_MAXPARAMETERS) {
    SET_EXT_ERROR3(ERR_MAXINDEX, "Maximum number of config parameters exceeded (%d)!", DF_CONFIG_MAXPARAMETERS);
    return (FALSE);
   }

	/* Clear the records current contents */
  Records[NumRecords].Destroy();

	/* Set the new config record values */
  Records[NumRecords].SetName(pName);
  Records[NumRecords].SetString(pValue);
  Records[NumRecords].SetID(ID);
  NumRecords++;

  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFConfig::AddString()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFConfigRecord::GetNumber()"
/*===========================================================================
 *
 * Class CDFConfig Method - int GetNumber (ID);
 *
 * Finds the parameter ID in the current config and returns the 
 * number value associated with it.  Returns -1 on any error.
 *
 *=========================================================================*/
int CDFConfig::GetNumber (const int ID) {
  int LoopCounter;

  for (LoopCounter = 0; LoopCounter < NumRecords; LoopCounter++) {
    if (Records[LoopCounter].GetID() == ID) return (Records[LoopCounter].GetNumber());
   }
 
	/* None found */
  return (-1);
 }
/*===========================================================================
 *		End of Class Method CDFConfig::GetNumber()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFConfigRecord::GetNumber()"
/*===========================================================================
 *
 * Class CDFConfig Method - int GetNumber (pString);
 *
 * Finds the parameter pString in the current config and returns the 
 * number value associated with it.  Returns -1 on any error.
 *
 *=========================================================================*/
int CDFConfig::GetNumber (const char* pString) {
  int LoopCounter;

	/* Ensure a valid string input */
  if (pString == NULL) {
    SET_EXT_ERROR2(ERR_NULL, "Invalid NULL input received!");
    return (-1);
   }

  for (LoopCounter = 0; LoopCounter < NumRecords; LoopCounter++) {
    if (Records[LoopCounter].GetName() == NULL) continue;
    if (stricmp(pString, Records[LoopCounter].GetName()) == 0) return (Records[LoopCounter].GetNumber());
   }
 
	/* None found */
  return (-1);
 }
/*===========================================================================
 *		End of Class Method CDFConfig::GetNumber()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFConfigRecord::GetString()"
/*===========================================================================
 *
 * Class CDFConfig Method - char* GetString (ID);
 *
 * Finds the parameter ID in the current config and returns the 
 * string value associated with it.  Returns -1 on any error.
 *
 *=========================================================================*/
char* CDFConfig::GetString (const int ID) {
  int LoopCounter;

  for (LoopCounter = 0; LoopCounter < NumRecords; LoopCounter++) {
    if (Records[LoopCounter].GetID() == ID) return (Records[LoopCounter].GetString());
   }
 
	/* None found */
  return (NULL);
 }
/*===========================================================================
 *		End of Class Method CDFConfig::GetString()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFConfigRecord::GetString()"
/*===========================================================================
 *
 * Class CDFConfig Method - int GetString (pString);
 *
 * Finds the parameter pString in the current config and returns the 
 * string value associated with it.  Returns NULL on any error.
 *
 *=========================================================================*/
char* CDFConfig::GetString (const char* pString) {
  int LoopCounter;

	/* Ensure a valid string input */
  if (pString == NULL) {
    SET_EXT_ERROR2(ERR_NULL, "Invalid NULL input received!");
    return (NULL);
   }

  for (LoopCounter = 0; LoopCounter < NumRecords; LoopCounter++) {
    if (Records[LoopCounter].GetName() == NULL) continue;
    if (stricmp(pString, Records[LoopCounter].GetName()) == 0) return (Records[LoopCounter].GetString());
   }
 
	/* None found */
  return (NULL);
 }
/*===========================================================================
 *		End of Class Method CDFConfig::GetString()
 *=========================================================================*/


#undef  __FUNC__
#define __FUNC__ "CDFConfigRecord::Load()"
/*===========================================================================
 *
 * Class CDFConfig Method - boolean Load (pFilename);
 *
 * Attempt to load the specified config file.  Returns FALSE on any error.
 *
 *=========================================================================*/
boolean CDFConfig::Load (const char* pFilename) {
  FILE* pFileHandle;
  int   Result;

	/* Attempt to open file for input */
  pFileHandle = openfile(pFilename, "rt");

  if (pFileHandle == NULL) {
    SET_EXT_ERROR3(ERR_FILE, "Failed to open config file '%s'!", pFilename);
    return (FALSE);
   }

	/* Clear the current information */
  Destroy();

	/* Read in the entire file */
  while (!feof(pFileHandle)) {

		/* Ensure not too many records have been added */
    if (NumRecords >= DF_CONFIG_MAXPARAMETERS) {
      SET_EXT_ERROR3(ERR_MAXINDEX, "Maximum number of parameters exceeded (%d)!", DF_CONFIG_MAXPARAMETERS);
      fclose (pFileHandle);
      return (FALSE);
     }

		/* Attempt to read one record */
    Result = Records[NumRecords].Read(pFileHandle);

		/* Check the result of the read operation */
    if (Result == DF_CFGREAD_RECORD) 
      NumRecords++;
    else if (Result == DF_CFGREAD_ERROR) {
      fclose (pFileHandle);
      return (FALSE);
     }
   }

  fclose (pFileHandle);
  return (TRUE);
 }
/*===========================================================================
 *		End of Class Method CDFConfig::Load()
 *========================================================================*/


#undef  __FUNC__
#define __FUNC__ "DFLookupConfig()"
/*===========================================================================
 *
 * Function - df_config_lookup_t* DFLookupConfig (pString);
 *
 * Lookup a parameter name in the config lookup table and return the matching
 * record.  Returns NULL on error of if nothing was found.
 *
 *=========================================================================*/
df_config_lookup_t* DFLookupConfig (const char* pString) {
  int LoopCounter;

	/* Search entire array */
  for (LoopCounter = 0; LoopCounter < DF_CONFIG_MAXPARAMETERS; LoopCounter++) {
    if (DFLookupTable[LoopCounter].pName == NULL) return (NULL);
    if (stricmp(DFLookupTable[LoopCounter].pName, pString) == 0) return (&DFLookupTable[LoopCounter]);
   }

	/* No match found */
  return (NULL);
 }
/*===========================================================================
 *		End of Function DFLookupConfig()
 *=========================================================================*/