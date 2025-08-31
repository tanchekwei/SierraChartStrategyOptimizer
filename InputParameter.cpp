// Copyright (c) 2025 Chek Wei Tan
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#include "InputParameter.hpp"
#include <sstream>
#include <iomanip>

std::string InputParameter::GetParameterValueAsString(const SCInputRef &Input)
{
    std::stringstream ss;
    switch (Input.ValueType)
    {
    case OHLC_VALUE:
    case STUDYINDEX_VALUE:
    case SUBGRAPHINDEX_VALUE:
    case MOVAVGTYPE_VALUE:
    case TIME_PERIOD_LENGTH_UNIT_VALUE:
    case STUDYID_VALUE:
    case CANDLESTICK_PATTERNS_VALUE:
    case CUSTOM_STRING_VALUE:
    case TIMEZONE_VALUE:
    case ALERT_SOUND_NUMBER_VALUE:
        ss << Input.GetIndex();
        break;
    case INT_VALUE:
    case CHART_NUMBER:
        ss << Input.GetInt();
        break;
    case FLOAT_VALUE:
        ss << Input.GetFloat();
        break;
    case YESNO_VALUE:
        ss << (Input.GetYesNo() ? "Yes" : "No");
        break;
    case DATE_VALUE:
        ss << SCDateTime(Input.DateTimeValue).GetDate();
        break;
    case TIME_VALUE:
    {
        int totalSeconds = SCDateTime(Input.DateTimeValue).GetTimeInSeconds();
        int hours = totalSeconds / 3600;
        int minutes = (totalSeconds % 3600) / 60;
        int seconds = totalSeconds % 60;

        std::ostringstream oss;
        oss << std::setw(2) << std::setfill('0') << hours << ":"
            << std::setw(2) << std::setfill('0') << minutes << ":"
            << std::setw(2) << std::setfill('0') << seconds;

        ss << oss.str();
        break;
    }
    case DATETIME_VALUE:
        ss << Input.DateTimeValue;
        break;
    case COLOR_VALUE:
        ss << Input.GetColor();
        break;
    case DOUBLE_VALUE:
        ss << Input.GetDouble();
        break;
    case STRING_VALUE:
    case PATH_AND_FILE_NAME_VALUE:
    case FIND_SYMBOL_VALUE:
        ss << Input.GetString();
        break;
    case CHART_STUDY_SUBGRAPH_VALUES:
    case STUDY_SUBGRAPH_VALUES:
    case CHART_STUDY_VALUES:
        ss << "ChartNumber=" << Input.ChartStudySubgraphValues.ChartNumber
           << "|StudyID=" << Input.ChartStudySubgraphValues.StudyID
           << "|SubgraphIndex=" << Input.ChartStudySubgraphValues.SubgraphIndex;
        break;
    default:
        ss << "";
        break;
    }
    return ss.str();
}

std::string InputParameter::GetParameterValueByStudyId(SCStudyInterfaceRef sc, int studyId, int index, int valueType)
{
    std::stringstream ss;
    switch (valueType)
    {
    case INT_VALUE:
    case YESNO_VALUE:
    {
        int input = 0;
        sc.GetChartStudyInputInt(sc.ChartNumber, studyId, index, input);
        return std::to_string(input);
    }
    case FLOAT_VALUE:
    case DOUBLE_VALUE:
    {
        double input = 0;
        sc.GetChartStudyInputFloat(sc.ChartNumber, studyId, index, input);
        return std::to_string(input);
    }
    case STRING_VALUE:
    {
        SCString input;
        sc.GetChartStudyInputString(sc.ChartNumber, studyId, index, input);
        return input.GetChars();
    }
    default:
        ss << "unsupported type";
        break;
    }
    return ss.str();
}

json InputParameter::GetStudyParameters(SCStudyInterfaceRef sc, int studyId)
{
    json studyParameters;
    bool isContinue = true;
    int index = 0;
    while (isContinue)
    {
        SCString inputName;
        sc.GetStudyInputName(sc.ChartNumber, studyId, index, inputName);
        if (inputName.IsEmpty())
        {
            isContinue = false;
            break;
        }
        int type = sc.GetChartStudyInputType(sc.ChartNumber, studyId, index);
        std::string value = GetParameterValueByStudyId(sc, studyId, index, type);
        studyParameters[inputName.GetChars()] = value;
        index++;
    }
    return studyParameters;
}

