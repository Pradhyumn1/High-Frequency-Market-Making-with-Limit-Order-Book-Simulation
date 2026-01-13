#include "simulation/MarketSimulator.hpp"
#include "strategies/AvellanedaStoikov.hpp"
#include <iostream>

int main() {
    using namespace lob;

    std::cout << "Initializing Simulation..." << std::endl;

    // Simulation Parameters
    double T = 600.0;    // 10 minutes session
    double dt = 0.05;    // 50ms time step
    
    // Model Parameters
    double latencyMean = 0.05; // 50ms latency
    double latencyStd = 0.01;
    
    // AS Strategy Parameters
    double gamma = 0.1;  // Risk aversion
    double sigma = 2.0;  // Volatility
    double k = 1.5;      // Intensity

    OrderBook book;
    LatencyModel latency(latencyMean, latencyStd);
    AvellanedaStoikov strategy(gamma, sigma, k, T);
    MatchingEngine engine(book);
    
    MarketSimulator sim(book, engine, strategy, latency);
    
    std::cout << "Starting Simulation for T=" << T << "s..." << std::endl;
    sim.run(T, dt);
    
    std::cout << "Simulation Complete. Data saved to CSVs." << std::endl;

    return 0;
}
