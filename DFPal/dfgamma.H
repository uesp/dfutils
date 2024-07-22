#ifndef __DAGPAL_H
#define __DAGPAL_H


	/* Error codes */
#define DF_ERR_NOTPAL 150

	/* The largest palette value allowed */
#define MAX_PAL 63



/*========= Structure for holding the palette data ========================*/
typedef struct {
  unsigned short r, g, b;
 } PAL_TYPE;
/*========= End of Structure PAL_TYPE =====================================*/


/*========= Define the Class to Hold the Palette Data =====================*/
class DAGPAL_TYPE {

public:
  char filename[101];		/* The current filename for the data */
  boolean is_col;		/* Is the data in COL or PAL format? */
  char col_data[8];		/* The extra COL format data */
  PAL_TYPE pal_data[256];	/* The palette data */


	/* Class Constructor */
  DAGPAL_TYPE (void);

	/* Changes the filename */
  void change_filename (const char *new_file);

	/* Function to load a PAL or COL file */
  boolean load_palette (const char *filename);

	/* Function to save a PAL or COL file */
  boolean save_palette (const char *filename);

	/* Brighten the palette by a certain degree */
  void modify (const float f);


 };
/*========= End of Classd DAGPAL_TYPE =====================================*/


	/* End of __DAGPAL_H */
#endif