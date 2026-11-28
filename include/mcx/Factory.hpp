#pragma once
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "mcx/PriceModel.hpp"
#include "mcx/Simulation.hpp"

namespace mcx {

struct StockSpec {
    std::string ticker;
    double initialPrice;
    std::string model;                      // "gbm" | "jump" | "meanrev"
    std::map<std::string, double> params;   // parametry modelu
};

struct SimulationConfig {
    std::vector<StockSpec> stocks;
    double dt = 1.0 / 252.0;     // 1 dzień sesyjny
    double initialCash = 100000.0;
    double commission = 0.001;   // 0,1%
    bool enableEvents = true;
    std::size_t historySize = 120;

    static SimulationConfig defaults();
};

// WZORZEC FACTORY: zamienia opis tekstowy na obiekty.
std::unique_ptr<PriceModel> createModel(const StockSpec& spec);
std::unique_ptr<Simulation> buildSimulation(const SimulationConfig& cfg, std::uint64_t seed);

}  // namespace mcx
