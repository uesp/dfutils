
#include "fileutil.h"

	/* The byte values we are looking for */
#define VAL_179 0x3e8
#define VAL_195 0x1f4
#define VAL_213 0x2ee



FILE *f_179, *f_213, *f_195;	/* Input FALL.EXE files */
boolean done = FALSE;
int ch = 0, ch1, i;
int pre, post;
int pre1, post1;
boolean ok = TRUE;
long l;


long count = 0;



typedef struct {
  int pre1;
  int pre;
  int post;
  int post1;
  long offset;
 } dffind_t;

dffind_t dag_213[200];
int num_213 = 0;
dffind_t dag_195[200];
int num_195 = 0;


/*========== Searches Through the File ====================================*/
void find_213 (void) {


	/* Return to beginning of file */
  fseek (f_213, 0, SEEK_SET);
  ok = TRUE;
  ch = 0;

  while (ok) {
    pre1 = pre;
    pre = ch;
    ch = fgetc(f_213);

    if (ch == 0xb8) {
      ch1 = fgetc(f_213);

      if (ch1 == EOF)
	ok = FALSE;
      else if (ch1 == 0x0b) {
	l = ftell(f_213) - 2;
	post = fgetc(f_213);
	post1 = fgetc(f_213);
	write_log_entry ("%3d) Found v2.13 (0x%08lX): 0x%02X 0x%02X  0x%02X 0x%02X", num_213, l, pre1, pre, post, post1);
	dag_213[num_213].pre = pre;
	dag_213[num_213].pre1 = pre1;
	dag_213[num_213].post = post;
	dag_213[num_213].post1 = post1;
	dag_213[num_213].offset = l;
	num_213++;
	ch = post1;
       }
      else
	ch = ch1;
     }
    else if (ch == EOF
    )
      ok = FALSE;

   }

  write_log_entry ("%d 0x0bb8 entries found in v2.13", num_213);
 }
/*========== End of Procedure find_213() ==================================*/



/*========== Searches Through the File ====================================*/
void find_195 (void) {


	/* Return to beginning of file */
  fseek (f_195, 0, SEEK_SET);
  ok = TRUE;
  ch = 0;

  while (ok) {
    pre1 = pre;
    pre = ch;
    ch = fgetc(f_195);

    if (ch == 0xd0) {
      ch1 = fgetc(f_195);

      if (ch1 == EOF)
	ok = FALSE;
      else if (ch1 == 0x07) {
	l = ftell(f_195) - 2;
	post = fgetc(f_195);
	post1 = fgetc(f_195);
	write_log_entry ("%3d) Found v1.95 (0x%08lX): 0x%02X 0x%02X  0x%02X 0x%02X", num_195, l, pre1, pre, post, post1);
	dag_195[num_195].pre = pre;
	dag_195[num_195].pre1 = pre1;
	dag_195[num_195].post = post;
	dag_195[num_195].post1 = post1;
	dag_195[num_195].offset = l;
	num_195++;
	ch = post1;
       }
      else
	ch = ch1;
     }
    else if (ch == EOF)
      ok = FALSE;

   }

  write_log_entry ("%d 0x07d0 entries found in v1.95", num_195);
 }
/*========== End of Procedure find_213() ==================================*/


int main (void) {
  clrscr();

	/* Open log file for  debugging things */
  open_log_file ("wagon.log");

	/* Attempt to open the files for reading */
  f_179 = openfile ("fall179.exe", "rb", TRUE);
  f_195 = openfile ("fall195.exe", "rb", TRUE);
  f_213 = openfile ("fall213.exe", "rb", TRUE);

	/* Find all numbers in v213 file */
  find_213();
  find_195();

  while (!done) {
    pre1 = pre;
    pre = ch;
    ch = fgetc(f_179);

    if (ch == 0xa0) {
      ch1 = fgetc(f_179);

      if (ch1 == EOF)
	done = TRUE;
      else if (ch1 == 0x0f) {
	count++;
	l = ftell(f_179) - 3;
	post = fgetc(f_179);
	post1 = fgetc(f_179);
	write_log_entry ("Found 0x03e8 (0x%08lX):  Pre=0x%02X 0x%02X   Post=0x%02X 0x%02X", l, pre1, pre, post, post1);

			/* Any matches? */
	for (i = 0; i < num_213; i++) {
	  if (dag_213[i].pre == pre && dag_213[i].post == post)
	      //&& dag_213[i].pre1 == pre1 && dag_213[i].post1 == post1)
	    write_log_entry ("   Matches v2.13 %3d!     (0x%08lX)", i, dag_213[i].offset);
	 }


				/* Any matches? */
	for (i = 0; i < num_195; i++) {
	  if (dag_195[i].pre == pre && dag_195[i].post == post
	      && dag_195[i].pre1 == pre1 && dag_195[i].post1 == post1)
	    write_log_entry ("   Matches v1.95 %3d!     (0x%08lX)", i, dag_195[i].offset);
	 }

       }
      else
	ch = ch1;
     }
    else if (ch == EOF)
      done = TRUE;
   }

  fclose (f_179);
  fclose (f_195);
  fclose (f_213);
  return (TRUE);
 }