#pragma once
#include <memory>
#include <vector>
#include "mcx/Market.hpp"
#include "mcx/Random.hpp"
#include "mcx/Trader.hpp"

namespace mcx {

class Simulation;

class SimulationObserver {
public:
    virtual ~SimulationObserver() = default;
    virtual void onTick(const Simulation& sim) = 0;
    virtual void onFinish(const Simulation&) {}
};

class Simulation {
public:
    Simulation(Market market, double dt, std::uint64_t seed, double commissionRate);

    void addTrader(std::unique_ptr<Trader> trader);
    void attach(std::shared_ptr<SimulationObserver> observer);

    void step();
    void run(std::size_t ticks);
    void finish();


    const Market& market() const noexcept { return market_; }
    const std::vector<std::unique_ptr<Trader>>& traders() const noexcept { return traders_; }
    std::size_t tick() const noexcept { return tick_; }
    std::uint64_t seed() const noexcept { return rng_.seed(); }

private:
    void execute(Trader& trader, const Order& order);

    Market market_;
    double dt_;
    double commission_;
    Random rng_;
    std::size_t tick_ = 0;
    std::vector<std::unique_ptr<Trader>> traders_;
    std::vector<std::shared_ptr<SimulationObserver>> observers_;
};

}  // namespace mcx
