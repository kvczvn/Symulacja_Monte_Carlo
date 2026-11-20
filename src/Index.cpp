#include "mcx/Index.hpp"
#include <iomanip>
#include <sstream>
#include "mcx/Exceptions.hpp"

namespace mcx {

Index::Index(std::string ticker, std::vector<const Stock*> constituents)
    : Instrument(std::move(ticker)), constituents_(std::move(constituents)) {
    if (constituents_.empty()) throw ConfigError("Index: brak skladnikow");
    for (const Stock* s : constituents_) basePrices_.push_back(s->price());
}

double Index::price() const {
    double sum = 0.0;
    for (std::size_t i = 0; i < constituents_.size(); ++i) sum += constituents_[i]->price() / basePrices_[i];
    return 100.0 * sum / static_cast<double>(constituents_.size());
}

std::string Index::describe() const {
    std::ostringstream os;
    os << "Index " << ticker() << " = " << std::fixed << std::setprecision(2) << price();
    return os.str();
}

}  // namespace mcx
