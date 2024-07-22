	/* Version of Qedit */
#define VERSION "0.30b - 30 April 97"

	/* Array size for the QRC_DATA class. Number of different QRC texts
	 * which can be loaded at one time */
#define MAX_QRC_ARRAY 50

	/* Size of QBN-Section Arrays */
#define MAX_QBN0_ARRAY 16
#define MAX_QBN1_ARRAY 1
#define MAX_QBN2_ARRAY 1
#define MAX_QBN3_ARRAY 15
#define MAX_QBN4_ARRAY 15
#define MAX_QBN5_ARRAY 1
#define MAX_QBN6_ARRAY 15
#define MAX_QBN7_ARRAY 15
#define MAX_QBN8_ARRAY 150
#define MAX_QBN9_ARRAY 75
#define MAX_QBN10_ARRAY 30

	/* The edit modes for the QBN screen */
#define QBN_ITEMS 0
#define  QBN_NPCS 3
#define   QBN_LOC 4
#define   QBN_MOB 7

	/* Types of Carriage Returns found in QRC files */
#define      CR_CENTER 0xFD
#define TEXT_CR_CENTER '|'
#define        CR_LEFT 0xFC
#define   TEXT_CR_LEFT '<'

	/* Length of defined character strings */
#define  QTEXT_TITLE_WIDTH 24
#define  QTEXT_VALUE_WIDTH 3

	/* Define some types of quest texts... These appear to be the only
	 * set values...all others fluctuate wildly */
#define  QTEXT_UNKNOWN 0
#define     QTEXT_DESC 0x3E8
#define       QTEXT_NO 0x3E9
#define      QTEXT_YES 0x3EA
#define   QTEXT_FAILED 0x3EB
#define  QTEXT_SUCCESS 0x3EC
#define QTEXT_RUM_DURING 0x3ED
#define    QTEXT_RUM_SUC 0x3EE
#define   QTEXT_RUM_FAIL 0x3EF
#define  QTEXT_GRT_SUC 0x3F0
#define QTEXT_GRT_FAIL 0x3F1
#define      QTEXT_LOG 0x3F2
#define      QTEXT_END 0xFFFF

	/* Location/Size of Buttons */
#define QTEXT_BUTTON_WIDTH 16

	/* Location/Size of some Windows */
#define   EXIT_WIDTH 40
#define  EXIT_HEIGHT 9
#define  QTEXT_WIDTH 54
#define QTEXT_HEIGHT 9
#define   LOAD_WIDTH 46
#define  LOAD_HEIGHT 9
#define  ABOUT_WIDTH 72
#define ABOUT_HEIGHT 30
#define QBN_BOX_X 16
#define QBN_BOX_Y 4
#define QBN_BOX_WIDTH 60
#define QBN_BOX_HEIGHT 39

	/* Location/Size of some texts */
#define     GUILD_X 18
#define     GUILD_Y 4
#define GUILD_WIDTH 22

#define     RANK_X 18
#define     RANK_Y 5
#define RANK_WIDTH 3

#define     PC_STATUS_X 62
#define     PC_STATUS_Y 4
#define PC_STATUS_WIDTH 14

#define     QGIVEN_X 62
#define     QGIVEN_Y 5
#define QGIVEN_WIDTH 9

#define     REWARD_TYPE_X 39
#define     REWARD_TYPE_Y 10
#define REWARD_TYPE_WIDTH 9

#define     REWARD_X 39
#define     REWARD_Y 13
#define REWARD_WIDTH 20

#define     MSG_TYPE_X 39
#define     MSG_TYPE_Y 16
#define MSG_TYPE_WIDTH 18

#define     MSG_X 39
#define     MSG_Y 18
#define MSG_WIDTH 12

#define      FIND_MSG_X 23
#define      FIND_MSG_Y 19
#define  FIND_MSG_WIDTH 34
#define FIND_MSG_HEIGHT 12

#define     GENDER_X 39
#define     GENDER_Y 13

#define LOC_X 39
#define LOC_Y 11

#define MOB_X 39
#define MOB_Y 13



/* ==========================================================================
 *  Structure for holding data from QBN file
 * ========================================================================*/
