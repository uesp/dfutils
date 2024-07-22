/*===========================================================================
 *
 * DFConfig.H - Dave Humphrey (uesp@m0use.net), 9 November 2000
 *
 *=========================================================================*/
#ifndef __DFCONFIG_H
#define __DFCONFIG_H

	/* Required Includes */
#include "genutil.h"


/*===========================================================================
 *
 * Begin Defines
 *
 *=========================================================================*/

	/* Config ID values */
  #define DF_CFGID_NONE		-1
  #define DF_CFGID_TYPE		0
  #define DF_CFGID_PATH		1
  #define DF_CFGID_PATHCD	2
  #define DF_CFGID_FADECOLOR	3
  #define DF_CFGID_MAPFILE	4
  #define DF_CFGID_RENDERGAME	5
  #define DF_CFGID_USER		6
  #define DF_CFGID_STARTMAP	7
  #define DF_CFGID_REGION	8
  #define DF_CFGID_HELMET	9
  #define DF_CFGID_MAXSPEED	10
  #define DF_CFGID_CONTROLS	11
  #define DF_CFGID_OBJMEMSIZE	12
  #define DF_CFGID_MAPS		13
  #define DF_CFGID_CHEATMODE	14
  #define DF_CFGID_MAGICREPAIR	15
  #define DF_CFGID_ROOTPATH     16

	/* Number of config parameters allowed in a config file */
  #define DF_CONFIG_MAXPARAMETERS 30

	/* Max length of a config line */
  #define DF_CONFIG_MAXLINELENGTH 256

	/* Return values for the config record read function */
  #define DF_CFGREAD_ERROR   -1
  #define DF_CFGREAD_NOTHING 0
  #define DF_CFGREAD_RECORD  1

/*===========================================================================
 *		End of Defines
 *=========================================================================*/


/*===========================================================================
 *
 * Begin Type and Structure Definitions
 *
 *=========================================================================*/

	/* Type of config records */
  typedef enum {
    DF_CONFIG_VALUE_INTEGER = 0,
    DF_CONFIG_VALUE_STRING  = 1
   } df_config_value_t;

	/* Used to store the ID-string lookup table */
  typedef struct {
    char*	      pName;
    int		      ID;
    df_config_value_t Type;
   } df_config_lookup_t;

/*===========================================================================
 *		End of Type and Structure Definitions
 *=========================================================================*/


/*===========================================================================
 *
 * Class CDFConfigRecord Definition
 *
 *=========================================================================*/
class CDFConfigRecord { 

  /*---------- Begin Protected Class Members -------------------------*/	
protected:

  char*		pName;		/* The parameter name */

  union {
    char*	pValue;		/* The string value */
    int		iValue;		/* Or the integer value of parameter */
   };

  df_config_value_t Type;	/* The type of record (string/integer/... */

  int		    ID;		/* The unique parameter identifier */



  /*---------- Begin Public Class Methods -----------------------------*/
public:

	/* Class Constructor/Destructor */
  CDFConfigRecord () { pName = NULL; Type = DF_CONFIG_VALUE_INTEGER; iValue = 0; }
  ~CDFConfigRecord () { Destroy(); }
  void Destroy (void);

	/* Get class values */
  char* GetName   (void) { return (pName); }
  int   GetID     (void) { return (ID); }
  char* GetString (void) { if (Type == DF_CONFIG_VALUE_STRING) return (pValue); else return (NULL); }
  int   GetNumber (void) { if (Type == DF_CONFIG_VALUE_STRING) return (-1); else return (iValue); }
  
	/* Read in one config record from a file stream */
  int Read (FILE* pFileHandle);

	/* Set class members */
  void SetName   (const char* pString) { DestroyPointer(pName); pName = CreateString(pString); }
  void SetID     (const int   NewID)   { ID = NewID; }
  void SetString (const char* pString) { if (Type == DF_CONFIG_VALUE_STRING) { DestroyPointer(pValue); } pValue = CreateString(pString); Type = DF_CONFIG_VALUE_STRING; }
  void SetNumber (const int   Number)  { if (Type == DF_CONFIG_VALUE_STRING) { DestroyPointer(pValue); } iValue = Number; Type = DF_CONFIG_VALUE_INTEGER; }

 };
/*===========================================================================
 *		End of Class CDFConfigRecord Definition
 *=========================================================================*/


/*===========================================================================
 *
 * Class CDFConfig Definition
 *
 *=========================================================================*/
class CDFConfig {

  /*---------- Begin Protected Class Members -------------------------*/
protected:
  CDFConfigRecord Records[DF_CONFIG_MAXPARAMETERS];	/* The parameter data */
  int		  NumRecords;

  /*---------- Begin Public Class Methods ----------------------------*/
public:

	/* Class Constructor/Destructor */
  CDFConfig() { NumRecords = 0; }
  ~CDFConfig() { Destroy(); }
  void Destroy (void) { NumRecords = 0; }

	/* Add a string value config record */
  boolean AddString (const char* pName, const char* pValue, const int ID);

	/* Get class members */
  int GetNumRecords (void) { return (NumRecords); }

	/* Get a parameter value */
  int   GetNumber (const char* pString);
  int   GetNumber (const int ID);
  char* GetString (const char* pString);
  char* GetString (const int ID);

	/* Attempt to load and parse the given config file */
  boolean Load (const char* pFilename);

 };
/*===========================================================================
 *		End of Class CDFConfig Definition
 *=========================================================================*/


/*===========================================================================
 *
 * Begin Function Definitions
 *
 *=========================================================================*/

	/* Find a parameter in the lookup table */
  df_config_lookup_t* DFLookupConfig (const char* pString);

/*===========================================================================
 *		End of Function Definitions
 *=========================================================================*/


#endif
/*===========================================================================
 *		End of File DFConfig.H
 *=========================================================================*/