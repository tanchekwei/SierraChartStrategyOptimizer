#include "ReportGenerator.hpp"
#include <sstream>

void ReportGenerator::WriteSummaryHeader(std::ofstream& log, const std::string& strategyName, const std::string& dllName, const std::vector<std::pair<std::string, double>>& params) {
    log << "Strategy,DLL Name,";
    for (const auto& p : params) {
        log << p.first << ",";
    }
    log << "\n"
        << strategyName << "," << dllName << ",";
    for (const auto& p : params) {
        log << p.second << ",";
    }
    log << "\n\n";
}

void ReportGenerator::WriteTradesData(SCStudyInterfaceRef sc, std::ofstream& log) {
    log << "\n";
    log << "OpenDateTime,CloseDateTime,TradeType,TradeQuantity,MaxClosedQuantity,MaxOpenQuantity,EntryPrice,ExitPrice,TradeProfitLoss,MaximumOpenPositionLoss,MaximumOpenPositionProfit,FlatToFlatMaximumOpenPositionProfit,FlatToFlatMaximumOpenPositionLoss,Commission,IsTradeClosed,Note\n";

    int tradeListSize = sc.GetTradeListSize();
    for (int i = 0; i < tradeListSize; ++i) {
        s_ACSTrade trade;
        sc.GetTradeListEntry(i, trade);
        if (trade.IsTradeClosed) {
            std::stringstream ss;
            ss << sc.DateTimeToString(trade.OpenDateTime, FLAG_DT_COMPLETE_DATETIME)
               << "," << sc.DateTimeToString(trade.CloseDateTime, FLAG_DT_COMPLETE_DATETIME)
               << "," << trade.TradeType
               << "," << trade.TradeQuantity
               << "," << trade.MaxClosedQuantity
               << "," << trade.MaxOpenQuantity
               << "," << trade.EntryPrice
               << "," << trade.ExitPrice
               << "," << trade.TradeProfitLoss
               << "," << trade.MaximumOpenPositionLoss
               << "," << trade.MaximumOpenPositionProfit
               << "," << trade.FlatToFlatMaximumOpenPositionProfit
               << "," << trade.FlatToFlatMaximumOpenPositionLoss
               << "," << trade.Commission
               << "," << trade.IsTradeClosed
               << "," << trade.Note.GetChars() << "\n";
            log << ss.str();
        }
    }
}

