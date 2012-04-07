#ifndef __CFGPARSELIB_H
#define __CFGPARSELIB_H


enum cfgp_node_type {
  CFGP_TYPE_INT,
  CFGP_TYPE_FLOAT,
  CFGP_TYPE_DOUBLE,
  CFGP_TYPE_STRING/*,
  CFGP_TYPE_COMMENT*/
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
  const char *longkey;
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
  char *prog;
  char *ver;
  cfgparse_obj_t **list;
} cfgparse_objlist_t;


#include "cfgparse_util.h"
//#include "cfgparse_file.h"
#include "cfgparse_arg.h"

#endif

/*
 * EOF
 */

