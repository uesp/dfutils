/*=========================================================================
 *
 * SPELLS.CPP, June 1998 - Dave Humphrey
 *
 * Class and Functions for handling the Daggerfall SPELLS.STD file.
 *
 *=======================================================================*/


	/* Include files */
#include "spells.h"


	/* The Spell Type Data */
SPELL_DESC_TYPE spells[] = {
       {  0,  -1,	"Paralysis" },
       {  1,   0,	"Cont Dmg:Health" },
       {  1,   1,	"Cont Dmg:Stamina" },
       {  1,   2,	"Cont Dmg:Spell Points" },
       {  3,   0,	"Cure Disease" },
       {  3,   1,	"Cure Poison" },
       {  3,   2,	"Cure Paralysis" },
       {  4,   0,	"Damage Health" },
       {  4,   1,       "Damage Stamina" },
       {  4,   2,	"Damage Spell Points" },
       {  5,  -1,	"Disentigrate" },
       {  6,   0,  	"Dispel Magic" },
       {  6,   1,	"Dispel Undead" },
       {  6,   2,  	"Dispel Daedra" },
       {  7,   0,	"Drain Strength" },
       {  7,   1,       "Drain Intelligence" },
       {  7,   2,       "Drain Willpower" },
       {  7,   3,	"Drain Agility" },
       {  7,   4,	"Drain Endurance" },
       {  7,   5,	"Drain Personality" },
       {  7,   6,	"Drain Speed" },
       {  7,   7,	"Drain Luck" },
       {  8,   0,	"Resist Fire" },
       {  8,   1,	"Resist Cold" },
       {  8,   2,	"Resist Poison" },
       {  8,   3,	"Resist Shock" },
       {  9,   0,	"Increase Strength" },
       {  9,   1,       "Increase Intelligence" },
       {  9,   2,       "Increase Willpower" },
       {  9,   3,	"Increase Agility" },
       {  9,   4,	"Increase Endurance" },
       {  9,   5,	"Increase Personality" },
       {  9,   6,	"Increase Speed" },
       {  9,   7,	"Increase Luck" },
       { 10,   0,	"Heal Strength" },
       { 10,   1,	"Heal Intelligence" },
       { 10,   2,	"Heal Willpower" },
       { 10,   3,	"Heal Agility" },
       { 10,   4,	"Heal Endurance" },
       { 10,   5,	"Heal Personality" },
       { 10,   6,	"Heal Speed" },
       { 10,   7,	"Heal Luck" },
       { 10,   8,	"Heal Health" },
       { 10,   9,	"Heal Stamina" },
       { 11,   0,	"Leech Strength" },
       { 11,   1,	"Leech Intelligence" },
       { 11,   2,	"Leech Willpower" },
       { 11,   3,	"Leech Agility" },
       { 11,   4,	"Leech Endurance" },
       { 11,   5,	"Leech Personality" },
       { 11,   6,	"Leech Speed" },
       { 11,   7,	"Leech Luck" },
       { 11,   8,	"Leech Health" },
       { 11,   9,	"Leech Stamina" },
       { 12,  -1,	"Soul Trap" },
       { 13,   0,	"Invisibility: Normal" },
       { 13,   1,	"Invisibility: True" },
       { 14,  -1,	"Levitate" },
       { 15,  -1,	"Light" },
       { 16,  -1,	"Lock" },
       { 17,  -1,       "Open" },
       { 18,  -1,	"Regenerate Health" },
       { 19,  -1,	"Silence" },
       { 20,  -1,	"Spell Absorption" },
       { 21,  -1,	"Spell Reflection" },
       { 22,  -1,	"Spell Resistance" },
       { 23,   0,	"Chameleon: Normal" },
       { 23,   1,	"Chameleon: True" },
       { 24,   0,	"Shadow Form: Normal" },
       { 24,   1,	"Shadow Form: True" },
       { 25,  -1,	"Slow Fall?" },
       { 29,  -1,	"Lycanthropy" },
       { 30,  -1,	"Water Breathing" },
       { 31,  -1,	"Water Walking" },
       { 32,  -1,	"Jumping" },
       { 33,   0,	"Calm Animal" },
       { 33,   1,	"Calm Undead" },
       { 33,   2,	"Calm Human" },
       { 34,  -1,	"Charm Mortal" },
       { 35,  -1,	"Shield" },
       { 43,  -1,	"Recall" },
       { 44,  -1,	"Tongues" },
       { -1,  -1,	"<nothing>" },
       { -1,  -1,       NULL },		/* Always this one last */
 };


/*=========================================================================
 *
 * Class SPELLS_STD_TYPE Method - boolean load (filename);
 *
 * Attempts to load data from a file.  Returns TRUE on success.
 *
 *=======================================================================*/
