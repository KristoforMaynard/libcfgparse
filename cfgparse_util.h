#ifndef __CFGPARSE_UTIL_H
#define __CFGPARSE_UTIL_H

#include "cfgparselib.h"

void 
cfgparseGroupCreate(cfgparse_group_t *group, char *label);
void
cfgparseObjCreate(cfgparse_obj_t *obj, char *fname, int *argc, char ***argv,
                  int n, ...);
void
cfgparseObjListCreate(cfgparse_objlist_t *objlst, char *prog, char *ver,
                      int n, ...);

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
cfgparseAddFlag(cfgparse_group_t *group, int parse_mask,
                  const char key, const char *longkey, const char *help);

/*void
cfgparseAddComment(cfgparse_group_t *group, const char *comment);*/

/* Do the file processing once all variables have been added to list */
/* returns 0 if config file was read                                 */
/* returns 1 if config file was written and defaults are in use      */
int
cfgparseParse(cfgparse_objlist_t *objlst);

void
cfgparsePrintHelp(cfgparse_objlist_t *objlst);

#endif

/*
 * EOF
 */

