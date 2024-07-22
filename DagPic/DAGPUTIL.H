#ifndef __DAGPUTIL_H
#define __DAGPUTIL_H

	/* Number of images which can loaded at one time */
#define MAX_IMAGES 100

	/* The 'hinge' byte of a RLE encoded file */
#define RLE_BYTE 162

#define MAX_THT 101
#define MAX_GROUPS 50
#define MAX_SUBGROUPS 10

	/* Error Codes */
#define        DAG_ERR_NONE 0
#define DAG_ERR_FILEUNKNOWN 1
#define    DAG_ERR_FILEOPEN 2
#define    DAG_ERR_FILEREAD 3
#define   DAG_ERR_IMAGESIZE 4
#define   DAG_ERR_NUMIMAGES 5




/*========== BEGIN CLASS DAG_PICTURE DEFINITION ===========================*/
class DAG_PICTURE {

public:
  int x_offset, y_offset;	/* Offset of Picture */
  unsigned int width, height;	/* Size of image in pixels */
  unsigned int image_size;	/* Size of image in pixels */
  unsigned int null_bytes;	/* Unknown */
  unsigned char *data;		/* Picture data */
  boolean rle;			/* Is current pic saved with RLE encoded or not */

	/* Constructor/Destructor */
  DAG_PICTURE (void);
  ~DAG_PICTURE (void);

	/* Deletes the picture element */
  void destroy (void);

	/* Loads an IMG file named *file_ptr into picture_ptr */
  boolean load_img (const char *filename, unsigned char *pal);

	/* Saves an IMG file */
  boolean save_img (const char *filename);

 };
/*========== END CLASS DAG_PICTURE DEFINITION =============================*/


/*========== BEGIN CLASS DAGPIC_ARRAY DEFINITION ==========================*/
class DAGPIC_ARRAY {

public:
  DAG_PICTURE *pics[MAX_IMAGES];	/* The pictures... */
  int num_pics;				/* Add the total number ... */

	/* Class Constructor and Destructors */
  DAGPIC_ARRAY (void);
  ~DAGPIC_ARRAY (void);

	/* Destroys the array */
  void destroy (void);

	/* Loads a CIF image into memory */
  boolean load_cif (const char *filename);

	/* Loads an arbitrary file into memory */
  boolean load_pic (const char *filename, unsigned char *pal, const int index);

	/* Saves a Multiple Image File */
  boolean save_cif (const char *filename);

	/* Saves an Arbitrary File Type into Picture Array */
  boolean save_pic (const char *filename, const int index);
 };
/*========== END CLASS DAGPIC_ARRAY DEFINITION ============================*/


/*========== CLASS TEXTURE_HEADER DEFINITION ==============================*/
class TEXTURE_HEADER {

public:
  unsigned int type1;  		/* Type? */
  unsigned long offset;		/* Offset to picture from beginning of file */
  unsigned int type2;		/* Another type? */
  char extra_header[12];	/* Extra header info? */

	/* Class Constructor, no Destructor needed */
  TEXTURE_HEADER (void);
 };
/*========== END OF CLASS TEXTURE_HEADER DEFINITION =======================*/


/*========== CLASS TEXTURE_PIC DEFINITITION ===============================*/
class TEXTURE_PIC : public DAG_PICTURE {

public:
  char extra_header[16];


	/* Class Constructor */
  TEXTURE_PIC (void) { memset(extra_header, 0, 16); }

	/* Class Destructor */
  ~TEXTURE_PIC (void);

 };
/*========== END CLASS TEXTURE_PIC DEFINITION =============================*/


/*========== CLASS TEXTURE_TYPE DEFINITION ================================*/
class TEXTURE_TYPE {

public:
  unsigned int num_images;		/* Type of texture file? */
  char text[26];			/* Text description */
  TEXTURE_HEADER *header[MAX_IMAGES];	/* The header for the picture info */
  TEXTURE_PIC *pics[MAX_IMAGES];	/* Contains the actual texture pictures */
  char *extra_file;           		/* Data + length of any extra file */
  unsigned long extra_file_length;

	/* Class Constructor and Destructor */
  TEXTURE_TYPE (void);
  ~TEXTURE_TYPE (void);

  void destroy (void);

	/* Loads Texture from File */
  boolean load_texture (const char *filename);
 };
/*========== END CLASS TEXTURE_TYPE DEFINITION ============================*/

/*========== End of Class Definitions =====================================*/


/*========== Function and Procedure Prototypes ============================*/

	/* Returns TRUE if i is not an invalid Texture Type Indentifier */
  boolean istexture (const char *filename);

	/* Loads a DF game palette into memory */
  boolean load_dagpal (const char *filename, unsigned char *pal);

	/* Saves a DF game palette to file */
  boolean save_dagpal (const char *filename, unsigned char *pal);

/*========== End of Function and Procedure Prototypes =====================*/


/*========== External Variables ===========================================*/
  extern int dag_error_code;
  extern char *dag_error_msgs[];
/*========== End of External Variables ====================================*/


#endif				/* Of __DAGPUTIL_H */