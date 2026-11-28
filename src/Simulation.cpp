#include "mcx/Simulation.hpp"
#include "mcx/Exceptions.hpp"

namespace mcx {

Simulation::Simulation(Market market, double dt, std::uint64_t seed, double commissionRate)
    : market_(std::move(market)), dt_(dt), commission_(commissionRate), rng_(seed) {
    if (dt <= 0.0) throw ConfigError("Simulation: dt musi byc > 0");
}

void Simulation::addTrader(std::unique_ptr<Trader> trader) {
    if (!trader) throw ConfigError("Simulation::addTrader: null");
    traders_.push_back(std::move(trader));
}

void Simulation::attach(std::shared_ptr<SimulationObserver> observer) {
    if (!observer) throw ConfigError("Simulation::attach: null");
    observers_.push_back(std::move(observer));
}

void Simulation::step() {
    ++tick_;
    market_.step(dt_, rng_);
    for (auto& trader : traders_) {
        for (const Order& order : trader->decide(market_, rng_)) execute(*trader, order);
    }
    for (auto& obs : observers_) obs->onTick(*this);
}

void Simulation::run(std::size_t ticks) {
    for (std::size_t i = 0; i < ticks; ++i) step();
    finish();
}

void Simulation::finish() {
    for (auto& obs : observers_) obs->onFinish(*this);
}


void Simulation::execute(Trader& trader, const Order& order) {
    try {
        const double price = market_.stock(order.ticker).price();
        if (order.side == Side::Buy) trader.portfolio().buy(order.ticker, order.quantity, price, commission_);
        else trader.portfolio().sell(order.ticker, order.quantity, price, commission_);
        trader.recordOrder(true);
    } catch (const SimulationError&) {
        trader.recordOrder(false);   // zlecenie odrzucone: symulacja idzie dalej
    }
}

}  // namespace mcx
