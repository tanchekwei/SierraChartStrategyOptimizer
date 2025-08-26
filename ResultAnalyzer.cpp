// Copyright (c) 2025 Chek Wei Tan
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#include "ResultAnalyzer.hpp"
#include "OnChartLogging.hpp"
#include "Enum.hpp"
#include "nlohmann/json.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <iomanip>

using json = nlohmann::json;

namespace fs = std::filesystem;

void ResultAnalyzer::AnalyzeResults(SCStudyInterfaceRef sc, const std::string &resultsDir, const std::string &reportFileName)
{
    SCString msg;
    std::vector<CombinationResult> results;
    for (const auto &entry : fs::directory_iterator(resultsDir))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".json")
        {
            results.push_back(ParseJsonResult(entry.path().string(), sc));
        }
    }

    if (results.empty())
    {
        OnChartLogging::AddLog(sc, "No JSON files found to analyze.");
        return;
    }

    std::sort(results.begin(), results.end(), [](const CombinationResult &a, const CombinationResult &b)
              { return a.totalProfitLoss > b.totalProfitLoss; });

    std::ofstream csvFile(reportFileName);

    if (!csvFile.is_open())
    {
        msg.Format("Failed to create summary report file at: %s", reportFileName.c_str());
        OnChartLogging::AddLog(sc, msg);
        return;
    }

    csvFile << CombinationResult::GetCsvHeader();
    for (const auto &result : results)
    {
        csvFile << result.ToCsvRow();
    }

    msg.Format("Summary report generated at: %s", reportFileName.c_str());
    OnChartLogging::AddLog(sc, msg);
}

CombinationResult ResultAnalyzer::ParseJsonResult(const std::string &filePath, SCStudyInterfaceRef sc)
{
    CombinationResult result;
    result.sourceFile = filePath;
    try
    {
        json j = GetJsonFromFile(filePath, sc);

        if (j.contains("customStudyInformation"))
        {
            const auto &header = j["customStudyInformation"];
            if (header.contains("StudyOriginalName"))
                result.strategyName = header["StudyOriginalName"];
            if (header.contains("DLLFileName"))
                result.dllName = header["DLLFileName"];

            for (auto it = header.begin(); it != header.end(); ++it)
            {
                if (it.key() != "StudyOriginalName" && it.key() != "DLLFileName")
                {
                    std::stringstream ss;
                    ss << it.value();
                    result.params.push_back({it.key(), ss.str()});
                }
            }
        }

        if (j.contains("tradeStatistics") && j["tradeStatistics"].contains("All Trades"))
        {
            const auto &stats = j["tradeStatistics"]["All Trades"];
            if (stats.contains("ClosedTradesProfitLoss"))
                result.totalProfitLoss = stats["ClosedTradesProfitLoss"];
            if (stats.contains("ProfitFactor"))
                result.profitFactor = stats["ProfitFactor"];
            if (stats.contains("TotalTrades"))
                result.totalTrades = stats["TotalTrades"];
            if (stats.contains("PercentProfitable"))
                result.winningTradesPercentage = stats["PercentProfitable"];
            if (stats.contains("MaximumDrawdown"))
                result.maxDrawdown = stats["MaximumDrawdown"];
        }
    }
    catch (const std::exception &e)
    {
        SCString msg;
        msg.Format("Error parsing JSON file %s: %s", filePath.c_str(), e.what());
        OnChartLogging::AddLog(sc, msg);
    }
    catch (...)
    {
        SCString msg;
        msg.Format("An unknown error occurred while parsing JSON file: %s", filePath.c_str());
        OnChartLogging::AddLog(sc, msg);
    }

    return result;
}

json ResultAnalyzer::GetJsonFromFile(const std::string &filePath, SCStudyInterfaceRef sc)
{
    std::ifstream ifs(filePath);
    if (!ifs.is_open())
    {
        SCString msg;
        msg.Format("Failed to open file: %s", filePath.c_str());
        OnChartLogging::AddLog(sc, msg);
        return {};
    }

    try
    {
        json j;
        ifs >> j;
        return j;
    }
    catch (const std::exception &e)
    {
        SCString msg;
        msg.Format("Exception while reading JSON from %s: %s", filePath.c_str(), e.what());
        OnChartLogging::AddLog(sc, msg);
        return {};
    }
    catch (...)
    {
        SCString msg;
        msg.Format("An unknown error occurred while reading JSON from file: %s", filePath.c_str());
        OnChartLogging::AddLog(sc, msg);
        return {};
    }
}

std::string CombinationResult::GetCsvHeader()
{
    return "Strategy,DLL Name,Parameters,Total P/L,Profit Factor,Total Trades,Win Rate (%),Max Drawdown,Source File\n";
}

std::string CombinationResult::ToCsvRow() const
{
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);
    ss << "\"" << strategyName << "\","
       << "\"" << dllName << "\",";

    std::stringstream params_ss;
    for (size_t i = 0; i < params.size(); ++i)
    {
        params_ss << params[i].first << ": " << params[i].second;
        if (i < params.size() - 1)
        {
            params_ss << " | ";
        }
    }
    ss << "\"" << params_ss.str() << "\","
       << totalProfitLoss << ","
       << profitFactor << ","
       << totalTrades << ","
       << winningTradesPercentage * 100 << ","
       << maxDrawdown << ","
       << "\"" << sourceFile << "\"\n";

    return ss.str();
}