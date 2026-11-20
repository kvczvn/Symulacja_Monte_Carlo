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


    JumpDiffusionModel::JumpDiffusionModel(double mu, double sigma, double jumpIntensity, double jumpMean, double jumpSigma)
        : GeometricBrownianMotion(mu, sigma), lambda_(jumpIntensity), jumpMean_(jumpMean), jumpSigma_(jumpSigma) {
        if (jumpIntensity < 0.0 || jumpSigma < 0.0) throw ConfigError("Jump: zle parametry skokow");
    }

    double JumpDiffusionModel::nextPrice(double current, double dt, Random& rng) const {
        double price = GeometricBrownianMotion::nextPrice(current, dt, rng);  // czesc bazowa
        const int jumps = rng.poisson(lambda_ * dt);
        for (int i = 0; i < jumps; ++i) price *= std::exp(jumpMean_ + jumpSigma_ * rng.normal());
        return price;
    }



}
