#ifndef __DAGBUTIL_H
#define __DAGBUTIL_H

	/* Various Record Lengths */
#define TITLE_WIDTH 64
#define AUTHOR_WIDTH 160
#define MAX_PAGES 100

	/* Some error types for this code specifically */
#define DAGB_ERR_NONE 0
#define DAGB_ERR_FILE 1
#define DAGB_ERR_READ 2
#define DAGB_ERR_MAXPAGES 3
#define DAGB_ERR_PAGESIZE 4
#define DAGB_ERR_FORMAT 5


/*========== The Class for Holding a DAGGERFALL Book File =================*/
class DAGBOOK_TYPE {

public:
  char title[TITLE_WIDTH + 1];
  char author[AUTHOR_WIDTH + 1];

	/* Some Header Stuff */
  char header[10];
  unsigned int num_pages;
  unsigned long page_offsets[MAX_PAGES];

	/* The Actual Text Data */
  char *pages[MAX_PAGES];

	/* Class Constructor and Destructor */
  DAGBOOK_TYPE (void);
  ~DAGBOOK_TYPE (void) { destroy(); };

	/* Deletes all the pages */
  void destroy (void);

	/* Init variables with default values */
  void init (void);

	/* Attempts to load a book into memory */
  boolean load_book (const char *filename);

	/* Attempts to Save a Book File to File */
  boolean save_book (const char *filename);

 };
/*========== End of Class DAGBOOK_TYPE ====================================*/


/*========== Function / Procedure Prototypes ==============================*/

	/* Tests to See if the File is a Book File or Not */
  boolean check_book_file (FILE *f);

/*========== End of Function/Procedure Prototypes =========================*/


/*========== Some External Variables ======================================*/

	/* Holds the current error status */
  extern int dagb_error;

	/* Error Messages */
  extern char *dagbutil_errmsg[];

/*========== End of External Variables ====================================*/


#endif			/* __DAGBUTIL_H */