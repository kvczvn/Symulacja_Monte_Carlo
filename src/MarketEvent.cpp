#include "mcx/MarketEvent.hpp"
#include <iomanip>
#include <sstream>
#include "mcx/Market.hpp"

namespace mcx {

namespace {
std::string describeAverage(const char* verb, double sumPct, int n) {
    std::ostringstream os;
    os << verb << " srednio o " << std::fixed << std::setprecision(1) << (n ? sumPct / n : 0.0) << "%";
    return os.str();
}
}  // namespace

std::string CrashEvent::apply(Market& market, Random& rng) {
    double sum = 0.0;
    int n = 0;
    market.forEachStock([&](Stock& s) {
        const double factor = 1.0 - (0.05 + 0.10 * rng.uniform());   // -5% .. -15%
        s.applyShock(factor);
        sum += (factor - 1.0) * 100.0;
        ++n;
    });
    return describeAverage("wszystkie spolki spadaja", sum, n);
}

std::string RallyEvent::apply(Market& market, Random& rng) {
    double sum = 0.0;
    int n = 0;
    market.forEachStock([&](Stock& s) {
        const double factor = 1.0 + (0.03 + 0.07 * rng.uniform());   // +3% .. +10%
        s.applyShock(factor);
        sum += (factor - 1.0) * 100.0;
        ++n;
    });
    return describeAverage("wszystkie spolki rosna", sum, n);
}

}  // namespace mcx
