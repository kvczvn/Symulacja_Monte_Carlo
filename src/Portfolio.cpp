#include "mcx/Portfolio.hpp"
#include <iomanip>
#include <ostream>
#include "mcx/Exceptions.hpp"
#include "mcx/Market.hpp"

namespace mcx {

Portfolio::Portfolio(double initialCash) : initialCash_(initialCash), cash_(initialCash) {
    if (initialCash < 0.0) throw ConfigError("Portfolio: ujemna gotowka");
}

void Portfolio::buy(const std::string& ticker, int quantity, double price, double commissionRate) {
    if (quantity <= 0 || price <= 0.0) throw InvalidOrderError("Zla ilosc lub cena zlecenia kupna");
    const double cost = quantity * price * (1.0 + commissionRate);
    if (cost > cash_ + 1e-9) throw InsufficientFundsError("Brak gotowki na zakup " + ticker);
    cash_ -= cost;
    positions_[ticker] += quantity;
}

void Portfolio::sell(const std::string& ticker, int quantity, double price, double commissionRate) {
    if (quantity <= 0 || price <= 0.0) throw InvalidOrderError("Zla ilosc lub cena zlecenia sprzedazy");
    auto it = positions_.find(ticker);
    const int held = (it == positions_.end()) ? 0 : it->second;
    if (held < quantity) throw InsufficientSharesError("Brak akcji do sprzedazy: " + ticker);
    cash_ += quantity * price * (1.0 - commissionRate);
    it->second -= quantity;
    if (it->second == 0) positions_.erase(it);
}

int Portfolio::shares(const std::string& ticker) const {
    auto it = positions_.find(ticker);
    return it == positions_.end() ? 0 : it->second;
}

double Portfolio::totalValue(const Market& market) const {
    double v = cash_;
    for (const auto& [ticker, qty] : positions_) v += qty * market.stock(ticker).price();
    return v;
}

std::ostream& operator<<(std::ostream& os, const Portfolio& p) {
    os << "gotowka=" << std::fixed << std::setprecision(2) << p.cash();
    for (const auto& [ticker, qty] : p.positions()) os << " " << ticker << ":" << qty;
    return os;
}

}  // namespace mcx
