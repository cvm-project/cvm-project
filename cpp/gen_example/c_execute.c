
#include "tuples.h"

result_type *execute();

void free_result(result_type *ptr);


result_type *c_execute() { return execute(); }

void c_free_result(result_type *ptr) { return free_result(ptr); }