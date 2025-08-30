// Copyright (c) 2025 Chek Wei Tan
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#pragma once

enum class ReplayState
{
    Idle,
    WaitingForReplayToStart,
    ReplayInProgress
};

namespace PersistentVars
{
    enum
    {
        ReplayStateEnum,
        ComboIndex,
        LogDrawingLineNumber, // For GetPersistentInt
        BacktestStartDateTime,
        BacktestConfigPtr, // For GetPersistentPointer
        CombinationsPtr,   // For GetPersistentPointer
        LoggingPtr,        // For GetPersistentPointer
        LogMessagesPtr,    // For GetPersistentPointer
        EnableLog,
        EnableShowLogOnChart,
        MaxLogLines,
    };
}

namespace StudyInputs
{
    enum
    {
        StartButtonNumber,
        ResetButtonNumber,
        VerifyConfigButtonNumber,
        ConfigFilePath,
    };
}

namespace Subgraphs
{
    enum
    {
        LogText
    };
}
