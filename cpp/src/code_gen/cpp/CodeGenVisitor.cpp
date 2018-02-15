#include "CodeGenVisitor.h"

#include <algorithm>
#include <cstdlib>
#include <regex>
#include <set>
#include <stdexcept>
#include <unordered_map>
#include <utility>

#include <boost/algorithm/string/join.hpp>
#include <boost/format.hpp>

#include "dag/DAG.h"
#include "dag/DAGOperators.h"
#include "utils/utils.h"

using boost::algorithm::join;
using boost::format;

auto CodeGenVisitor::TupleTypeDesc::fromTupleString(std::string &&name,
                                                    const std::string &s)
        -> TupleTypeDesc {
    // Types
    std::string types_string = s;
    types_string = string_replace(types_string, "(", "");
    types_string = string_replace(types_string, ")", "");
    types_string = string_replace(types_string, " ", "");
    std::vector<std::string> types = split_string(types_string, ",");

    // Names
    std::vector<std::string> names;
    for (size_t i = 0; i < types.size(); i++) {
        names.emplace_back("v" + std::to_string(i));
    }

    // Typed names
    return TupleTypeDesc{name, types, names};
}

std::string CodeGenVisitor::TupleTypeDesc::field_definitions() const {
    assert(types.size() == names.size());
    std::vector<std::string> typed_names;
    for (size_t i = 0; i < types.size(); i++) {
        typed_names.emplace_back(types[i] + " " + names[i]);
    }
    return join(typed_names, "; ");
}

auto CodeGenVisitor::TupleTypeDesc::computeHeadType(
        const size_t head_size) const -> TupleTypeDesc {
    assert(num_fields() >= head_size);
    TupleTypeDesc ret = *this;
    ret.types.resize(head_size);
    ret.names.resize(head_size);
    return ret;
}

auto CodeGenVisitor::TupleTypeDesc::computeTailType(
        const size_t head_size) const -> TupleTypeDesc {
    assert(num_fields() >= head_size);
    TupleTypeDesc ret = *this;
    ret.types.erase(ret.types.begin(), ret.types.begin() + head_size);
    ret.names.resize(ret.types.size());
    return ret;
}

void CodeGenVisitor::visit(DAGCollection *op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "CollectionSourceOperator");

    // TODO(sabir): Wrap input into (ptr, size) struct.
    //              This should go along with a generalization of DAG inputs.
    auto inputNamePair = getNextInputName();
    inputNames.push_back(inputNamePair);
    auto return_type = operatorNameTupleTypeMap[op->id].return_type;

    emitOperatorMake(
            var_name, "CollectionSourceOperator", op,
            {op->add_index ? "true" : "false"},
            {(format("(%s*)%s") % return_type.name % inputNamePair.first).str(),
             inputNamePair.second});
}

void CodeGenVisitor::visit(DAGMap *op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "MapOperator");

    auto input_type =
            operatorNameTupleTypeMap[op->predecessors[0]->id].return_type;
    auto return_type = operatorNameTupleTypeMap[op->id].return_type;
    const std::string functor =
            visitLLVMFunc(*op, {input_type}, return_type.name);

    emitOperatorMake(var_name, "MapOperator", op, {}, {functor});
}

void CodeGenVisitor::visit(DAGReduce *op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "ReduceOperator");

    auto return_type = operatorNameTupleTypeMap[op->id].return_type;
    const std::string functor =
            visitLLVMFunc(*op, {return_type, return_type}, return_type.name);

    emitOperatorMake(var_name, "ReduceOperator", op, {}, {functor});
}

void CodeGenVisitor::visit(DAGRange *op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "RangeSourceOperator");

    emitOperatorMake(var_name, "RangeSourceOperator", op, {},
                     {op->from, op->to, op->step});
};

void CodeGenVisitor::visit(DAGFilter *op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "FilterOperator");

    auto input_type =
            operatorNameTupleTypeMap[op->predecessors[0]->id].return_type;
    const std::string functor = visitLLVMFunc(*op, {input_type}, "bool");

    emitOperatorMake(var_name, "FilterOperator", op, {}, {functor});
};

void CodeGenVisitor::visit(DAGJoin *op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "JoinOperator");

    // Build key and value types
    const auto up1Type =
            operatorNameTupleTypeMap[op->predecessors[0]->id].return_type;
    const auto up2Type =
            operatorNameTupleTypeMap[op->predecessors[1]->id].return_type;

    // TODO(sabir): This currently only works for keys of size 1
    TupleTypeDesc key_type = up1Type.computeHeadType();
    key_type.name = emitTupleDefinition(key_type);

    TupleTypeDesc value_type1 = up1Type.computeTailType();
    value_type1.name = emitTupleDefinition(value_type1);

    TupleTypeDesc value_type2 = up2Type;
    value_type2.name = emitTupleDefinition(value_type2);

    // Build operator
    std::vector<std::string> template_args = {key_type.name, value_type1.name,
                                              value_type2.name};

    emitOperatorMake(var_name, "JoinOperator", op, template_args);
};

void CodeGenVisitor::visit(DAGCartesian *op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "CartesianOperator");
    std::string pred2Tuple =
            operatorNameTupleTypeMap[op->predecessors[1]->id].return_type.name;
    std::vector<std::string> template_args = {pred2Tuple};
    emitOperatorMake(var_name, "CartesianOperator", op, template_args);
};

