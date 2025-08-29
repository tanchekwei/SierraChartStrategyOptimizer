// Copyright (c) 2025 Chek Wei Tan
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#include "../sierrachart.h"
#include "StrategyOptimizer.hpp"
#include "ConfigManager.hpp"
#include "Logging.hpp"
#include "ReplayManager.hpp"
#include "OnChartLogging.hpp"
#include "Enum.hpp"
#include "ResultAnalyzer.hpp"
#include <string>
#include <vector>
#include <utility>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <windows.h>

void InitializePersistentPointers(SCStudyInterfaceRef sc);
void HandleSetDefaults(SCStudyInterfaceRef sc);
void HandleFullRecalculation(SCStudyInterfaceRef sc);
bool HandleReplayLogic(SCStudyInterfaceRef sc);
void HandleMenuEvents(SCStudyInterfaceRef sc);
void HandleReplayCompletion(SCStudyInterfaceRef sc);

SCDLLName("scsf_StrategyOptimizer");

SCSFExport scsf_StrategyOptimizer(SCStudyInterfaceRef sc)
{
    InitializePersistentPointers(sc);

    ReplayState &replayState = reinterpret_cast<ReplayState &>(sc.GetPersistentIntFast(PersistentVars::ReplayStateEnum));
    int &comboIndex = sc.GetPersistentIntFast(PersistentVars::ComboIndex);
    auto *config = reinterpret_cast<StrategyOptimizerConfig *>(sc.GetPersistentPointer(PersistentVars::BacktestConfigPtr));
    auto *combinations = reinterpret_cast<std::vector<std::vector<double>> *>(sc.GetPersistentPointer(PersistentVars::CombinationsPtr));
    auto *logging = reinterpret_cast<Logging *>(sc.GetPersistentPointer(PersistentVars::LoggingPtr));

    if (sc.LastCallToFunction)
    {
        StrategyOptimizerHelpers::HandleResetEvent(sc, replayState, comboIndex, config, combinations, logging);
        return;
    }

    if (sc.SetDefaults)
    {
        HandleSetDefaults(sc);
        return;
    }

    if (sc.IsFullRecalculation)
    {
        HandleFullRecalculation(sc);
    }

    if (sc.MenuEventID != 0)
    {
        HandleMenuEvents(sc);
        return;
    }

    if (HandleReplayLogic(sc))
    {
        return;
    }
}

void InitializePersistentPointers(SCStudyInterfaceRef sc)
{
    if (sc.GetPersistentPointer(PersistentVars::BacktestConfigPtr) == nullptr)
    {
        sc.SetPersistentPointer(PersistentVars::BacktestConfigPtr, new StrategyOptimizerConfig());
    }

    if (sc.GetPersistentPointer(PersistentVars::CombinationsPtr) == nullptr)
    {
        sc.SetPersistentPointer(PersistentVars::CombinationsPtr, new std::vector<std::vector<double>>());
    }

    if (sc.GetPersistentPointer(PersistentVars::LoggingPtr) == nullptr)
    {
        sc.SetPersistentPointer(PersistentVars::LoggingPtr, new Logging(sc));
    }
}

void HandleSetDefaults(SCStudyInterfaceRef sc)
{
    sc.GraphName = "Strategy Optimizer";
    sc.StudyDescription = "Runs automated backtests of a trading study or strategy across all possible parameter combinations within user-defined ranges.";
    sc.AutoLoop = 1;
    sc.GraphRegion = 0;
    sc.MaintainTradeStatisticsAndTradesData = true;

    sc.Subgraph[Subgraphs::LogText].Name = "Log";
    sc.Subgraph[Subgraphs::LogText].DrawStyle = DRAWSTYLE_HIDDEN;
    sc.Subgraph[Subgraphs::LogText].PrimaryColor = RGB(255, 255, 255);
    sc.Subgraph[Subgraphs::LogText].LineWidth = 10; // Used for font size

    sc.Input[StudyInputs::VerifyConfigButtonNumber].Name = "Verify Config Button Number";
    sc.Input[StudyInputs::VerifyConfigButtonNumber].SetInt(6);
    sc.Input[StudyInputs::VerifyConfigButtonNumber].SetIntLimits(1, MAX_ACS_CONTROL_BAR_BUTTONS);

    sc.Input[StudyInputs::StartButtonNumber].Name = "Start Button Number";
    sc.Input[StudyInputs::StartButtonNumber].SetInt(7);
    sc.Input[StudyInputs::StartButtonNumber].SetIntLimits(1, MAX_ACS_CONTROL_BAR_BUTTONS);

    sc.Input[StudyInputs::ResetButtonNumber].Name = "Reset Button Number";
    sc.Input[StudyInputs::ResetButtonNumber].SetInt(8);
    sc.Input[StudyInputs::ResetButtonNumber].SetIntLimits(1, MAX_ACS_CONTROL_BAR_BUTTONS);

    sc.Input[StudyInputs::ConfigFilePath].Name = "Config File Path";
    sc.Input[StudyInputs::ConfigFilePath].SetString("C:\\SierraChart\\Data\\StrategyOptimizerConfig.json");

    OnChartLogging::AddLog(sc, "Strategy Optimizer defaults set.");
}

