#pragma once

#include "engine/OrderBook.hpp"
#include <utility>

namespace lob {

class MarketMaker {
public:
    virtual ~MarketMaker() = default;

    // Returns a pair {BidPrice, AskPrice}
    // If interaction with order book is needed (canceling old quotes), it can return orders to place.
    // For simplicity, let's assume the simulator asks for "Target Quotes" and handles the order management (cancel/replace).
    // Or, the strategy returns a struct with desired actions.
    
    // Let's return target Bid and Ask prices.
    // We assume the strategy wants to maintain 1 unit or a fixed size at these levels.
    virtual std::pair<double, double> getQuotes(const OrderBook& book, double currentInventory, double time) = 0;
};

} // namespace lob
