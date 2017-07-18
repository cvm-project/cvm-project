//
// Created by sabir on 04.07.17.
//

#include "DAGCollection.h"

void DAGCollection::initWithJson(nlohmann::basic_json<> j){
    this->values = j["values"];
}
