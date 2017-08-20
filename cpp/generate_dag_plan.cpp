#include <utility>
#include <cstddef>
#include <tuple>
#include <iostream>
#include "code_gen/generate_code.h"
#include "src/utils/timing.cpp"

#include "dag/DAGCreation.hpp"


std::string exec(const char *cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != nullptr) {
            result += buffer.data();
        }
    }
    return result;
}

extern "C" {
int generate_dag_plan(char *dagstr) {

    exec(("rm -r -f " + get_lib_path() + "cpp/gen").c_str());

    DAG *dag = parse_dag(std::string(dagstr));

    generate_code(dag);

    //call make in the subdir
    //compile the code into shared lib
    TICK1
    exec(("cd " + get_lib_path() + "cpp/gen && make -f ../src/utils/Makefile -j").c_str());
    TOCK1
    std::cout << "call make " << DIFF1 << std::endl;

    return 0;
}
}