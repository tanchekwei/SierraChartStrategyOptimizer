// Copyright (c) 2025 Chek Wei Tan
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#include "../sierrachart.h"
#include "ConfigManager.hpp"
#include "Logging.hpp"
#include "ReplayManager.hpp"
#include "OnChartLogging.hpp"
#include "Enum.hpp"
#include "ResultAnalyzer.hpp"
#include "CombinationGenerator.hpp"
#include <string>
#include <vector>
#include <utility>
#include <string>
#include <filesystem>
#include <sstream>
#include <algorithm>
#include <windows.h>

namespace StrategyOptimizerHelpers
{
    void HandleStartEvent(SCStudyInterfaceRef sc, SCInputRef Input_ConfigFilePath, StrategyOptimizerConfig *config, std::vector<std::vector<double>> *combinations, ReplayState &replayState, int &ComboIndex)
    {
        SCDateTime &backtestStartDateTime = sc.GetPersistentSCDateTimeFast(PersistentVars::BacktestStartDateTime);
        backtestStartDateTime = sc.CurrentSystemDateTimeMS;

        ComboIndex = 0;
        config->ParamConfigs.clear();
        combinations->clear();
        OnChartLogging::AddLog(sc, "'Start Replay' button clicked.");
        SCString configPath = Input_ConfigFilePath.GetString();
        SCString msg;
        msg.Format("Attempting to load configuration from '%s'...", configPath.GetChars());
        OnChartLogging::AddLog(sc, msg);

        bool isConfigLoaded = false;
        if (ConfigLoader::LoadConfig(sc, configPath.GetChars(), *config))
        {
            OnChartLogging::AddLog(sc, "Configuration loaded successfully. Generating parameter combinations...");
            *combinations = CombinationGenerator::Generate(config->ParamConfigs);

            int &enableLog = sc.GetPersistentIntFast(PersistentVars::EnableLog);
            enableLog = config->LogConfig.EnableLog;

            int &enableShowLogOnChart = sc.GetPersistentIntFast(PersistentVars::EnableShowLogOnChart);
            enableShowLogOnChart = config->LogConfig.EnableShowLogOnChart;

            int &maxLogLines = sc.GetPersistentIntFast(PersistentVars::MaxLogLines);
            maxLogLines = config->LogConfig.MaxLogLines;

            if (combinations->empty())
            {
                OnChartLogging::AddLog(sc, "No parameter combinations found.");
                return;
            }
            else
            {
                msg.Format("Generated %d combinations.", (int)combinations->size());
                OnChartLogging::AddLog(sc, msg);
                isConfigLoaded = true;
            }
        }
        else
        {
            OnChartLogging::AddLog(sc, "Failed to load config file.");
        }

        if (isConfigLoaded == false)
        {
            return;
        }

        ComboIndex = 0;
        OnChartLogging::AddLog(sc, "Starting backtesting process with the first combination.");
        ReplayManager::StartReplayForCombination(sc, *config, *combinations, ComboIndex, replayState);
    }

    void HandleVerifyConfigEvent(SCStudyInterfaceRef sc, SCInputRef Input_ConfigFilePath, StrategyOptimizerConfig *config, std::vector<std::vector<double>> *combinations)
    {
        config->ParamConfigs.clear();
        combinations->clear();
        OnChartLogging::AddLog(sc, "'Verify Config' button clicked.");
        SCString configPath = Input_ConfigFilePath.GetString();
        SCString msg;
        msg.Format("Attempting to load configuration from '%s'...", configPath.GetChars());
        OnChartLogging::AddLog(sc, msg);

        bool isConfigLoaded = false;
        if (ConfigLoader::LoadConfig(sc, configPath.GetChars(), *config))
        {
            OnChartLogging::AddLog(sc, "Configuration loaded successfully. Generating parameter combinations...");
            *combinations = CombinationGenerator::Generate(config->ParamConfigs);

            int &enableLog = sc.GetPersistentIntFast(PersistentVars::EnableLog);
            enableLog = config->LogConfig.EnableLog;

            int &enableShowLogOnChart = sc.GetPersistentIntFast(PersistentVars::EnableShowLogOnChart);
            enableShowLogOnChart = config->LogConfig.EnableShowLogOnChart;

            int &maxLogLines = sc.GetPersistentIntFast(PersistentVars::MaxLogLines);
            maxLogLines = config->LogConfig.MaxLogLines;

            if (combinations->empty())
            {
                OnChartLogging::AddLog(sc, "No parameter combinations found.");
                return;
            }
            else
            {
                msg.Format("Generated %d combinations.", (int)combinations->size());
                OnChartLogging::AddLog(sc, msg);
                isConfigLoaded = true;
            }
        }
        else
        {
            OnChartLogging::AddLog(sc, "Failed to load config file.");
        }

        if (isConfigLoaded == false)
        {
            return;
        }

        OnChartLogging::AddLog(sc, "Configuration verified. Setting study inputs for display.");
        if (!combinations->empty())
        {
            ReplayManager::SetStudyInputs(sc, *config, (*combinations)[0]);
        }
        else
        {
            OnChartLogging::AddLog(sc, "No combinations generated, cannot set study inputs.");
        }
    }

    void HandleResetEvent(SCStudyInterfaceRef sc, ReplayState &replayState, int &ComboIndex, StrategyOptimizerConfig *config, std::vector<std::vector<double>> *combinations, Logging *logging)
    {
        replayState = ReplayState::Idle;
        ComboIndex = 0;

        if (config != nullptr)
        {
            delete config;
            sc.SetPersistentPointer(PersistentVars::BacktestConfigPtr, nullptr);
        }
        if (combinations != nullptr)
        {
            delete combinations;
            sc.SetPersistentPointer(PersistentVars::CombinationsPtr, nullptr);
        }
        if (logging != nullptr)
        {
            delete logging;
            sc.SetPersistentPointer(PersistentVars::LoggingPtr, nullptr);
        }
        OnChartLogging::ClearLogs(sc);
        auto *logMessages = reinterpret_cast<std::vector<SCString> *>(sc.GetPersistentPointer(PersistentVars::LogMessagesPtr));
        if (logMessages != nullptr)
        {
            delete logMessages;
            sc.SetPersistentPointer(PersistentVars::LogMessagesPtr, nullptr);
        }
    }
}