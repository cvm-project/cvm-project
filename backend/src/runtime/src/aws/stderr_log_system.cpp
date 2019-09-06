/*
 * Copyright 2010-2017 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://aws.amazon.com/apache2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */
// Based on ConsoleLogSystem from AWS C++ SDK v1.7.138

#include "stderr_log_system.hpp"

#include <iostream>

namespace runtime::aws {

void StderrLogSystem::ProcessFormattedStatement(Aws::String&& statement) {
    std::cerr << statement;
}

void StderrLogSystem::Flush() { std::cerr.flush(); }

}  // namespace runtime::aws
