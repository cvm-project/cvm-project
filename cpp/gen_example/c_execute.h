typedef struct tuple_1 {
    long v1;
    long v2;
    double v3;
} tuple_1;

typedef struct  {
    unsigned int size;
    tuple_1 *data;
} result_type;

result_type *c_execute();

void free_result(result_type *);