#pragma once

#include "engine/Order.hpp"
#include <queue>
#include <vector>

namespace lob {

struct PendingOrder {
    Order order;
    double releaseTime; // When the order hits the book
    bool isCancellation;
    uint64_t orderIdToCancel; // If cancellation
};

class LatencyModel {
public:
    LatencyModel(double latencyMean, double latencyStd);

    void addOrder(const Order& order, double currentTime);
    void addCancellation(uint64_t orderId, double currentTime);

    std::vector<PendingOrder> getReadyOrders(double currentTime);

private:
    double latencyMean;
    double latencyStd;
    
    // Priority queue to order by release time
    struct CompareOrder {
        bool operator()(const PendingOrder& a, const PendingOrder& b) {
            return a.releaseTime > b.releaseTime; // Min-heap
        }
    };

    std::priority_queue<PendingOrder, std::vector<PendingOrder>, CompareOrder> eventQueue;
};

} // namespace lob
