#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "cfgparselib.h"

int main(int argc, char **argv){
  cfgparse_group_t cmd_group1 = {0};
  cfgparse_group_t cmd_group2 = {0};
  cfgparse_group_t cmd_group3 = {0};
  cfgparse_obj_t cmdobj = {0};
  cfgparse_obj_t cmdfileobj = {0};
  cfgparse_objlist_t cmdobjlist = {0};

  int ret;
  int a, A;
  float b, B;
  double c, C;
  char *d, *D;

  cfgparseGroupCreate(&cmd_group1, "General");
  cfgparseAddFlag(&cmd_group1, CFGP_MASK_HELP, 'h', "help", "help");
  cfgparseAddFlag(&cmd_group1, CFGP_MASK_VERSION, 'V', "version", "version");  
  cfgparseGroupCreate(&cmd_group2, "");
  cfgparseAddInt(&cmd_group2, CFGP_MODE_STORE, 'a', "av", &a, -1, "a h");
  cfgparseAddFloat(&cmd_group2, CFGP_MODE_STORE, 'b', "bv", &b, -2.0, "b h");
  cfgparseAddDouble(&cmd_group2, CFGP_MODE_STORE, 'c', "cv", &c, -3.0, "c h");
  cfgparseAddString(&cmd_group2, CFGP_MODE_STORE, 'd', "dv", &d, "-4", "d h");
  cfgparseObjCreate(&cmdobj, NULL, &argc, &argv, 2, &cmd_group1, &cmd_group2);

  cfgparseGroupCreate(&cmd_group3, NULL);
  cfgparseAddInt(&cmd_group3, CFGP_MODE_STORE, 'A', "fA", &A, -1, "A h");
  cfgparseAddFloat(&cmd_group3, CFGP_MODE_STORE, 'B', "fB", &B, -2.0, "B h");
  cfgparseAddDouble(&cmd_group3, CFGP_MODE_STORE, 'C', "fC", &C, -3.0, "C h");
  cfgparseAddString(&cmd_group3, CFGP_MODE_STORE, 'D', "fD", &D, "-4", "D h");
  cfgparseObjCreate(&cmdfileobj, "intest.txt", NULL, NULL, 1, &cmd_group3);
  
  cfgparseObjListCreate(&cmdobjlist, "./test_parse", "0.01", 
                        2, &cmdobj, &cmdfileobj);

  ret = cfgparseParse(&cmdobjlist);
  if(ret != 0){
    cfgparseObjListDestroy(&cmdobjlist);
    return (ret > 0) ? 0: -1;
  }

  fprintf(stderr, "a = %d\n", a);
  fprintf(stderr, "b = %g\n", b);
  fprintf(stderr, "c = %g\n", c);
  fprintf(stderr, "d = %s\n", d);

  fprintf(stderr, "A = %d\n", A);
  fprintf(stderr, "B = %g\n", B);
  fprintf(stderr, "C = %g\n", C);
  fprintf(stderr, "D = %s\n", D);
  
  assert(a==-1);
  assert(b==-2.0);
  assert(c==-3.0);
  assert(d[0]=='-' && d[1]=='4');
  assert(A==1);
  assert(B==2.0);
  assert(C==3.0);
  assert(strncmp("with space", D, 12) == 0);

  cfgparseObjListDestroy(&cmdobjlist);

  return 0;
}

/*
 * EOF
 */