class QBN_SECTION0 {

public:
  unsigned char section_index;
  unsigned int reward_type;
  unsigned long reward;
	 unsigned int rew1, rew2;
  unsigned long message_type;
  unsigned long null_bytes;
  unsigned int message_id1;
  unsigned int message_id2;
 };

class QBN_SECTION1 {

public:

  char far *data;
 };

class QBN_SECTION2 {

public:
  char far *data;
 };

class QBN_SECTION3 {

public:
  unsigned char section_index;
  unsigned int gender;
  unsigned char val1;
  unsigned int val2;
  unsigned int val3;
  unsigned long message_type;
  unsigned long null_bytes;
  unsigned int message_id1;
  unsigned int message_id2;
 };

class QBN_SECTION4 {

public:
  unsigned char section_index;
  unsigned int null1;
  unsigned char gen_location;
  unsigned int location;
  unsigned int val1;
  unsigned int val2;
  unsigned char val3;
  unsigned char val4;
  unsigned long message_type;
  unsigned long null2;
  unsigned int message_id1;
  unsigned int message_id2;
 };

class QBN_SECTION5 {

public:
  char far *data;
 };

class QBN_SECTION6 {

public:
  char far *data;
 };

class QBN_SECTION7 {

public:
  unsigned char section_index;
  unsigned int null1;
  unsigned char mob_index;
  unsigned int val1;
  unsigned long message_type;
  unsigned long null2;
 };

class QBN_SECTION8 {

public:
  char far *data;
 };

class QBN_SECTION9 {

public:
  char far *data;
 };


class QBN_SECTION10 {

public:
  char far *data;
 };




class QBN_DATA {

public:
  char header[15];	/* 1st 15-Usually NULL Bytes */
  char section10_status;
  int section_size[10]; /* Section Size values */
  int section_offset[10];	/* Section start offset values */

	/* The section data */
  QBN_SECTION0 section0[MAX_QBN0_ARRAY];
  QBN_SECTION1 section1[MAX_QBN1_ARRAY];
  QBN_SECTION2 section2[MAX_QBN2_ARRAY];
  QBN_SECTION3 section3[MAX_QBN3_ARRAY];
  QBN_SECTION4 section4[MAX_QBN4_ARRAY];
  QBN_SECTION5 section5[MAX_QBN5_ARRAY];
  QBN_SECTION6 section6[MAX_QBN6_ARRAY];
  QBN_SECTION7 section7[MAX_QBN7_ARRAY];
  QBN_SECTION8 section8[MAX_QBN8_ARRAY];
  QBN_SECTION9 section9[MAX_QBN9_ARRAY];
  QBN_SECTION10 section10[MAX_QBN10_ARRAY];

	/* Class Constructor */
  QBN_DATA (void);

	/* Class Destructor */
  ~QBN_DATA (void);

	/* Deletes All Data in the QBN_DATA Class */
  void delete_data (void);

	/* Attempts to Load a QBN File */
  boolean load (const char far *filename);

	/* Saves The QRC-Hopefully */
  boolean save (const char far *filename);
 };
/* ========= End of Class QBN_DATA ======================================= */


/* ==========================================================================
 *  How the QRC Texts are Displayed
 * ========================================================================*/
class QUEST_TEXT {

public:
  int x, y;		/* Location to print title+value */
  boolean visible;	/* Is this text displayed currently? */
  boolean active;	/* Is there any text in current array? */

	/* Class Constructor */
  QUEST_TEXT (void);

	/* Draws One Quest Text Entry on Screen */
  void draw (const int index);

 };
/* ========= End of Class QUEST_TEXT ===================================== */



/* ==========================================================================
 *  The QRC Texts are Described
 * ========================================================================*/
typedef struct {
  char desc[24];	/* The Description of the type */
  unsigned int type;		/* The numerical associated type */
 } QTEXT_DESC_TYPE;
/* ========= End of Class QUEST_TEXT_DESC ================================ */


/* ==========================================================================
 *  How the QRC Texts are Displayed - Array of them
 * ========================================================================*/
class QUEST_TEXT_ARRAY {

public:
  QUEST_TEXT texts[MAX_QRC_ARRAY];

