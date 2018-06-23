#include "CodeGenVisitor.h"

#include <regex>
#include <stdexcept>
#include <string>
#include <vector>

#include <boost/algorithm/string/join.hpp>
#include <boost/format.hpp>
#include <boost/mpl/list.hpp>

#include "dag/DAGOperators.h"
#include "dag/collection/array.hpp"
#include "dag/collection/atomic.hpp"
#include "dag/utils/field_visitor.hpp"
#include "utils/utils.h"
#include "utils/visitor.hpp"

using boost::algorithm::join;
using boost::format;

namespace code_gen {
namespace cpp {

class FieldTypeVisitor : public dag::utils::FieldVisitor {
public:
    std::string ComputeTypeString(dag::collection::Field *field) {
        field->Accept(this);
        return this->ret_type_string_;
    }

protected:
    void Visit(dag::collection::Atomic *field) override {
        this->ret_type_string_ = field->field_type()->type;
    }
    void Visit(dag::collection::Array *field) override {
        std::vector<std::string> item_types;
        for (auto pos = 0; pos < field->tuple()->fields.size(); pos++) {
            auto f = field->tuple()->fields[pos];
            f.get()->Accept(this);
            auto item_type = this->ret_type_string_;
            item_types.push_back(item_type + " v" + std::to_string(pos));
            pos++;
        }
        std::vector<std::string> sizes;
        for (size_t i = 0; i < field->field_type()->number_dim; i++) {
            sizes.push_back("size_t size" + std::to_string(i));
        }

        this->ret_type_string_ =
                (boost::basic_format<char>(
                         "struct { struct {%s;}* v0; size_t sizes [%s];}") %
                 join(item_types, ";") %
                 std::to_string(field->field_type()->number_dim))
                        .str();
    }

private:
    std::string ret_type_string_;
};

auto CodeGenVisitor::TupleTypeDesc::MakeFromCollection(
        std::string &&name, const dag::collection::Tuple &tuple)
        -> TupleTypeDesc {
    std::vector<std::string> types;

    for (auto &ft : tuple.fields) {
        FieldTypeVisitor visitor;
        types.emplace_back(visitor.ComputeTypeString(ft.get()));
    }
    std::vector<std::string> names;
    for (size_t i = 0; i < types.size(); i++) {
        names.emplace_back("v" + std::to_string(i));
    }

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

void CodeGenVisitor::operator()(DAGCollection *op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "CollectionSourceOperator");

    emitOperatorMake(var_name, "CollectionSourceOperator", op,
                     {op->add_index ? "true" : "false"}, {});
}

void CodeGenVisitor::operator()(DAGConstantTuple *op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "ConstantTupleOperator");

    const auto return_type = operatorNameTupleTypeMap[op->id].return_type;
    const auto tuple_arg =
            (format("%s{%s}") % return_type.name % join(op->values, ",")).str();

    emitOperatorMake(var_name, "ConstantTupleOperator", op, {}, {tuple_arg});
}

void CodeGenVisitor::operator()(DAGMap *op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "MapOperator");

    auto input_type =
            operatorNameTupleTypeMap[dag_->predecessor(op)->id].return_type;
    auto return_type = operatorNameTupleTypeMap[op->id].return_type;
    const std::string functor =
            visitLLVMFunc(*op, {input_type}, return_type.name);

    emitOperatorMake(var_name, "MapOperator", op, {}, {functor});
}

void CodeGenVisitor::operator()(DAGMaterializeRowVector *op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "MaterializeRowVectorOperator");

    auto tuple_type = operatorNameTupleTypeMap[op->id].return_type;
    auto return_type =
            TupleTypeDesc{"", {tuple_type.name + "*", "size_t"}, {"v0", "v1"}};
    return_type.name = emitTupleDefinition(return_type);
    operatorNameTupleTypeMap[op->id].return_type = return_type;

    auto input_type =
            operatorNameTupleTypeMap[dag_->predecessor(op)->id].return_type;
    emitOperatorMake(var_name, "MaterializeRowVectorOperator", op,
                     {input_type.name}, {});
}

