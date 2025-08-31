// Copyright (c) 2025 Chek Wei Tan
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#include "ConfigManager.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include "Enum.hpp"
#include "OnChartLogging.hpp"
#include "CombinationGenerator.hpp"

using json = nlohmann::json;

namespace
{
    void ParseMainSettings(const json &root, StrategyOptimizerConfig &outConfig)
    {
        outConfig.OpenResultsFolder = root.value("openResultsFolder", true);
    }

    void ParseReplayConfig(const json &root, StrategyOptimizerConfig &outConfig, SCStudyInterfaceRef sc)
    {
        if (!root.contains("replayConfig"))
            throw std::runtime_error("Missing required section: 'replayConfig'");
        const auto &replayParams = root["replayConfig"];

        if (!replayParams.contains("replaySpeed"))
            throw std::runtime_error("Missing required field in 'replayConfig': 'replaySpeed'");
        outConfig.ReplayConfig.ReplaySpeed = replayParams["replaySpeed"].get<float>();

        if (!replayParams.contains("startDate"))
            throw std::runtime_error("Missing required field in 'replayConfig': 'startDate'");
        outConfig.ReplayConfig.StartDate = replayParams["startDate"].get<std::string>().c_str();

        if (!replayParams.contains("startTime"))
            throw std::runtime_error("Missing required field in 'replayConfig': 'startTime'");
        outConfig.ReplayConfig.StartTime = replayParams["startTime"].get<std::string>().c_str();

        if (!replayParams.contains("replayMode"))
            throw std::runtime_error("Missing required field in 'replayConfig': 'replayMode'");
        outConfig.ReplayConfig.ReplayMode = replayParams["replayMode"].get<int>();

        if (!replayParams.contains("chartsToReplay"))
            throw std::runtime_error("Missing required field in 'replayConfig': 'chartsToReplay'");
        outConfig.ReplayConfig.ChartsToReplay = replayParams["chartsToReplay"].get<int>();

        if (!replayParams.contains("clearExistingTradeSimulationDataForSymbolAndTradeAccount"))
            throw std::runtime_error("Missing required field in 'replayConfig': 'clearExistingTradeSimulationDataForSymbolAndTradeAccount'");
        outConfig.ReplayConfig.ClearExistingTradeSimulationDataForSymbolAndTradeAccount = replayParams["clearExistingTradeSimulationDataForSymbolAndTradeAccount"].get<int>();

        SCDateTimeMS DateValue = sc.DateStringToSCDateTime(outConfig.ReplayConfig.StartDate);
        SCDateTimeMS TimeValue = sc.TimeStringToSCDateTime(outConfig.ReplayConfig.StartTime);
        outConfig.ReplayConfig.StartDateTime = DateValue + TimeValue;
    }

    void ParseLogConfig(const json &root, StrategyOptimizerConfig &outConfig)
    {
        if (root.contains("logConfig"))
        {
            const auto &logParams = root["logConfig"];
            outConfig.LogConfig.EnableLog = logParams.value("enableLog", true);
            outConfig.LogConfig.EnableShowLogOnChart = logParams.value("enableShowLogOnChart", true);
            outConfig.LogConfig.MaxLogLines = logParams.value("maxLogLines", 20);
        }
        else // Defaults if section is missing
        {
            outConfig.LogConfig.EnableLog = true;
            outConfig.LogConfig.EnableShowLogOnChart = true;
            outConfig.LogConfig.MaxLogLines = 20;
        }
    }

    void ParseParamConfigs(const json &root, StrategyOptimizerConfig &outConfig)
    {
        if (!root.contains("paramConfigs"))
            throw std::runtime_error("Missing required section: 'paramConfigs'");
        const auto &inputs = root["paramConfigs"];
        if (inputs.is_array())
        {
            for (const auto &input : inputs)
            {
                if (input.contains("increment") && input["increment"].get<double>() == 0)
                {
                    continue;
                }
                
                InputType type = InputType::INT; // Default to INT
                if (input.contains("type"))
                {
                    std::string typeStr = input["type"].get<std::string>();
                    if (typeStr == "float")
                    {
                        type = InputType::FLOAT;
                    }
                    else if (typeStr == "bool")
                    {
                        type = InputType::BOOL;
                    }
                    else if (typeStr == "int")
                    {
                        type = InputType::INT;
                    }
                    else
                    {
                        continue; // Skip unsupported types
                    }

                    if (!input.contains("index") || !input.contains("min") || !input.contains("max") || !input.contains("increment"))
                    {
                        throw std::runtime_error("A 'paramConfigs' entry is missing a required field (index, min, max, or increment).");
                    }
                }

                outConfig.ParamConfigs.push_back({input["index"].get<int>(), input["min"].get<double>(), input["max"].get<double>(), input["increment"].get<double>(), type});
            }
        }
        else
        {
            throw std::runtime_error("'paramConfigs' must be an array.");
        }
    }
}

