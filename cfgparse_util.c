#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdarg.h>

#include "cfgparse_util.h"

void
cfgparseGroupCreate(cfgparse_group_t *group, char *label){
  assert(group->first == NULL);
  group->first = NULL;
  group->last = NULL;
  group->n = 0;
  group->label = label;
}

void
cfgparseObjCreate(cfgparse_obj_t *obj, char *fname, int n, ...){
  int i;
  cfgparse_group_t *g;
  va_list ap;

  assert(obj->list == NULL);
  obj->list = calloc(n, sizeof(cfgparse_group_t*));
  obj->n = n;
  obj->fname = fname;

  va_start(ap, n);
  for(i = 0; i < n; i++){
    g = va_arg(ap, cfgparse_group_t*);
    obj->list[i] = g;
  }
  va_end(ap);
}

void
cfgparseObjListCreate(cfgparse_objlist_t *objlst, int n, ...){
  int i;
  cfgparse_obj_t *obj;
  va_list ap;

  assert(objlst->list == NULL);
  objlst->list = calloc(n, sizeof(cfgparse_objlist_t*));
  objlst->n = n;

  va_start(ap, n);
  for(i = 0; i < n; i++){
    obj = va_arg(ap, cfgparse_obj_t*);
    objlst->list[i] = obj;
  }
  va_end(ap);
}

void
cfgparseGroupDestroy(cfgparse_group_t *group){
  cfgparse_node_t *cur = NULL;
  cfgparse_node_t *nxt = group->first;
  while(group->n > 0){
    cur = nxt;
    nxt = cur->next;
    free(cur);
    group->n--;
  }
  group->first = NULL;
  group->last = NULL;
}

void
cfgparseObjDestroy(cfgparse_obj_t *obj){
  int i;
  for(i = 0; i < obj->n; i++){
    cfgparseGroupDestroy(obj->list[i]);
  }
  free(obj->list);
}

void
cfgparseObjListDestroy(cfgparse_objlist_t *objlst){
  int i;
  for(i = 0; i < objlst->n; i++){
    cfgparseObjDestroy(objlst->list[i]);
  }
  free(objlst->list);  
}

/* calloc a new node and append it to the list */
cfgparse_node_t *
cfgparseAddNode(cfgparse_group_t *group, int mode, 
               const char key, const char *longkey, void *dest, 
               const char *help){
  cfgparse_node_t *node = calloc(1, sizeof(cfgparse_node_t));
  assert(node != NULL);
  /* add node to the list */
  node->prev = group->last;
  node->next = NULL;
  group->last = node;
  if(node->prev == NULL){
    group->first = node;
  } else{
    node->prev->next = node;
  }
  group->n++;

  /* fill values from arguments */
  node->mode = mode;
  node->key = key;
  node->longkey = longkey;
  node->dest = dest;
  node->help = help;
  return node;
}

void
cfgparseAddInt(cfgparse_group_t *group, int mode, 
               const char key, const char *longkey, int *dest, 
               int def_val, const char *help){
  cfgparse_node_t *node = cfgparseAddNode(group, mode, key, longkey, 
                                          (void*)dest, help);
  node->type = CFGP_TYPE_INT;
  *(int*)node->dest = def_val;
}

void 
cfgparseAddFloat(cfgparse_group_t *group, int mode, 
                 const char key, const char *longkey, float *dest,
                 float def_val, const char *help){
  cfgparse_node_t *node = cfgparseAddNode(group, mode, key, longkey, 
                                          (void*)dest, help);
  node->type = CFGP_TYPE_FLOAT;
  *(float*)node->dest = def_val;
}

void
cfgparseAddDouble(cfgparse_group_t *group, int mode, 
                  const char key, const char *longkey, double *dest,
                  float def_val, const char *help){
  cfgparse_node_t *node = cfgparseAddNode(group, mode, key, longkey, 
                                          (void*)dest, help);
  node->type = CFGP_TYPE_DOUBLE;
  *(double*)node->dest = def_val;
}

void
cfgparseAddString(cfgparse_group_t *group, int mode, 
                  const char key, const char *longkey, char **dest,
                  char *def_val, const char *help){
  cfgparse_node_t *node = cfgparseAddNode(group, mode, key, longkey, 
                                          (void*)dest, help);
  node->type = CFGP_TYPE_STRING;
  *(char**)node->dest = def_val;
}

void
cfgparseAddComment(cfgparse_group_t *group, const char *comment){
  cfgparse_node_t *node = cfgparseAddNode(group, CFGP_MODE_COMMENT, '\0',
                                          NULL, NULL, comment);
  node->type = CFGP_TYPE_COMMENT;
}


/* Do the file processing once all variables have been added to list */
/* returns 0 if config file was read                                 */
/* returns 1 if config file was written and defaults are in use      */
int cfgparseParseFile(cfgparse_group_t *group){return 1;}

/* Print all the config variables to the screen with thier associated */
/* help blurbs, if any are supplied                                   */

void
cfgparseObjPrintHelp(cfgparse_obj_t *obj){
  int i;
  char *tp;
  cfgparse_group_t *curgroup = NULL;
  cfgparse_node_t *curnode = NULL;
  
  if(obj->fname == NULL){
    fprintf(stderr, "Usage: [options]\n");
    fprintf(stderr, "\nOPTIONS:\n");
  } else{
    fprintf(stderr, "\nFILE: %s\n", obj->fname);
  }

  for(i = 0; i < obj->n; i++){
    curgroup = obj->list[i];
    
    if(curgroup->label != NULL){
      fprintf(stderr, "  %s\n", curgroup->label);
    }

    curnode = curgroup->first;
    while(curnode != NULL){
      switch(curnode->type){
      case CFGP_TYPE_INT:     tp="INT   ";  break;
      case CFGP_TYPE_FLOAT:   tp="FLOAT ";  break;
      case CFGP_TYPE_DOUBLE:  tp="DOUBLE";  break;
      case CFGP_TYPE_STRING:  tp="STRING";  break;
      default: continue;
      }

      if(obj->fname == NULL){
        fprintf(stderr, "    -%c  --%s = %s\t%s\n", curnode->key, 
                curnode->longkey, tp, curnode->help);
      } else{
        fprintf(stderr, "    %s = %s\t%s\n", curnode->longkey, tp,
                curnode->help);
      }
      curnode = curnode->next;
    }
  }
}

void
cfgparsePrintHelp(cfgparse_objlist_t *objlst){
  int i;
  cfgparse_obj_t *curobj = NULL;

  /*fprintf(stderr, "Usage: [options]\n");*/
  for(i = 0; i < objlst->n; i++){
    curobj = objlst->list[i];
    cfgparseObjPrintHelp(curobj);
  }
}

/*
 * EOF
 */

