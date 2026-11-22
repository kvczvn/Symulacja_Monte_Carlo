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

}
