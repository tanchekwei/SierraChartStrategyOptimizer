// Copyright (c) 2025 Chek Wei Tan
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#pragma once
#include "../sierrachart.h"
#include <string>
#include <vector>
#include "nlohmann/json.hpp"
using json = nlohmann::json;

struct CombinationResult
{
    std::string strategyName;
    std::string dllName;
    std::vector<std::pair<std::string, std::string>> params;
    double totalProfitLoss = 0.0;
    double profitFactor = 0.0;
    int totalTrades = 0;
    double winningTradesPercentage = 0.0;
    double maxDrawdown = 0.0;
    std::string sourceFile;

    static std::string GetCsvHeader();
    std::string ToCsvRow() const;
};

class ResultAnalyzer
{
public:
    static void AnalyzeResults(SCStudyInterfaceRef sc, const std::string &resultsDir, const std::string &reportFileName);

private:
    static CombinationResult ParseJsonResult(const std::string &filePath, SCStudyInterfaceRef sc);
    static json GetJsonFromFile(const std::string &filePath, SCStudyInterfaceRef sc);
};