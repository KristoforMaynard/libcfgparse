#include <stdlib.h>
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "cfgparse_file.h"

/* this is not prototyped, only used in this file */
char*
skip_whitespace(char *chr){
  char *ret = chr;
  while(ret && (*ret == ' ' || *ret == '\t' || *ret == '\n')){
    ret += 1;
  }
  return ret;
}

int
cfgparseFileParse(cfgparse_obj_t *obj){
  FILE *f;
  char line[CFGP_LINE_LEN];
  char *crawler;
  char *key;
  char *val;
  int vallen;
  char *first_comment_char;
  int i;
  int iline;
  int inode;
  int found;
  int nopts = 0;
  int ret = CFGP_MASK_NONE;
  cfgparse_group_t *curgroup;
  cfgparse_node_t *curnode;
  cfgparse_node_t **nodearr;

  assert(obj != NULL);

  /* how many options are we going to have? */
  for(i = 0; i < obj->n; i++){
    curgroup = obj->list[i];
    if(curgroup == NULL){
      continue;
    }
    curnode = curgroup->first;
    while(curnode != NULL){
      if(curnode->type != CFGP_TYPE_COMMENT){
        if(curnode->longkey != NULL){
          nopts += 1;
        }
      }
      curnode = curnode->next;
    }
  }

  /* initialize what we have */
  nodearr = calloc(nopts, sizeof(cfgparse_node_t*));

  /* put all nodes into one list */
  inode = 0;
  for(i = 0; i < obj->n; i++){
    curgroup = obj->list[i];
    if(curgroup == NULL){
      continue;
    }
    curnode = curgroup->first;
    while(curnode != NULL){
      if(curnode->type != CFGP_TYPE_COMMENT){
        if(curnode->longkey != NULL){
          nodearr[inode] = curnode;
          inode += 1;
        } else{
          fprintf(stderr, "cfgparse Warning: option skipped, must have a "
                          "longkey to be parsed.\n");
        }
      }
      curnode = curnode->next;
    }
  }

  /* parse the file */
  f = fopen(obj->fname, "r");
  if (f == NULL){
    fprintf(stderr, "cfgparse Warning: problem opening %s\n", obj->fname);
    return -1;  
  }

  iline = 0;
  while(fgets(line, CFGP_LINE_LEN, f)){
    iline++;
    /* find the key */
    first_comment_char = strchr(line, CFGP_COMMENT_CHAR);
    if(first_comment_char != NULL){
      *first_comment_char = '\0';
    }
    
    crawler = skip_whitespace(line);
    key = strtok(crawler, " =\t");
    if(key == NULL){
      /* the line was only whitespace and comments */
      continue;
    }
    val = strtok(NULL, "=\n");
    val = skip_whitespace(val);

    found = 0;
    for(i = 0; i < nopts; i++){
      if(strncmp(nodearr[i]->longkey, key, CFGP_LINE_LEN) == 0){
        if(nodearr[i]->mode == CFGP_MODE_STORE){
          if(nodearr[i]->type == CFGP_TYPE_INT){
            sscanf(val, "%d", (int*)nodearr[i]->dest);
          } else if(nodearr[i]->type == CFGP_TYPE_FLOAT){
            sscanf(val, "%g", (float*)nodearr[i]->dest);
          } else if(nodearr[i]->type == CFGP_TYPE_DOUBLE){
            sscanf(val, "%lg", (double*)nodearr[i]->dest);
          } else if(nodearr[i]->type == CFGP_TYPE_STRING){
            /* strings are special */
            if(nodearr[i]->dest_alloced != 0){
              free(nodearr[i]->dest);
            }
            vallen = strlen(val);
            *(char**)(nodearr[i]->dest) = calloc(vallen + 1, sizeof(char));
            strncpy(*(char**)nodearr[i]->dest, val, vallen);
          }
        } else{
          fprintf(stderr, "cfgparse Warning: can only store when reading "
                          "from a config file.\n");
        }
        found = 1;
        break;
      }
    }
    if(found == 0){
      fprintf(stderr, "cfgparse Warning: line %d of file %s not "
                      "used\n", iline, obj->fname);
    }
  }
  fclose(f);

  free(nodearr);
  return ret;
}

int
cfgparseFileDump(cfgparse_obj_t *obj){
  assert(0); /* not implemented */
}

