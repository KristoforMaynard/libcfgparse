#ifndef __CFGPARSELIB_H
#define __CFGPARSELIB_H

enum cfgp_type {
  CFGP_INT,
  CFGP_FLOAT,
  CFGP_DOUBLE,
  CFGP_STRING,
  CFGP_COMMENT
}

#include "argparse.h"
#include "fileparse.h"

#endif
