#include "mcx/PriceModel.hpp"
#include <cmath>
#include "mcx/Exceptions.hpp"

namespace mcx {

    GeometricBrownianMotion::GeometricBrownianMotion(double mu, double sigma) : mu_(mu), sigma_(sigma) {
        if (sigma < 0.0) throw ConfigError("GBM: sigma musi byc >= 0");
    }

    double GeometricBrownianMotion::nextPrice(double current, double dt, Random& rng) const {
        const double drift = (mu_ - 0.5 * sigma_ * sigma_) * dt;
        const double diffusion = sigma_ * std::sqrt(dt) * rng.normal();
        return current * std::exp(drift + diffusion);
    }




}
