#ifndef __CFGPARSE_UTIL_H
#define __CFGPARSE_UTIL_H

enum cfgp_node_type {
  CFGP_TYPE_INT,
  CFGP_TYPE_FLOAT,
  CFGP_TYPE_DOUBLE,
  CFGP_TYPE_STRING,
  CFGP_TYPE_COMMENT
};

enum cfgp_mode {
  CFGP_MODE_STORE,
  CFGP_MODE_COMMENT
};

//enum cfgp_list_type {
  //CFGP_CFG_ARGS,
  //CFGP_CFG_FILE
//};

struct cfgparse_node{
  enum cfgp_node_type type;
  enum cfgp_mode mode;
  char key;
  const char *longkey; /* only used  */
  void *dest;
  const char *help;
  struct cfgparse_node *next;
  struct cfgparse_node *prev;
};
typedef struct cfgparse_node cfgparse_node_t ;

typedef struct cfgparse_group{
  //enum cfgp_list_type type;
  struct cfgparse_node *first;
  struct cfgparse_node *last;
  int n;
  char *label;
} cfgparse_group_t;


typedef struct cfgparse_obj{
  char *fname;
  int n;
  cfgparse_group_t **list;
} cfgparse_obj_t;


typedef struct cfgparse_objlist{
  int n;
  cfgparse_obj_t **list;
} cfgparse_objlist_t;


/* init cfgparseFileInit with the filename */

void 
cfgparseGroupCreate(cfgparse_group_t *group, char *label);
void
cfgparseObjCreate(cfgparse_obj_t *obj, char *fname, int n, ...);
void
cfgparseObjListCreate(cfgparse_objlist_t *objlst, int n, ...);

void 
cfgparseGroupDestroy(cfgparse_group_t *group);
void
cfgparseObjDestroy(cfgparse_obj_t *obj);
void
cfgparseObjListDestroy(cfgparse_objlist_t *objlst);

cfgparse_node_t *
cfgparseAddNode(cfgparse_group_t *group, int mode, 
               const char key, const char *longkey, void *dest, 
               const char *help);

void
cfgparseAddInt(cfgparse_group_t *group, int mode, 
               const char key, const char *longkey, int *dest, 
               int def_val, const char *help);
void 
cfgparseAddFloat(cfgparse_group_t *group, int mode, 
                 const char key, const char *longkey, float *dest,
                 float def_val, const char *help);
void
cfgparseAddDouble(cfgparse_group_t *group, int mode, 
                  const char key, const char *longkey, double *dest,
                  float def_val, const char *help);
void
cfgparseAddString(cfgparse_group_t *group, int mode, 
                  const char key, const char *longkey, char **dest,
                  char *def_val, const char *help);
void
cfgparseAddComment(cfgparse_group_t *group, const char *comment);

/* Do the file processing once all variables have been added to list */
/* returns 0 if config file was read                                 */
/* returns 1 if config file was written and defaults are in use      */
int
cfgparseParse(cfgparse_group_t *group);

/* Print all the config variables to the screen with thier associated */
/* help blurbs, if any are supplied                                   */
void
cfgparsePrintHelp(cfgparse_objlist_t *objlst);

#endif

/*
 * EOF
 */

