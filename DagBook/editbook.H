
	/* Type of Screen Redraws */
#define      DRAW_ALL 0
#define  DRAW_CURRENT 1
#define DRAW_PREVIOUS 2

	/* Which kind of delete */
#define    DEL_DELETE 0
#define DEL_BACKSPACE 1

	/* Hmm... */
#define BOOKEDIT_VERSION "0.30 - Dec 1997"

	/* For selecting things */
#define SELECT_NONE 0
#define SELECT_START 1
#define SELECT_MIDDLE 2
#define SELECT_END 3
#define SELECT_BOTH 4

	/* Default strings and characters */
#define   CENTER_CHAR '|'
#define      HEX_CHAR '#'
#define END_PAGE_CHAR '~'
#define END_PAGE_STRING "~"
#define  FANCY_FONT_STRING "{FancyFont}"
#define NORMAL_FONT_STRING "{NormFont}"

	/* Define special colors for QRC text codes */
#define         CR_TEXT_COLOR YELLOW
#define         CR_BACK_COLOR BLUE
#define       FONT_TEXT_COLOR YELLOW
#define       FONT_BACK_COLOR BLACK
#define   END_PAGE_TEXT_COLOR WHITE
#define   END_PAGE_BACK_COLOR MAGENTA
#define        HEX_TEXT_COLOR WHITE
#define        HEX_BACK_COLOR RED
#define     SELECT_TEXT_COLOR BLACK
#define     SELECT_BACK_COLOR LIGHTGRAY

	/* Define max string length for edit code */
#define MAX_EDIT_LENGTH 77

	/* Type of Line combines */
#define COMBINE_NEXT 0
#define COMBINE_PREV 1

	/* Shortcut for moving to current position in window */
#define GOTO_CURSOR gotoxy (current_char + x + 1, current_line + y + 1 - view)


/*========== Define the Doubly Linked List Class EDIT_TYPE ================*/
class EDIT_TYPE {

public:
  char far data[MAX_EDIT_LENGTH + 1];
  EDIT_TYPE *next;
  EDIT_TYPE *prev;

	/* Class Constructor and Destructors */
  EDIT_TYPE (void);
  ~EDIT_TYPE (void);

	/* Destroys the Linked List */
  void destroy (void);

 };
/*========== End of Class EDIT_TYPE =======================================*/


/*========== Contains Everything Needed for a Complete Editor =============*/
class EDIT_LIST {

public:
  EDIT_TYPE *edit_head, *edit_tail;	/* The First and last lines of the linked list */
  EDIT_TYPE *edit_view;			/* Top of the viewable page */
  EDIT_TYPE *edit_current;              /* The Current Line */
  EDIT_TYPE clipboard;			/* Contains copy/paste/cut data */
  EDIT_TYPE *end_clip;
  char *edit_filename;			/* Filename of current file, if any */
  boolean file_loaded;
  boolean modified;			/* Modified since last save? */
  boolean pressed;
  boolean shift_pressed;
  boolean start_fixed;
  int num_lines;			/* Total Number of lines */
  int start_col;			/* Starting column of text for display */
  int view;
  int current_line;			/* Numerical location of cursor */
  int current_char;			/* Relative to top of document */
  int x, y;				/* Upper-Left Corner of Text Window */
  int width, height;			/* Size of Text Window */

	/* For selecting text */
  int start_select_char, start_select_line;
  int end_select_char, end_select_line;

	/* The Main Menus */
  MENU_TYPE file_menu;
  MENU_TYPE edit_menu;
  MENU_TYPE help_menu;

	/* The Main Scroll Bar */
  SCROLL_TYPE scroll_bar;

	/* Class Constructor and Destructor */
  EDIT_LIST (const int x1, const int y1, const int w, const int h);
  ~EDIT_LIST (void) { destroy(); clip_destroy(); };

	/* Adds a character to The Current Line */
  void add_char (const char ch);

	/* Adds a tail to the clipboard list */
  void add_cliptail (const char *string);

	/* Add a New Node at the Current Line */
  void add_current_line (const char *string = NULL);

