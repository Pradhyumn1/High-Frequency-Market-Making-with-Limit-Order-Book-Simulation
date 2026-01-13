#include "engine/OrderBook.hpp"
#include <limits>
#include <iostream>

namespace lob {

OrderBook::OrderBook() {}

void OrderBook::addOrder(const Order& order) {
    if (order.side == Side::Buy) {
        auto& list = bids[order.price];
        list.push_back(order);
        orderLookup[order.id] = {order.price, Side::Buy, --list.end()};
    } else {
        auto& list = asks[order.price];
        list.push_back(order);
        orderLookup[order.id] = {order.price, Side::Sell, --list.end()};
    }
}

void OrderBook::cancelOrder(uint64_t orderId) {
    auto it = orderLookup.find(orderId);
    if (it != orderLookup.end()) {
        removeOrderInternal(orderId, it->second);
        orderLookup.erase(it);
    }
}

void OrderBook::removeOrderInternal(uint64_t orderId, const OrderLocation& loc) {
    if (loc.side == Side::Buy) {
        auto mapIt = bids.find(loc.price);
        if (mapIt != bids.end()) {
            mapIt->second.erase(loc.it);
            if (mapIt->second.empty()) {
                bids.erase(mapIt);
            }
        }
    } else {
        auto mapIt = asks.find(loc.price);
        if (mapIt != asks.end()) {
            mapIt->second.erase(loc.it);
            if (mapIt->second.empty()) {
                asks.erase(mapIt);
            }
        }
    }
}

double OrderBook::getBestBid() const {
    if (bids.empty()) return 0.0;
    return bids.begin()->first;
}

double OrderBook::getBestAsk() const {
    if (asks.empty()) return std::numeric_limits<double>::infinity();
    return asks.begin()->first;
}

double OrderBook::getMidPrice() const {
    double bb = getBestBid();
    double ba = getBestAsk();
    if (bb == 0.0 || ba == std::numeric_limits<double>::infinity()) {
        return 0.0; // Market is one-sided or empty
    }
    return (bb + ba) / 2.0;
}

std::vector<OrderBook::Level> OrderBook::getBids(int depth) const {
    std::vector<Level> levels;
    int count = 0;
    for (const auto& [price, list] : bids) {
        if (count++ >= depth) break;
        double totalQty = 0;
        for (const auto& ord : list) totalQty += ord.quantity;
        levels.push_back({price, totalQty});
    }
    return levels;
}

std::vector<OrderBook::Level> OrderBook::getAsks(int depth) const {
    std::vector<Level> levels;
    int count = 0;
    for (const auto& [price, list] : asks) {
        if (count++ >= depth) break;
        double totalQty = 0;
        for (const auto& ord : list) totalQty += ord.quantity;
        levels.push_back({price, totalQty});
    }
    return levels;
}

} // namespace lob
