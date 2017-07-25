#include <utility>
#include <cstddef>
#include <tuple>
#include <iostream>
#include "code_gen/generate_code.h"

#include "dag/DAGCreation.hpp"


#include <string>
#include <fstream>
#include <streambuf>

std::string exec(const char *cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != nullptr){
            result += buffer.data();
        }
    }
    return result;
}

extern "C" {
int generate_dag_plan(char *dagstr) {
    //to run with main:
//    std::string filename("dag.json");
//
//    std::ifstream t(filename);
//    std::string str((std::istreambuf_iterator<char>(t)),
//                    std::istreambuf_iterator<char>());
//    DAG *dag = parse_dag(str);

    DAG *dag = parse_dag(std::string(dagstr));
    generate_code(dag);

    //compile the code to some object file
    //call make in the subdir
    cout << exec("cd cpp/gen && make -j");
    //return

    //in python call the execute() function

    //which should return a pointer to the result
    //which should be interpreted as specified by the user
    //e.g. list or an integer(if the action is count)
    return 0;
}
}