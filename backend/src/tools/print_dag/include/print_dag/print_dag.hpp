#ifndef PRINT_DAG_PRINT_DAG_HPP
#define PRINT_DAG_PRINT_DAG_HPP

#include <cstdio>

#include <ostream>
#include <string>

#include "dag/dag.hpp"

std::string ToDotString(const DAG *dag);
const char *ToDotCharPtr(const DAG *dag);
void ToDotFile(const DAG *dag, FILE *outfile);
void ToDotStream(const DAG *dag, std::ostream *outstream);

#endif  // PRINT_DAG_PRINT_DAG_HPP
