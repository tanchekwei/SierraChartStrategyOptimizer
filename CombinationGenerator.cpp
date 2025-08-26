// Copyright (c) 2025 Chek Wei Tan
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#include <vector>
#include "ConfigManager.hpp"
#include "CombinationGenerator.hpp"

namespace CombinationGenerator
{
    void GenerateCombinations(
        size_t k,
        std::vector<std::vector<int>> &combinations,
        std::vector<int> &current_combination,
        const std::vector<InputConfig> &params)
    {
        if (k == params.size())
        {
            combinations.push_back(current_combination);
            return;
        }

        for (int i = params[k].MinValue; i <= params[k].MaxValue; i += params[k].Increment)
        {
            current_combination.push_back(i);

            GenerateCombinations(k + 1, combinations, current_combination, params);
            current_combination.pop_back();
        }
    }

    std::vector<std::vector<int>> Generate(const std::vector<InputConfig> &params)
    {
        std::vector<std::vector<int>> combinations;
        std::vector<int> current_combination;
        GenerateCombinations(0, combinations, current_combination, params);
        return combinations;
    }
}