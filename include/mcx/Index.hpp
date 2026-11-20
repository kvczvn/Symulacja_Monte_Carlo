#pragma once
#include <vector>
#include "mcx/Instrument.hpp"
#include "mcx/Stock.hpp"

namespace mcx {

// AGREGACJA: indeks nie jest właścicielem spółek, tylko je obserwuje (surowe, nieposiadające wskaźniki).
// Wartość bazowa = 100 w chwili utworzenia.
class Index final : public Instrument {
public:
    Index(std::string ticker, std::vector<const Stock*> constituents);

    double price() const override;
    void update(double, Random&) override {}   // wartość wynika ze spółek, nic nie losujemy
    std::string describe() const override;

private:
    std::vector<const Stock*> constituents_;
    std::vector<double> basePrices_;
};

}  // namespace mcx