void ReportGenerator::WriteTradeStatisticsV2(SCStudyInterfaceRef sc, std::ofstream& log) {
    log << "\n\n--- Trade Statistics V2 ---\n";

    const char* headers[] = {
        "Statistic", "All Trades", "Long Trades", "Short Trades"
    };

    log << headers << "," << headers << "," << headers << "," << headers << "\n";

    n_ACSIL::s_TradeStatistics allStats, longStats, shortStats;
    sc.GetTradeStatisticsForSymbolV2(n_ACSIL::STATS_TYPE_ALL_TRADES, allStats);
    sc.GetTradeStatisticsForSymbolV2(n_ACSIL::STATS_TYPE_LONG_TRADES, longStats);
    sc.GetTradeStatisticsForSymbolV2(n_ACSIL::STATS_TYPE_SHORT_TRADES, shortStats);

    log << "ClosedTradesProfitLoss," << allStats.ClosedTradesProfitLoss << "," << longStats.ClosedTradesProfitLoss << "," << shortStats.ClosedTradesProfitLoss << "\n";
    log << "ClosedTradesTotalProfit," << allStats.ClosedTradesTotalProfit << "," << longStats.ClosedTradesTotalProfit << "," << shortStats.ClosedTradesTotalProfit << "\n";
    log << "ClosedTradesTotalLoss," << allStats.ClosedTradesTotalLoss << "," << longStats.ClosedTradesTotalLoss << "," << shortStats.ClosedTradesTotalLoss << "\n";
    log << "ProfitFactor," << allStats.ProfitFactor << "," << longStats.ProfitFactor << "," << shortStats.ProfitFactor << "\n";
    log << "EquityPeak," << allStats.EquityPeak << "," << longStats.EquityPeak << "," << shortStats.EquityPeak << "\n";
    log << "EquityValley," << allStats.EquityValley << "," << longStats.EquityValley << "," << shortStats.EquityValley << "\n";
    log << "MaximumRunup," << allStats.MaximumRunup << "," << longStats.MaximumRunup << "," << shortStats.MaximumRunup << "\n";
    log << "MaximumDrawdown," << allStats.MaximumDrawdown << "," << longStats.MaximumDrawdown << "," << shortStats.MaximumDrawdown << "\n";
    log << "MaximumFlatToFlatTradeOpenProfit," << allStats.MaximumFlatToFlatTradeOpenProfit << "," << longStats.MaximumFlatToFlatTradeOpenProfit << "," << shortStats.MaximumFlatToFlatTradeOpenProfit << "\n";
    log << "MaximumFlatToFlatTradeOpenLoss," << allStats.MaximumFlatToFlatTradeOpenLoss << "," << longStats.MaximumFlatToFlatTradeOpenLoss << "," << shortStats.MaximumFlatToFlatTradeOpenLoss << "\n";
    log << "AverageTradeOpenProfit," << allStats.AverageTradeOpenProfit << "," << longStats.AverageTradeOpenProfit << "," << shortStats.AverageTradeOpenProfit << "\n";
    log << "AverageTradeOpenLoss," << allStats.AverageTradeOpenLoss << "," << longStats.AverageTradeOpenLoss << "," << shortStats.AverageTradeOpenLoss << "\n";
    log << "AverageWinningTradeOpenProfit," << allStats.AverageWinningTradeOpenProfit << "," << longStats.AverageWinningTradeOpenProfit << "," << shortStats.AverageWinningTradeOpenProfit << "\n";
    log << "AverageWinningTradeOpenLoss," << allStats.AverageWinningTradeOpenLoss << "," << longStats.AverageWinningTradeOpenLoss << "," << shortStats.AverageWinningTradeOpenLoss << "\n";
    log << "AverageLosingTradeOpenProfit," << allStats.AverageLosingTradeOpenProfit << "," << longStats.AverageLosingTradeOpenProfit << "," << shortStats.AverageLosingTradeOpenProfit << "\n";
    log << "AverageLosingTradeOpenLoss," << allStats.AverageLosingTradeOpenLoss << "," << longStats.AverageLosingTradeOpenLoss << "," << shortStats.AverageLosingTradeOpenLoss << "\n";
    log << "MaximumTradeOpenProfit," << allStats.MaximumTradeOpenProfit << "," << longStats.MaximumTradeOpenProfit << "," << shortStats.MaximumTradeOpenProfit << "\n";
    log << "MaximumTradeOpenLoss," << allStats.MaximumTradeOpenLoss << "," << longStats.MaximumTradeOpenLoss << "," << shortStats.MaximumTradeOpenLoss << "\n";
    log << "HighestPriceDuringPositions," << allStats.HighestPriceDuringPositions << "," << longStats.HighestPriceDuringPositions << "," << shortStats.HighestPriceDuringPositions << "\n";
    log << "LowestPriceDuringPositions," << allStats.LowestPriceDuringPositions << "," << longStats.LowestPriceDuringPositions << "," << shortStats.LowestPriceDuringPositions << "\n";
    log << "TotalCommissions," << allStats.TotalCommissions << "," << longStats.TotalCommissions << "," << shortStats.TotalCommissions << "\n";
    log << "TotalTrades," << allStats.TotalTrades << "," << longStats.TotalTrades << "," << shortStats.TotalTrades << "\n";
    log << "TotalFlatToFlatTrades," << allStats.TotalFlatToFlatTrades << "," << longStats.TotalFlatToFlatTrades << "," << shortStats.TotalFlatToFlatTrades << "\n";
    log << "TotalFilledQuantity," << allStats.TotalFilledQuantity << "," << longStats.TotalFilledQuantity << "," << shortStats.TotalFilledQuantity << "\n";
    log << "PercentProfitable," << allStats.PercentProfitable << "," << longStats.PercentProfitable << "," << shortStats.PercentProfitable << "\n";
    log << "FlatToFlatPercentProfitable," << allStats.FlatToFlatPercentProfitable << "," << longStats.FlatToFlatPercentProfitable << "," << shortStats.FlatToFlatPercentProfitable << "\n";
    log << "WinningTrades," << allStats.WinningTrades << "," << longStats.WinningTrades << "," << shortStats.WinningTrades << "\n";
    log << "WinningFlatToFlatTrades," << allStats.WinningFlatToFlatTrades << "," << longStats.WinningFlatToFlatTrades << "," << shortStats.WinningFlatToFlatTrades << "\n";
    log << "LosingTrades," << allStats.LosingTrades << "," << longStats.LosingTrades << "," << shortStats.LosingTrades << "\n";
    log << "LosingFlatToFlatTrades," << allStats.LosingFlatToFlatTrades << "," << longStats.LosingFlatToFlatTrades << "," << shortStats.LosingFlatToFlatTrades << "\n";
    log << "LongTrades," << allStats.LongTrades << "," << longStats.LongTrades << "," << shortStats.LongTrades << "\n";
    log << "LongFlatToFlatTrades," << allStats.LongFlatToFlatTrades << "," << longStats.LongFlatToFlatTrades << "," << shortStats.LongFlatToFlatTrades << "\n";
    log << "ShortTrades," << allStats.ShortTrades << "," << longStats.ShortTrades << "," << shortStats.ShortTrades << "\n";
    log << "ShortFlatToFlatTrades," << allStats.ShortFlatToFlatTrades << "," << longStats.ShortFlatToFlatTrades << "," << shortStats.ShortFlatToFlatTrades << "\n";
    log << "AverageTradeProfitLoss," << allStats.AverageTradeProfitLoss << "," << longStats.AverageTradeProfitLoss << "," << shortStats.AverageTradeProfitLoss << "\n";
    log << "AverageFlatToFlatTradeProfitLoss," << allStats.AverageFlatToFlatTradeProfitLoss << "," << longStats.AverageFlatToFlatTradeProfitLoss << "," << shortStats.AverageFlatToFlatTradeProfitLoss << "\n";
    log << "AverageWinningTrade," << allStats.AverageWinningTrade << "," << longStats.AverageWinningTrade << "," << shortStats.AverageWinningTrade << "\n";
    log << "AverageFlatToFlatWinningTrade," << allStats.AverageFlatToFlatWinningTrade << "," << longStats.AverageFlatToFlatWinningTrade << "," << shortStats.AverageFlatToFlatWinningTrade << "\n";
    log << "AverageLosingTrade," << allStats.AverageLosingTrade << "," << longStats.AverageLosingTrade << "," << shortStats.AverageLosingTrade << "\n";
    log << "AverageFlatToFlatLosingTrade," << allStats.AverageFlatToFlatLosingTrade << "," << longStats.AverageFlatToFlatLosingTrade << "," << shortStats.AverageFlatToFlatLosingTrade << "\n";
    log << "AverageProfitFactor," << allStats.AverageProfitFactor << "," << longStats.AverageProfitFactor << "," << shortStats.AverageProfitFactor << "\n";
    log << "AverageFlatToFlatProfitFactor," << allStats.AverageFlatToFlatProfitFactor << "," << longStats.AverageFlatToFlatProfitFactor << "," << shortStats.AverageFlatToFlatProfitFactor << "\n";
    log << "LargestWinningTrade," << allStats.LargestWinningTrade << "," << longStats.LargestWinningTrade << "," << shortStats.LargestWinningTrade << "\n";
    log << "LargestFlatToFlatWinningTrade," << allStats.LargestFlatToFlatWinningTrade << "," << longStats.LargestFlatToFlatWinningTrade << "," << shortStats.LargestFlatToFlatWinningTrade << "\n";
    log << "LargestLosingTrade," << allStats.LargestLosingTrade << "," << longStats.LargestLosingTrade << "," << shortStats.LargestLosingTrade << "\n";
    log << "LargestFlatToFlatLosingTrade," << allStats.LargestFlatToFlatLosingTrade << "," << longStats.LargestFlatToFlatLosingTrade << "," << shortStats.LargestFlatToFlatLosingTrade << "\n";
    log << "LargestWinnerPercentOfProfit," << allStats.LargestWinnerPercentOfProfit << "," << longStats.LargestWinnerPercentOfProfit << "," << shortStats.LargestWinnerPercentOfProfit << "\n";
    log << "LargestFlatToFlatWinnerPercentOfProfit," << allStats.LargestFlatToFlatWinnerPercentOfProfit << "," << longStats.LargestFlatToFlatWinnerPercentOfProfit << "," << shortStats.LargestFlatToFlatWinnerPercentOfProfit << "\n";
    log << "LargestLoserPercentOfLoss," << allStats.LargestLoserPercentOfLoss << "," << longStats.LargestLoserPercentOfLoss << "," << shortStats.LargestLoserPercentOfLoss << "\n";
    log << "LargestFlatToFlatLoserPercentOfLoss," << allStats.LargestFlatToFlatLoserPercentOfLoss << "," << longStats.LargestFlatToFlatLoserPercentOfLoss << "," << shortStats.LargestFlatToFlatLoserPercentOfLoss << "\n";
    log << "MaxConsecutiveWinners," << allStats.MaxConsecutiveWinners << "," << longStats.MaxConsecutiveWinners << "," << shortStats.MaxConsecutiveWinners << "\n";
    log << "MaxConsecutiveLosers," << allStats.MaxConsecutiveLosers << "," << longStats.MaxConsecutiveLosers << "," << shortStats.MaxConsecutiveLosers << "\n";
    log << "AverageTimeInTrades," << allStats.AverageTimeInTrades << "," << longStats.AverageTimeInTrades << "," << shortStats.AverageTimeInTrades << "\n";
    log << "AverageTimeInWinningTrades," << allStats.AverageTimeInWinningTrades << "," << longStats.AverageTimeInWinningTrades << "," << shortStats.AverageTimeInWinningTrades << "\n";
    log << "AverageTimeInLosingTrades," << allStats.AverageTimeInLosingTrades << "," << longStats.AverageTimeInLosingTrades << "," << shortStats.AverageTimeInLosingTrades << "\n";
    log << "LongestHeldWinningTrade," << allStats.LongestHeldWinningTrade << "," << longStats.LongestHeldWinningTrade << "," << shortStats.LongestHeldWinningTrade << "\n";
    log << "LongestHeldLosingTrade," << allStats.LongestHeldLosingTrade << "," << longStats.LongestHeldLosingTrade << "," << shortStats.LongestHeldLosingTrade << "\n";
    log << "TotalQuantity," << allStats.TotalQuantity << "," << longStats.TotalQuantity << "," << shortStats.TotalQuantity << "\n";
    log << "WinningQuantity," << allStats.WinningQuantity << "," << longStats.WinningQuantity << "," << shortStats.WinningQuantity << "\n";
    log << "LosingQuantity," << allStats.LosingQuantity << "," << longStats.LosingQuantity << "," << shortStats.LosingQuantity << "\n";
    log << "AverageQuantityPerTrade," << allStats.AverageQuantityPerTrade << "," << longStats.AverageQuantityPerTrade << "," << shortStats.AverageQuantityPerTrade << "\n";
    log << "AverageQuantityPerFlatToFlatTrade," << allStats.AverageQuantityPerFlatToFlatTrade << "," << longStats.AverageQuantityPerFlatToFlatTrade << "," << shortStats.AverageQuantityPerFlatToFlatTrade << "\n";
    log << "AverageQuantityPerWinningTrade," << allStats.AverageQuantityPerWinningTrade << "," << longStats.AverageQuantityPerWinningTrade << "," << shortStats.AverageQuantityPerWinningTrade << "\n";
    log << "AverageQuantityPerFlatToFlatWinningTrade," << allStats.AverageQuantityPerFlatToFlatWinningTrade << "," << longStats.AverageQuantityPerFlatToFlatWinningTrade << "," << shortStats.AverageQuantityPerFlatToFlatWinningTrade << "\n";
    log << "AverageQuantityPerLosingTrade," << allStats.AverageQuantityPerLosingTrade << "," << longStats.AverageQuantityPerLosingTrade << "," << shortStats.AverageQuantityPerLosingTrade << "\n";
    log << "AverageQuantityPerFlatToFlatLosingTrade," << allStats.AverageQuantityPerFlatToFlatLosingTrade << "," << longStats.AverageQuantityPerFlatToFlatLosingTrade << "," << shortStats.AverageQuantityPerFlatToFlatLosingTrade << "\n";
    log << "LargestTradeQuantity," << allStats.LargestTradeQuantity << "," << longStats.LargestTradeQuantity << "," << shortStats.LargestTradeQuantity << "\n";
    log << "LargestFlatToFlatTradeQuantity," << allStats.LargestFlatToFlatTradeQuantity << "," << longStats.LargestFlatToFlatTradeQuantity << "," << shortStats.LargestFlatToFlatTradeQuantity << "\n";
    log << "MaximumOpenPositionQuantity," << allStats.MaximumOpenPositionQuantity << "," << longStats.MaximumOpenPositionQuantity << "," << shortStats.MaximumOpenPositionQuantity << "\n";
    log << "LastTradeProfitLoss," << allStats.LastTradeProfitLoss << "," << longStats.LastTradeProfitLoss << "," << shortStats.LastTradeProfitLoss << "\n";
    log << "LastTradeQuantity," << allStats.LastTradeQuantity << "," << longStats.LastTradeQuantity << "," << shortStats.LastTradeQuantity << "\n";
    log << "NumberOfOpenTrades," << allStats.NumberOfOpenTrades << "," << longStats.NumberOfOpenTrades << "," << shortStats.NumberOfOpenTrades << "\n";
    log << "OpenTradesOpenQuantity," << allStats.OpenTradesOpenQuantity << "," << longStats.OpenTradesOpenQuantity << "," << shortStats.OpenTradesOpenQuantity << "\n";
    log << "OpenTradesAverageEntryPrice," << allStats.OpenTradesAverageEntryPrice << "," << longStats.OpenTradesAverageEntryPrice << "," << shortStats.OpenTradesAverageEntryPrice << "\n";
    log << "LastFillDateTime," << sc.DateTimeToString(allStats.LastFillDateTime, FLAG_DT_COMPLETE_DATETIME).GetChars() << "," << sc.DateTimeToString(longStats.LastFillDateTime, FLAG_DT_COMPLETE_DATETIME).GetChars() << "," << sc.DateTimeToString(shortStats.LastFillDateTime, FLAG_DT_COMPLETE_DATETIME).GetChars() << "\n";
    log << "LastEntryDateTime," << sc.DateTimeToString(allStats.LastEntryDateTime, FLAG_DT_COMPLETE_DATETIME).GetChars() << "," << sc.DateTimeToString(longStats.LastEntryDateTime, FLAG_DT_COMPLETE_DATETIME).GetChars() << "," << sc.DateTimeToString(shortStats.LastEntryDateTime, FLAG_DT_COMPLETE_DATETIME).GetChars() << "\n";
    log << "LastExitDateTime," << sc.DateTimeToString(allStats.LastExitDateTime, FLAG_DT_COMPLETE_DATETIME).GetChars() << "," << sc.DateTimeToString(longStats.LastExitDateTime, FLAG_DT_COMPLETE_DATETIME).GetChars() << "," << sc.DateTimeToString(shortStats.LastExitDateTime, FLAG_DT_COMPLETE_DATETIME).GetChars() << "\n";
    log << "TotalBuyQuantity," << allStats.TotalBuyQuantity << "," << longStats.TotalBuyQuantity << "," << shortStats.TotalBuyQuantity << "\n";
    log << "TotalSellQuantity," << allStats.TotalSellQuantity << "," << longStats.TotalSellQuantity << "," << shortStats.TotalSellQuantity << "\n";
    log << "ClosedFlatToFlatTradesProfitLoss," << allStats.ClosedFlatToFlatTradesProfitLoss << "," << longStats.ClosedFlatToFlatTradesProfitLoss << "," << shortStats.ClosedFlatToFlatTradesProfitLoss << "\n";
}

