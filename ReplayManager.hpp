// Copyright (c) 2025 Chek Wei Tan
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#pragma once
#include "../sierrachart.h"
#include "ConfigManager.hpp"
#include "Enum.hpp"
#include <vector>

namespace ReplayManager
{
    void ResetButton(SCStudyInterfaceRef sc, SCInputRef input);

    void StartReplayForCombination(
        SCStudyInterfaceRef sc,
        const StrategyOptimizerConfig& config,
        const std::vector<std::vector<int>>& combinations,
        int comboIndex,
        ReplayState& replayState
    );
}