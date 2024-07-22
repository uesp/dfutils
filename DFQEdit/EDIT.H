#if defined(TRADITIONAL)
#define const
#define args( list )			( )
#define DECLARE_DO_MENU( menu )		void menu( )
#else
#define args( list )			list
#define DECLARE_DO_MENU( menu )		DO_MENU    menu
#endif

//Add text where in line...
#define    END 0
#define MIDDLE 1
#define  START 2

//Which kind of delete
#define    DELETE 0
#define BACKSPACE 1


//Define special colors for QRC text codes
#define         CR_TEXT_COLOR YELLOW
#define         CR_BACK_COLOR BLUE
#define    _STRING_TEXT_COLOR YELLOW
#define    _STRING_BACK_COLOR BLACK
#define   __STRING_TEXT_COLOR LIGHTBLUE
#define   __STRING_BACK_COLOR BLACK
#define  ___STRING_TEXT_COLOR LIGHTGREEN
#define  ___STRING_BACK_COLOR BLACK
#define ____STRING_TEXT_COLOR LIGHTMAGENTA
#define ____STRING_BACK_COLOR BLACK
#define PER_STRING_TEXT_COLOR RED
#define PER_STRING_BACK_COLOR LIGHTGRAY
#define  EQ_STRING_TEXT_COLOR BLACK
#define  EQ_STRING_BACK_COLOR CYAN
#define   END_PART_TEXT_COLOR WHITE
#define   END_PART_BACK_COLOR MAGENTA

	//Define max string length for edit code
#define MAX_EDIT_LENGTH 80


typedef	void DO_MENU args( (char *argument ) );
typedef struct edit_type EDIT_TYPE;
typedef struct menu_type MENU_TYPE;


struct edit_type {
  char far data[MAX_EDIT_LENGTH+1];
  EDIT_TYPE far *next;
  EDIT_TYPE far *prev;
 };

struct menu_type {
  int x;
  int y;
  int width;
  int height;
  char title[12];
  char name[8][20];
  DO_MENU *do_menu[8];
 };


void combine_lines (EDIT_TYPE far *line);
void create_new_end (char *string);
void create_new_line (char *string);
void delete_all_editlines (void);
void delete_edit (EDIT_TYPE far *line);
void do_editmenu (MENU_TYPE *menu);
void draw_all_lines (EDIT_TYPE far *line, int type);
void draw_edit_line (EDIT_TYPE far *line, int y);
void draw_editmenu (MENU_TYPE *menu);
void draw_editmenu_item (MENU_TYPE *menu, int item);
void draw_edit_screen (const char *title);
void edit_addchar (EDIT_TYPE far *line, char ch);
void edit_delchar (EDIT_TYPE far *line, int type);
boolean edit_isalpha(char ch);
void editmenu_help(char *argument);
void editmenu_load (char *argument);
void editmenu_new (char *argument);
void editmenu_quit (char *argument);
void editmenu_save (char *argument);
boolean edit_scanf (int x, int y, int length, char *string);
int find_mouse_menu (const int x, const int y, MENU_TYPE *menu);
long int get_editsize(void);
boolean init_edit (void);
boolean load_editfile (char *filename);
void parse_edit_line(char *string);
boolean save_editfile (char *filename);
void text_editor(const char *title);

	/* External Variables */
extern int max_length;
extern EDIT_TYPE far *edit_line, far *edit_start;
extern char orig_directory[];
extern int orig_drive;
