#pragma once
#include <string>
#include "mcx/Random.hpp"

namespace mcx {


class Market;

class MarketEvent {
public:
    virtual ~MarketEvent() = default;
    virtual std::string name() const = 0;
    virtual double probabilityPerTick() const = 0;
    virtual std::string apply(Market& market, Random& rng) = 0;   // zwraca opis dla widoku na żywo
};

class CrashEvent final : public MarketEvent {
public:
    explicit CrashEvent(double probability = 0.008) : p_(probability) {}
    std::string name() const override { return "KRACH"; }
    double probabilityPerTick() const override { return p_; }
    std::string apply(Market& market, Random& rng) override;
private:
    double p_;
};

class RallyEvent final : public MarketEvent {
public:
    explicit RallyEvent(double probability = 0.012) : p_(probability) {}
    std::string name() const override { return "HOSSA"; }
    double probabilityPerTick() const override { return p_; }
    std::string apply(Market& market, Random& rng) override;
private:
    double p_;
};

}
