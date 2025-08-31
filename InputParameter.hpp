// Copyright (c) 2025 Chek Wei Tan
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#pragma once

#include "../sierrachart.h"
#include "nlohmann/json.hpp"
#include <string>
#include <vector>

using json = nlohmann::json;

class InputParameter {
public:
    static std::string GetParameterValueAsString(const SCInputRef& Input);
    static json GetStudyParameters(SCStudyInterfaceRef sc, int studyId);
    static std::string GetParameterValueByStudyId(SCStudyInterfaceRef sc, int studyId, int index, int valueType);
    static json GetCustomStudyInformation(SCStudyInterfaceRef sc, int studyId);
    static std::string GetCurrentDllName(SCStudyInterfaceRef sc, int studyId);
    static std::vector<std::pair<std::string, std::string>> GetParameters(SCStudyInterfaceRef sc, int lastInputIndex);
    static const char* InputValueTypeToString(int inputType);
};