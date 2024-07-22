#ifndef __DAGSND_H
#define __DAGSND_H


	/* Number of songs we can load */
#define MAX_MIDIS 200

	/* Number of tracks allowed in a song */
#define MAX_TRACKS 50

	/* Strange writing defines for the MID file */
#define mid_write_int(f, i) fputc(((i)>>8) & 0xff, f); fputc((i) & 0xff, f);
#define mid_write_long(f, l) fputc((int)(((l)>>24)&0xff), f); fputc((int)(((l)>>16)&0xff), f); fputc ((int)(((l)>>8)&0xff), f); fputc ((int)((l)&0xff), f);


/*========== Begin Class DAGTRACK =========================================*/
class DAGTRACK {

public:
  char *data;		/* Pointer to the actual track data */
  long length;		/* Size of track data */
  long offset;		/* Offset of track data from record start */

	/* Class Constructor and Destructors */
  DAGTRACK (void);
  ~DAGTRACK (void) { destroy(); }
  void  destroy(void);
 };
/*========== End of Class DAGTRACK ========================================*/


/*========== Begin Class DAGMIDI_RECORD ===================================*/
class DAGMIDI_RECORD {

public:
  char name[15];		/* Filename of the midi */
  long offset;			/* Offset of song from file start */
  long length;			/* Total size of record */
  short file_type;		/* Type of midi file */
  short tick_size;		/* Size of Delta-Ticks in Midi */
  short num_tracks;		/* Number of tracks in record */

  DAGTRACK *tracks[MAX_TRACKS];	/* The actual track data */

	/* Class Constructor and Destructors */
  DAGMIDI_RECORD (void);
  ~DAGMIDI_RECORD (void) { destroy(); }
  void destroy (void);

 };
/*========== End of Class DAGMIDI_RECOCD ==================================*/


/*========== Begin Class DAGMIDI_TYPE =====================================*/
class DAGMIDI_TYPE {

public:
  short index;			/* Unknown Data */
  short num_records;		/* Number of records currently loaded */
  long length;			/* Total size of file */
  DAGMIDI_RECORD *records[MAX_MIDIS];	/* Record data */

	/* Class Constructor and Destructors */
  DAGMIDI_TYPE (void);
  ~DAGMIDI_TYPE (void) { destroy(); }
  void destroy (void);

	/* Attempts to extract the Midi data and save as a .MID file */
  boolean extract_midi (const char *filename, const char *midifile, const short index);

	/* Loads all the header information for the Midi File */
  boolean load_header (const char *filename);

 };
/*========== End of Class DAGMIDI_TYPE ====================================*/


/*========== External Variables ===========================================*/
  extern DAGMIDI_TYPE dagmidi;
/*========== End of External Variable Definitions =========================*/

#endif	/* End of DAGSND_H */