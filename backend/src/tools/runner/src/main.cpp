#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include <boost/dll.hpp>
#include <boost/polymorphic_pointer_cast.hpp>
#include <boost/program_options.hpp>

#include "dag/dag.hpp"
#include "dag/operators/compiled_pipeline.hpp"
#include "runtime/execute_plan.hpp"
#include "utils/lib_path.hpp"

namespace po = boost::program_options;

auto main(int argc, char *argv[]) -> int {
    std::string dag_file_name;
    std::string input_file_name;
    std::string output_file_name;

    po::options_description desc("Run DAG on given input.");
    desc.add_options()                             //
            ("help", "Produce this help message")  //
            ("dag,d", po::value<std::string>(&dag_file_name)->required(),
             "Path to file with DAG in JSON format")  //
            ("input,i", po::value<std::string>(&input_file_name),
             "Path to file with input values in JSON format")  //
            ("output,o", po::value<std::string>(&output_file_name),
             "Path to file with output values in JSON format")  //
            ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);

    if (vm.count("help") > 0) {
        std::cout << desc << std::endl;
        return 0;
    }

    po::notify(vm);

    // Open input and output files
    std::ifstream dag_file(dag_file_name);
    std::ifstream input_file(input_file_name);
    std::ofstream output_file(output_file_name);

    std::istream &input = input_file_name.empty() ? std::cin : input_file;
    std::ostream &output = output_file_name.empty() ? std::cout : output_file;

    // Load DAG, run, and output result
    std::unique_ptr<DAG> dag(ParseDag(&dag_file));
    const std::string inputs_str(std::istreambuf_iterator<char>(input), {});

    auto const output_str = runtime::ExecutePlan(dag.get(), inputs_str);

    output << output_str << std::endl;
}