void CodeGenVisitor::visit(DAGReduceByKey *op) {
    // TODO(sabir): This should be done in some earlier optimizer pass
    const auto &properties = op->fields[0].properties;
    bool is_grouped = properties->count(FL_GROUPED) > 0 ||
                      properties->count(FL_SORTED) > 0 ||
                      properties->count(FL_UNIQUE) > 0;
    // TODO(sabir): This should be done in some earlier optimizer pass
    std::string operatorName =
            is_grouped ? "ReduceByKeyGroupedOperator" : "ReduceByKeyOperator";

    const std::string var_name = CodeGenVisitor::visit_common(op, operatorName);

    // Build key and value types
    auto return_type = operatorNameTupleTypeMap[op->id].return_type;

    // TODO(sabir): This currently only works for keys of size 1
    TupleTypeDesc key_type = return_type.computeHeadType();
    key_type.name = emitTupleDefinition(key_type);

    TupleTypeDesc value_type = return_type.computeTailType();
    value_type.name = emitTupleDefinition(value_type);

    // Construct functor
    const std::string functor =
            visitLLVMFunc(*op, {value_type, value_type}, value_type.name);

    // Collect template arguments
    std::vector<std::string> template_args = {key_type.name, value_type.name};

    // Generate call
    emitOperatorMake(var_name, operatorName, op, template_args, {functor});
};

// TODO(ingo): This could be an independent visitor
std::string CodeGenVisitor::visit_common(DAGOperator *op,
                                         const std::string &operator_name) {
    planBody_ << format("/** %s **/\n") % operator_name;
    const std::string var_name = getNextOperatorName();

    auto output_type = TupleTypeDesc::fromTupleString("", op->output_type);
    output_type.name = emitTupleDefinition(output_type);

    operatorNameTupleTypeMap.emplace(
            op->id, OperatorDesc{op->id, var_name, output_type});
    includes.insert("\"operators/" + operator_name + ".h\"");
    return var_name;
}

std::string CodeGenVisitor::visitLLVMFunc(
        const DAGOperator &op, const std::vector<TupleTypeDesc> &input_types,
        const std::string &return_type) {
    const std::string func_name = op.name() + getNextLLVMFuncName();
    const std::string functor_name = snake_to_camel_string(func_name);
    storeLLVMCode(op.llvm_ir, func_name);
    emitLLVMFunctionWrapper(func_name, input_types, return_type);
    return functor_name + "()";
}

void CodeGenVisitor::emitOperatorMake(
        const std::string &variable_name, const std::string &operator_name,
        const DAGOperator *const op,
        const std::vector<std::string> &extra_template_args,
        const std::vector<std::string> &extra_args) {
    // First template argument: current tuple
    auto return_type = operatorNameTupleTypeMap[op->id].return_type;
    std::vector<std::string> template_args = {return_type.name};

    // Take over extra template arguments
    template_args.insert(template_args.end(), extra_template_args.begin(),
                         extra_template_args.end());

    // Default input arguments: references to predecessors
    std::vector<std::string> args;
    std::transform(op->predecessors.begin(), op->predecessors.end(),
                   std::back_inserter(args), [this](auto op) {
                       return "&" + operatorNameTupleTypeMap[op->id].var_name;
                   });

    // Take over extra arguments
    args.insert(args.end(), extra_args.begin(), extra_args.end());

    // Generate call
    planBody_ << format("auto %s = make%s<%s>(%s);") % variable_name %
                         operator_name % join(template_args, ",") %
                         join(args, ",");
}

std::string CodeGenVisitor::emitTupleDefinition(const TupleTypeDesc &type) {
    const auto ret = tupleDefinitions_.emplace(type.field_definitions(),
                                               getNextTupleName());
    const auto name = ret.first->second;

    if (ret.second) {
        planDeclarations_ << format("typedef struct { %s; } %s;") %
                                     type.field_definitions() % name;
    }

    return name;
}

void CodeGenVisitor::emitLLVMFunctionWrapper(
        const std::string &func_name,
        const std::vector<TupleTypeDesc> &input_types,
        const std::string &return_type) {
    const std::string class_name = snake_to_camel_string(func_name);

    std::vector<std::string> input_args;
    std::vector<std::string> call_args;
    std::vector<std::string> call_types;
    for (auto const &input_type : input_types) {
        // Each input tuple is one argument of the functor
        const auto input_var_name = "t" + std::to_string(input_args.size());
        input_args.emplace_back(input_type.name + " " + input_var_name);

        // Each field of all input tuples is one argument to the function
        // --> construct values for the call
        for (auto const &field : input_type.names) {
            // NOLINTNEXTLINE performance-inefficient-string-concatenation
            call_args.emplace_back(input_var_name + "." + field);
        }

        // --> construct types for the declaration
        call_types.insert(call_types.begin(), input_type.types.begin(),
                          input_type.types.end());
    }

    // Emit functor definition
    planBody_ << format("class %s {"
                        "public:"
                        "    auto operator()(%s) {"
                        "        return %s(%s);"
                        "    }"
                        "};") %
                         class_name % join(input_args, ",") % func_name %
                         join(call_args, ",");

    // Emit function declaration
    planDeclarations_ << format("extern \"C\" { %s %s(%s); }") % return_type %
                                 func_name % join(call_types, ",");
}

void CodeGenVisitor::storeLLVMCode(const std::string &ir,
                                   const std::string &func_name) {
    std::string patched_ir = ir;
    // the local_unnamed_addr std::string is not llvm-3.7 compatible:
    std::regex reg1("local_unnamed_addr #.? ");
    patched_ir = std::regex_replace(patched_ir, reg1, "");
    //        patched_ir = string_replace(patched_ir, "local_unnamed_addr #1
    //        ", "");
    // replace the func name with our
    std::regex reg("@cfuncnotuniquename");
    patched_ir = std::regex_replace(patched_ir, reg, "@\"" + func_name + "\"");
    // write code to the gen dir
    llvmCode_ << patched_ir;
}
