#include <cstdio>
#include <cstring>
using namespace std;

#include "unit.h"
#include "defs.h"

Unit::Unit() {
  }

static char buf[BUF_LEN];

int Unit::Load(FILE *f) {
  memset(buf, 0, BUF_LEN);
  if(fscanf(f, "%d;%[^\n;];\n", &id, buf) < 2) return 0;
  name = buf;
  return 1;
  }

int Unit::Save(FILE *f) {
  if(fprintf(f, "%d;%s;\n", id, name.c_str()) < 4) return 0;
  return 1;
  }
