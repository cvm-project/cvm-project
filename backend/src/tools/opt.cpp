#include <fstream>
#include <iostream>
#include <string>

#include <boost/program_options.hpp>

#include "dag/DAG.h"
#include "dag/DAGCreation.hpp"
#include "optimize/Optimizer.h"
#include "utils/printDAG.h"

namespace po = boost::program_options;

// Enum and I/O of output formats
enum class OutputFormat { kJson, kDot };

std::istream& operator>>(std::istream& in, OutputFormat& format) {
    std::string token;
    in >> token;
    if (token == "JSON") {
        format = OutputFormat::kJson;
    } else if (token == "DOT") {
        format = OutputFormat::kDot;
    } else {
        in.setstate(std::ios_base::failbit);
    }
    return in;
}

std::ostream& operator<<(std::ostream& out, const OutputFormat& format) {
    switch (format) {
        case OutputFormat::kJson:
            out << "JSON";
            break;
        case OutputFormat::kDot:
            out << "DOT";
            break;
        default:
            out.setstate(std::ios_base::failbit);
    }
    return out;
}

int main(int argc, char* argv[]) {
    // Read command line parameters
    std::string input_file_name;
    std::string output_file_name;
    OutputFormat output_format{};
    size_t opt_level = 0;

    po::options_description desc("Deserialize, optimize, and serialize a DAG");
    desc.add_options()                             //
            ("help", "Produce this help message")  //
            ("optimization-level,O",
             po::value<size_t>(&opt_level)->default_value(0),
             "Optimization level")  //
            ("input,i", po::value<std::string>(&input_file_name),
             "Path to input file")  //
            ("output,o", po::value<std::string>(&output_file_name),
             "Path to output file")  //
            ("output-format,f",
             po::value<OutputFormat>(&output_format)
                     ->default_value(OutputFormat::kJson),
             "Output format (JSON, DOT)");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help") > 0) {
        std::cout << desc << std::endl;
        return 0;
    }

    // Open input and output files
    std::ifstream input_file(input_file_name);
    std::ifstream output_file(output_file_name);

    std::istream& input = vm.count("input") > 0 ? input_file : std::cin;
    std::ostream output(vm.count("output") > 0 ? output_file.rdbuf()
                                               : std::cout.rdbuf());

    // Read file content and parse
    std::unique_ptr<DAG> dag(parse_dag(&input));

    if (opt_level > 0) {
        // Optimize
        Optimizer opt;
        opt.run(dag.get());
    }

    switch (output_format) {
        case OutputFormat::kJson: {
            nlohmann::json json(dag);
            output << json << std::endl;
        } break;
        case OutputFormat::kDot:
            ToDotStream(dag.get(), &output);
            break;
        default:
            throw std::runtime_error("Invalid output format");
    }
}
