// Copyright (c) 2025 Chek Wei Tan
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#pragma once
#include "../sierrachart.h"
#include <vector>

namespace OnChartLogging
{
    void AddLog(SCStudyInterfaceRef sc, const SCString& message);
    void DrawLogs(SCStudyInterfaceRef sc);
    void ClearLogs(SCStudyInterfaceRef sc);
    std::vector<SCString>* GetLogMessagesVector(SCStudyInterfaceRef sc);
};