#define VERSION "0.30"	//Hmm...
#define MAX_IMAGES 100   //Number of images which can loaded at one time
#define MAX_STRING_LENGTH 100
#define RLE_BYTE 162	//The 'hinge' byte of a RLE encoded file
#define MAX_THT 101
#define MAX_GROUPS 50
#define MAX_SUBGROUPS 10


////Begin Class Definitions

//========== BEGIN CLASS PCX_HEADER DEFINITION ==============================
class PCX_HEADER {

public:
  char manufacturer;
  char version;
  char encoding;
  char bits_per_pixel;
  int x, y;
  int width, height;
  int horz_res;
  int vert_res;
  char ega_palette[48];
  char reserved;
  char num_color_planes;
  int bytes_per_line;
  int palette_type;
  char padding[58];
 };
//========== END CLASS PCX_HEADER DEFINITION ================================


//========== CLASS PCX_PICTURE DEFINITION ===================================
class PCX_PICTURE {

public:
  PCX_HEADER header;
  char far *buffer;
 };
//========== END CLASS PCX_PICTURE DEFINITION ===============================


//========== BEGIN CLASS DAG_PICTURE DEFINITION =============================
class DAG_PICTURE {

public:
  int x_offset, y_offset;	//Offset?
  unsigned int width, height;	//Size of image in pixels
  long int image_size;		//Size of image in pixels
  char far *buffer;		//Picture data
  boolean rle;			//Is current pic saved with RLE encoded or not

  void init (void);	//Initializes the picture element
  void destroy (void);	//Deletes the picture element
  void save_pcx (const char *filename, char *pal);
 };
//========== END CLASS DAG_PICTURE DEFINITION ===============================


//========== CLASS TEXTURE_HEADER DEFINITION ================================
class TEXTURE_HEADER {

public:
  unsigned int type1;  		//Type?
  unsigned long offset;		//Offset to picture from beginning of file
  unsigned int type2;		//Another type?
  char far extra_header[12];	//Extra header info?
 };
//========== END OF CLASS TEXTURE_HEADER DEFINITION =========================


//========== CLASS TEXTURE_PIC DEFINITITION =================================
class TEXTURE_PIC : public DAG_PICTURE {

public:
  char far extra_header[20];

  TEXTURE_PIC (void);	//Constructor
  ~TEXTURE_PIC (void);	//Destructor

 };
//========== END CLASS TEXTURE_PIC DEFINITION ===============================


//========== CLASS TEXTURE_TYPE DEFINITION ==================================
class TEXTURE_TYPE {

public:
  unsigned int type;			//Type of texture file?
  char far text[26];			//Text description
  TEXTURE_HEADER far *header[MAX_IMAGES];//The header for the picture info
  TEXTURE_PIC far *pics[MAX_IMAGES];	//Contains the actual texture pictures
  char far *extra_file;           	//Data + length of any extra file
  unsigned long extra_file_length;

  void init (void);
  void destroy (void);
 };
//========== END CLASS TEXTURE_TYPE DEFINITION ==============================

//// End of Class Definitions




//// Begin Function/Procedure prototypes
void display_help  (void);
boolean edit_scanf (int x, int y, int length, char *string);
int find_color     (char *pal);
boolean input	   (char *pcx_filename, char *img_filename, char *bak_filename, int *image_pos);
boolean istexture  (const unsigned int i);
boolean load_config(char *filename);
boolean load_pcx   (const char *filename, PCX_PICTURE *image);
void load_dagpal   (const char *filename, char *pal);
boolean load_dagpic(const char *filename, DAG_PICTURE *picture_ptr, int *num_pic, char *pal);
boolean load_font  (char *userfont);
void mod_savefile  (char *buffer, int i);
boolean overwrite  (char *filename);
void put_dagpic    (int x, int y, unsigned width, unsigned height, char far *pic);
boolean save_img   (const char *filename, DAG_PICTURE *pic, PCX_PICTURE *pcx_pic, int num_pic, const int image_pos);
void save_pcx      (const char *filename, DAG_PICTURE *pic, char *pal);
boolean save_texture(const char *filename, TEXTURE_TYPE *pic, PCX_PICTURE *pcx_pic, int num_pic, const int image_pos);


