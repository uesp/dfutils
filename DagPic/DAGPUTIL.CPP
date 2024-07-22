	/* Standard C Includes */
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <ctype.h>

	/* User Defined Includes */
#include "genutil.h"
#include "fileutil.h"
#include "dagputil.h"

	/* Various Error Codes/Messages */
int dag_error_code = 0;

char *dag_error_msgs[] = {
  "No Error Code Generated",
  "Unknown File Format",
  "Failed to Open File",
  "Short Count Received on File Read/Write",
  "Invalid Image Size Received",
  "Invalid Number of Images Received"
 };


/*========== Class DAGPIC_ARRAY Constructor ===============================*/
void DAGPIC_ARRAY::DAGPIC_ARRAY (void) {
  int i;	/* Loop counter */

  for (i = 0; i < MAX_IMAGES; i++)
    pics[i] = NULL;

  num_pics = 0;
 }
/*========== End Class DAGPIC_ARRAY Constructor ===========================*/


/*========== Class DAGPIC_ARRAY Destructor ================================*/
void DAGPIC_ARRAY::~DAGPIC_ARRAY (void) {
  destroy();
 }
/*========== End Class DAGPIC_ARRAY Destructor ============================*/


/*========== Destroys an Array of Pictures ================================*/
void DAGPIC_ARRAY::destroy(void) {
  int i;	/* Loop counter */

  for (i = 0; i < MAX_IMAGES; i++)
    if (pics[i]) {
      delete pics[i];
      pics[i] = NULL;
     }

  num_pics = 0;
 }
/*========== End of Procedure DAGPIC_ARRAY::destroy() =====================*/


/*========== Loads an Arbitrary File Type into Picture Array ==============*/
boolean DAGPIC_ARRAY::load_pic (const char *filename, unsigned char *pal, const int index) {
  char buffer[4];	/* Temp buffer */
  int i;

	/* Copy last 3 characters of filename into buffer */
  i = strlen(filename) - 3;
  if (i < 0) i = 0;
  strcpy (buffer, filename + i);

	/* Determine the file type by the filename extension */
  if (stricmp(buffer, "IMG") == 0) {

	/* Allocate the first picture if not already allocated */
    if (!pics[index]) {
      if (!(pics[index] = new DAG_PICTURE))
	bug (MEM_ERR_MSG, "*pics[0] (%d)", sizeof(DAG_PICTURE));

      if (index >= num_pics) num_pics++;
     }

    return (pics[index]->load_img(filename, pal));
   }
  else if (stricmp(buffer, "CIF") == 0)
    return (load_cif(filename));
  else if (istexture (filename)) {
    TEXTURE_TYPE texture;

	/* Load the File */
    if (!texture.load_texture (filename))
      return (FALSE);

	/* Copy to the Pictures */
    destroy();

    for (i = 0; i < texture.num_images && i < MAX_IMAGES; i++) {

		/* Allocate New Object */
      if (!(pics[i] = new DAG_PICTURE))
	bug (MEM_ERR_MSG, "DAGPIC_ARRAY::load_pic() - *pics[i] (%d)", sizeof(DAG_PICTURE));

		/* Copy Values */
      pics[i]->x_offset = texture.pics[i]->x_offset;
      pics[i]->y_offset = texture.pics[i]->y_offset;
      pics[i]->width = texture.pics[i]->width;
      pics[i]->height = texture.pics[i]->height;
      pics[i]->null_bytes = texture.pics[i]->null_bytes;
      pics[i]->image_size = texture.pics[i]->image_size;
      pics[i]->data = texture.pics[i]->data;
      texture.pics[i]->data = NULL;
     }

    num_pics = texture.num_images;
    return (TRUE);
   }

  dag_error_code = DAG_ERR_FILEUNKNOWN;
  return (FALSE);
 }
/*========== End of Function DAGPIC_ARRAY::load_pic() =====================*/


