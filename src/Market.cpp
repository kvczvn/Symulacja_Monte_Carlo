#include "mcx/Market.hpp"
#include "mcx/Exceptions.hpp"

namespace mcx {

namespace { constexpr std::size_t kEventLogLimit = 50; }

void Market::addStock(std::unique_ptr<Stock> stock) {
    if (!stock) throw ConfigError("Market::addStock: null");
    if (byTicker_.count(stock->ticker())) throw ConfigError("Duplikat tickera: " + stock->ticker());
    byTicker_[stock->ticker()] = stock.get();
    stocks_.push_back(std::move(stock));
}

void Market::addEvent(std::unique_ptr<MarketEvent> event) {
    if (!event) throw ConfigError("Market::addEvent: null");
    events_.push_back(std::move(event));
}

void Market::buildIndex(std::string ticker) { index_ = std::make_unique<Index>(std::move(ticker), stocks()); }

void Market::step(double dt, Random& rng) {
    ++tick_;
    for (auto& s : stocks_) s->update(dt, rng);
    for (auto& e : events_) {
        if (rng.uniform() < e->probabilityPerTick()) {
            const std::string msg = e->apply(*this, rng);
            eventLog_.push_back("[dzien " + std::to_string(tick_) + "] " + e->name() + ": " + msg);
            if (eventLog_.size() > kEventLogLimit) eventLog_.pop_front();
        }
    }
}

bool Market::hasStock(const std::string& ticker) const { return byTicker_.count(ticker) > 0; }

const Stock& Market::stock(const std::string& ticker) const {
    auto it = byTicker_.find(ticker);
    if (it == byTicker_.end()) throw UnknownInstrumentError("Nieznany ticker: " + ticker);
    return *it->second;
}

std::vector<const Stock*> Market::stocks() const {
    std::vector<const Stock*> out;
    out.reserve(stocks_.size());
    for (const auto& s : stocks_) out.push_back(s.get());
    return out;
}

void Market::forEachStock(const std::function<void(Stock&)>& fn) {
    for (auto& s : stocks_) fn(*s);
}

}  // namespace mcx
