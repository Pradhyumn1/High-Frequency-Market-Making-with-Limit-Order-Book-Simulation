#pragma once

#include "engine/Order.hpp"
#include <map>
#include <unordered_map>
#include <list>
#include <vector>
#include <optional>
#include <string>

namespace lob {

class OrderBook {
    friend class MatchingEngine;
public:
    OrderBook();

    // Core actions
    void addOrder(const Order& order);
    void cancelOrder(uint64_t orderId);
    
    // Read actions
    double getBestBid() const;
    double getBestAsk() const;
    double getMidPrice() const;
    
    // For analysis
    struct Level {
        double price;
        double quantity;
    };
    
    std::vector<Level> getBids(int depth = 10) const;
    std::vector<Level> getAsks(int depth = 10) const;

private:
    struct OrderEntry {
        Order order;
        std::list<Order>::iterator listIterator;
    };

    // Price -> List of Orders (FIFO)
    std::map<double, std::list<Order>, std::greater<double>> bids;
    std::map<double, std::list<Order>, std::less<double>> asks;

    // Fast lookup for modifications/cancellations
    // OrderID -> (Price, Iterator to list)
    // We need to know which price level map to look into.
    // Actually, storing the price is enough to find the map entry, 
    // but the iterator requires knowing the specific list.
    // Let's store pointers or iterators.
    
    struct OrderLocation {
        double price;
        Side side;
        std::list<Order>::iterator it;
    };
    
    std::unordered_map<uint64_t, OrderLocation> orderLookup;

    void removeOrderInternal(uint64_t orderId, const OrderLocation& loc);
};

} // namespace lob
