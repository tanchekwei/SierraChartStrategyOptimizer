// Copyright (c) 2025 Chek Wei Tan
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#include "ReplayManager.hpp"
#include "OnChartLogging.hpp"
#include "Enum.hpp"

namespace
{
    // Helper function to set study inputs for the current combination
    void SetStudyInputs(SCStudyInterfaceRef sc, int studyID, const std::vector<int> &combo, const std::vector<InputConfig> &paramConfigs)
    {
        SCString msg;
        OnChartLogging::AddLog(sc, "Setting study inputs for current combination...");
        for (size_t i = 0; i < combo.size(); ++i)
        {
            SCString inputName;
            sc.GetStudyInputName(sc.ChartNumber, studyID, paramConfigs[i].Index, inputName);
            msg.Format("  Input '%s' (Index %d) set to Value: %d", inputName.GetChars(), paramConfigs[i].Index, combo[i]);
            OnChartLogging::AddLog(sc, msg);
            sc.SetChartStudyInputInt(sc.ChartNumber, studyID, paramConfigs[i].Index, combo[i]);
        }
    }

    // Helper function to start the chart replay
    void InitiateReplay(SCStudyInterfaceRef sc, const ReplayConfig &replayConfig)
    {
        n_ACSIL::s_ChartReplayParameters ReplayParameters;
        ReplayParameters.ChartNumber = sc.ChartNumber;
        ReplayParameters.ReplaySpeed = replayConfig.ReplaySpeed;
        ReplayParameters.StartDateTime = replayConfig.StartDateTime;
        ReplayParameters.ReplayMode = static_cast<n_ACSIL::ChartReplayModeEnum>(replayConfig.ReplayMode);
        ReplayParameters.ChartsToReplay = static_cast<n_ACSIL::ChartsToReplayEnum>(replayConfig.ChartsToReplay);
        ReplayParameters.ClearExistingTradeSimulationDataForSymbolAndTradeAccount = replayConfig.ClearExistingTradeSimulationDataForSymbolAndTradeAccount;

        OnChartLogging::AddLog(sc, "Starting new chart replay...");
        if (sc.StartChartReplayNew(ReplayParameters) == 0)
        {
            OnChartLogging::AddLog(sc, "Chart replay start FAILED.");
        }
        else
        {
            OnChartLogging::AddLog(sc, "Chart replay start command sent successfully.");
        }
    }
}

namespace ReplayManager
{
    void ResetButton(SCStudyInterfaceRef sc, SCInputRef input)
    {
        const int ButtonState = (sc.PointerEventType == SC_ACS_BUTTON_ON) ? 1 : 0;
        if (ButtonState == 1)
        {
            sc.SetCustomStudyControlBarButtonEnable(input.GetInt(), 0);
        }
    }

    void StartReplayForCombination(
        SCStudyInterfaceRef sc,
        const StrategyOptimizerConfig& config,
        const std::vector<std::vector<int>>& combinations,
        int comboIndex,
        ReplayState &replayState)
    {
        SCString msg;
        msg.Format("--- Starting Combination %d/%d ---", comboIndex + 1, (int)combinations.size());
        OnChartLogging::AddLog(sc, msg);

        const auto &currentCombo = combinations[comboIndex];
        int studyID = sc.GetStudyIDByName(sc.ChartNumber, config.CustomStudyShortName.c_str(), 1);
        if (studyID == 0)
        {
            msg.Format("Error: Failed to find study with short name=%s", config.CustomStudyShortName.c_str());
            OnChartLogging::AddLog(sc, msg);
            return;
        }

        SetStudyInputs(sc, studyID, currentCombo, config.ParamConfigs);
        InitiateReplay(sc, config.ReplayConfig);

        sc.RecalculateChart(sc.ChartNumber);

        replayState = ReplayState::WaitingForReplayToStart;
        OnChartLogging::AddLog(sc, "State changed: Waiting for replay data to load.");
    }
}