	/* Class Constructor */
  QUEST_TEXT_ARRAY (void);

	/* Adds a Quest Text to End of Array */
  boolean add_text (const int x = 1, const int y = 1, const boolean visible = TRUE);

	/* Checks for a mouse click on any of the quest text items */
  boolean check (const int mx, const int my, const int event);

	/* Draws Visible Portions of the Array */
  void draw (void);
 };
/* ========= End of Class QUEST_TEXT_ARRAY ================================*/


/* =========================================================================
 *   Structure for holding data found in QRC header
 * ========================================================================*/
class QRC_HEADER {

public:
  unsigned int type;	/* Hex offset type, 0x03E9, etc... */
  unsigned int tag;	/* I use this to tag the text as a certain type */
  unsigned long offset;	/* The offset from beginning of QRC file to text beginning */

	/* Class Constructor */
  QRC_HEADER (void);

 };
/*========= End of Class QRC_HEADER Definition ============================*/


/* ==========================================================================
 * Holds all the data from the Loaded QRC file
 * ========================================================================*/
class QRC_DATA {

public:
  QRC_HEADER header[MAX_QRC_ARRAY];
  char far *text[MAX_QRC_ARRAY];

  int num_entries;

	/* Class Constructor */
  QRC_DATA (void);

	/* Class Destructor */
  ~QRC_DATA (void);

	/* Deletes All Allocated Texts Loaded From QRC */
  void delete_texts (void);

	/* Loads a QRC into memory */
  boolean load (const char far *filename);

	/* Attempts to save a QRC file */
  boolean save (const char far *filename);

 };
/* ========= End of CLASS QRC_DATA Definition ============================ */


/*===========================================================================
 *		     FUNCTION/PROCECURE PROTOTYPES
 *
 * For a better description of these routines, see the file QEDIT.CPP
 *=========================================================================*/

	/* Routines for various menu/button functions */
void click_about (void);
void click_exit (void);
void click_load (void);
void click_new (void);
void click_save (void);

boolean convert_edit_list (char far **text);

void convert_qtext (char far *text);

	/* Creates a new quest filename based on the current quests characteristics */
void determine_new_quest_filename (char far *string);

	/* Draws the Quest Edit Text window on Screen */
int draw_qtext_window (const char far *string);

	/* Displays the Filenames at top of Screen */
void draw_filenames (void);

	/* Redraws all the edit screen */
void draw_screen(void);

	/* Draws only the QBN data screen */
void draw_qbn_screen (void);

	/* Edits a Quest Text With a Given Message ID# */
void edit_msgid (const unsigned id);

	/* Exit procedure run when program halts */
void exit_qedit(void);

	/* Searches QRC Texts for a particular message type */
void find_msg (const char far *message);

	/* Returns the Quest text's Description */
char *get_qtext_desc (unsigned int type);

	/* Takes values in Drag-Lists and tranfers them to the QBN_DATA */
void get_qbn_data (void);

	/* Does everything needed to init program */
void init_dfqedit(void);

	/* Loads a Data file for a Dual List Values */
boolean load_dual_list (const char *filename, LIST_BOX *list);
boolean load_guilds (const char *filename, LIST_BOX *list);

	/* Loads the Message Data file */
boolean load_msg (const char *filename);

	/* Converts a Message Name into a Hash Value */
long msg_hash (const char far *msg);

	/* Resets all quest values to default */
void new_quest(void);

	/* Translates a Quest filename into Screen List Values */
void parse_filename (const char far *filename);

	/* Translates the QBN data into list values */
void parse_qbn_data (void);

	/* Removes QRC/QBN file from inputted text */
void parse_quest_filename(char far *filename);

/*===========================================================================
 *		     END FUNCTION/PROCECURE PROTOTYPES
 *=========================================================================*/


/*===========================================================================
 *			EXTERNAL VARIABLES DEFINITIONS
 *
 * For a description of these variables, see the file QEDIT.CPP
 *=========================================================================*/
extern long int initial_far_heapsize;
/*===========================================================================
 * 			END EXTERNAL VARIABLES DEFINITIONS
 *=========================================================================*/

