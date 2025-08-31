// Copyright (c) 2025 Chek Wei Tan
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#include "OnChartLogging.hpp"
#include "Enum.hpp"

namespace OnChartLogging
{
    std::vector<SCString> *GetLogMessagesVector(SCStudyInterfaceRef sc)
    {
        auto *logMessages = reinterpret_cast<std::vector<SCString> *>(sc.GetPersistentPointer(PersistentVars::LogMessagesPtr));
        if (logMessages == nullptr)
        {
            logMessages = new std::vector<SCString>();
            sc.SetPersistentPointer(PersistentVars::LogMessagesPtr, logMessages);
        }
        return logMessages;
    }

    void AddLog(SCStudyInterfaceRef sc, const SCString &message, const SCString &fontFace)
    {
        int &enableLog = sc.GetPersistentIntFast(PersistentVars::EnableLog);
        if (enableLog == 0)
            return;

        sc.AddMessageToLog(message, 0);
        auto *logMessages = GetLogMessagesVector(sc);
        int &maxLogLines = sc.GetPersistentIntFast(PersistentVars::MaxLogLines);
        if (logMessages->size() >= maxLogLines)
        {
            logMessages->erase(logMessages->begin());
        }
        logMessages->push_back(message);

        int &enableShowLogOnChart = sc.GetPersistentIntFast(PersistentVars::EnableShowLogOnChart);
        if (enableShowLogOnChart != 0)
        {
            DrawLogs(sc, fontFace);
        }
    }

    void DrawLogs(SCStudyInterfaceRef sc, const SCString &fontFace)
    {
        int &textDrawingLineNumber = sc.GetPersistentIntFast(PersistentVars::LogDrawingLineNumber);
        auto *logMessages = GetLogMessagesVector(sc);

        if (logMessages->empty())
        {
            // If there are no messages but the drawing exists, delete it.
            if (textDrawingLineNumber != 0)
            {
                sc.DeleteUserDrawnACSDrawing(sc.ChartNumber, textDrawingLineNumber);
                textDrawingLineNumber = 0;
            }
            return;
        }

        SCString logText;
        for (const auto &msg : *logMessages)
        {
            logText += msg;
            logText += "\n";
        }

        s_UseTool tool;
        tool.Clear();
        tool.ChartNumber = sc.ChartNumber;
        tool.DrawingType = DRAWING_TEXT;
        tool.Region = sc.GraphRegion;
        tool.LineNumber = textDrawingLineNumber;
        tool.AddMethod = UTAM_ADD_OR_ADJUST;
        tool.AddAsUserDrawnDrawing = 1;
        tool.AllowSaveToChartbook = 0;
        tool.UseRelativeVerticalValues = true;
        tool.BeginDateTime = 5;
        tool.BeginValue = 95;
        tool.Color = RGB(255, 255, 255);
        tool.Text = logText;
        tool.FontSize = 10;
        tool.TextAlignment = DT_LEFT | DT_TOP;
        tool.FontFace = "Consolas";
        if (sc.UseTool(tool) > 0)
        {
            textDrawingLineNumber = tool.LineNumber;
        }
    }

    void ClearLogs(SCStudyInterfaceRef sc)
    {
        int &textDrawingLineNumber = sc.GetPersistentIntFast(PersistentVars::LogDrawingLineNumber);
        auto *logMessages = GetLogMessagesVector(sc);

        logMessages->clear();
        if (textDrawingLineNumber != 0)
        {
            sc.DeleteUserDrawnACSDrawing(sc.ChartNumber, textDrawingLineNumber);
            textDrawingLineNumber = 0;
        }
    }
}