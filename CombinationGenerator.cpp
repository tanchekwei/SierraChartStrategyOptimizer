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

        const auto &param = params[k];

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

    std::vector<std::vector<double>> GenerateIterative(const std::vector<InputConfig> &params)
    {
        std::vector<std::vector<double>> combinations;
        std::vector<InputConfig> varyingParams;
        for (const auto &p : params)
        {
            if (std::fabs(p.Increment) > 1e-9)
            {
                varyingParams.push_back(p);
            }
        }

        if (varyingParams.empty())
        {
            if (!params.empty())
            {
                combinations.push_back({});
            }
            return combinations;
        }

        std::vector<double> currentCombination;
        std::vector<size_t> p_indices(varyingParams.size(), 0);
        std::vector<std::vector<double>> paramValues;

        for (const auto &p : varyingParams)
        {
            std::vector<double> values;
            if (p.Increment > 0)
            {
                for (double i = p.MinValue; i <= p.MaxValue + 1e-9; i += p.Increment)
                {
                    values.push_back(i);
                }
            }
            else
            {
                for (double i = p.MinValue; i >= p.MaxValue - 1e-9; i += p.Increment)
                {
                    values.push_back(i);
                }
            }
            paramValues.push_back(values);
        }

        while (true)
        {
            currentCombination.clear();
            for (size_t i = 0; i < varyingParams.size(); ++i)
            {
                currentCombination.push_back(paramValues[i][p_indices[i]]);
            }
            combinations.push_back(currentCombination);

            size_t k = varyingParams.size() - 1;
            while (k >= 0)
            {
                p_indices[k]++;
                if (p_indices[k] < paramValues[k].size())
                {
                    break;
                }
                p_indices[k] = 0;
                k--;
            }

            if (k < 0)
            {
                break;
            }
        }

        return combinations;
    }
}