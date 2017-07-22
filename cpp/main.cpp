#include <utility>
#include <cstddef>
#include <tuple>
#include <iostream>
#include "code_gen/generate_code.h"

#include "src/dag/DAGCreation.hpp"

//
//struct small_tuple{
//    long a;
//    long b;
//};
//
//struct big_tuple {
//    long a;
//    long b;
//    long c;
//    long d;
//};
//
//small_tuple func(small_tuple t){
//    return {t.a, t.b};
//}
//
int main(int argc, char **argv) {
    DAG *dag = parse_dag();
    generate_code(dag);
    return 0;
}