/*========== Loads a CIF Into Memory ======================================*/
boolean DAGPIC_ARRAY::load_cif (const char *filename) {
  FILE *f;		/* File pointer */
  DAG_PICTURE *pic_ptr;	/* Temp pointer */
  long current_fpos;	/* Current position in file */
  long file_size;
  int ch;		/* Input buffer */
  int count, i;		/* Loop counters */
  unsigned char *temp_ptr;	/* Temp pointer */

	/* Attempt to open the file */
  if (!(f = openfile(filename, "rb"))) {
    dag_error_code = DAG_ERR_FILEOPEN;
    return (FALSE);
   }

	/* Get file size */
  file_size = get_filesize (f);

  do { /* Load multiple images in CIF/IMG file */

	/* Attempt to allocate the picture, if not already allocated */
    if (!pics[num_pics]) {
      if (!(pics[num_pics] = new DAG_PICTURE))
	bug (MEM_ERR_MSG, "load_cif(%s) - *pics[] (%d)", filename, sizeof(DAG_PICTURE));
     }

    pic_ptr = pics[num_pics];

	/* Get current fileposition */
    current_fpos = ftell(f);

    pic_ptr->x_offset = read_int (f);
    pic_ptr->y_offset = read_int (f);
    pic_ptr->width = read_int (f);
    pic_ptr->height = read_int (f);
    pic_ptr->null_bytes = read_int(f);
    pic_ptr->image_size = read_int(f);

	/* Check for weapon/alternate type RLE CIF file */
    if (pic_ptr->height * pic_ptr->width != pic_ptr->image_size)
      pic_ptr->rle = TRUE;
    else
      pic_ptr->rle = FALSE;

   // write_log_entry ("Cif Image #%d: (%d/%d) %d, Offset = %lX", num_pics, pic_ptr->width, pic_ptr->height, pic_ptr->image_size, ftell(f));

		/* Check image size....make sure its a valid one */
    if (pic_ptr->image_size > 65000l || pic_ptr->width > 340 || pic_ptr->height > 220) {
      write_log_entry ("ERROR: Cif Image #%d too Large (%d/%d) %d, Offset = %lX", num_pics, pic_ptr->width, pic_ptr->height, pic_ptr->image_size, ftell(f));
      dag_error_code = DAG_ERR_IMAGESIZE;
      fclose (f);
      return (FALSE);
     }

	/* Allocate picture buffer */
    if (pic_ptr->data) delete pic_ptr->data;
    pic_ptr->data = (unsigned char *) create_ptr (pic_ptr->height * pic_ptr->width + 1);
    temp_ptr = pic_ptr->data;

	/* For a Non-RLE encoded picture */
    if (!pic_ptr->rle) {

      if (fread (pic_ptr->data, sizeof(char), pic_ptr->image_size, f) != pic_ptr->image_size) {
	dag_error_code = DAG_ERR_FILEREAD;
	fclose (f);
	return (FALSE);
       }

     }
    else {	/* Must decode picture data */
      fseek (f, 64, SEEK_CUR);	/* Skip header_info? */

      for (count = 1; count <= pic_ptr->image_size && count <= current_fpos + file_size; temp_ptr++, count++) {
	ch = fgetc(f);

	if (ch >= RLE_BYTE) { /* Is encoded bit */

	  for (i = 2*RLE_BYTE - ch, ch = fgetc(f); i < RLE_BYTE; i++, temp_ptr++, count++)
	    *temp_ptr = ch;
	 }
	else 	/* Regular byte */
	  *temp_ptr = ch;
       } /* End of for loop */

      pic_ptr->image_size = pic_ptr->height * pic_ptr->width;	/* Set proper size */
      num_pics++;
      break;
     }

    num_pics++;  /* Increase number of pictures loaded */
  } while (ftell(f) + 10 < file_size && num_pics < MAX_IMAGES);

  return (TRUE);
 }
/*========== End of Function DAGPIC_ARRAY::load_cif() =====================*/


/*========== Saves a Multiple Image File ==================================*/
boolean DAGPIC_ARRAY::save_cif (const char *filename) {
  FILE *f;	/* File pointer */
  int i;	/* Loop counter */

  if (!(f = openfile (filename, "wb"))) {
    dag_error_code = DAG_ERR_FILEOPEN;
    return (FALSE);
   }

  for (i = 0; i < num_pics; i++) {
    if (pics[i]) {
      write_int (f, pics[i]->x_offset);
      write_int (f, pics[i]->y_offset);
      write_int (f, pics[i]->width);
      write_int (f, pics[i]->height);
      write_int (f, pics[i]->null_bytes);
      write_int (f, pics[i]->image_size);

		/* Write in the important data */
      if (fwrite (pics[i]->data, sizeof(char), pics[i]->image_size, f) != pics[i]->image_size) {
	dag_error_code = DAG_ERR_FILEREAD;
	fclose (f);
	return (FALSE);
       }
     }
   }

  fclose (f);
  return (TRUE);
 }
