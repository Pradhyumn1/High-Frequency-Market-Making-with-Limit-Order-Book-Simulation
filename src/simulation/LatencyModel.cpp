#include "simulation/LatencyModel.hpp"
#include <random>
#include <iostream>

namespace lob {

LatencyModel::LatencyModel(double latencyMean, double latencyStd)
    : latencyMean(latencyMean), latencyStd(latencyStd) {}

void LatencyModel::addOrder(const Order& order, double currentTime) {
    // Determine delay
    static std::mt19937 gen(42);
    std::normal_distribution<> d(latencyMean, latencyStd);
    
    double delay = d(gen);
    if (delay < 0) delay = 0; // No negative latency

    PendingOrder po;
    po.order = order;
    po.releaseTime = currentTime + delay;
    po.isCancellation = false;
    
    eventQueue.push(po);
}

void LatencyModel::addCancellation(uint64_t orderId, double currentTime) {
    static std::mt19937 gen(42);
    std::normal_distribution<> d(latencyMean, latencyStd);
    
    double delay = d(gen);
    if (delay < 0) delay = 0;

    PendingOrder po;
    po.orderIdToCancel = orderId;
    po.releaseTime = currentTime + delay;
    po.isCancellation = true;

    eventQueue.push(po);
}

std::vector<PendingOrder> LatencyModel::getReadyOrders(double currentTime) {
    std::vector<PendingOrder> ready;
    while (!eventQueue.empty()) {
        const auto& top = eventQueue.top();
        if (top.releaseTime <= currentTime) {
            ready.push_back(top);
            eventQueue.pop();
        } else {
            break;
        }
    }
    return ready;
}

} // namespace lob