json InputParameter::GetCustomStudyInformation(SCStudyInterfaceRef sc, int studyId)
{
    json customStudyInformation;
    n_ACSIL::s_CustomStudyInformation CustomStudyInformation;
    if (sc.GetCustomStudyInformation(sc.ChartNumber, studyId, CustomStudyInformation) > 0)
    {
        customStudyInformation["DLLFilePath"] = CustomStudyInformation.DLLFilePath;
        customStudyInformation["DLLFileName"] = CustomStudyInformation.DLLFileName;
        customStudyInformation["DLLFunctionName"] = CustomStudyInformation.DLLFunctionName;
        customStudyInformation["StudyOriginalName"] = CustomStudyInformation.StudyOriginalName;
    }
    return customStudyInformation;
}

std::string InputParameter::GetCurrentDllName(SCStudyInterfaceRef sc, int studyId)
{
    n_ACSIL::s_CustomStudyInformation CustomStudyInformation;
    if (sc.GetCustomStudyInformation(sc.ChartNumber, studyId, CustomStudyInformation) > 0)
    {
        std::string fullPath(CustomStudyInformation.DLLFileName.GetChars());
        size_t pos = fullPath.find_last_of("\\/");
        if (pos != std::string::npos)
            return fullPath.substr(pos + 1);
        return fullPath;
    }
    return "UnknownDll";
}

std::vector<std::pair<std::string, std::string>> InputParameter::GetParameters(SCStudyInterfaceRef sc, int lastInputIndex)
{
    std::vector<std::pair<std::string, std::string>> params;
    for (int i = 0; i <= lastInputIndex; i++)
    {
        params.emplace_back(sc.Input[i].Name.GetChars(), GetParameterValueAsString(sc.Input[i]));
    }
    return params;
}

const char* InputParameter::InputValueTypeToString(int inputType)
{
    switch (inputType)
    {
    case NO_VALUE: return "NO_VALUE";
    case OHLC_VALUE: return "OHLC_VALUE";
    case FLOAT_VALUE: return "FLOAT_VALUE";
    case STUDYINDEX_VALUE: return "STUDYINDEX_VALUE";
    case SUBGRAPHINDEX_VALUE: return "SUBGRAPHINDEX_VALUE";
    case YESNO_VALUE: return "YESNO_VALUE";
    case MOVAVGTYPE_VALUE: return "MOVAVGTYPE_VALUE";
    case DATE_VALUE: return "DATE_VALUE";
    case TIME_VALUE: return "TIME_VALUE";
    case DATETIME_VALUE: return "DATETIME_VALUE";
    case INT_VALUE: return "INT_VALUE";
    case STUDYID_VALUE: return "STUDYID_VALUE";
    case COLOR_VALUE: return "COLOR_VALUE";
    case ALERT_SOUND_NUMBER_VALUE: return "ALERT_SOUND_NUMBER_VALUE";
    case CANDLESTICK_PATTERNS_VALUE: return "CANDLESTICK_PATTERNS_VALUE";
    case TIME_PERIOD_LENGTH_UNIT_VALUE: return "TIME_PERIOD_LENGTH_UNIT_VALUE";
    case CHART_STUDY_SUBGRAPH_VALUES: return "CHART_STUDY_SUBGRAPH_VALUES";
    case CHART_NUMBER: return "CHART_NUMBER";
    case STUDY_SUBGRAPH_VALUES: return "STUDY_SUBGRAPH_VALUES";
    case CHART_STUDY_VALUES: return "CHART_STUDY_VALUES";
    case CUSTOM_STRING_VALUE: return "CUSTOM_STRING_VALUE";
    case DOUBLE_VALUE: return "DOUBLE_VALUE";
    case TIMEZONE_VALUE: return "TIMEZONE_VALUE";
    case TIME_WITH_TIMEZONE_VALUE: return "TIME_WITH_TIMEZONE_VALUE";
    case STRING_VALUE: return "STRING_VALUE";
    case PATH_AND_FILE_NAME_VALUE: return "PATH_AND_FILE_NAME_VALUE";
    case FIND_SYMBOL_VALUE: return "FIND_SYMBOL_VALUE";
    default: return "UNKNOWN_VALUE";
    }
}
