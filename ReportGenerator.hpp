// Copyright (c) 2025 Chek Wei Tan
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#pragma once

#include "../sierrachart.h"
#include "nlohmann/json.hpp"
#include <string>
#include <vector>
#include <fstream>

using json = nlohmann::json;

class ReportGenerator {
public:
    static void WriteSummaryHeader(std::ofstream& log, const std::string& strategyName, const std::string& dllName, const std::vector<std::pair<std::string, double>>& params);
    static void WriteTradesData(SCStudyInterfaceRef sc, std::ofstream& log);
    static void WriteTradeStatisticsV2(SCStudyInterfaceRef sc, std::ofstream& log);
    static json GetTradesData(SCStudyInterfaceRef sc);
    static json GetTradeStatistics(SCStudyInterfaceRef sc);
    static json GetCombination(const std::vector<std::pair<std::string, double>>& params);
};