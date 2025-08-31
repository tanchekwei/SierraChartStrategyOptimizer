// Copyright (c) 2025 Chek Wei Tan
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#include <vector>
#include <cmath>
#include "ConfigManager.hpp"
#include "CombinationGenerator.hpp"

namespace CombinationGenerator
{
    void GenerateCombinations(
        size_t k,
        std::vector<std::vector<double>> &combinations,
        std::vector<double> &current_combination,
        const std::vector<InputConfig> &params)
    {
        if (k == params.size())
        {
            combinations.push_back(current_combination);
            return;
        }

        const auto& param = params[k];

        if (std::fabs(param.Increment) < 1e-9)
        {
            if (param.MinValue <= param.MaxValue)
            {
                current_combination.push_back(param.MinValue);
                GenerateCombinations(k + 1, combinations, current_combination, params);
                current_combination.pop_back();
            }
        }
        else if (param.Increment > 0)
        {
            for (double i = param.MinValue; i <= param.MaxValue + 1e-9; i += param.Increment)
            {
                current_combination.push_back(i);
                GenerateCombinations(k + 1, combinations, current_combination, params);
                current_combination.pop_back();
            }
        }
        else // param.Increment < 0
        {
             for (double i = param.MinValue; i >= param.MaxValue - 1e-9; i += param.Increment)
            {
                current_combination.push_back(i);
                GenerateCombinations(k + 1, combinations, current_combination, params);
                current_combination.pop_back();
            }
        }
    }

    std::vector<std::vector<double>> Generate(const std::vector<InputConfig> &params)
    {
        std::vector<std::vector<double>> combinations;
        std::vector<double> current_combination;
        GenerateCombinations(0, combinations, current_combination, params);
        return combinations;
    }
}