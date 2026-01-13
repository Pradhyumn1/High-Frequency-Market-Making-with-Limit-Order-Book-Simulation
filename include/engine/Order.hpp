#pragma once

#include <cstdint>
#include <string>

namespace lob {

enum class Side {
    Buy,
    Sell
};

enum class OrderType {
    Limit,
    Market
};

struct Order {
    uint64_t id;
    uint64_t timestamp;
    double price;
    double quantity;
    Side side;
    OrderType type;

    Order(uint64_t id, uint64_t timestamp, double price, double quantity, Side side, OrderType type)
        : id(id), timestamp(timestamp), price(price), quantity(quantity), side(side), type(type) {}
    
    Order() = default;
};

} // namespace lob
