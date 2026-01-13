#include "strategies/AvellanedaStoikov.hpp"
#include <algorithm>
#include <iostream>

namespace lob {

AvellanedaStoikov::AvellanedaStoikov(double gamma, double sigma, double k, double T)
    : gamma(gamma), sigma(sigma), k(k), T(T) {}

double AvellanedaStoikov::calculateReservationPrice(double s, double q, double t) const {
    // r(s, q, t) = s - q * gamma * sigma^2 * (T - t)
    double time_remaining = std::max(0.0, T - t);
    return s - q * gamma * (sigma * sigma) * time_remaining;
}

double AvellanedaStoikov::calculateOptimalSpread(double t) const {
    // Standard symmetric spread around reservation price
    // spread = (2/gamma) * ln(1 + gamma/k)
    // Note: The prompt had a slightly different formula, but this is the classic result for spread width.
    // The prompt: delta = 1/gamma * ln(...)
    // If delta is the half-spread, then 2*delta is the full spread.
    // Let's use the standard "half spread" (delta) to subtract/add from r.
    
    // delta = (1/gamma) * ln(1 + gamma/k)
    double term = 1.0 + (gamma / k);
    double delta = (1.0 / gamma) * std::log(term);
    return delta; // This is the half-spread distance from r
}

std::pair<double, double> AvellanedaStoikov::getQuotes(const OrderBook& book, double currentInventory, double time) {
    double midPrice = book.getMidPrice();
    if (midPrice == 0.0) {
        // Fallback if book is empty/one-sided. 
        // In a real sim, we might wait or use a default price.
        // For now, return {0,0} or handle externally.
        return {0.0, 0.0};
    }

    double r = calculateReservationPrice(midPrice, currentInventory, time);
    double halfSpread = calculateOptimalSpread(time);

    // Ensure we don't cross? Avellaneda-Stoikov can cross the mid if inventory is extreme.
    // But typically we quote around r.
    
    double bid = r - halfSpread;
    double ask = r + halfSpread;

    return {bid, ask};
}

} // namespace lob
