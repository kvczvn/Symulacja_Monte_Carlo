#pragma once
#include <deque>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "mcx/Index.hpp"
#include "mcx/MarketEvent.hpp"
#include "mcx/Random.hpp"
#include "mcx/Stock.hpp"

namespace mcx {

// Rynek: właściciel spółek, indeksu i zdarzeń (kompozycja).
// Brak kopiowania (unique_ptr), jest przenoszenie.
class Market {
public:
    Market() = default;
    Market(Market&&) noexcept = default;
    Market& operator=(Market&&) noexcept = default;
    Market(const Market&) = delete;
    Market& operator=(const Market&) = delete;

    void addStock(std::unique_ptr<Stock> stock);
    void addEvent(std::unique_ptr<MarketEvent> event);
    void buildIndex(std::string ticker);

    void step(double dt, Random& rng);   // jeden dzień sesji

    bool hasStock(const std::string& ticker) const;
    const Stock& stock(const std::string& ticker) const;     // rzuca UnknownInstrumentError
    std::vector<const Stock*> stocks() const;
    const Index* index() const noexcept { return index_.get(); }
    void forEachStock(const std::function<void(Stock&)>& fn);

    std::size_t tick() const noexcept { return tick_; }
    const std::deque<std::string>& recentEvents() const noexcept { return eventLog_; }

private:
    std::vector<std::unique_ptr<Stock>> stocks_;
    std::unordered_map<std::string, Stock*> byTicker_;
    std::unique_ptr<Index> index_;
    std::vector<std::unique_ptr<MarketEvent>> events_;
    std::deque<std::string> eventLog_;
    std::size_t tick_ = 0;
};

}  // namespace mcx
