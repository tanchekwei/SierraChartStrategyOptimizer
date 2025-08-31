// Copyright (c) 2025 Chek Wei Tan
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#pragma once
#include "../sierrachart.h"
#include "nlohmann/json.hpp"
#include <string>
#include <vector>

enum class InputType
{
    INT,
    FLOAT,
    BOOL
};

struct InputConfig
{
    int Index;
    double MinValue;
    double MaxValue;
    double Increment;
    InputType Type;
};

struct ReplayConfig
{
    float ReplaySpeed;
    SCString StartDate;
    SCString StartTime;
    SCDateTimeMS StartDateTime;
    int ReplayMode;
    int ChartsToReplay;
    int ClearExistingTradeSimulationDataForSymbolAndTradeAccount;
};

struct LogConfig
{
    bool EnableLog;
    bool EnableShowLogOnChart;
    int MaxLogLines;
};

struct StrategyOptimizerConfig
{
    bool OpenResultsFolder;
    ReplayConfig ReplayConfig;
    std::vector<InputConfig> ParamConfigs;
    LogConfig LogConfig;
};

namespace ConfigLoader
{
    bool LoadConfig(SCStudyInterfaceRef sc, const std::string &filePath, StrategyOptimizerConfig& outConfig);
}