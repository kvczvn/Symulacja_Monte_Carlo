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

class GeometricBrownianMotion : public PriceModel {
public:
    GeometricBrownianMotion(double mu, double sigma);
    double nextPrice(double current, double dt, Random& rng) const override;
    std::string name() const override { return "GBM"; }

protected:  // klasy pochodne mają dostęp do parametrów
    double mu_;
    double sigma_;
};

class JumpDiffusionModel : public GeometricBrownianMotion {
public:
    JumpDiffusionModel(double mu, double sigma, double jumpIntensity, double jumpMean, double jumpSigma);
    double nextPrice(double current, double dt, Random& rng) const override;
    std::string name() const override { return "Jump"; }

private:
    double lambda_;      // średnia liczba skoków na rok
    double jumpMean_;    // średni log-rozmiar skoku
    double jumpSigma_;   // rozrzut skoku
};


class MeanRevertingModel : public PriceModel {
public:
    MeanRevertingModel(double kappa, double longRunPrice, double sigma);
    double nextPrice(double current, double dt, Random& rng) const override;
    std::string name() const override { return "MeanRev"; }

private:
    double kappa_;
    double theta_;
    double sigma_;
};

}