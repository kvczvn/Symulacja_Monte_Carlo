#include "mcx/Trader.hpp"
#include <cmath>
#include "mcx/Exceptions.hpp"

namespace mcx {

Trader::Trader(std::string name, double initialCash) : name_(std::move(name)), portfolio_(initialCash) {}

double Trader::sma(const RingBuffer<double>& h, std::size_t window) {
    if (window == 0 || h.size() < window) return h.empty() ? 0.0 : h.back();
    double sum = 0.0;
    for (std::size_t i = h.size() - window; i < h.size(); ++i) sum += h[i];
    return sum / static_cast<double>(window);
}

int Trader::quantityForBudget(double budget, double price) {
    if (budget <= 0.0 || price <= 0.0) return 0;
    return static_cast<int>(std::floor(budget / (price * 1.01)));   // 1% zapasu na prowizję
}


std::vector<Order> BuyAndHoldTrader::decide(const Market& market, Random&) {
    std::vector<Order> orders;
    if (invested_) return orders;
    invested_ = true;
    const auto stocks = market.stocks();
    const double perStock = portfolio().cash() / static_cast<double>(stocks.size());
    for (const Stock* s : stocks) {
        const int q = quantityForBudget(perStock, s->price());
        if (q > 0) orders.push_back({s->ticker(), Side::Buy, q});
    }
    return orders;
}

std::vector<Order> RandomTrader::decide(const Market& market, Random& rng) {
    std::vector<Order> orders;
    if (rng.uniform() > 0.3) return orders;
    const auto stocks = market.stocks();
    const Stock* s = stocks[static_cast<std::size_t>(rng.uniformInt(0, static_cast<int>(stocks.size()) - 1))];
    if (rng.uniform() < 0.5) {
        const int q = quantityForBudget(portfolio().cash() * 0.10, s->price());
        if (q > 0) orders.push_back({s->ticker(), Side::Buy, q});
    } else {
        const int held = portfolio().shares(s->ticker());
        if (held > 0) orders.push_back({s->ticker(), Side::Sell, rng.uniformInt(1, held)});
    }
    return orders;
}

}