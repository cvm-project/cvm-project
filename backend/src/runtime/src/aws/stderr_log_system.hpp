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

#ifndef AWS_STDERR_LOG_SYSTEM_HPP
#define AWS_STDERR_LOG_SYSTEM_HPP

#include <aws/core/utils/logging/FormattedLogSystem.h>

namespace runtime {
namespace aws {

/**
 * Log system interface that logs to std::cerr
 */
class StderrLogSystem : public Aws::Utils::Logging::FormattedLogSystem {
public:
    using Base = Aws::Utils::Logging::FormattedLogSystem;
    using LogLevel = Aws::Utils::Logging::LogLevel;

    explicit StderrLogSystem(LogLevel logLevel) : Base(logLevel) {}

    ~StderrLogSystem() override = default;

    /**
     * Flushes buffered messages to stdout.
     */
    void Flush() override;

protected:
    void ProcessFormattedStatement(Aws::String&& statement) override;
};

}  // namespace aws
}  // namespace runtime

#endif  // AWS_STDERR_LOG_SYSTEM_HPP
