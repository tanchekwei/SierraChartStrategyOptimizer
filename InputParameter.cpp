#include "InputParameter.hpp"
#include <sstream>
#include <iomanip>

std::string InputParameter::GetParameterValueAsString(const SCInputRef& Input) {
    std::stringstream ss;
    switch (Input.ValueType) {
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
        case TIME_VALUE: {
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

namespace {
    std::string GetParameterValueByStudyId(SCStudyInterfaceRef sc, int studyId, int index, int valueType) {
        std::stringstream ss;
        switch (valueType) {
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
            case INT_VALUE:
            case CHART_NUMBER:
            case YESNO_VALUE:
            case DATE_VALUE:
            case TIME_VALUE:
            case DATETIME_VALUE:
            case CHART_STUDY_SUBGRAPH_VALUES:
            case STUDY_SUBGRAPH_VALUES:
            case CHART_STUDY_VALUES: {
                int input = 0;
                sc.GetChartStudyInputInt(sc.ChartNumber, studyId, index, input);
                return std::to_string(input);
            }
            case FLOAT_VALUE:
            case DOUBLE_VALUE: {
                double input = 0;
                sc.GetChartStudyInputFloat(sc.ChartNumber, studyId, index, input);
                return std::to_string(input);
            }
            case STRING_VALUE:
            case PATH_AND_FILE_NAME_VALUE:
            case FIND_SYMBOL_VALUE: {
                SCString input;
                sc.GetChartStudyInputString(sc.ChartNumber, studyId, index, input);
                return input.GetChars();
            }
            default:
                ss << "";
                break;
        }
        return ss.str();
    }
}

json InputParameter::GetStudyParameters(SCStudyInterfaceRef sc, int studyId) {
    json studyParameters;
    bool isContinue = true;
    int index = 0;
    while (isContinue) {
        SCString inputName;
        sc.GetStudyInputName(sc.ChartNumber, studyId, index, inputName);
        if (inputName.IsEmpty()) {
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

json InputParameter::GetCustomStudyInformation(SCStudyInterfaceRef sc, int studyId) {
    json customStudyInformation;
    n_ACSIL::s_CustomStudyInformation CustomStudyInformation;
    if (sc.GetCustomStudyInformation(sc.ChartNumber, studyId, CustomStudyInformation) > 0) {
        customStudyInformation["DLLFilePath"] = CustomStudyInformation.DLLFilePath;
        customStudyInformation["DLLFileName"] = CustomStudyInformation.DLLFileName;
        customStudyInformation["DLLFunctionName"] = CustomStudyInformation.DLLFunctionName;
        customStudyInformation["StudyOriginalName"] = CustomStudyInformation.StudyOriginalName;
    }
    return customStudyInformation;
}

std::string InputParameter::GetCurrentDllName(SCStudyInterfaceRef sc, int studyId) {
    n_ACSIL::s_CustomStudyInformation CustomStudyInformation;
    if (sc.GetCustomStudyInformation(sc.ChartNumber, studyId, CustomStudyInformation) > 0) {
        std::string fullPath(CustomStudyInformation.DLLFileName.GetChars());
        size_t pos = fullPath.find_last_of("\\/");
        if (pos != std::string::npos)
            return fullPath.substr(pos + 1);
        return fullPath;
    }
    return "UnknownDll";
}

std::vector<std::pair<std::string, std::string>> InputParameter::GetParameters(SCStudyInterfaceRef sc, int lastInputIndex) {
    std::vector<std::pair<std::string, std::string>> params;
    for (int i = 0; i <= lastInputIndex; i++) {
        params.emplace_back(sc.Input[i].Name.GetChars(), GetParameterValueAsString(sc.Input[i]));
    }
    return params;
}