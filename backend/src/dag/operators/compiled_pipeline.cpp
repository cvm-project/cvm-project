#include "compiled_pipeline.hpp"

void DAGCompiledPipeline::from_json(const nlohmann::json &json) {
    this->num_inputs = json.at("num_inputs");
    this->library_name = json.at("library_name");
    this->function_name = json.at("function_name");
}

void DAGCompiledPipeline::to_json(nlohmann::json *json) const {
    json->emplace("num_inputs", this->num_inputs);
    json->emplace("library_name", this->library_name);
    json->emplace("function_name", this->function_name);
}