	/* Adds a String at the Current Location */
  void add_string (const char *string, const boolean draw = TRUE);

	/* Adds a New Node at the Tail */
  void add_tail (const char *string = NULL);

	/* Adds a line break at the current position */
  void break_line (const boolean draw = TRUE);

	/* Special Variation of the clreol() function */
  void clear_eol (void);

	/* Destroys the clipboard list */
  void clip_destroy (void);

	/* Attempts to combine to adjacent lines */
  void combine_lines (const int combine_type);

	/* Copies any selected lines into clipboard */
  void copy (void);

	/* Cuts any Selected Lines to the Clipboard */
  void cut (void);

	/* Creates a new node but doesn't insert it in list....yet */
  EDIT_TYPE *create_line (const char *string = NULL);

	/* Deletes a character from the current position */
  void del_char (const int del_type);

	/* Properly Deletes a Node in the List, list_current */
  void delete_node (void);

	/* Deletes the List */
  void destroy(void);

	/* Redraws all the text in the window */
  void draw_lines (const int draw_type);

	/* Draws The Current Edit Line */
  void draw_current_line (void);

	/* Draws the border etc... of the edit Window */
  void draw_edit_window (const char *title);

	/* Updates the Position Status Line at the Bottom of the Screen */
  void draw_status_line ();
  void draw_filename ();

	/* Returns String Length of All Strings in Edit List */
  unsigned long get_editsize (void);

	/* We Will Always Need At Least One Line Allocated Initially */
  void init (const char *string = NULL);

	/* Moves to a given line */
  boolean moveline_rel (const int rel_line, const boolean draw = TRUE) { return (moveline_abs(current_line + rel_line, draw)); };
  boolean moveline_abs (const int new_line, const boolean draw = TRUE);

	/* Moves to a given character */
  boolean movechar_abs (const int new_char, const boolean draw = TRUE);
  boolean movechar_rel (const int rel_char, const boolean draw = TRUE) { return (movechar_abs(current_char + rel_char, draw)); };

	/* Moves the Cursor to a New Position relative to the screen */
  void move_cursor (int mx, int my, const boolean draw = TRUE);

	/* Parses An Edit Line, Printing in Color? */
  void parse_edit_line(const char *string, const int line);

	/* Pastes the clipboard contents at the current location */
  void paste (void);

	/* Procedure For Selecting Text Lines */
  void select_text (const int mx, const int my);

	/* Main loop for the text editor */
  void text_editor(const char *title);

 };
/*========== End of Class EDIT_LIST Definition ============================*/


/*========== Function and Procedure Prototypes ============================*/

	/* Attempts to Convert a book format into the edit text */
  boolean convert_book (DAGBOOK_TYPE *book);

	/* Attempts to Convert some Text into a Book File */
  boolean convert_text (DAGBOOK_TYPE *book);

	/* Custom function to determine if a character is printable or not */
  boolean edit_isalpha(char ch);

	/* Special String Search Function */
  int edit_strrchr (char *string, int start);

	/* The Various Menu functions */
  void editmenu_help(__CPPARGS);
  void editmenu_load (__CPPARGS);
  void editmenu_loadbook (__CPPARGS);
  void editmenu_new (__CPPARGS);
  void editmenu_quit (__CPPARGS);
  void editmenu_save (__CPPARGS);
  void editmenu_savebook (__CPPARGS);
  void editmenu_author (__CPPARGS);
  void editmenu_title (__CPPARGS);
  void editmenu_fontfancy (__CPPARGS);
  void editmenu_fontnormal (__CPPARGS);
  void editmenu_copy (__CPPARGS);
  void editmenu_cut (__CPPARGS);
  void editmenu_paste (__CPPARGS);
  void editmenu_endpage (__CPPARGS);
  void editmenu_center (__CPPARGS);

	/* Loads a Normal Text File */
  boolean load_editfile (char *filename);

	/* Saves a normal text file */
  boolean save_editfile (char *filename);

/*========== End of Function and Procedure Prototypes =====================*/

