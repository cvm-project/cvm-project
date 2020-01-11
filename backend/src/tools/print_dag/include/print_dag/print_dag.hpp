#ifndef PRINT_DAG_PRINT_DAG_HPP
#define PRINT_DAG_PRINT_DAG_HPP

#include <cstdio>

#include <ostream>
#include <string>

#include "dag/dag.hpp"

auto ToDotString(const DAG *dag) -> std::string;
auto ToDotCharPtr(const DAG *dag) -> const char *;
void ToDotFile(const DAG *dag, FILE *outfile);
void ToDotStream(const DAG *dag, std::ostream *outstream);

#endif  // PRINT_DAG_PRINT_DAG_HPP