void HandleFullRecalculation(SCStudyInterfaceRef sc)
{
    sc.SetCustomStudyControlBarButtonHoverText(sc.Input[StudyInputs::StartButtonNumber].GetInt(), "Start Strategy Optimizer");
    sc.SetCustomStudyControlBarButtonShortCaption(sc.Input[StudyInputs::StartButtonNumber].GetInt(), "Start Strategy Optimizer");
    sc.SetCustomStudyControlBarButtonHoverText(sc.Input[StudyInputs::ResetButtonNumber].GetInt(), "Reset / Stop Strategy Optimizer");
    sc.SetCustomStudyControlBarButtonShortCaption(sc.Input[StudyInputs::ResetButtonNumber].GetInt(), "Reset / Stop Strategy Optimizer");
    sc.SetCustomStudyControlBarButtonHoverText(sc.Input[StudyInputs::VerifyConfigButtonNumber].GetInt(), "Verify Strategy Optimizer Configuration");
    sc.SetCustomStudyControlBarButtonShortCaption(sc.Input[StudyInputs::VerifyConfigButtonNumber].GetInt(), "Verify Config");
}

bool HandleReplayLogic(SCStudyInterfaceRef sc)
{
    ReplayState &replayState = reinterpret_cast<ReplayState &>(sc.GetPersistentIntFast(PersistentVars::ReplayStateEnum));

    if (replayState == ReplayState::WaitingForReplayToStart && sc.IsFullRecalculation == 0)
    {
        int replayStatus = sc.GetReplayStatusFromChart(sc.ChartNumber);
        SCString msg;
        msg.Format("Waiting for replay to start. Current status: %d (0=Stopped, 1=Running, 2=Paused)", replayStatus);
        OnChartLogging::AddLog(sc, msg);

        if (replayStatus == REPLAY_RUNNING)
        {
            OnChartLogging::AddLog(sc, "Replay has started successfully.");
            replayState = ReplayState::ReplayInProgress;
            OnChartLogging::AddLog(sc, "State changed: Replay in progress.");
        }
        else if (replayStatus == REPLAY_STOPPED || replayStatus == REPLAY_PAUSED)
        {
            OnChartLogging::AddLog(sc, "Attempting to resume replay...");
            sc.ResumeChartReplay(sc.ChartNumber);
        }
        return true;
    }

    if (replayState == ReplayState::ReplayInProgress && sc.GetReplayHasFinishedStatus())
    {
        HandleReplayCompletion(sc);
        return true;
    }

    return false;
}