json ReportGenerator::GetTradesData(SCStudyInterfaceRef sc) {
    json tradesData = json::array();
    int tradeListSize = sc.GetTradeListSize();
    for (int i = 0; i < tradeListSize; ++i) {
        s_ACSTrade trade;
        sc.GetTradeListEntry(i, trade);
        if (trade.IsTradeClosed) {
            json tradeData;
            tradeData["OpenDateTime"] = sc.DateTimeToString(trade.OpenDateTime, FLAG_DT_COMPLETE_DATETIME).GetChars();
            tradeData["CloseDateTime"] = sc.DateTimeToString(trade.CloseDateTime, FLAG_DT_COMPLETE_DATETIME).GetChars();
            tradeData["TradeType"] = trade.TradeType;
            tradeData["TradeQuantity"] = trade.TradeQuantity;
            tradeData["MaxClosedQuantity"] = trade.MaxClosedQuantity;
            tradeData["MaxOpenQuantity"] = trade.MaxOpenQuantity;
            tradeData["EntryPrice"] = trade.EntryPrice;
            tradeData["ExitPrice"] = trade.ExitPrice;
            tradeData["TradeProfitLoss"] = trade.TradeProfitLoss;
            tradeData["MaximumOpenPositionLoss"] = trade.MaximumOpenPositionLoss;
            tradeData["MaximumOpenPositionProfit"] = trade.MaximumOpenPositionProfit;
            tradeData["FlatToFlatMaximumOpenPositionProfit"] = trade.FlatToFlatMaximumOpenPositionProfit;
            tradeData["FlatToFlatMaximumOpenPositionLoss"] = trade.FlatToFlatMaximumOpenPositionLoss;
            tradeData["Commission"] = trade.Commission;
            tradeData["IsTradeClosed"] = trade.IsTradeClosed;
            tradeData["Note"] = trade.Note.GetChars();
            tradesData.push_back(tradeData);
        }
    }
    return tradesData;
}

