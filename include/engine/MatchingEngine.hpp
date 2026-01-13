#pragma once

#include "engine/OrderBook.hpp"
#include <vector>

namespace lob {

struct Trade {
    uint64_t restingOrderId;
    uint64_t aggressiveOrderId;
    double price;
    double quantity;
    uint64_t timestamp;
    Side aggressiveSide;
};

class MatchingEngine {
public:
    explicit MatchingEngine(OrderBook& book);

    // Process an incoming order.
    // If it's a Limit order and marketable, it matches.
    // If it's a Market order, it matches.
    // Remaining quantity is added to the book (if Limit).
    std::vector<Trade> matchOrder(Order& incomingOrder);

private:
    OrderBook& orderBook;
};

} // namespace lob