/*========== End of Function DAGPIC_ARRAY::save_cif() =====================*/


/*========== Saves an Arbitrary File Type into Picture Array ==============*/
boolean DAGPIC_ARRAY::save_pic (const char *filename, const int index) {
  char buffer[4];	/* Temp buffer */
  int i;

	/* Copy last 3 characters of filename into buffer */
  i = strlen(filename) - 3;
  if (i < 0) i = 0;
  strcpy (buffer, filename + i);

	/* Determine the file type by the filename extension */
  if (stricmp(buffer, "IMG") == 0) {
    return (pics[index]->save_img(filename));
   }
  else if (stricmp(buffer, "CIF") == 0) {
    return (save_cif (filename));
   }
  else
    dag_error_code = DAG_ERR_FILEUNKNOWN;

  return (FALSE);
 }
/*========== End of Function DAGPIC_ARRAY::save_pic() =====================*/


/*========== Class DAG_PICTURE Constructor ================================*/
void DAG_PICTURE::DAG_PICTURE (void) {
  x_offset = y_offset = 0;
  width = height = 0;
  image_size = 0;
  null_bytes = 0;
  data = NULL;
  rle = FALSE;
 }
/*========== End of Class DAG_PICTURE Constructor =========================*/


/*========== Class DAG_PICTURE Destructor =================================*/
void DAG_PICTURE::~DAG_PICTURE(void) {
  destroy();
 }
/*========== End of Class DAG_PICTURE Destructor ==========================*/


/*========== Deletes the Current Picture ==================================*/
void DAG_PICTURE::destroy (void) {
  if (data) delete data;
  data = NULL;
  image_size = 0;
  width = height = 0;
  x_offset = y_offset = 0;
  rle = FALSE;
 }
/*========== End of Procedure DAG_PICTURE::destroy() ======================*/


/*========= Loads a picture named *file_ptr into picture_ptr ==============*/
boolean DAG_PICTURE::load_img (const char *filename, unsigned char *pal) {
  FILE *f;		/* File Pointer */
  unsigned long file_size;

	/* Attempt to open the file */
  if (!(f = openfile (filename, "rb"))) {
    dag_error_code = DAG_ERR_FILEOPEN;
    return(FALSE);
   }

  file_size = get_filesize (f);  /* Get file size */

	/* Special file format for image size of 64kb */
  if (file_size == 64000l && strstr(filename, ".IMG")) {
    y_offset = 0;
    x_offset = 0;
    width = 320;
    height = 200;
    null_bytes = 0;
    image_size = (unsigned int) 64000l;

	/* Attempt to allocate space for the picture */
    if (data) delete data;
    data = (unsigned char *) create_ptr (image_size + 1);

	/* Read in image data */
    if (fread (data, sizeof(char), image_size, f) != image_size) {
      dag_error_code = DAG_ERR_FILEREAD;
      fclose (f);
      return (FALSE);
     }

   } /* End of loading file_size = 64768b */

	/* 320x200 image with attached palette */
  else if (file_size == 64768l && strstr(filename, ".IMG")) {
    y_offset = 0;
    x_offset = 0;
    width = 320;
    height = 200;
    image_size = 64000u;
    null_bytes = 0;

	/* Attempt to allocate space */
    if (data) delete data;
    data = (unsigned char *) create_ptr (image_size + 1);

	/* Read in image data */
    if (fread (data, sizeof(char), image_size, f) != image_size) {
      dag_error_code = DAG_ERR_FILEREAD;
      fclose (f);
      return (FALSE);
     }

	/* Load palette contained in file */
    if (fread(pal, sizeof(char), 768, f) != 768) {
      dag_error_code = DAG_ERR_FILEREAD;
      fclose (f);
      return (FALSE);
     }

   } /* End of load 320x200 image with attached palette */

  else {	/* Load regular IMG */
    x_offset = read_int (f);
    y_offset = read_int (f);
    width = read_int (f);
    height = read_int (f);
    null_bytes = read_int (f);	/* Skip 00's */

	/* Read 11th and 12th bytes...image size */
    image_size = read_int (f);
    rle = FALSE;

	/* Check image size....make sure its a valid one */
    if (image_size > 65000u || width > 340 || height > 220) {
      dag_error_code = DAG_ERR_IMAGESIZE;
      fclose (f);
      return(FALSE);
     }

	/* Allocate the image data */
    if (data) delete data;
    data = (unsigned char *) create_ptr (image_size + 1);

	/* Read in file */
    if (fread (data, sizeof(char), image_size, f) != image_size) {
      dag_error_code = DAG_ERR_FILEREAD;
      fclose (f);
      return (FALSE);
     }

   }

  fclose(f);
  return (TRUE);
 }
