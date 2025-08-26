// Copyright (c) 2025 Chek Wei Tan
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#ifndef LOGGING_HPP
#define LOGGING_HPP

#include "../sierrachart.h"
#include <string>
#include <vector>
#include <fstream>
#include <utility>
#include "nlohmann/json.hpp"

class Logging
{
public:
    Logging(SCStudyInterfaceRef sc);
    void LogMetrics(SCStudyInterfaceRef sc, const std::string& strategyName, const std::string& reportPath, const std::vector<std::pair<std::string, double>>& params, int studyId);

    static void WriteTradesData(SCStudyInterfaceRef sc, std::ofstream &log);
    static void WriteTradeStatisticsV2(SCStudyInterfaceRef sc, std::ofstream &log);


private:
    SCStudyInterfaceRef sc;

    std::string getCurrentDllName(SCStudyInterfaceRef sc, int studyId);
    std::string GetParameterValueAsString(const SCInputRef &Input);
    std::vector<std::pair<std::string, std::string>> GetParameters(int lastInputIndex);
    void WriteSummaryHeader(std::ofstream &log, const std::string &strategyName, const std::string &dllName, const std::vector<std::pair<std::string, double>> &params);

    nlohmann::json GetCustomStudyInformation(SCStudyInterfaceRef sc, int studyId);
    nlohmann::json GetCombination(const std::vector<std::pair<std::string, double>> &params);
    nlohmann::json GetTradesData(SCStudyInterfaceRef sc);
    nlohmann::json GetTradeStatistics(SCStudyInterfaceRef sc);
    nlohmann::json GetStudyParameters(SCStudyInterfaceRef sc, int studyId);
};

#endif // LOGGING_HPP