void HandleReplayCompletion(SCStudyInterfaceRef sc)
{
    int &comboIndex = sc.GetPersistentIntFast(PersistentVars::ComboIndex);
    auto *combinations = reinterpret_cast<std::vector<std::vector<double>> *>(sc.GetPersistentPointer(PersistentVars::CombinationsPtr));
    auto *config = reinterpret_cast<StrategyOptimizerConfig *>(sc.GetPersistentPointer(PersistentVars::BacktestConfigPtr));
    auto *logging = reinterpret_cast<Logging *>(sc.GetPersistentPointer(PersistentVars::LoggingPtr));
    ReplayState &replayState = reinterpret_cast<ReplayState &>(sc.GetPersistentIntFast(PersistentVars::ReplayStateEnum));

    SCString msg;
    msg.Format("--- Combination %d/%d finished ---", comboIndex + 1, (int)combinations->size());
    OnChartLogging::AddLog(sc, msg);

    const auto &currentCombo = (*combinations)[comboIndex];
    int studyID = sc.GetStudyIDByName(sc.ChartNumber, config->CustomStudyShortName.c_str(), 1);
    std::vector<std::pair<std::string, double>> params;

    std::stringstream paramStream;
    for (size_t i = 0; i < currentCombo.size(); ++i)
    {
        SCString inputName;
        sc.GetStudyInputName(sc.ChartNumber, studyID, config->ParamConfigs[i].Index, inputName);
        params.push_back({inputName.GetChars(), currentCombo[i]});
        paramStream << "-" << inputName.GetChars() << "-" << currentCombo[i];
    }

    SCDateTime &backtestStartDateTime = sc.GetPersistentSCDateTimeFast(PersistentVars::BacktestStartDateTime);
    std::string startDateTimeString(sc.FormatDateTime(backtestStartDateTime).GetChars());
    std::replace(startDateTimeString.begin(), startDateTimeString.end(), '/', '-');
    std::replace(startDateTimeString.begin(), startDateTimeString.end(), ':', '-');
    std::replace(startDateTimeString.begin(), startDateTimeString.end(), ' ', '_');

    std::stringstream reportFileName;
    reportFileName << config->CustomStudyFileAndFunctionName
                   << "-" << comboIndex;

    std::string resultsDir = std::filesystem::path(sc.Input[StudyInputs::ConfigFilePath].GetString()).parent_path().string() + "/results/" + config->CustomStudyFileAndFunctionName + "-" + startDateTimeString + "/";
    std::filesystem::create_directories(resultsDir);
    std::string reportPath = resultsDir + reportFileName.str() + ".json";

    logging->LogMetrics(sc, config->CustomStudyFileAndFunctionName, reportPath, params, studyID);
    OnChartLogging::AddLog(sc, "Logged metrics for completed combination.");

    replayState = ReplayState::Idle;
    comboIndex++;

    if (comboIndex < (int)combinations->size())
    {
        OnChartLogging::AddLog(sc, "Proceeding to next combination.");
        sc.StopChartReplay(sc.ChartNumber);
        ReplayManager::StartReplayForCombination(sc, *config, *combinations, comboIndex, replayState);
    }
    else
    {
        OnChartLogging::AddLog(sc, "--- All combinations finished. Backtesting complete. ---");
        ResultAnalyzer::AnalyzeResults(sc, resultsDir, resultsDir + reportFileName.str() + "-summary.csv");
        if (config->OpenResultsFolder)
        {
            ShellExecuteA(NULL, "open", resultsDir.c_str(), NULL, NULL, SW_SHOWNORMAL);
        }
    }
}

void HandleMenuEvents(SCStudyInterfaceRef sc)
{
    SCInputRef Input_Start = sc.Input[StudyInputs::StartButtonNumber];
    SCInputRef Input_Reset = sc.Input[StudyInputs::ResetButtonNumber];
    SCInputRef Input_VerifyConfig = sc.Input[StudyInputs::VerifyConfigButtonNumber];
    SCInputRef Input_ConfigFilePath = sc.Input[StudyInputs::ConfigFilePath];

    ReplayState &replayState = reinterpret_cast<ReplayState &>(sc.GetPersistentIntFast(PersistentVars::ReplayStateEnum));
    int &comboIndex = sc.GetPersistentIntFast(PersistentVars::ComboIndex);
    auto *config = reinterpret_cast<StrategyOptimizerConfig *>(sc.GetPersistentPointer(PersistentVars::BacktestConfigPtr));
    auto *combinations = reinterpret_cast<std::vector<std::vector<double>> *>(sc.GetPersistentPointer(PersistentVars::CombinationsPtr));
    auto *logging = reinterpret_cast<Logging *>(sc.GetPersistentPointer(PersistentVars::LoggingPtr));

    if (sc.MenuEventID == Input_Start.GetInt())
    {
        if (sc.GlobalTradeSimulationIsOn)
        {
            StrategyOptimizerHelpers::HandleStartEvent(sc, Input_ConfigFilePath, config, combinations, replayState, comboIndex);
        }
        else
        {
            OnChartLogging::AddLog(sc, "Trade Simulation Mode is off.");
        }
        ReplayManager::ResetButton(sc, Input_Start);
    }
    else if (sc.MenuEventID == Input_Reset.GetInt())
    {
        sc.StopChartReplay(sc.ChartNumber);
        StrategyOptimizerHelpers::HandleResetEvent(sc, replayState, comboIndex, config, combinations, logging);
        ReplayManager::ResetButton(sc, Input_Reset);
    }
    else if (sc.MenuEventID == Input_VerifyConfig.GetInt())
    {
        StrategyOptimizerHelpers::HandleVerifyConfigEvent(sc, Input_ConfigFilePath, config, combinations);
        ReplayManager::ResetButton(sc, Input_VerifyConfig);
    }
}