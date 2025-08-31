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
#include "InputParameter.hpp"
#include <string>
#include <vector>
#include <utility>
#include <string>
#include <filesystem>
#include <sstream>
#include <algorithm>
#include <windows.h>
#include <fstream>
#include "nlohmann/json.hpp"
#include <iomanip>

using json = nlohmann::json;

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
            *combinations = CombinationGenerator::GenerateIterative(config->ParamConfigs);

            int &enableLog = sc.GetPersistentIntFast(PersistentVars::EnableLog);
            enableLog = config->LogConfig.EnableLog;

            int &enableShowLogOnChart = sc.GetPersistentIntFast(PersistentVars::EnableShowLogOnChart);
            enableShowLogOnChart = config->LogConfig.EnableShowLogOnChart;

            int &maxLogLines = sc.GetPersistentIntFast(PersistentVars::MaxLogLines);
            maxLogLines = config->LogConfig.MaxLogLines;

            if (combinations->empty() && config->ParamConfigs.empty())
            {
                OnChartLogging::AddLog(sc, "No varying parameters found.");
                return;
            }
            
            msg.Format("Generated %d combinations.", (int)combinations->size());
            OnChartLogging::AddLog(sc, msg);
            isConfigLoaded = true;
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
        OnChartLogging::ClearLogs(sc);
        OnChartLogging::AddLog(sc, "'Verify Config' button clicked.");
        SCString configPath = Input_ConfigFilePath.GetString();
        SCString msg;
        msg.Format("Attempting to load configuration from '%s'...", configPath.GetChars());
        OnChartLogging::AddLog(sc, msg);

        bool isConfigLoaded = false;
        if (ConfigLoader::LoadConfig(sc, configPath.GetChars(), *config))
        {
            OnChartLogging::AddLog(sc, "Configuration loaded successfully. Generating parameter combinations...");
            *combinations = CombinationGenerator::GenerateIterative(config->ParamConfigs);

            int &enableLog = sc.GetPersistentIntFast(PersistentVars::EnableLog);
            enableLog = config->LogConfig.EnableLog;

            int &enableShowLogOnChart = sc.GetPersistentIntFast(PersistentVars::EnableShowLogOnChart);
            enableShowLogOnChart = config->LogConfig.EnableShowLogOnChart;

            int &maxLogLines = sc.GetPersistentIntFast(PersistentVars::MaxLogLines);
            maxLogLines = config->LogConfig.MaxLogLines;

            if (combinations->empty() && !config->ParamConfigs.empty())
            {
                 combinations->push_back({}); 
            }

            msg.Format("Generated %d combinations.", (int)combinations->size());
            OnChartLogging::AddLog(sc, msg);
            isConfigLoaded = true;
        }
        else
        {
            OnChartLogging::AddLog(sc, "Failed to load config file.");
        }

        if (isConfigLoaded == false)
        {
            return;
        }

        OnChartLogging::AddLog(sc, "Configuration verified. Input:");
        
        unsigned int studyId = sc.Input[StudyInputs::TargetStudyRef].GetStudyID();
        if (studyId == 0)
        {
        OnChartLogging::AddLog(sc, "Could not find study to log inputs.");
        return;
        }

        const SCString fontFace = "Consolas";
        std::stringstream header_ss;
        header_ss << "| " << std::right << std::setw(30) << "Input Name" << "| " << std::left << std::setw(15) << "Value" << "|";
        OnChartLogging::AddLog(sc, SCString(header_ss.str().c_str()), fontFace);
        
        std::stringstream separator_ss;
        separator_ss << "|" << std::string(31, '-') << "|" << std::string(16, '-') << "|";
        OnChartLogging::AddLog(sc, SCString(separator_ss.str().c_str()), fontFace);

        const auto& firstCombination = combinations->empty() ? std::vector<double>() : (*combinations)[0];
        int varyingParamIndex = 0;

        for (const auto& param : config->ParamConfigs)
        {
            SCString inputName;
            sc.GetStudyInputName(sc.ChartNumber, studyId, param.Index, inputName);
            if (inputName.IsEmpty()) continue;

            double value;
            if (std::fabs(param.Increment) > 1e-9) 
            {
                if (varyingParamIndex < firstCombination.size())
                {
                    value = firstCombination[varyingParamIndex];
                    varyingParamIndex++;
                }
                else continue; 
            }
            else 
            {
                value = param.MinValue;
            }

            std::stringstream ss;
            ss << "| " << std::right << std::setw(30) << inputName.GetChars() 
               << "| " << std::left << std::setw(15) << value << "|";
            OnChartLogging::AddLog(sc, SCString(ss.str().c_str()), fontFace);
        }
        
        OnChartLogging::AddLog(sc, "--- Verify config finished. ---");
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

    void HandleGenerateConfigEvent(SCStudyInterfaceRef sc)
    {
        OnChartLogging::ClearLogs(sc);
        OnChartLogging::AddLog(sc, "Generate config button clicked.");
        unsigned int studyId = sc.Input[StudyInputs::TargetStudyRef].GetStudyID();
        if (studyId == 0)
        {
            SCString msg;
            msg.Format("Could not find study with id '%d'.", studyId);
            OnChartLogging::AddLog(sc, msg);
            return;
        }

        n_ACSIL::s_CustomStudyInformation customStudyInfo;
        sc.GetCustomStudyInformation(sc.ChartNumber, studyId, customStudyInfo);

        const char *fileNameChars = customStudyInfo.DLLFileName.GetChars();
        const char *functionNameChars = customStudyInfo.DLLFunctionName.GetChars();

        // Find the position of the last '.'
        const char *dotPos = strrchr(fileNameChars, '.');

        // Calculate the length of the filename without the extension
        size_t lengthWithoutExtension = dotPos ? (dotPos - fileNameChars) : strlen(fileNameChars);
        char buffer[256]; // Use a buffer to build the final string
        snprintf(buffer, sizeof(buffer), "%.*s.%s", (int)lengthWithoutExtension, fileNameChars, functionNameChars);

        SCDateTime threeDaysAgo = sc.GetCurrentDateTime().SubtractDays(3);

        nlohmann::ordered_json config;
        config["_customStudyFileAndFunctionName"] = buffer;
        config["openResultsFolder"] = true;
        config["replayConfig"] = {
            {"replaySpeed", 888},
            {"startDate", sc.DateTimeToString(threeDaysAgo, FLAG_DT_COMPLETE_DATE).GetChars()},
            {"startTime", "00:00:00.000"},
            {"replayMode", 2},
            {"chartsToReplay", 0},
            {"clearExistingTradeSimulationDataForSymbolAndTradeAccount", 1}};
        config["logConfig"] = {
            {"enableLog", true},
            {"enableShowLogOnChart", true},
            {"maxLogLines", 25}};

        nlohmann::ordered_json paramConfigs = nlohmann::ordered_json::array();
        int inputIndex = 0;
        while (true)
        {
            SCString inputName;
            sc.GetStudyInputName(sc.ChartNumber, studyId, inputIndex, inputName);
            if (inputName.IsEmpty())
            {
                break;
            }

            int inputType = sc.GetChartStudyInputType(sc.ChartNumber, studyId, inputIndex);
            nlohmann::ordered_json paramConfig;
            paramConfig["_name"] = inputName.GetChars();
            paramConfig["index"] = inputIndex;
            switch (inputType)
            {
            case (INT_VALUE):
                paramConfig["type"] = "int";
                break;
            case (FLOAT_VALUE):
                paramConfig["type"] = "float";
                break;
            case (YESNO_VALUE):
                paramConfig["type"] = "bool";
                break;
            default:
                paramConfig["type"] = std::string(InputParameter::InputValueTypeToString(inputType)) + " (unsupported)";
                paramConfig["_name"] = inputName.GetChars();
                paramConfigs.push_back(paramConfig);
                inputIndex++;
                continue;
            }

            auto currentValueStr = InputParameter::GetParameterValueByStudyId(sc, studyId, inputIndex, inputType);
            bool isFloat = currentValueStr.find('.') != std::string::npos;
            if (isFloat)
            {
                double numericValue = std::stod(currentValueStr);
                paramConfig["min"] = numericValue;
                paramConfig["max"] = numericValue;
            }
            else
            {
                int numericValue = std::stoi(currentValueStr);
                paramConfig["min"] = numericValue;
                paramConfig["max"] = numericValue;
            }
            paramConfig["increment"] = 0;
            paramConfigs.push_back(paramConfig);
            inputIndex++;
        }
        config["paramConfigs"] = paramConfigs;

        std::string configDir = std::filesystem::path(sc.Input[StudyInputs::ConfigFilePath].GetString()).parent_path().string() + "/StrategyOptimizerGeneratedConfig/";
        std::filesystem::create_directories(configDir);
        std::string configPath = configDir + buffer + ".json";
        std::ofstream o(configPath);
        o << std::setw(4) << config << std::endl;
        o.close();

        SCString msg;
        msg.Format("Configuration file generated at '%s'.", configPath.c_str());
        OnChartLogging::AddLog(sc, msg);

        ShellExecuteA(NULL, "open", configDir.c_str(), NULL, NULL, SW_SHOWNORMAL);
    }
}