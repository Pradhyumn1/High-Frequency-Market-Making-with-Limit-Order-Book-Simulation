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
            if (processOrderLevel(incomingOrder, levelOrders, bestAskPrice, Side::Buy, trades)) {
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
            if (processOrderLevel(incomingOrder, levelOrders, bestBidPrice, Side::Sell, trades)) {
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
