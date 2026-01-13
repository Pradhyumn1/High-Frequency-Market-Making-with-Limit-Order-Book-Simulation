#include "strategies/AvellanedaStoikov.hpp"
#include <algorithm>
#include <iostream>
#include <cmath>

namespace lob {

AvellanedaStoikov::AvellanedaStoikov(double gamma, double sigma, double k, double T)
    : gamma(gamma), sigma(sigma), k(k), T(T) {}

double AvellanedaStoikov::calculateReservationPrice(double s, double q, double t) const {
    double time_remaining = std::max(0.0, T - t);
    return s - q * gamma * (sigma * sigma) * time_remaining;
}

double AvellanedaStoikov::calculateOptimalSpread(double t) const {
    double term = 1.0 + (gamma / k);
    double delta = (1.0 / gamma) * std::log(term);
    return delta;
}

std::pair<double, double> AvellanedaStoikov::getQuotes(const OrderBook& book, double currentInventory, double time) {
    double midPrice = book.getMidPrice();
    if (midPrice == 0.0) return {0.0, 0.0};

    double r = calculateReservationPrice(midPrice, currentInventory, time);
    double halfSpread = calculateOptimalSpread(time);

    return {r - halfSpread, r + halfSpread};
}

} // namespace lob
