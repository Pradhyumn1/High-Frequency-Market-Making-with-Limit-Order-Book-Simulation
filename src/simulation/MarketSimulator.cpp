#include "simulation/MarketSimulator.hpp"
#include <iostream>
#include <random>
#include <iomanip>

namespace lob {

MarketSimulator::MarketSimulator(OrderBook& book, MatchingEngine& matchingEngine, MarketMaker& strategy, LatencyModel& latencyModel)
    : book(book), matchingEngine(matchingEngine), strategy(strategy), latencyModel(latencyModel), currentTime(0.0) {
    
    tradeLog.open("trades.csv");
    tradeLog << "timestamp,price,quantity,side,resting_id,aggressive_id\n";
    
    bookLog.open("book.csv");
    bookLog << "timestamp,best_bid,best_ask,mid_price,inventory\n";

    quoteLog.open("quotes.csv");
    quoteLog << "timestamp,bid_price,ask_price,reservation_price\n";
}

void MarketSimulator::run(double duration, double dt) {
    std::mt19937 gen(12345);
    std::exponential_distribution<> arrivalDist(0.5); 
    std::uniform_real_distribution<> priceMoveDist(-0.5, 0.5);
    std::uniform_real_distribution<> sizeDist(1, 10);
    
    double nextArrivalTime = 0.0;
    double currentFairPrice = 100.0;

    for (double t = 0; t < duration; t += dt) {
        currentTime = t;

        // Process Latency Queue
        auto readyEvents = latencyModel.getReadyOrders(currentTime);
        for (const auto& ev : readyEvents) {
            if (ev.isCancellation) {
                book.cancelOrder(ev.orderIdToCancel);
            } else {
                Order ord = ev.order;
                auto trades = matchingEngine.matchOrder(ord);
                
                for (const auto& tr : trades) {
                    tradeLog << tr.timestamp << "," << tr.price << "," << tr.quantity << "," 
                             << (tr.aggressiveSide == Side::Buy ? "Buy" : "Sell") << ","
                             << tr.restingOrderId << "," << tr.aggressiveOrderId << "\n";
                             
                    if (tr.restingOrderId == mmBidId) mmInventory += tr.quantity;
                    if (tr.restingOrderId == mmAskId) mmInventory -= tr.quantity;
                }
            }
        }

        // Generate Background Noise
        if (t >= nextArrivalTime) {
            currentFairPrice += priceMoveDist(gen);
            
            Side side = (rand() % 2 == 0) ? Side::Buy : Side::Sell;
            double quantity = (int)sizeDist(gen);
            bool isAggressive = (rand() % 10 < 3); 
            
            double price;
            if (isAggressive) {
                 price = (side == Side::Buy) ? 1e9 : 1e-9;
                 Order mo(nextOrderId++, (uint64_t)t, price, quantity, side, OrderType::Limit);
                 latencyModel.addOrder(mo, t);
            } else {
                double spread = 1.0; 
                price = (side == Side::Buy) ? currentFairPrice - spread/2 : currentFairPrice + spread/2;
                Order lo(nextOrderId++, (uint64_t)t, price, quantity, side, OrderType::Limit);
                latencyModel.addOrder(lo, t);
            }
            
            nextArrivalTime += arrivalDist(gen);
        }
        
        // Strategy Update
        std::pair<double, double> quotes = strategy.getQuotes(book, mmInventory, t);
        
        double targetBid = quotes.first;
        double targetAsk = quotes.second;
        
        if (mmBidId != 0) latencyModel.addCancellation(mmBidId, t);
        if (mmAskId != 0) latencyModel.addCancellation(mmAskId, t);
        
        if (targetBid > 0) {
            mmBidId = nextOrderId++;
            Order bidOrder(mmBidId, (uint64_t)t, targetBid, 1.0, Side::Buy, OrderType::Limit);
            latencyModel.addOrder(bidOrder, t);
        }
        if (targetAsk > 0) {
            mmAskId = nextOrderId++;
            Order askOrder(mmAskId, (uint64_t)t, targetAsk, 1.0, Side::Sell, OrderType::Limit);
            latencyModel.addOrder(askOrder, t);
        }

        if ((int)t % 10 == 0) logState();
        
        quoteLog << t << "," << targetBid << "," << targetAsk << "," << (targetBid+targetAsk)/2.0 << "\n";
    }
    
    tradeLog.close();
    bookLog.close();
    quoteLog.close();
}

void MarketSimulator::logState() {
     bookLog << currentTime << "," << book.getBestBid() << "," << book.getBestAsk() 
             << "," << book.getMidPrice() << "," << mmInventory << "\n";
}

} // namespace lob
