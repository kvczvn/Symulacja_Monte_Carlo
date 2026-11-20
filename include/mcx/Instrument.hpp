#pragma once
#include <string>
#include <utility>
#include "mcx/Random.hpp"

namespace mcx {

// Klasa abstrakcyjna: wszystko, co ma ticker i cenę (akcja, indeks...).
// Kopiowanie zabronione - instrument ma jedną tożsamość na rynku.
class Instrument {
public:
    explicit Instrument(std::string ticker) : ticker_(std::move(ticker)) {}
    virtual ~Instrument() = default;
    Instrument(const Instrument&) = delete;
    Instrument& operator=(const Instrument&) = delete;

    const std::string& ticker() const noexcept { return ticker_; }

    virtual double price() const = 0;
    virtual void update(double dt, Random& rng) = 0;
    virtual std::string describe() const = 0;

private:
    std::string ticker_;
};

}  // namespace mcx
