#pragma once

#include "../sierrachart.h"
#include <string>
#include <vector>

class Logging {
public:
    Logging(SCStudyInterfaceRef sc);
    void LogMetrics(SCStudyInterfaceRef sc, const std::string& strategyName, const std::string& reportPath, const std::vector<std::pair<std::string, double>>& params, int studyId);

private:
    SCStudyInterfaceRef sc;
};