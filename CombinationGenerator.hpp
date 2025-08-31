// Copyright (c) 2025 Chek Wei Tan
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#ifndef COMBINATION_GENERATOR_HPP
#define COMBINATION_GENERATOR_HPP

#include <vector>
#include "ConfigManager.hpp"

namespace CombinationGenerator
{
    std::vector<std::vector<double>> Generate(const std::vector<InputConfig> &param_configs);
    std::vector<std::vector<double>> GenerateIterative(const std::vector<InputConfig>& params);
}

#endif // COMBINATION_GENERATOR_HPP