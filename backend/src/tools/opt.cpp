#include <fstream>
#include <iostream>
#include <string>

#include <boost/program_options.hpp>

#include "dag/DAG.h"
#include "optimize/optimizer.hpp"
#include "utils/printDAG.h"

namespace po = boost::program_options;

// Enum and I/O of output formats
enum class OutputFormat { kJson, kDot, kBin };

std::istream& operator>>(std::istream& in, OutputFormat& format) {
    std::string token;
    in >> token;
    if (token == "JSON") {
        format = OutputFormat::kJson;
    } else if (token == "DOT") {
        format = OutputFormat::kDot;
    } else if (token == "BIN") {
        format = OutputFormat::kBin;
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
        case OutputFormat::kBin:
            out << "BIN";
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
    bool verbose = false;
    std::vector<std::string> optimizations;

    po::options_description desc("Deserialize, optimize, and serialize a DAG");
    desc.add_options()                             //
            ("help", "Produce this help message")  //
            ("optimization-level,O",
             po::value<size_t>(&opt_level)->default_value(0),
             "Optimization level")  //
            ("optimizations,t",
             po::value<std::vector<std::string>>(&optimizations),
             "Optimization level")  //
            ("input,i", po::value<std::string>(&input_file_name),
             "Path to input file")  //
            ("output,o", po::value<std::string>(&output_file_name),
             "Path to output file")  //
            ("output-format,f",
             po::value<OutputFormat>(&output_format)
                     ->default_value(OutputFormat::kJson),
             "Output format (JSON, DOT, BIN)")  //
            ("verbose,v",
             po::value<bool>(&verbose)
                     ->default_value(false)   //
                     ->implicit_value(true),  //
             "Print some debug information");

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

    // Parse DAG
    const std::string dagstr(std::istreambuf_iterator<char>(input), {});
    std::unique_ptr<DAG> dag(ParseDag(dagstr));

    // Assemble backend configuration
    nlohmann::json conf_json;
    conf_json["/optimizer/optimization-level"] = opt_level;

    if (output_format != OutputFormat::kBin) {
        conf_json["/optimizer/optimizations/code_gen/active"] = false;
    }

    for (auto const& opt : optimizations) {
        conf_json["/optimizer/optimizations/" + opt + "/active"] = true;
    }

    conf_json["/optimizer/verbose"] = verbose;

    // Run the optimizer
    optimize::Optimizer opt(conf_json.unflatten().dump());
    opt.Run(dag.get());

    // Produce output
    switch (output_format) {
        case OutputFormat::kJson: {
            nlohmann::json json(dag);
            output << json << std::endl;
        } break;
        case OutputFormat::kDot: {
            ToDotStream(dag.get(), &output);
        } break;
        case OutputFormat::kBin: {
        } break;
        default:
            throw std::runtime_error("Invalid output format");
    }
}
