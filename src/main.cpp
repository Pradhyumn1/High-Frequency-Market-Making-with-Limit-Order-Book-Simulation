#include "simulation/MarketSimulator.hpp"
#include "strategies/AvellanedaStoikov.hpp"
#include <iostream>

int main() {
    using namespace lob;

    std::cout << "Initializing Simulation..." << std::endl;

    double T = 600.0;
    double dt = 0.05;
    
    double latencyMean = 0.05;
    double latencyStd = 0.01;
    
    double gamma = 0.1;
    double sigma = 2.0;
    double k = 1.5;

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
