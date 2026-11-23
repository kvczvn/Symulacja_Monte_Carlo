#pragma once
#include <iosfwd>
#include <map>
#include <string>

namespace mcx {

class Market;

// ENKAPSULACJA: gotówka i pozycje są prywatne, zmieniać je można tylko przez buy()/sell(),
// które pilnują reguł (brak debetu, brak krótkiej sprzedaży).
class Portfolio {
public:
    explicit Portfolio(double initialCash);

    void buy(const std::string& ticker, int quantity, double price, double commissionRate);
    void sell(const std::string& ticker, int quantity, double price, double commissionRate);

    double cash() const noexcept { return cash_; }
    double initialCash() const noexcept { return initialCash_; }
    int shares(const std::string& ticker) const;
    const std::map<std::string, int>& positions() const noexcept { return positions_; }
    double totalValue(const Market& market) const;

private:
    double initialCash_;
    double cash_;
    std::map<std::string, int> positions_;
};

std::ostream& operator<<(std::ostream& os, const Portfolio& p);

}  // namespace mcx