/*========== End of Function DAG_PICTURE::load_img() ======================*/


/*========== Saves a Regular IMG File =====================================*/
boolean DAG_PICTURE::save_img (const char *filename) {
  FILE *f;	/* File pointer */

  if (!(f = openfile(filename, "wb"))) {
    dag_error_code = DAG_ERR_FILEOPEN;
    return (FALSE);
   }

  write_int (f, x_offset);
  write_int (f, y_offset);
  write_int (f, width);
  write_int (f, height);
  write_int (f, null_bytes);
  write_int (f, image_size);

	/* Write in the important data */
  if (fwrite (data, sizeof(char), image_size, f) != image_size) {
    dag_error_code = DAG_ERR_FILEREAD;
    fclose (f);
    return (FALSE);
   }

  fclose (f);
  return (TRUE);
 }
/*========== End of Function DAG_PICTRE::save_img() =======================*/


/*========== Class TEXTURE_HEADER Constructor =============================*/
void TEXTURE_HEADER::TEXTURE_HEADER (void) {
  type1 = 0;
  offset = 0;
  type2 = 0;
 }
/*========== End of Class TEXTURE_HEADER Constructor ======================*/


/*========= CLASS TEXTURE_PIC PROCEDURE/FUNCTIONS =========================*/

void TEXTURE_PIC::~TEXTURE_PIC (void) {
  memset(extra_header, 0, 16);
  destroy ();
 }
/*========= END CLASS TEXTURE_PIC PROCEDURE/FUNCTIONS =====================*/


/*========= CLASS TEXTURE_TYPE PROCEDURE/FUNCTIONS ========================*/
void TEXTURE_TYPE::TEXTURE_TYPE (void) {
  int i;	/* Loop Counter */

	/* Set all pointers to initially NULL */
  for (i = 0; i < MAX_IMAGES; i++) {
    header[i] = NULL;
    pics[i] = NULL;
   }

  extra_file = NULL;
  num_images = 0;
  text[0] = 0;
  extra_file_length = 0;
 }


void TEXTURE_TYPE::~TEXTURE_TYPE (void) {
  destroy();
 }


void TEXTURE_TYPE::destroy (void) {
  int i;	/* Loop Counter */

  for (i = 0; i < MAX_IMAGES; i++) {
    if (pics[i]) delete pics[i];
    if (header[i]) delete header[i];
    header[i] = NULL;
    pics[i] = NULL;
   }

  if (extra_file) delete extra_file;
  extra_file = NULL;
 }
/*========== END CLASS TEXTURE_TYPE PROCEDURE/FUNCTIONS ===================*/