boolean SPELLS_STD_TYPE::load (const char *filename) {
  FILE *f;		/* File pointer */
  short i;		/* Loop counter */

	/* Ignore any invalid parameters */
  if (!filename) {
    err_code = ERR_NULL;
    return (FALSE);
   }

	/* Attempt to open the file */
  if (!(f = openfile(filename, "rb"))) {
    err_code = ERR_FILE;
    return (FALSE);
   }

	/* Clear the contents of the current records */
  destroy();

	/* Determine how many records to load */
  num_records = get_filesize(f)/89;

	/* Ensure that there aren't too many records */
  if (num_records >= MAX_SPELLS) {
    fclose (f);
    err_code = DF_ERR_MAXSPELL;
    return (FALSE);
   }

	/* Load all the records */
  for (i = 0; i < num_records; i++) {
    records[i].spells[0].effect = fgetc(f);
    records[i].spells[0].effect1 = fgetc(f);
    records[i].spells[1].effect = fgetc(f);
    records[i].spells[1].effect1 = fgetc(f);
    records[i].spells[2].effect = fgetc(f);
    records[i].spells[2].effect1 = fgetc(f);
    records[i].element = fgetc(f);
    records[i].target = fgetc(f);
    records[i].cost = read_int(f);
    records[i].u1 = read_int(f);
    records[i].u2 = read_int(f);
    fread (records[i].spells[0].duration, sizeof(char), 3, f);
    fread (records[i].spells[1].duration, sizeof(char), 3, f);
    fread (records[i].spells[2].duration, sizeof(char), 3, f);
    fread (records[i].spells[0].chance, sizeof(char), 3, f);
    fread (records[i].spells[1].chance, sizeof(char), 3, f);
    fread (records[i].spells[2].chance, sizeof(char), 3, f);
    fread (records[i].spells[0].strength, sizeof(char), 5, f);
    fread (records[i].spells[1].strength, sizeof(char), 5, f);
    fread (records[i].spells[2].strength, sizeof(char), 5, f);
    fread (records[i].name, sizeof(char), 25, f);
    records[i].icon = fgetc(f);
    records[i].index = fgetc(f);
    records[i].null1 = read_int(f);
    records[i].null2 = read_long(f);
    records[i].null3 = read_long(f);
    records[i].null4 = read_long(f);
    records[i].null5 = fgetc(f);
   }

	/* Close the file */
  fclose (f);

  return (TRUE);
 }
/*=========================================================================
 *		End of Class Method SPELLS_STD_TYPE::load()
 *=======================================================================*/



/*=========================================================================
 *
 * Class SPELLS_STD_TYPE Method - boolean save (filename);
 *
 * Attempts to save data to a STD file.  Returns TRUE on success.
 *
 *=======================================================================*/
boolean SPELLS_STD_TYPE::save (const char *filename) {
  FILE *f;		/* File pointer */
  short i;		/* Loop counter */

	/* Ignore any invalid parameters */
  if (!filename) {
    err_code = ERR_NULL;
    return (FALSE);
   }

	/* Attempt to open the file */
  if (!(f = openfile(filename, "wb"))) {
    err_code = ERR_FILE;
    return (FALSE);
   }

	/* Load all the records */
  for (i = 0; i < num_records; i++) {
    fputc (records[i].spells[0].effect, f);
    fputc (records[i].spells[0].effect1, f);
    fputc (records[i].spells[1].effect, f);
    fputc (records[i].spells[1].effect1, f);
    fputc (records[i].spells[2].effect, f);
    fputc (records[i].spells[2].effect1, f);
    fputc (records[i].element, f);
    fputc (records[i].target, f);
    write_int (f, records[i].cost);
    write_int (f, records[i].u1);
    write_int (f, records[i].u2);
    fwrite (records[i].spells[0].duration, sizeof(char), 3, f);
    fwrite (records[i].spells[1].duration, sizeof(char), 3, f);
    fwrite (records[i].spells[2].duration, sizeof(char), 3, f);
    fwrite (records[i].spells[0].chance, sizeof(char), 3, f);
    fwrite (records[i].spells[1].chance, sizeof(char), 3, f);
    fwrite (records[i].spells[2].chance, sizeof(char), 3, f);
    fwrite (records[i].spells[0].strength, sizeof(char), 5, f);
    fwrite (records[i].spells[1].strength, sizeof(char), 5, f);
    fwrite (records[i].spells[2].strength, sizeof(char), 5, f);
    fwrite (records[i].name, sizeof(char), 25, f);
    fputc (records[i].icon, f);
    fputc (records[i].index, f);
    write_int (f, records[i].null1);
    write_long (f, records[i].null2);
    write_long (f, records[i].null3);
    write_long (f, records[i].null4);
    fputc (records[i].null5, f);
   }

	/* Close the file */
  fclose (f);

  return (TRUE);
 }
/*=========================================================================
 *		End of Class Method SPELLS_STD_TYPE::load()
 *=======================================================================*/






