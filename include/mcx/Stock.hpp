#pragma once
#include <iosfwd>
#include <memory>
#include "mcx/Instrument.hpp"
#include "mcx/PriceModel.hpp"
#include "mcx/RingBuffer.hpp"

namespace mcx {

// KOMPOZYCJA: Stock "ma" model ceny (unique_ptr = wyłączna własność, RAII).
class Stock final : public Instrument {
public:
    Stock(std::string ticker, double initialPrice, std::unique_ptr<PriceModel> model,
          std::size_t historySize = 120);

    double price() const override { return price_; }
    void update(double dt, Random& rng) override;
    std::string describe() const override;

    void applyShock(double factor);          // np. krach: factor = 0.9 => -10%

    double initialPrice() const noexcept { return initialPrice_; }
    double previousPrice() const noexcept { return previous_; }
    double changePct() const noexcept;       // zmiana względem poprzedniego kroku, w %
    const RingBuffer<double>& history() const noexcept { return history_; }
    const PriceModel& model() const noexcept { return *model_; }

private:
    double initialPrice_;
    double price_;
    double previous_;
    std::unique_ptr<PriceModel> model_;
    RingBuffer<double> history_;
};

std::ostream& operator<<(std::ostream& os, const Stock& s);  // PRZECIĄŻANIE OPERATORA

}  // namespace mcx