namespace ConfigLoader
{
    bool LoadConfig(SCStudyInterfaceRef sc, const std::string &filePath, StrategyOptimizerConfig &outConfig)
    {
        SCString logMessage;
        logMessage.Format("INFO: Attempting to load configuration from: %s", filePath.c_str());
        OnChartLogging::AddLog(sc, logMessage);

        std::ifstream configFile(filePath);
        if (!configFile.is_open())
        {
            logMessage.Format("ERROR: Could not open configuration file at: %s", filePath.c_str());
            OnChartLogging::AddLog(sc, logMessage);
            return false;
        }

        try
        {
            json root;
            configFile >> root;
            configFile.close();

            OnChartLogging::AddLog(sc, "INFO: Config file parsed successfully. Loading settings...");

            ParseMainSettings(root, outConfig);
            unsigned int studyId = sc.Input[StudyInputs::TargetStudyRef].GetStudyID();
            n_ACSIL::s_CustomStudyInformation customStudyInfo;
            sc.GetCustomStudyInformation(sc.ChartNumber, studyId, customStudyInfo);
            logMessage.Format("INFO:   - Study: %s (%s)", customStudyInfo.DLLFileName.GetChars(), customStudyInfo.StudyOriginalName.GetChars());
            OnChartLogging::AddLog(sc, logMessage);
            logMessage.Format("INFO:   - Open Results Folder: %s", outConfig.OpenResultsFolder ? "true" : "false");
            OnChartLogging::AddLog(sc, logMessage);

            ParseReplayConfig(root, outConfig, sc);
            OnChartLogging::AddLog(sc, "INFO: Replay Config Loaded:");
            logMessage.Format("INFO:   - StartDateTime: %s", sc.DateTimeToString(outConfig.ReplayConfig.StartDateTime, FLAG_DT_COMPLETE_DATETIME_MS).GetChars());
            OnChartLogging::AddLog(sc, logMessage);
            logMessage.Format("INFO:   - Replay Speed: %.1f", outConfig.ReplayConfig.ReplaySpeed);
            OnChartLogging::AddLog(sc, logMessage);

            ParseLogConfig(root, outConfig);
            OnChartLogging::AddLog(sc, "INFO: Log Config Loaded:");
            logMessage.Format("INFO:   - Enable Log: %s", outConfig.LogConfig.EnableLog ? "true" : "false");
            OnChartLogging::AddLog(sc, logMessage);
            logMessage.Format("INFO:   - Show Log on Chart: %s", outConfig.LogConfig.EnableShowLogOnChart ? "true" : "false");
            OnChartLogging::AddLog(sc, logMessage);
            logMessage.Format("INFO:   - Max Log Lines: %d", outConfig.LogConfig.MaxLogLines);
            OnChartLogging::AddLog(sc, logMessage);

            ParseParamConfigs(root, outConfig);
            logMessage.Format("INFO: Loaded %d parameter configurations for optimization.", (int)outConfig.ParamConfigs.size());
            OnChartLogging::AddLog(sc, logMessage);

            OnChartLogging::AddLog(sc, "INFO: Configuration loading complete.");

            return true;
        }
        catch (const json::exception &e)
        {
            SCString error_msg;
            error_msg.Format("ERROR: Failed to parse JSON from '%s'. Details: %s", filePath.c_str(), e.what());
            OnChartLogging::AddLog(sc, error_msg);
            OnChartLogging::AddLog(sc, "ERROR: Please check the JSON structure, syntax, and ensure all required fields are present.");
            return false;
        }
        catch (const std::exception &e)
        {
            SCString error_msg;
            error_msg.Format("ERROR: An unexpected error occurred while loading config from '%s': %s", filePath.c_str(), e.what());
            OnChartLogging::AddLog(sc, error_msg);
            return false;
        }
    }
}
