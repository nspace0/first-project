#ifndef _CAT_S21_CAT_H_
#define _CAT_S21_CAT_H_

#include <stdio.h>

typedef struct flags {
  int n, b, v, e, s, t;
} flags;

flags parser(int argc, char** argv);

#endif