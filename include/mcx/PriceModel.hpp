#pragma once
#include <string>
#include "mcx/Random.hpp"

namespace mcx {


class PriceModel {
public:
    virtual ~PriceModel() = default;
    virtual double nextPrice(double current, double dt, Random& rng) const = 0;
    virtual std::string name() const = 0;
};

// Geometryczny ruch Browna: dS = mu*S*dt + sigma*S*dW
class GeometricBrownianMotion : public PriceModel {
public:
    GeometricBrownianMotion(double mu, double sigma);
    double nextPrice(double current, double dt, Random& rng) const override;
    std::string name() const override { return "GBM"; }

protected:  // klasy pochodne mają dostęp do parametrów
    double mu_;
    double sigma_;
};



}