/*========== Loads a Texture File Into Memory =============================*/
boolean TEXTURE_TYPE::load_texture (const char *filename) {
  TEXTURE_PIC *temp_pic;	/* Temp Pointer */
  FILE *f;
  short ch, ch1, c1;
  long size = 0, l = 0, offset;
  unsigned int bytes_read[MAX_IMAGES];
  int texture_groups[MAX_GROUPS][MAX_SUBGROUPS];
  int i, j, k, dx, count, c;		/* Loop Counters */

	/* Attempt to Open File For Reading */
  if (!(f = openfile (filename, "rb"))) {
    dag_error_code = DAG_ERR_FILEOPEN;
    return (FALSE);
   }

	/* Clear the Array */
  destroy ();

	/* Read in the Number of Textures */
  num_images = read_int(f);

	/* Make sure the Number of Images is a Valid Number */
  if (num_images <= 0) {
    dag_error_code = DAG_ERR_NUMIMAGES;
    fclose (f);
    return (FALSE);
   }

	/* Read in the Text Title of Texture */
  if (fread (text, sizeof(char), 24, f) != 24) {
    dag_error_code = DAG_ERR_FILEREAD;
    fclose (f);
    return (FALSE);
   }

	/* Read in the Header Offset Section */
  for (i = 0; i < num_images && i < MAX_IMAGES; i++) {

		/* Allocate new header object */
    if (!(header[i] = new TEXTURE_HEADER))
      bug (MEM_ERR_MSG, "TEXTURE_TYPE::load_texture() - header[] (%d)", sizeof(TEXTURE_HEADER));

    header[i]->type1 = read_int (f);
    header[i]->offset = read_long(f);
    header[i]->type2 = read_int(f);

    if (fread (header[i]->extra_header, sizeof(char), 12, f) != 12) {
      dag_error_code = DAG_ERR_FILEREAD;
      fclose (f);
      return (FALSE);
     }

    write_log_entry ("Texture Offset: %ld", header[i]->offset);
   }

	/* Read in All the Picture Headers... */
  for (i = 0; i < num_images && i < MAX_IMAGES; i++) {

	/* Allocate New Picture */
    if (!(pics[i] = new TEXTURE_PIC))
      bug (MEM_ERR_MSG, "TEXTURE_TYPE::load_texture() - pics[] (%d)", sizeof(TEXTURE_PIC));

	/* Move to the Appropiate Offset */
    fseek (f, header[i]->offset, SEEK_SET);

	/* Read in the Picture Header Information */
    pics[i]->x_offset = read_int(f);
    pics[i]->y_offset = read_int(f);
    pics[i]->width = read_int (f);
    pics[i]->height = read_int (f);
    pics[i]->null_bytes = read_int (f);
    pics[i]->image_size = read_int (f);

    if (fread (pics[i]->extra_header, sizeof(char), 16, f) != 16) {
      dag_error_code = DAG_ERR_FILEREAD;
      fclose (f);
      return (FALSE);
     }

	/* Check to Make Sure Valid Width/Height Received */
    if (pics[i]->width * pics[i]->height > 65000u || pics[i]->image_size > 65000u) {
      dag_error_code = DAG_ERR_IMAGESIZE;
      fclose (f);
      return (FALSE);
     }

	/* Create the Picture Pointer */
    pics[i]->data = (unsigned char *) create_ptr (pics[i]->image_size + 1 - 28);
   } /* End of for read in pictures */

	/* Init the groupings */
  for (i = 0; i < MAX_GROUPS; i++) {
    texture_groups[i][0] = -1;
   }

	/* Create the Groupings */
  for (i = 0; i < num_images; i++) {

	/* Find the proper group to add current image, or create new */
    for (j = 0; j < MAX_GROUPS && texture_groups[j][0] != -1; j++) {
      offset = labs (header[texture_groups[j][0]]->offset - header[i]->offset);

		/* Successful match found */
      if (offset % 28 == 0 && offset/28 <= 10) break;
//	  pics[texture_groups[j][0]]->width == pics[i]->width &&
//	  pics[texture_groups[j][0]]->height == pics[i]->height) break;
     } /* End of for j loop... */

	/* Too many groups, do nothing */
    if (j >= MAX_GROUPS) {
      write_log_entry ("ERROR: Maximum number of texture groups exceeded (%d)", MAX_GROUPS);
     }
	/* Create new group */
    else if (texture_groups[j][0] == -1) {
      texture_groups[j][0] = i;
      texture_groups[j][1] = -1;
      write_log_entry ("Created new texture group for pic #%d", i);
     }
	/* Add to existing group */
    else {
      for (k = 0; k < MAX_SUBGROUPS && texture_groups[j][k] != -1; k++);

	/* Add to end of group */
      if (k < MAX_SUBGROUPS) {
	texture_groups[j][k] = i;
	if (k + 1 != MAX_SUBGROUPS) texture_groups[j][k + 1] = -1;
	write_log_entry ("Added pic #%d to texture group %d", i, j);
       }
	/* Too Many Subgroups */
      else {
	write_log_entry ("ERROR: Maximum number of texture sub-groups exceeded (%d)", MAX_SUBGROUPS);
       }
     }

   } /* End of for i loop (create groupings) */


  offset = 0;

	/* Read in All the Picture Data by Groups */
  for (i = 0; i < MAX_GROUPS && texture_groups[i][0] != -1; i++) {

	/* Find the Last Image in the Group */
    for (j = 0, size = 0, k = 0; j < MAX_SUBGROUPS && texture_groups[i][j] != -1; j++) {

      if (header[k]->offset < header[texture_groups[i][j]]->offset)
	k = texture_groups[i][j];

      bytes_read[j] = 0;
      //size += pics[texture_groups[i][j]]->image_size - 28 - 20;
      size += pics[texture_groups[i][j]]->width * pics[texture_groups[i][j]]->height;
     }

	/* Jump to Start of Image Data, hopefully */
    fseek (f, header[k]->offset + 28 + 20, SEEK_SET);
    write_log_entry ("Jumped to Offset %ld (#%d)", ftell(f), k);
    l = 0;
    k = 0;
    dx = 1;
    j = 0;

	/* Read this group of images */
    while (l < size && k < 32000) {
      k++;

      if (j >= MAX_SUBGROUPS || (texture_groups[i][j] == -1 && j > 0)) {
	dx = -1;
	j--;
       }
      else if (j < 0) {
	j = 0;
	dx = 1;
       }

      temp_pic = pics[texture_groups[i][j]];

		/* Make sure we don't read more than we allocated */
      if (bytes_read[j] + temp_pic->width <= temp_pic->image_size - 28) {

		/* Read one line */
	c1 = bytes_read[j];
	count = 0;

	while (count < temp_pic->width) {
	  ch = fgetc(f);
	  ch1 = fgetc(f);

	  for (c = 0; c < ch; c++)
	    temp_pic->data[c1 + c] = 0;

	  c1 += ch;
	  count += ch;

	  for (c = 0; c < ch1; c++) {
	    temp_pic->data[c1 + c] = fgetc(f);
	   }

	  count += ch1;
	  c1 += ch1;
	 }

	l += temp_pic->width;
	bytes_read[j] += temp_pic->width;
       }

      j += dx;
     }

//    return (TRUE);
    write_log_entry ("Offset = %ld,  l = %ld,  Lines Read = %d", ftell(f), l, k);
   }

	/* Close File And Return Success */
  fclose (f);
  return (TRUE);
 }
