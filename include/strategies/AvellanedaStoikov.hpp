#pragma once

#include "strategies/MarketMaker.hpp"
#include <cmath>

namespace lob {

class AvellanedaStoikov : public MarketMaker {
public:
    AvellanedaStoikov(double gamma, double sigma, double k, double T);

    std::pair<double, double> getQuotes(const OrderBook& book, double currentInventory, double time) override;

    // Helper to calculate reservation price
    double calculateReservationPrice(double s, double q, double t) const;
    
    // Helper to calculate optimal spread
    double calculateOptimalSpread(double t) const;

private:
    double gamma; // Risk aversion
    double sigma; // Volatility
    double k;     // Order arrival intensity parameter
    double T;     // Terminal time (session length)
};

} // namespace lob
