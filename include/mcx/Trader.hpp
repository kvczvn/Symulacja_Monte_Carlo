#pragma once
#include <cstddef>
#include <string>
#include <vector>
#include "mcx/Market.hpp"
#include "mcx/Order.hpp"
#include "mcx/Portfolio.hpp"
#include "mcx/Random.hpp"

namespace mcx {


class Trader {
public:
    Trader(std::string name, double initialCash);
    virtual ~Trader() = default;

    virtual std::vector<Order> decide(const Market& market, Random& rng) = 0;
    virtual std::string strategy() const = 0;

    const std::string& name() const noexcept { return name_; }
    Portfolio& portfolio() noexcept { return portfolio_; }
    const Portfolio& portfolio() const noexcept { return portfolio_; }

    void recordOrder(bool accepted) noexcept { accepted ? ++accepted_ : ++rejected_; }
    std::size_t acceptedOrders() const noexcept { return accepted_; }
    std::size_t rejectedOrders() const noexcept { return rejected_; }

protected:  // narzędzia dla klas pochodnych
    static double sma(const RingBuffer<double>& history, std::size_t window);
    static int quantityForBudget(double budget, double price);

private:
    std::string name_;
    Portfolio portfolio_;
    std::size_t accepted_ = 0;
    std::size_t rejected_ = 0;
};

class BuyAndHoldTrader final : public Trader {
public:
    using Trader::Trader;
    std::vector<Order> decide(const Market& market, Random& rng) override;
    std::string strategy() const override { return "BuyAndHold"; }
private:
    bool invested_ = false;
};

class RandomTrader final : public Trader {
public:
    using Trader::Trader;
    std::vector<Order> decide(const Market& market, Random& rng) override;
    std::string strategy() const override { return "Random"; }
};

class MomentumTrader final : public Trader {
public:
    MomentumTrader(std::string name, double cash, std::size_t shortWindow = 5, std::size_t longWindow = 20);
    std::vector<Order> decide(const Market& market, Random& rng) override;
    std::string strategy() const override { return "Momentum"; }
private:
    std::size_t shortW_;
    std::size_t longW_;
};

}