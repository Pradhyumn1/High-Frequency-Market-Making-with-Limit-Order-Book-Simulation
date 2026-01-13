#include "engine/MatchingEngine.hpp"
#include <algorithm>
#include <limits>
#include <iostream>

namespace lob {

MatchingEngine::MatchingEngine(OrderBook& book) : orderBook(book) {}

std::vector<Trade> MatchingEngine::matchOrder(Order& incomingOrder) {
    std::vector<Trade> trades;
    
    while (incomingOrder.quantity > 0) {
        if (incomingOrder.side == Side::Buy) {
            if (orderBook.asks.empty()) break;

            auto bestAskIt = orderBook.asks.begin();
            double bestAskPrice = bestAskIt->first;
            auto& levelOrders = bestAskIt->second;

            // Price check for Limit orders
            if (incomingOrder.type == OrderType::Limit && incomingOrder.price < bestAskPrice) {
                break; 
            }

            // Match against orders at this level
            while (!levelOrders.empty() && incomingOrder.quantity > 0) {
                auto& restingOrder = levelOrders.front();
                
                double matchQty = std::min(incomingOrder.quantity, restingOrder.quantity);
                
                Trade trade;
                trade.restingOrderId = restingOrder.id;
                trade.aggressiveOrderId = incomingOrder.id;
                trade.price = bestAskPrice;
                trade.quantity = matchQty;
                trade.timestamp = incomingOrder.timestamp; // Match time = arrival time
                trade.aggressiveSide = Side::Buy;
                trades.push_back(trade);

                incomingOrder.quantity -= matchQty;
                restingOrder.quantity -= matchQty;

                if (restingOrder.quantity <= 1e-9) { // Epsilon check
                    // Remove from lookup
                    orderBook.orderLookup.erase(restingOrder.id);
                    // Remove from list
                    levelOrders.pop_front();
                }
            }

            // If level executed, remove it
            if (levelOrders.empty()) {
                orderBook.asks.erase(bestAskIt);
            }

        } else { // Side::Sell
            if (orderBook.bids.empty()) break;

            auto bestBidIt = orderBook.bids.begin();
            double bestBidPrice = bestBidIt->first;
            auto& levelOrders = bestBidIt->second;

            // Price check for Limit orders
            if (incomingOrder.type == OrderType::Limit && incomingOrder.price > bestBidPrice) {
                break;
            }

            // Match against orders at this level
            while (!levelOrders.empty() && incomingOrder.quantity > 0) {
                auto& restingOrder = levelOrders.front();
                
                double matchQty = std::min(incomingOrder.quantity, restingOrder.quantity);
                
                Trade trade;
                trade.restingOrderId = restingOrder.id;
                trade.aggressiveOrderId = incomingOrder.id;
                trade.price = bestBidPrice; // Trade happens at resting price
                trade.quantity = matchQty;
                trade.timestamp = incomingOrder.timestamp;
                trade.aggressiveSide = Side::Sell;
                trades.push_back(trade);

                incomingOrder.quantity -= matchQty;
                restingOrder.quantity -= matchQty;

                if (restingOrder.quantity <= 1e-9) {
                    orderBook.orderLookup.erase(restingOrder.id);
                    levelOrders.pop_front();
                }
            }

            if (levelOrders.empty()) {
                orderBook.bids.erase(bestBidIt);
            }
        }
    }

    // If remaining quantity, add to book (Limit Only)
    // Market orders are IOC/FOK usually, but here we assume defined behavior.
    // Standard limit order adds remainder. Market order usually cancels remainder if no liquidity.
    if (incomingOrder.quantity > 1e-9 && incomingOrder.type == OrderType::Limit) {
        orderBook.addOrder(incomingOrder);
    }

    return trades;
}

} // namespace lob