/*========== End of Function TEXTURE_TYPE::load_texture() =================*/


/*========== Returns TRUE if filename is a Valid Texture Name =============*/
boolean istexture (const char *filename) {

  if (strlen(filename) != 11 || strnicmp (filename, "TEXTURE", 7) != 0) return (FALSE);

	/* Make sure the Extension is Purely Numerical */
  if (!isdigit(filename[8]) || !isdigit(filename[9]) || !isdigit(filename[10])) return (FALSE);

  return (TRUE);
 }
/*========== End of function istexture() ==================================*/


/*========== Loads a DF game palette into memory ==========================*/
boolean load_dagpal (const char *filename, unsigned char *pal) {
  int count, ch;
  FILE *f;

	/* Attempt and open the file */
  if (!(f = openfile(filename, "rb"))) {
    dag_error_code = DAG_ERR_FILEOPEN;
    return (FALSE);
   }

	/* If palette is a COL file skip header information */
  if (get_filesize (f) == 776) fseek (f, 8, SEEK_SET);

	/* Read in the colour values */
  for (count = 0; count < 768; count++) {
    if ((ch = fgetc(f)) == EOF) break;
    *pal = ch;
    pal++;
   }

  fclose (f);

  if (count != 768) {
    dag_error_code = DAG_ERR_FILEREAD;
    return (FALSE);
   }

  return (TRUE);
 }
/*========= End of procedure load_dagpal() ================================*/


/*========== Saves a DF game palette to file ==============================*/
boolean save_dagpal (const char *filename, unsigned char *pal) {
  int count;
  FILE *f;

	/* Attempt and open the file */
  if (!(f = openfile(filename, "rb"))) {
    dag_error_code = DAG_ERR_FILEOPEN;
    return (FALSE);
   }

	/* Write the colour values */
  for (count = 0; count < 768; count++) {
    fputc (*pal, f);
    pal++;
   }

  fclose (f);
  return (TRUE);
 }
/*========= End of procedure save_dagpal() ================================*/


