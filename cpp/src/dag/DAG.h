//
// Created by sabir on 04.07.17.
//

#ifndef DAG_DAG_H
#define DAG_DAG_H

#include <iostream>
#include <memory>
#include <set>
#include <vector>

#include <json.hpp>

#include "Column.h"

class DAG;
class DAGOperator;

namespace nlohmann {
template <>
struct adl_serializer<std::unique_ptr<DAG>>;
}  // namespace nlohmann

/**
 * destructor responsible for freeing the columns and the dag operators
 */
// TODO(sabir): refactor class in order to remove custom destructor
class DAG {  // NOLINT hicpp-special-member-functions
public:
    DAG() = default;
    DAG(DAG &&other) = default;

    ~DAG() { Column::delete_columns(); }

    size_t last_operator_id() const;
    const std::vector<std::unique_ptr<DAGOperator>> &operators() const {
        return operators_;
    }

    void addOperator(DAGOperator *op);

    DAGOperator *sink{};

private:
    std::vector<std::unique_ptr<DAGOperator>> operators_;
    std::set<size_t> operator_ids_;
};

namespace nlohmann {

template <>
struct adl_serializer<std::unique_ptr<DAG>> {
    static std::unique_ptr<DAG> from_json(const nlohmann::json &json);
    // NOLINTNEXTLINE google-runtime-references
    static void to_json(nlohmann::json &json, const std::unique_ptr<DAG> &dag);
};

}  // namespace nlohmann

#endif  // DAG_DAG_H
