#include "Logging.hpp"
#include "InputParameter.hpp"
#include "ReportGenerator.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

Logging::Logging(SCStudyInterfaceRef sc) : sc(sc) {}

void Logging::LogMetrics(SCStudyInterfaceRef sc, const std::string &strategyName, const std::string &reportPath, const std::vector<std::pair<std::string, double>> &params, int studyId)
{
    std::filesystem::create_directories(std::filesystem::path(reportPath).parent_path());

    std::ofstream log(reportPath, std::ios::app);
    if (!log.is_open())
        return;

    json result;
    result["customStudyInformation"] = InputParameter::GetCustomStudyInformation(sc, studyId);
    result["combination"] = ReportGenerator::GetCombination(params);
    result["studyParameters"] = InputParameter::GetStudyParameters(sc, studyId);
    result["tradesData"] = ReportGenerator::GetTradesData(sc);
    result["tradeStatistics"] = ReportGenerator::GetTradeStatistics(sc);

    log << result.dump(4);

    // csv
    std::stringstream fileNameStream;
    fileNameStream
        << reportPath << ".csv";

    std::ofstream csvLog(fileNameStream.str(), std::ios::app);
    if (!csvLog.is_open())
        return;

    ReportGenerator::WriteSummaryHeader(csvLog, strategyName, InputParameter::GetCurrentDllName(sc, studyId), params);
    ReportGenerator::WriteTradesData(sc, csvLog);
    ReportGenerator::WriteTradeStatisticsV2(sc, csvLog);
    // csv
}
