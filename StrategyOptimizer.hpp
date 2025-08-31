// Copyright (c) 2025 Chek Wei Tan
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#ifndef STRATEGY_OPTIMIZER_HPP
#define STRATEGY_OPTIMIZER_HPP

#include "../sierrachart.h"
#include "Enum.hpp"
#include "ConfigManager.hpp"
#include "Logging.hpp"

namespace StrategyOptimizerHelpers
{
    void HandleStartEvent(SCStudyInterfaceRef sc, SCInputRef Input_ConfigFilePath, StrategyOptimizerConfig* config, std::vector<std::vector<double>>* combinations, ReplayState& replayState, int& ComboIndex);
    void HandleResetEvent(SCStudyInterfaceRef sc, ReplayState& replayState, int& ComboIndex, StrategyOptimizerConfig* config, std::vector<std::vector<double>>* combinations, Logging* logging);
    void HandleVerifyConfigEvent(SCStudyInterfaceRef sc, SCInputRef Input_ConfigFilePath, StrategyOptimizerConfig *config, std::vector<std::vector<double>> *combinations);
}

#endif