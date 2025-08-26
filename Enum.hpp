// Copyright (c) 2025 Chek Wei Tan
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#pragma once

enum class ReplayState
{
    Idle = 0,
    WaitingForReplayToStart = 1,
    ReplayInProgress = 2
};

namespace PersistentVars
{
    enum
    {
        ReplayStateEnum = 1,
        ComboIndex = 2,
        LogDrawingLineNumber = 3, // For GetPersistentInt
        BacktestStartDateTime = 4,
        BacktestConfigPtr = 5, // For GetPersistentPointer
        CombinationsPtr = 6,   // For GetPersistentPointer
        LoggingPtr = 7,        // For GetPersistentPointer
        LogMessagesPtr = 8,    // For GetPersistentPointer
        EnableLog = 9,
        EnableShowLogOnChart = 10,
        MaxLogLines = 11,
    };
}

namespace StudyInputs
{
    enum
    {
        StartResumeReplay = 0,
        ConfigFilePath = 1,
        Reset = 2,
        OpenResultsFolder = 3,
    };
}

namespace Subgraphs
{
    enum
    {
        LogText = 0
    };
}
