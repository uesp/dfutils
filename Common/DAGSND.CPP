	/* User Defined Includes */
#include "fileutil.h"

	/* Daggerfall specific includes */
#include "daggen.h"
#include "dagsnd.h"

#define DEBUG TRUE

#define OFFSET (184)

	/* Data for the MIDI.BSA file */
DAGMIDI_TYPE dagmidi;


/*========== Class DAGTRACK Constructor ===================================*/
DAGTRACK::DAGTRACK (void) {
  data = NULL;
  offset = length = -1;
 }
/*========== End of Class DAGTRACK Constructor ============================*/


/*========== Class DAGTRACK Destructor ====================================*/
void DAGTRACK::destroy (void) {
  DESTROY(data);
  offset = length = -1;
 }
/*========== End of Class DAGTRACK Destructor =============================*/


/*========== Class DAGMIDI_RECORD Constructor =============================*/
DAGMIDI_RECORD::DAGMIDI_RECORD (void) {
  short i;

  for (i = 0; i < MAX_TRACKS; i++)
    tracks[i] = NULL;

  num_tracks = 0;
  length = offset = -1;
  file_type = 0;
  tick_size = 0x1e0;
  name[0] = 0;
  name[14] = 0;
 }
/*========== End of Class DAGMIDI_RECORD Constructor ======================*/


/*========== Class DAGMIDI_RECORD Destructor ==============================*/
void DAGMIDI_RECORD::destroy (void) {
  short i;

  for (i = 0; i < MAX_TRACKS; i++) {
    DESTROY(tracks[i]);
   }

  num_tracks = 0;
  offset = length = -1;
  tick_size = 0x1e0;
  file_type = 1;
 }
/*========== End of Class DAGMIDI_RECORD Destructor =======================*/


/*========== Class DAGMIDI_TYPE Constructor ===============================*/
DAGMIDI_TYPE::DAGMIDI_TYPE (void) {
  short i;

  for (i = 0; i < MAX_MIDIS; i++)
    records[i] = NULL;

  index = 0x100;
  num_records = 0;
  length = -1;
 }
/*========== End of Class DAGMIDI_TYPE Constructor ========================*/


/*========== Class DAGMIDI_TYPE Destructor ================================*/
void DAGMIDI_TYPE::destroy (void) {
  short i;

  for (i = 0; i < MAX_MIDIS; i++) {
    DESTROY(records[i]);
   }

  num_records = 0;
  length = -1;
  index = 0x100;
 }
/*========== End of Class DAGMIDI_TYPE Destructor =========================*/


