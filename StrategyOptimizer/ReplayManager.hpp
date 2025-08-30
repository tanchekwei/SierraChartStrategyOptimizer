// Copyright (c) 2025 Chek Wei Tan
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#ifndef REPLAY_MANAGER_HPP
#define REPLAY_MANAGER_HPP

#include "../sierrachart.h"
#include "StrategyOptimizer.hpp"
#include <vector>
#include <string>

namespace ReplayManager
{
    void ResetButton(SCStudyInterfaceRef sc, SCInputRef input);

    void StartReplayForCombination(
        SCStudyInterfaceRef sc,
        const StrategyOptimizerConfig &config,
        const std::vector<std::vector<double>> &combinations,
        int comboIndex,
        ReplayState &replayState
    );

    void SetStudyInputs(SCStudyInterfaceRef sc, const StrategyOptimizerConfig &config, const std::vector<double> &combinations);
}

#endif // REPLAY_MANAGER_HPP