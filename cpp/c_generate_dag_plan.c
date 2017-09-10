#include "generate_dag_plan.hpp"

int c_generate_dag_plan(char *dagstr,  unsigned long counter) {
    return generate_dag_plan(dagstr, counter);
}

int c_close_library(void * handle) {
    return close_library(handle);
}