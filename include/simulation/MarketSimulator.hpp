#pragma once

#include "engine/OrderBook.hpp"
#include "engine/MatchingEngine.hpp"
#include "strategies/MarketMaker.hpp"
#include "simulation/LatencyModel.hpp"
#include <vector>
#include <string>
#include <fstream>

namespace lob {

class MarketSimulator {
public:
    MarketSimulator(OrderBook& book, MatchingEngine& matchingEngine, MarketMaker& strategy, LatencyModel& latencyModel);

    void run(double duration, double dt);

private:
    OrderBook& book;
    MatchingEngine& matchingEngine;
    MarketMaker& strategy;
    LatencyModel& latencyModel;

    double currentTime;
    uint64_t nextOrderId = 1;

    // Logging
    std::ofstream tradeLog;
    std::ofstream bookLog;
    std::ofstream quoteLog; 

    void generateBackgroundInteractions(double dt);
    void logState();
    
    // Simple storage for MM active orders to manage cancellations
    uint64_t mmBidId = 0;
    uint64_t mmAskId = 0;
    double mmInventory = 0.0;
};

} // namespace lob
