/*===========================================================================
 *
 * DFCommon.H - Dave Humphrey (uesp@m0use.net), 2 November 2000
 *
 *=========================================================================*/
#ifndef __DFCOMMON_H
#define __DFCOMMON_H

	/* Required Includes */
#include "genutil.h"
#include "dfconfig.h"
#include "dfwoods.h"
#include "dfmaps.h"


/*===========================================================================
 *
 * Begin Defines
 *
 *=========================================================================*/

	/* Various Daggerfall specific error codes */
  #define DFERR_NODATA     101
  #define DFERR_TOOMUCH    102
  #define DFERR_64SIZE     103
  #define DFERR_NOTAMD     104
  #define DFERR_MIDITRACK  105
  #define DFERR_MIDISONG   106
  #define DFERR_ARTMIN     107
  #define DFERR_ARTMAX     108
  #define DFERR_BADREC     109
  #define DFERR_NOTEXT     110
  #define DFERR_NOART      111
  #define DFERR_MAXSPELL   112
  #define DFERR_BSA_DIRTYPE 113
  #define DFERR_BSA_NOTFOUND 114


	/* Statistics counting defines */
#if defined(DOSTATS) && defined(_DEBUG)

  #define DEF_STAT_REC(Name)	long Name##NZCount = 0; \
				long Name##TotalCount = 0; \
				long Name##Max = 0; \
				long Name##Min = 0xfffffff; 
  #define DEF_STAT_VAR(Name)	long Name##NZCount = 0; \
				long Name##Max = 0; \
				long Name##Min = 0xfffffff;
  #define DEF_STAT_ARR(Name,N)	long Name##NZCount[N]; \
				long Name##Max[N]; \
				long Name##Min[N]; 
  #define DEF_STAT_CNT(Name)    long Name##CNT[256];
  #define INIT_STAT_CNT(Name)   memset(Name##CNT, 0, 256*sizeof(long));
  #define INIT_STAT_ARR(Name,N)  { long LC; \
				for (LC = 0; LC < N; LC++) { \
				  Name##NZCount[LC] = 0; \
				  Name##Max[LC] = 0; \
				  Name##Min[LC] = 0xfffffff; \
				 } }

  #define DO_STAT_REC2(Name, Var)	if ((Var) != 0) Name##NZCount++; \
				Name##TotalCount += Var; \
				if ((long)(Var) > Name##Max) Name##Max = (long) (Var); \
				if ((long)(Var) < Name##Min) Name##Min = (long) (Var);	
  #define DO_STAT_REC1(Name)    Name##TotalCount++;
  #define DO_STAT_VAR(Name, Var)	if ((Var) != 0) Name##NZCount++; \
				if ((long)(Var) > Name##Max) Name##Max = (long) (Var); \
				if ((long)(Var) < Name##Min) Name##Min = (long) (Var);	
  #define DO_STAT_CNT(Name, Var)     Name##CNT[(Var)&0xFF]++;
  #define DO_STAT_ARR(Name,Var,N) { long LC; \
				for (LC = 0; LC < N; LC++) { \
				  if ((Var[LC]) != 0) Name##NZCount[LC]++; \
				  if ((long)(Var[LC]) > Name##Max[LC]) Name##Max[LC] = (long) (Var[LC]); \
				  if ((long)(Var[LC]) < Name##Min[LC]) Name##Min[LC] = (long) (Var[LC]); \
				 } }

  #define OUT_STAT_REC(Name)	SystemLog.Printf("%sCount = %ld Total (%ld NZ) (%ld / %ld Min/Max)", #Name, Name##TotalCount, Name##NZCount, Name##Min, Name##Max);
  #define OUT_STAT_VAR(Name)	SystemLog.Printf("%sCount = %ld NZ ( %ld / %ld Min/Max)", #Name, Name##NZCount, Name##Min, Name##Max);
  #define OUT_STAT_CNT(Name) { long LC; \
				for (LC = 0; LC < 256; LC++) { \
				  SystemLog.Printf("%sCnt[%3d] = %ld", #Name, LC, Name##CNT[LC]); \
				 } }
  #define OUT_STAT_ARR(Name,N) { long LC; \
				for (LC = 0; LC < N; LC++) { \
				  SystemLog.Printf("%sCount[%d] = %ld NZ (%ld / %ld Min/Max)", #Name, LC, Name##NZCount[LC], Name##Min[LC], Name##Max[LC]); \
				 } }
#else
  #define DEF_STAT_REC(Name)
  #define DEF_STAT_REC2(Name)
  #define DEF_STAT_VAR(Name)
  #define DEF_STAT_ARR(Name,N)
  #define DEF_STAT_CNT(Name)
  #define INIT_STAT_ARR(Name,N) 
  #define INIT_STAT_CNT(Name)
  #define DO_STAT_REC(Name, Var)	
  #define DO_STAT_REC2(Name, Var)
  #define DO_STAT_VAR(Name, Var)	
  #define DO_STAT_ARR(Name,Var,N) 
  #define DO_STAT_CNT(Name)
  #define OUT_STAT_REC(Name)	
  #define OUT_STAT_VAR(Name)	
  #define OUT_STAT_ARR(Name,N) 
  #define OUT_STAT_CNT(Name)
#endif /* End of ifdef DOSTATS

/*===========================================================================
 *		End of Defines
 *=========================================================================*/


/*===========================================================================
 *
 * Begin Function Definitions
 *
 *=========================================================================*/

	/* Attempt to retreive the DF path */
  char* GetDFPath         (void);
  char* GetDFArena2Path   (void);
  char* GetDFArena2CDPath (void);

	/* Get strings */
  char* GetDFRMBFilename (const int Index);

	/* Coordinate conversion functions */
  float DFPixelToMapX (const int PixelX);
  float DFPixelToMapZ (const int PixelY);
  int   DFMapToPixelX (const float MapX);
  int   DFMapToPixelY (const float MapZ);

	/* Define the Daggerfall Specific Error Codes */
  boolean PostDFErrors (void);

/*===========================================================================
 *		End of Function Definitions
 *=========================================================================*/


/*===========================================================================
 *
 * Begin External Variables
 *
 *=========================================================================*/

	/* Raw VGA palette for DF images */
  extern byte DFPalette[768];

	/* Daggerfall region names */
  extern char DFRegionNames[62][33];

	/* Default game config file */
  extern CDFConfig DFConfigFile;

	/* File objects */
  extern CDFWoodsFile DFWoodsFile;
  extern CDFMapsFile  DFMaps;

/*===========================================================================
 *		End of External Variables
 *=========================================================================*/


#endif
/*===========================================================================
 *		End of File DFCommon.H
 *=========================================================================*/