template <typename T>
nlohmann::json MakeTradeStats(SCStudyInterfaceRef sc, const T& stats) {
    return {
        {"ClosedTradesProfitLoss", stats.ClosedTradesProfitLoss},
        {"ClosedTradesTotalProfit", stats.ClosedTradesTotalProfit},
        {"ClosedTradesTotalLoss", stats.ClosedTradesTotalLoss},
        {"ProfitFactor", stats.ProfitFactor},
        {"EquityPeak", stats.EquityPeak},
        {"EquityValley", stats.EquityValley},
        {"MaximumRunup", stats.MaximumRunup},
        {"MaximumDrawdown", stats.MaximumDrawdown},
        {"MaximumFlatToFlatTradeOpenProfit", stats.MaximumFlatToFlatTradeOpenProfit},
        {"MaximumFlatToFlatTradeOpenLoss", stats.MaximumFlatToFlatTradeOpenLoss},
        {"AverageTradeOpenProfit", stats.AverageTradeOpenProfit},
        {"AverageTradeOpenLoss", stats.AverageTradeOpenLoss},
        {"AverageWinningTradeOpenProfit", stats.AverageWinningTradeOpenProfit},
        {"AverageWinningTradeOpenLoss", stats.AverageWinningTradeOpenLoss},
        {"AverageLosingTradeOpenProfit", stats.AverageLosingTradeOpenProfit},
        {"AverageLosingTradeOpenLoss", stats.AverageLosingTradeOpenLoss},
        {"MaximumTradeOpenProfit", stats.MaximumTradeOpenProfit},
        {"MaximumTradeOpenLoss", stats.MaximumTradeOpenLoss},
        {"HighestPriceDuringPositions", stats.HighestPriceDuringPositions},
        {"LowestPriceDuringPositions", stats.LowestPriceDuringPositions},
        {"TotalCommissions", stats.TotalCommissions},
        {"TotalTrades", stats.TotalTrades},
        {"TotalFlatToFlatTrades", stats.TotalFlatToFlatTrades},
        {"TotalFilledQuantity", stats.TotalFilledQuantity},
        {"PercentProfitable", stats.PercentProfitable},
        {"FlatToFlatPercentProfitable", stats.FlatToFlatPercentProfitable},
        {"WinningTrades", stats.WinningTrades},
        {"WinningFlatToFlatTrades", stats.WinningFlatToFlatTrades},
        {"LosingTrades", stats.LosingTrades},
        {"LosingFlatToFlatTrades", stats.LosingFlatToFlatTrades},
        {"LongTrades", stats.LongTrades},
        {"LongFlatToFlatTrades", stats.LongFlatToFlatTrades},
        {"ShortTrades", stats.ShortTrades},
        {"ShortFlatToFlatTrades", stats.ShortFlatToFlatTrades},
        {"AverageTradeProfitLoss", stats.AverageTradeProfitLoss},
        {"AverageFlatToFlatTradeProfitLoss", stats.AverageFlatToFlatTradeProfitLoss},
        {"AverageWinningTrade", stats.AverageWinningTrade},
        {"AverageFlatToFlatWinningTrade", stats.AverageFlatToFlatWinningTrade},
        {"AverageLosingTrade", stats.AverageLosingTrade},
        {"AverageFlatToFlatLosingTrade", stats.AverageFlatToFlatLosingTrade},
        {"AverageProfitFactor", stats.AverageProfitFactor},
        {"AverageFlatToFlatProfitFactor", stats.AverageFlatToFlatProfitFactor},
        {"LargestWinningTrade", stats.LargestWinningTrade},
        {"LargestFlatToFlatWinningTrade", stats.LargestFlatToFlatWinningTrade},
        {"LargestLosingTrade", stats.LargestLosingTrade},
        {"LargestFlatToFlatLosingTrade", stats.LargestFlatToFlatLosingTrade},
        {"LargestWinnerPercentOfProfit", stats.LargestWinnerPercentOfProfit},
        {"LargestFlatToFlatWinnerPercentOfProfit", stats.LargestFlatToFlatWinnerPercentOfProfit},
        {"LargestLoserPercentOfLoss", stats.LargestLoserPercentOfLoss},
        {"LargestFlatToFlatLoserPercentOfLoss", stats.LargestFlatToFlatLoserPercentOfLoss},
        {"MaxConsecutiveWinners", stats.MaxConsecutiveWinners},
        {"MaxConsecutiveLosers", stats.MaxConsecutiveLosers},
        {"AverageTimeInTrades", stats.AverageTimeInTrades},
        {"AverageTimeInWinningTrades", stats.AverageTimeInWinningTrades},
        {"AverageTimeInLosingTrades", stats.AverageTimeInLosingTrades},
        {"LongestHeldWinningTrade", stats.LongestHeldWinningTrade},
        {"LongestHeldLosingTrade", stats.LongestHeldLosingTrade},
        {"TotalQuantity", stats.TotalQuantity},
        {"WinningQuantity", stats.WinningQuantity},
        {"LosingQuantity", stats.LosingQuantity},
        {"AverageQuantityPerTrade", stats.AverageQuantityPerTrade},
        {"AverageQuantityPerFlatToFlatTrade", stats.AverageQuantityPerFlatToFlatTrade},
        {"AverageQuantityPerWinningTrade", stats.AverageQuantityPerWinningTrade},
        {"AverageQuantityPerFlatToFlatWinningTrade", stats.AverageQuantityPerFlatToFlatWinningTrade},
        {"AverageQuantityPerLosingTrade", stats.AverageQuantityPerLosingTrade},
        {"AverageQuantityPerFlatToFlatLosingTrade", stats.AverageQuantityPerFlatToFlatLosingTrade},
        {"LargestTradeQuantity", stats.LargestTradeQuantity},
        {"LargestFlatToFlatTradeQuantity", stats.LargestFlatToFlatTradeQuantity},
        {"MaximumOpenPositionQuantity", stats.MaximumOpenPositionQuantity},
        {"LastTradeProfitLoss", stats.LastTradeProfitLoss},
        {"LastTradeQuantity", stats.LastTradeQuantity},
        {"NumberOfOpenTrades", stats.NumberOfOpenTrades},
        {"OpenTradesOpenQuantity", stats.OpenTradesOpenQuantity},
        {"OpenTradesAverageEntryPrice", stats.OpenTradesAverageEntryPrice},
        {"LastFillDateTime", sc.DateTimeToString(stats.LastFillDateTime, FLAG_DT_COMPLETE_DATETIME).GetChars()},
        {"LastEntryDateTime", sc.DateTimeToString(stats.LastEntryDateTime, FLAG_DT_COMPLETE_DATETIME).GetChars()},
        {"LastExitDateTime", sc.DateTimeToString(stats.LastExitDateTime, FLAG_DT_COMPLETE_DATETIME).GetChars()},
        {"TotalBuyQuantity", stats.TotalBuyQuantity},
        {"TotalSellQuantity", stats.TotalSellQuantity},
        {"ClosedFlatToFlatTradesProfitLoss", stats.ClosedFlatToFlatTradesProfitLoss}
    };
}

json ReportGenerator::GetTradeStatistics(SCStudyInterfaceRef sc) {
    json tradeStatistics;
    n_ACSIL::s_TradeStatistics allStats, longStats, shortStats;
    sc.GetTradeStatisticsForSymbolV2(n_ACSIL::STATS_TYPE_ALL_TRADES, allStats);
    sc.GetTradeStatisticsForSymbolV2(n_ACSIL::STATS_TYPE_LONG_TRADES, longStats);
    sc.GetTradeStatisticsForSymbolV2(n_ACSIL::STATS_TYPE_SHORT_TRADES, shortStats);
    tradeStatistics["All Trades"] = MakeTradeStats(sc, allStats);
    tradeStatistics["Long Trades"] = MakeTradeStats(sc, longStats);
    tradeStatistics["Short Trades"] = MakeTradeStats(sc, shortStats);
    return tradeStatistics;
}

json ReportGenerator::GetCombination(const std::vector<std::pair<std::string, double>>& params) {
    json combination;
    for (const auto& p : params) {
        combination[p.first] = p.second;
    }
    return combination;
}