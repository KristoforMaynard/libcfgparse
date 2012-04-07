#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdarg.h>

#include "cfgparse_util.h"
#include "cfgparse_arg.h"
#include "cfgparse_file.h"

void
cfgparseGroupCreate(cfgparse_group_t *group, char *label){
  assert(group->first == NULL);
  group->first = NULL;
  group->last = NULL;
  group->n = 0;
  group->label = label;
}

void
cfgparseObjCreate(cfgparse_obj_t *obj, char *fname, int *argc, char ***argv,
                  int n, ...){
  int i;
  cfgparse_group_t *g;
  va_list ap;

  assert(obj->list == NULL);
  obj->list = calloc(n, sizeof(cfgparse_group_t*));
  obj->n = n;
  obj->fname = fname;
  obj->argc = argc;
  obj->argv = argv;

  va_start(ap, n);
  for(i = 0; i < n; i++){
    g = va_arg(ap, cfgparse_group_t*);
    obj->list[i] = g;
  }
  va_end(ap);
}

void
cfgparseObjListCreate(cfgparse_objlist_t *objlst, char *prog, char *ver,
                      int n, ...){
  int i;
  cfgparse_obj_t *obj;
  va_list ap;

  assert(objlst->list == NULL);
  objlst->list = calloc(n, sizeof(cfgparse_objlist_t*));
  objlst->n = n;
  objlst->prog = prog;
  objlst->ver = ver;

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
    if(cur->dest_alloced){
      free(cur->dest);
    }
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
  obj->list = NULL;
}

void
cfgparseObjListDestroy(cfgparse_objlist_t *objlst){
  int i;
  for(i = 0; i < objlst->n; i++){
    cfgparseObjDestroy(objlst->list[i]);
  }
  free(objlst->list);  
  objlst->list = NULL;
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
  node->parse_mask = CFGP_MASK_NONE;
  node->mode = mode;
  node->key = key;
  node->longkey = longkey;
  node->dest = dest;
  node->help = help;
  node->dest_alloced = 0;
  return node;
}

void
cfgparseAddInt(cfgparse_group_t *group, int mode,
               const char key, const char *longkey, int *dest, 
               int def_val, const char *help){
  cfgparse_node_t *node = cfgparseAddNode(group, mode,
                                          key, longkey, 
                                          (void*)dest, help);
  node->type = CFGP_TYPE_INT;
  *(int*)node->dest = def_val;
}

void 
cfgparseAddFloat(cfgparse_group_t *group, int mode,
                 const char key, const char *longkey, float *dest,
                 float def_val, const char *help){
  cfgparse_node_t *node = cfgparseAddNode(group, mode,
                                          key, longkey, 
                                          (void*)dest, help);
  node->type = CFGP_TYPE_FLOAT;
  *(float*)node->dest = def_val;
}

void
cfgparseAddDouble(cfgparse_group_t *group, int mode,
                  const char key, const char *longkey, double *dest,
                  float def_val, const char *help){
  cfgparse_node_t *node = cfgparseAddNode(group, mode,
                                          key, longkey,
                                          (void*)dest, help);
  node->type = CFGP_TYPE_DOUBLE;
  *(double*)node->dest = def_val;
}

void
cfgparseAddString(cfgparse_group_t *group, int mode,
                  const char key, const char *longkey, char **dest,
                  char *def_val, const char *help){
  cfgparse_node_t *node = cfgparseAddNode(group, mode,
                                          key, longkey,
                                          (void*)dest, help);
  node->type = CFGP_TYPE_STRING;
  *(char**)node->dest = def_val;
}

void
cfgparseAddFlag(cfgparse_group_t *group, int parse_mask,
                  const char key, const char *longkey, const char *help){
  cfgparse_node_t *node = cfgparseAddNode(group, CFGP_MODE_FLAG, 
                                          key, longkey, NULL, help);
  node->type = CFGP_TYPE_INT;
  node->parse_mask = parse_mask;
}

/*void
cfgparseAddComment(cfgparse_group_t *group, const char *comment){
  cfgparse_node_t *node = cfgparseAddNode(group, CFGP_MODE_COMMENT, (char)0,
                                          NULL, NULL, comment);
  node->type = CFGP_TYPE_COMMENT;
}*/


void
cfgparsePrintVersion(cfgparse_objlist_t *objlst){
  fprintf(stderr, "Version: %s\n", objlst->ver);
}

void
cfgparseObjPrintHelp(cfgparse_obj_t *obj){
  int i;
  char *tp = "         ";
  cfgparse_group_t *curgroup = NULL;
  cfgparse_node_t *curnode = NULL;
  
  if(obj->fname == NULL){
    fprintf(stderr, "\nCOMMAND LINE OPTIONS:\n");
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
      if(curnode->mode == CFGP_MODE_STORE){
        switch(curnode->type){
        case CFGP_TYPE_INT:     tp=" = INT   ";  break;
        case CFGP_TYPE_FLOAT:   tp=" = FLOAT ";  break;
        case CFGP_TYPE_DOUBLE:  tp=" = DOUBLE";  break;
        case CFGP_TYPE_STRING:  tp=" = STRING";  break;
        default: continue;
        }
      }

      if(obj->fname == NULL){
        fprintf(stderr, "    -%c, --%-15s %s    %s\n", curnode->key,
                curnode->longkey, tp, curnode->help);
      } else{
        fprintf(stderr, "    %-20s %s    %s\n", curnode->longkey, tp,
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

  fprintf(stderr, "Usage: %s [options]\n", objlst->prog);
  for(i = 0; i < objlst->n; i++){
    curobj = objlst->list[i];
    cfgparseObjPrintHelp(curobj);
  }
}

/* < 0 if something bad happend */
/* 0 if everything went normally  */
/* &2 if help should be printed    */
/* &4 if version should be printed */
int
cfgparseParse(cfgparse_objlist_t *objlst){
  int i;
  int ret = 0;
  cfgparse_obj_t *obj;

  for(i = 0; i < objlst->n; i++){
    obj = objlst->list[i];
    if(obj->fname == NULL){
      ret |= cfgparseCmdParse(obj);
    } else{
      ret |= cfgparseFileParse(obj);
    }
  }

  if(ret > 0 && ret & 4){
    cfgparsePrintVersion(objlst);
  }
  if(ret > 0 && ret & 2){
    cfgparsePrintHelp(objlst);
  }

  return ret;
}

/*
 * EOF
 */

