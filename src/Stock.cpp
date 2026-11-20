#include "mcx/Stock.hpp"
#include <algorithm>
#include <iomanip>
#include <ostream>
#include <sstream>
#include "mcx/Exceptions.hpp"

namespace mcx {

namespace { constexpr double kMinPrice = 0.01; }

Stock::Stock(std::string ticker, double initialPrice, std::unique_ptr<PriceModel> model, std::size_t historySize)
    : Instrument(std::move(ticker)),
      initialPrice_(initialPrice),
      price_(initialPrice),
      previous_(initialPrice),
      model_(std::move(model)),
      history_(historySize) {
    if (initialPrice <= 0.0) throw ConfigError("Stock: cena poczatkowa musi byc > 0");
    if (!model_) throw ConfigError("Stock: brak modelu ceny");
    history_.push(price_);
}

void Stock::update(double dt, Random& rng) {
    previous_ = price_;
    price_ = std::max(kMinPrice, model_->nextPrice(price_, dt, rng));
    history_.push(price_);
}

void Stock::applyShock(double factor) {
    if (factor <= 0.0) throw InvalidOrderError("Stock::applyShock: factor must be > 0");
    price_ = std::max(kMinPrice, price_ * factor);
    history_.back() = price_;   // szok widoczny od razu na wykresie
}

double Stock::changePct() const noexcept {
    return previous_ > 0.0 ? (price_ / previous_ - 1.0) * 100.0 : 0.0;
}

std::string Stock::describe() const {
    std::ostringstream os;
    os << ticker() << " @ " << std::fixed << std::setprecision(2) << price_ << " [" << model_->name() << "]";
    return os.str();
}

std::ostream& operator<<(std::ostream& os, const Stock& s) { return os << s.describe(); }

}  // namespace mcx
