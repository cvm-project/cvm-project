#ifndef UTILS_PRINTDAG_H
#define UTILS_PRINTDAG_H

#include <cstdio>

#include <ostream>
#include <string>

#include "dag/DAG.h"

std::string ToDotString(const DAG *dag);
const char *ToDotCharPtr(const DAG *dag);
void ToDotFile(const DAG *dag, FILE *outfile);
void ToDotStream(const DAG *dag, std::ostream *outstream);

#endif  // UTILS_PRINTDAG_H
