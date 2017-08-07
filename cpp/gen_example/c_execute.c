#include "c_execute.h"

result_type *execute(void*, unsigned long);

void free_result(result_type *ptr);


result_type *c_execute(void *input_ptr, unsigned long size) { return execute(input_ptr, size); }

void c_free_result(result_type *ptr) { return free_result(ptr); }