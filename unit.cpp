#include <cstdio>
#include <cstring>
using namespace std;

#include "unit.h"
#include "defs.h"

Unit::Unit(int ident, const string &nm) {
  id = ident;
  name = nm;
  }

static char buf[BUF_LEN];

void Unit::Load(FILE *f) {
  memset(buf, 0, BUF_LEN);
  fscanf(f, "%d;%s;\n", &id, buf);
  name = buf;
  }

void Unit::Save(FILE *f) {
  fprintf(f, "%d;%s;\n", id, name.c_str());
  }
