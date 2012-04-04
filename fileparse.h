#ifndef __FILEPARSE_H
#define __FILEPARSE_H

/**********/
/* MACROS */
#define CONF_LINE_LEN   200
#define COMMENT_CHAR    '#'
#define COMMENT_STR     "#"
#define CP_FALSE        0
#define CP_TRUE         !CP_FALSE


/**************/
/* STRUCTURES */

/* node structures */
struct fileparse_node{
  int dtype; /* use enum cfgp_type */
  const char *key;
  void *dest;
  const char *def; /* default value as a string */
  const char *comment;
  const char *help;
  int num;
  struct fileparse_node *next;
  struct fileparse_node *prev;
};

/* configList_t houses 5 lists, one                            *
 * for each data type in (int, float, double, string, comment) */
typedef struct fileparse_nodelist {
  struct fileparse_node *first;
  struct fileparse_node *last;
  int total_entries;
  char *name;
}

/***********************/
/* FUNCTION PROTOTYPES */
configList_t* configParseInitList(char *name);
configList_t* configParseFreeList(configList_t *list);

/* Do the file processing once all variables have been added to list */
/* returns 0 if config file was read                                 */
/* returns 1 if config file was written and defaults are in use      */
int configParseExecute(configList_t *list, const char *fname);

/* Print all the config variables to the screen with thier associated */
/* help blurbs, if any are supplied                                   */
void configParsePrintHelp(configList_t *list);

/* resets values to their defaults                        */
/* reset_locks is true/false on whether you want          */
/* all values reset(even locked ones) and all locks       */
/* reset to CP_FALSE                       */
/* else, locked values will remain unchanged by the reset */
void configParseReset(configList_t *list, int reset_locks);

void configParseAddInt(configList_t *list, char *key,
                       int *destination, int default_val);
void configParseAddIntWithHelp(configList_t *list, char *key,
                               int *destination, int default_val,
                               char *comment, char *help);

void configParseAddFloat(configList_t *list, char *key, float *destination,
                         float default_val, int use_sci_note);
void configParseAddFloatWithHelp(configList_t *list, char *key, float *destination,
                                 float default_val, int use_sci_note,
                                 char *comment, char *help);

void configParseAddDouble(configList_t *list, char *key, double *destination,
                          double default_val, int use_sci_note);
void configParseAddDoubleWithHelp(configList_t *list, char *key, 
                                  double *destination, double default_val, 
                                  int use_sci_note, char *comment,
                                  char *help);
/*NOTE: the destination of a string should NEVER be changed directly */
/*      it should only change as a result of reading a config file   */
/*      or a call to configParseReset(), else segfaults and memory   */
/*      leaks will happen                                            */
void configParseAddString(configList_t *list, char *key,
               char **destination, char *default_val);
void configParseAddStringWithHelp(configList_t *list, char *key,
               char **destination, char *default_val,
               char *comment, char *help);
void configParseAddComment(configList_t *list, const char *text);

/***************************************************************************/
/* locking a value makes it so a call to execute won't change the value    */
/* you can for instance lock a variable that's specified in the command    */
/* line, to have that take priority over a config file value               */
/* ----------------------------------------------------------------------- */
/* configParseLock*Value first sets to value, then locks it                */
/*                       (this will overwrite a previous lock)             */
/* configParseLock* just locks the current value                           */
/* for configParseSet* functions, force is true/false whether you want     */
/*                     to override a lock                                  */
/* ----------------------------------------------------------------------- */
/* Returns: 0 on success, -1 on key not found, 1 on no change made         */
/*          no change made can result from trying to change a locked value */
int configParseSetInt(configList_t *list, char *skey, int value, int force);
int configParseLockInt(configList_t *list, char *skey);
int configParseUnlockInt(configList_t *list, char *skey);
int configParseLockIntValue(configList_t *list, char *skey, int value);

int configParseSetFloat(configList_t *list, char *skey, float value, int force);
int configParseLockFloat(configList_t *list, char *skey);
int configParseUnlockFloat(configList_t *list, char *skey);
int configParseLockFloatValue(configList_t *list, char *skey, float value);

int configParseSetDouble(configList_t *list, char *skey, double value, int force);
int configParseLockDouble(configList_t *list, char *skey);
int configParseUnlockDouble(configList_t *list, char *skey);
int configParseLockDoubleValue(configList_t *list, char *skey, double value);

/* set string assumes a value that has already been allocated */
int configParseSetString(configList_t *list, char *skey, char* value, int force);
int configParseLockString(configList_t *list, char *skey);
int configParseUnlockString(configList_t *list, char *skey);
int configParseLockStringValue(configList_t *list, char *skey, char* value);

#endif

