#include <stdlib.h>
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "cfgparse_arg.h"

int
cfgparseCmdParse(cfgparse_obj_t *obj){
  int i;
  int inode, iopt, ilongopt;
  int nopts = 0;
  int nlongopts = 0;
  int ret = CFGP_MASK_NONE;
  char *optchars;
  struct option *longopts;
  int c;
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
      if(curnode->type != CFGP_TYPE_COMMENT && curnode->key != '\0'){
        if(curnode->longkey != NULL){
          nlongopts += 1;
        }
        nopts += 1;
      }
      curnode = curnode->next;
    }
  }

  /* initialize what we have */
  nodearr = calloc(nopts, sizeof(cfgparse_node_t*));
  optchars = calloc(2 * nopts + 1, sizeof(char));
  longopts = calloc(nlongopts + 1, sizeof(struct option));

  /* fill nodearr, optchars and longopts */
  inode = 0;
  iopt = 0;
  ilongopt = 0;
  for(i = 0; i < obj->n; i++){
    curgroup = obj->list[i];
    if(curgroup == NULL){
      continue;
    }
    curnode = curgroup->first;
    while(curnode != NULL){
      if(curnode->type != CFGP_TYPE_COMMENT && curnode->key != '\0'){
        nodearr[inode] = curnode;
        /* append longkey array if necessary */
        if(curnode->longkey != NULL){
          longopts[ilongopt].name = curnode->longkey;
          longopts[ilongopt].flag = NULL;
          longopts[ilongopt].val = curnode->key;
          if(curnode->mode == CFGP_MODE_STORE){
            longopts[ilongopt].has_arg = required_argument;
          } else{
            longopts[ilongopt].has_arg = no_argument;
          }
          ilongopt++;
        }

        /* append key list */
        strncat(optchars, &curnode->key, 1);
        if(curnode->mode == CFGP_MODE_STORE){
          strncat(optchars, ":", 1);
        }

        inode += 1;
      }

      curnode = curnode->next;
    }
  }
  longopts[ilongopt].name = NULL;
  longopts[ilongopt].flag = NULL;
  longopts[ilongopt].has_arg = 0;
  longopts[ilongopt].val = 0;

  /* now do the actual parsing */
  assert(obj->argv != NULL);
  assert(obj->argc != NULL);
  while((c = getopt_long(*obj->argc, *obj->argv, optchars, 
         longopts, NULL)) != -1){
    for(i = 0; i < nopts; i++){
      if(c == nodearr[i]->key){
        if(nodearr[i]->mode == CFGP_MODE_STORE){
          if(nodearr[i]->type == CFGP_TYPE_INT){
            sscanf(optarg, "%d", (int*)nodearr[i]->dest);
          }else if(nodearr[i]->type == CFGP_TYPE_FLOAT){
            sscanf(optarg, "%g", (float*)nodearr[i]->dest);
          }else if(nodearr[i]->type == CFGP_TYPE_DOUBLE){
            sscanf(optarg, "%lg", (double*)nodearr[i]->dest);
          }else if(nodearr[i]->type == CFGP_TYPE_STRING){
            /* strings are special */
            *(char**)(nodearr[i]->dest) = optarg;
          }
        } else if (nodearr[i]->mode == CFGP_MODE_STORE_TRUE){
          assert(nodearr[i]->type == CFGP_TYPE_INT);
          *(int*)nodearr[i]->dest = !0;
        } else if (nodearr[i]->mode == CFGP_MODE_STORE_FALSE){
          assert(nodearr[i]->type == CFGP_TYPE_INT);
          *(int*)nodearr[i]->dest = 0;
        } else if (nodearr[i]->mode == CFGP_MODE_FLAG){
          /* do nothing, just make sure parse_mod is handled */
        } else if(nodearr[i]->mode == CFGP_MODE_INCREMENT){
          assert(nodearr[i]->type != CFGP_TYPE_STRING);
          if(nodearr[i]->type == CFGP_TYPE_INT)
            *(int*)nodearr[i]->dest += 1;
          else if(nodearr[i]->type == CFGP_TYPE_FLOAT)
            *(float*)nodearr[i]->dest += 1.0;
          else if(nodearr[i]->type == CFGP_TYPE_DOUBLE)
            *(double*)nodearr[i]->dest += 1.0;
        } else if(nodearr[i]->mode == CFGP_MODE_DECREMENT){
          assert(nodearr[i]->type != CFGP_TYPE_STRING);
          if(nodearr[i]->type == CFGP_TYPE_INT)
            *(int*)nodearr[i]->dest -= 1;
          else if(nodearr[i]->type == CFGP_TYPE_FLOAT)
            *(float*)nodearr[i]->dest -= 1.0;
          else if(nodearr[i]->type == CFGP_TYPE_DOUBLE)
            *(double*)nodearr[i]->dest -= 1.0;
        }
        ret |= nodearr[i]->parse_mask;
        break;
      }
    }
  }

  /* do the parsing */
  free(nodearr);
  free(optchars);
  free(longopts);
  return ret;
}

/*
 * EOF
 */

