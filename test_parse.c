#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "cfgparselib.h"

int main(int argc, char **argv){
  cfgparse_group_t cmd_group1 = {0};
  cfgparse_group_t cmd_group2 = {0};
  cfgparse_obj_t cmdobj = {0};
  cfgparse_obj_t cmdfileobj = {0};
  cfgparse_objlist_t cmdobjlist = {0};

  int a, A;
  float b, B;
  double c, C;
  char *d, *D;

  cfgparseGroupCreate(&cmd_group1, NULL);
  cfgparseAddInt(&cmd_group1, CFGP_MODE_STORE, 'a', "av", &a, -1, "a h");
  cfgparseAddFloat(&cmd_group1, CFGP_MODE_STORE, 'b', "bv", &b, -2.0, "b h");
  cfgparseAddDouble(&cmd_group1, CFGP_MODE_STORE, 'c', "cv", &c, -3.0, "c h");
  cfgparseAddString(&cmd_group1, CFGP_MODE_STORE, 'd', "dv", &d, "-4", "d h");
  cfgparseObjCreate(&cmdobj, NULL, 1, &cmd_group1);

  cfgparseGroupCreate(&cmd_group2, NULL);
  cfgparseAddInt(&cmd_group2, CFGP_MODE_STORE, 'A', "Av", &A, -1, "A h");
  cfgparseAddFloat(&cmd_group2, CFGP_MODE_STORE, 'B', "Bv", &B, -2.0, "B h");
  cfgparseAddDouble(&cmd_group2, CFGP_MODE_STORE, 'C', "Cv", &C, -3.0, "C h");
  cfgparseAddString(&cmd_group2, CFGP_MODE_STORE, 'D', "Dv", &D, "-4", "D h");
  cfgparseObjCreate(&cmdfileobj, "input.txt", 1, &cmd_group2);
  
  cfgparseObjListCreate(&cmdobjlist, 2, &cmdobj, &cmdfileobj);

  fprintf(stderr, "a = %d\n", a);
  fprintf(stderr, "b = %g\n", b);
  fprintf(stderr, "c = %g\n", c);
  fprintf(stderr, "d = %s\n", d);

  fprintf(stderr, "A = %d\n", A);
  fprintf(stderr, "B = %g\n", B);
  fprintf(stderr, "C = %g\n", C);
  fprintf(stderr, "D = %s\n", D);

  cfgparsePrintHelp(&cmdobjlist);
  
  assert(a==-1);
  assert(b==-2.0);
  assert(c==-3.0);
  assert(d[0]=='-' && d[1]=='4');

  cfgparseObjListDestroy(&cmdobjlist);

  return 0;
}

/*
 * EOF
 */

