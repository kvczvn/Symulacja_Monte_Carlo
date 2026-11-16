#include "mcx/Random.hpp"
#include <stdexcept>

namespace mcx {

Random::Random(std::uint64_t seed) : seed_(seed), engine_(seed) {}

double Random::uniform() {
    std::uniform_real_distribution<double> d(0.0, 1.0);
    return d(engine_);
}

double Random::normal() {
    std::normal_distribution<double> d(0.0, 1.0);
    return d(engine_);
}

int Random::poisson(double lambda) {
    if (lambda <= 0.0) return 0;
    std::poisson_distribution<int> d(lambda);
    return d(engine_);
}

int Random::uniformInt(int lo, int hi) {
    if (lo > hi) throw std::invalid_argument("Random::uniformInt: lo > hi");
    std::uniform_int_distribution<int> d(lo, hi);
    return d(engine_);
}

}  // namespace mcx