void CodeGenVisitor::operator()(DAGParameterLookup *op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "ConstantTupleOperator");

    auto tuple_type = operatorNameTupleTypeMap[op->id].return_type;
    auto return_type =
            TupleTypeDesc{"", {tuple_type.name + "*", "size_t"}, {"v0", "v1"}};
    return_type.name = emitTupleDefinition(return_type);
    operatorNameTupleTypeMap[op->id].return_type = return_type;

    auto input_name_pair = getNextInputName();
    inputNames.push_back(input_name_pair);

    const auto input_arg =
            (boost::format("%1%{ reinterpret_cast<%2% *>(%3%), %4% }") %
             return_type.name % tuple_type.name % input_name_pair.first %
             input_name_pair.second)
                    .str();

    emitOperatorMake(var_name, "ConstantTupleOperator", op, {}, {input_arg});
}

void CodeGenVisitor::operator()(DAGReduce *op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "ReduceOperator");

    auto return_type = operatorNameTupleTypeMap[op->id].return_type;
    const std::string functor =
            visitLLVMFunc(*op, {return_type, return_type}, return_type.name);

    emitOperatorMake(var_name, "ReduceOperator", op, {}, {functor});
}

void CodeGenVisitor::operator()(DAGRange *op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "RangeSourceOperator");

    emitOperatorMake(var_name, "RangeSourceOperator", op, {}, {});
};

void CodeGenVisitor::operator()(DAGFilter *op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "FilterOperator");

    auto input_type =
            operatorNameTupleTypeMap[dag_->predecessor(op)->id].return_type;
    const std::string functor = visitLLVMFunc(*op, {input_type}, "bool");

    emitOperatorMake(var_name, "FilterOperator", op, {}, {functor});
};

void CodeGenVisitor::operator()(DAGJoin *op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "JoinOperator");

    // Build key and value types
    const auto up1Type =
            operatorNameTupleTypeMap[dag_->predecessor(op, 0)->id].return_type;
    const auto up2Type =
            operatorNameTupleTypeMap[dag_->predecessor(op, 1)->id].return_type;

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

void CodeGenVisitor::operator()(DAGCartesian *op) {
    const std::string var_name =
            CodeGenVisitor::visit_common(op, "CartesianOperator");
    std::string pred2Tuple =
            operatorNameTupleTypeMap[dag_->predecessor(op, 1)->id]
                    .return_type.name;
    std::vector<std::string> template_args = {pred2Tuple};
    emitOperatorMake(var_name, "CartesianOperator", op, template_args);
};

void CodeGenVisitor::visit_reduce_by_key(DAGOperator *op,
                                         const std::string &operator_name) {
    assert(dynamic_cast<DAGReduceByKey *>(op) != nullptr ||
           dynamic_cast<DAGReduceByKeyGrouped *>(op) != nullptr);

    const std::string var_name =
            CodeGenVisitor::visit_common(op, operator_name);

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
    emitOperatorMake(var_name, operator_name, op, template_args, {functor});
}

void CodeGenVisitor::operator()(DAGReduceByKey *op) {
    visit_reduce_by_key(op, "ReduceByKeyOperator");
}

void CodeGenVisitor::operator()(DAGReduceByKeyGrouped *op) {
    visit_reduce_by_key(op, "ReduceByKeyGroupedOperator");
}

void CodeGenVisitor::operator()(DAGOperator *const op) {
    throw std::runtime_error("CodeGen encountered unknown operator type: " +
                             op->name());
}

// TODO(ingo): This could be an independent visitor
std::string CodeGenVisitor::visit_common(DAGOperator *op,
                                         const std::string &operator_name) {
    planBody_ << format("/** %s **/\n") % operator_name;
    const std::string var_name = getNextOperatorName();

    auto output_type = TupleTypeDesc::MakeFromCollection("", *op->tuple);
    output_type.name = emitTupleDefinition(output_type);

    operatorNameTupleTypeMap.emplace(
            op->id, OperatorDesc{op->id, var_name, output_type});
    includes.insert("\"" + operator_name + ".h\"");
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
    for (size_t i = 0; i < op->num_in_ports(); i++) {
        const auto pred = dag_->predecessor(op, i);
        const auto arg = "&" + operatorNameTupleTypeMap[pred->id].var_name;
        args.emplace_back(arg);
    }

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

        // Each collection of all input tuples is one argument to the function
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

}  // namespace cpp
}  // namespace code_gen
