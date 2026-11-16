#pragma once
#include <cstdint>
#include <random>

namespace mcx {

// Cienka otoczka na generator liczb losowych.
// Ten sam seed => ta sama symulacja (powtarzalność Monte Carlo).
class Random {
public:
    explicit Random(std::uint64_t seed);

    double uniform();                 // rozkład jednostajny [0, 1)
    double normal();                  // rozkład normalny N(0, 1)
    int poisson(double lambda);       // liczba skoków w kroku
    int uniformInt(int lo, int hi);   // liczba całkowita z [lo, hi]
    std::uint64_t seed() const noexcept { return seed_; }

private:
    std::uint64_t seed_;
    std::mt19937_64 engine_;
};

}  // namespace mcx