/*========== Attempts to Extract Midi Data and Save it as a .MID File =====*/
boolean DAGMIDI_TYPE::extract_midi (const char *filename, const char *midifile, const short index) {
  FILE *fi, *fo;	/* Input and output file pointers */
  DAGMIDI_RECORD *rec;	/* Temp pointer for speed and easier code */
  char *data;
  short i;
  long l;

	/* Make sure the filenames are valid */
  if (!filename || !midifile) {
    df_err_code = DF_ERR_FILE;
    return (FALSE);
   }

	/* Make sure the index is valid */
  if (index < 0 || index >= num_records) {
    df_err_code = DF_ERR_RECORDSIZE;
    return (FALSE);
   }

  rec = records[index];

	/* Open the Midi File to input Data */
  if (!(fi = openfile (filename, "rb"))) {
    df_err_code = DF_ERR_FILE;
    return (FALSE);
   }

	/* Open the output .MID file */
  if (!(fo = openfile (midifile, "wb"))) {
    df_err_code = DF_ERR_FILE;
    fclose (fi);
    return (FALSE);
   }

	/* Write the header for the .MID File */
  if (fwrite ("MThd\0\0\0\6", sizeof(char), 8, fo) != 8) goto EXTRWRTERR;

  mid_write_int (fo, rec->file_type);
  mid_write_int (fo, rec->num_tracks + 1);
  mid_write_int (fo, rec->tick_size);

	/* Write the initial track with various information */
  if (fwrite ("MTrk", sizeof(char), 4, fo) != 4) goto EXTRWRTERR;
  mid_write_long (fo, 28);
  fputc (0, fo);
  mid_write_long (fo, 0xff540501l);
  fputc (0, fo);
  mid_write_long (fo, 0);
  mid_write_long (fo, 0xff580404l);
  mid_write_long (fo, 0x02180800l);
  mid_write_long (fo, 0xff510308l);
  mid_write_long (fo, 0xB82400ffl);
  mid_write_int (fo, 0x2f00);

	/* Create the temp input buffer */
  data = create_ptr (64000u);

	/* Read and write all the track data */
  for (i = 0; i < rec->num_tracks; i++) {

		/* Jump to start of Midi data */
    fseek (fi, rec->offset + rec->tracks[i]->offset + OFFSET, SEEK_SET);

#if DEBUG
    write_log_entry ("   Reading Track %d from 0x%08Xl", i, ftell(fi));
#endif

		/* Read the track from the MIDI.BSA file */
    l = rec->tracks[i]->length - OFFSET;

    if (l <= 0 || l > 64000u) {
      delete data;
      write_log_entry ("Invalid Midi data length of %ld received at offset 0x%08lX", l, ftell(fi));
      fclose (fi);
      fclose (fo);
      df_err_code = DF_ERR_RECORDSIZE;
      return (FALSE);
     }

    if (fread (data, sizeof(char), (unsigned)l, fi) != l) goto EXTRREADERR;

		/* Write the track to the .MID file */
    if (fwrite ("MTrk", sizeof(char), 4, fo) != 4) goto EXTRWRTERR;
    mid_write_long (fo, l);
    if (fwrite (data, sizeof(char), (unsigned)l, fo) != l) goto EXTRWRTERR;
   }


  delete data;
  fclose (fi);
  fclose (fo);
  return (TRUE);

EXTRWRTERR:
  fclose (fi);
  fclose (fo);
  df_err_code = DF_ERR_WRITE;
  return (FALSE);

EXTRREADERR:
  fclose (fi);
  fclose (fo);
  df_err_code = DF_ERR_READ;
  return (FALSE);
 }
/*========== End of Function DAGMIDI_TYPE::extract_midi() =================*/


