

#define VERSION "0.30"
#define MAX_IMAGES 100
#define MAX_STRING_LENGTH 100
#define RLE_BYTE 162
#define MAX_THT 101
#define MAX_GROUPS 50
#define MAX_SUBGROUPS 10



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
 };
//========== END CLASS DAG_PICTURE DEFINITION ===============================



//// Begin Function/Procedure prototypes
void display_help  (void);
boolean edit_scanf (int x, int y, int length, char *string);
int find_color     (char *pal);
boolean load_config(char *filename);
void load_dagpal   (const char *filename, char *pal);
boolean load_dagpic(const char *filename, DAG_PICTURE *picture_ptr, int *num_pic, char *pal);
boolean load_font  (char *userfont);
void put_dagpic    (int x, int y, unsigned width, unsigned height, char far *pic);


