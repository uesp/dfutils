#ifndef __DAGGEN_H
#define __DAGGEN_H

	/* Size of temp character buffers */
#ifndef MAX_STRING_LENGTH
  #define MAX_STRING_LENGTH 256
#endif

	/* Various Error Codes */
#define DF_ERR_NODATA     101
#define DF_ERR_TOOMUCH    102
#define DF_ERR_64SIZE     103
#define DF_ERR_NOTAMD     104
#define DF_ERR_MIDITRACK  105
#define DF_ERR_MIDISONG   106
#define DF_ERR_ARTMIN     107
#define DF_ERR_ARTMAX     108
#define DF_ERR_BADREC     109
#define DF_ERR_NOTEXT     110
#define DF_ERR_NOART      111
#define DF_ERR_MAXSPELL   112

	/* Defines the Daggerfall Specific Error Codes */
  void post_df_errors (void);

	/* End of __DAGGEN_H */
#endif