/*========== Loads all the Header information from the MIDI.BSA file ======*/
boolean DAGMIDI_TYPE::load_header (const char *filename) {
  FILE *f;		/* File pointer */
  long offset;
  short i, j, k;	/* Loop counters */
  char buf[24];		/* Temp input buffer */

	/* Make sure we have a valid filename */
  if (!filename) {
    df_err_code = DF_ERR_FILE;
    return (FALSE);
   }

	/* Attempt to open file for input */
  if (!(f = openfile (filename, "rb"))) {
    df_err_code = DF_ERR_FILE;
    return (FALSE);
   }

	/* Clear the current contents */
  destroy();

	/* Read in the number of records */
  num_records = read_int (f);

#if DEBUG
  write_log_entry ("%d Songs in File", num_records);
#endif

	/* Read in the unknown index field */
  index = read_int (f);

	/* Make sure we haven't exceeded the EOF */
  if (feof(f)) goto MIDIREADERR;

	/* Attempt to read in all the midi record sizes and names */
  fseek (f, -18 * num_records, SEEK_END);

  for (i = 0; i < num_records; i++) {

		/* Attempt to Allocate the new record */
    if (!(records[i] = new DAGMIDI_RECORD))
      bug (MEM_ERR_MSG, "DAGMIDI_TYPE::load_header() - *records[] (%d)", sizeof(DAGMIDI_RECORD));

		/* Attempt to read in the filename of Midi song */
    if (fread (records[i]->name, sizeof(char), 14, f) != 14) goto MIDIREADERR;

		/* Read in the length of song in bytes */
    records[i]->length = read_long(f);

#if DEBUG
    write_log_entry ("Song %3d: Name = '%014s', Length = 0x%08lX", i, records[i]->name, records[i]->length);
#endif
   }

	/* Attempt to read in all the midi record headers */
  fseek (f, 4, SEEK_SET);

  for (i = 0; i < num_records; i++) {

		/* Read in the starting offset of header */
    offset = records[i]->offset = ftell(f);

#if DEBUG
    write_log_entry ("Reading Song %3d (0x%08lX) (%s)", i, offset, records[i]->name);
#endif

		/* Read in the text header */
    if (fread (buf, sizeof(char), 19, f) != 19) goto MIDIREADERR;

		/* Make sure it is correct */
    if (strcmp(buf, "HMI-MIDISONG061595")) {
      fclose (f);
      write_log_entry ("Unknown Midi Text Entry: '%s'!", buf);
      df_err_code = DF_ERR_MIDISONG;
      return (FALSE);
     }

		/* Jump to tick offset */
    fseek (f, offset + 210, SEEK_SET);
    records[i]->tick_size = read_int (f);
    if (feof(f)) goto MIDIREADERR;

#if DEBUG
    write_log_entry ("     Read Tick Size of 0x%04X", records[i]->tick_size);
#endif

		/* Read in file type? */
    fseek (f, offset + 227, SEEK_SET);
    if ((records[i]->file_type = fgetc(f)) == -1) goto MIDIREADERR;

#if DEBUG
    write_log_entry ("     Read File Type of 0x%02X", records[i]->file_type);
#endif

		/* Read in number of tracks */
    k = records[i]->num_tracks = read_int(f);

#if DEBUG
    write_log_entry ("     Number of Tracks = %d", k);
#endif

		/* Make sure the number of tracks is valid */
    if (k <= 0 || k >= MAX_TRACKS) {
      write_log_entry ("Song %d(%ld): Invalid number of tracks read!", i, ftell(f) - 2);
      fclose (f);
      df_err_code = DF_ERR_RECORDSIZE;
      return (FALSE);
     }

		/* Read in the offsets to the track data */
    fseek (f, offset + 370, SEEK_SET);

    for (j = 0; j < k; j++) {

		/* Create the track */
      if (!(records[i]->tracks[j] = new DAGTRACK))
	bug (MEM_ERR_MSG, "DAGMIDI_TYPE::load_header() - *tracks[] (%d)", sizeof(DAGTRACK));

		/* Read in the offset */
      records[i]->tracks[j]->offset = read_long (f);
     }

    if (feof(f)) goto MIDIREADERR;

		/* Set the lengths of the tracks (except the last) */
    for (j = 0; j < k - 1; j++) {
      records[i]->tracks[j]->length = records[i]->tracks[j+1]->offset - records[i]->tracks[j]->offset;

#if DEBUG
      write_log_entry ("     Track %d is at offset 0x%08lX (Length = %ld)", j, records[i]->tracks[j]->offset, records[i]->tracks[j]->length);
#endif
     }

		/* Find the length of the last track */
    records[i]->tracks[k - 1]->length = records[i]->length - 16 - records[i]->tracks[k - 1]->offset;

#if DEBUG
    write_log_entry ("     Track %d is at offset 0x%08lX (Found Length = %ld)", k- 1, records[i]->tracks[k - 1]->offset, records[i]->tracks[k - 1]->length);
    write_log_entry ("   Length of Song = %ld bytes", records[i]->length);
#endif

	/* Move to the end of the song */
    fseek (f, offset + records[i]->length, SEEK_SET);

	/* Make sure we haven't gone past the EOF yet */
    if (feof(f)) goto MIDIREADERR;
   }

#if DEBUG
  write_log_entry ("Length of Midi File = %ld bytes", ftell(f));
  write_log_entry ("True Length of File = %ld bytes", get_filesize(f));
  write_log_entry ("Length Unaccounted for = %ld bytes", get_filesize(f) - ftell(f));
#endif

	/* Set the total length of the Midi file */

  fclose (f);
  return (TRUE);

MIDIREADERR:
  fclose (f);
  df_err_code = DF_ERR_READ;
  return (FALSE);
 }
/*========== End of Function DAGMIDI_TYPE::load_header() ==================*/