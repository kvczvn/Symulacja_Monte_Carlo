// Minimalny framework testowy (bez zewnętrznych bibliotek).
#include <atomic>
#include <cmath>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "mcx/Exceptions.hpp"
#include "mcx/RingBuffer.hpp"
#include "mcx/PriceModel.hpp"
#include "mcx/Portfolio.hpp"
#include "mcx/Market.hpp"
#include "mcx/MarketEvent.hpp"
#include "mcx/Factory.hpp"

using namespace mcx;

static int g_failed = 0;
static int g_checks = 0;

#define CHECK(cond)                                                                   \
    do {                                                                              \
        ++g_checks;                                                                   \
        if (!(cond)) {                                                                \
            ++g_failed;                                                               \
            std::cerr << "  FAIL " << __FILE__ << ":" << __LINE__ << "  " #cond "\n"; \
        }                                                                             \
    } while (0)

#define CHECK_THROWS(expr, ExType)                       \
    do {                                                 \
        ++g_checks;                                      \
        bool thrown = false;                             \
        try { expr; } catch (const ExType&) { thrown = true; } \
        if (!thrown) {                                   \
            ++g_failed;                                  \
            std::cerr << "  FAIL " << __FILE__ << ":" << __LINE__ << "  expected " #ExType "\n"; \
        }                                                \
    } while (0)

struct TestCase { std::string name; std::function<void()> fn; };
static std::vector<TestCase>& registry() { static std::vector<TestCase> r; return r; }
struct Registrar { Registrar(std::string n, std::function<void()> f) { registry().push_back({std::move(n), std::move(f)}); } };
#define TEST(name) \
    static void name(); \
    static Registrar reg_##name(#name, name); \
    static void name()

TEST(ringbuffer_overwrites_oldest) {
    RingBuffer<int> rb(3);
    for (int i = 1; i <= 5; ++i) rb.push(i);
    CHECK(rb.size() == 3);
    CHECK(rb[0] == 3 && rb[1] == 4 && rb[2] == 5);
    CHECK(rb.back() == 5 && rb.front() == 3);
    CHECK_THROWS(rb.at(3), std::out_of_range);
    CHECK_THROWS(RingBuffer<int>(0), std::invalid_argument);
}

TEST(ringbuffer_works_with_other_types) {
    RingBuffer<std::string> rb(2);
    rb.push("a"); rb.push("b"); rb.push("c");
    CHECK(rb.toVector() == (std::vector<std::string>{"b", "c"}));
}

TEST(gbm_matches_theoretical_mean) {
    // E[S_T] = S0 * exp(mu*T). Jeden krok dt = 1 rok, 40 000 prób.
    GeometricBrownianMotion m(0.10, 0.20);
    Random rng(7);
    double sum = 0.0;
    const int n = 40000;
    for (int i = 0; i < n; ++i) sum += m.nextPrice(100.0, 1.0, rng);
    CHECK(std::abs(sum / n - 100.0 * std::exp(0.10)) < 1.0);
}

TEST(all_models_stay_positive_via_base_pointer) {
    std::vector<std::unique_ptr<PriceModel>> models;
    models.push_back(std::make_unique<GeometricBrownianMotion>(0.1, 0.5));
    models.push_back(std::make_unique<JumpDiffusionModel>(0.1, 0.3, 10.0, -0.05, 0.2));
    models.push_back(std::make_unique<MeanRevertingModel>(3.0, 50.0, 0.4));
    Random rng(1);
    for (const auto& m : models) {
        double p = 50.0;
        for (int i = 0; i < 2000; ++i) p = m->nextPrice(p, 1.0 / 252.0, rng);
        CHECK(p > 0.0 && std::isfinite(p));
        CHECK(!m->name().empty());
    }
}

TEST(mean_reverting_is_pulled_to_theta) {
    MeanRevertingModel m(8.0, 100.0, 0.0);   // sigma=0 => deterministycznie
    Random rng(1);
    double p = 50.0;
    for (int i = 0; i < 2000; ++i) p = m.nextPrice(p, 1.0 / 252.0, rng);
    CHECK(std::abs(p - 100.0) < 1.0);
}

TEST(models_reject_bad_parameters) {
    CHECK_THROWS(GeometricBrownianMotion(0.1, -0.2), ConfigError);
    CHECK_THROWS(MeanRevertingModel(1.0, 0.0, 0.1), ConfigError);
}

TEST(portfolio_buy_sell_rules) {
    Portfolio p(1000.0);
    p.buy("ALFA", 5, 100.0, 0.0);
    CHECK(p.shares("ALFA") == 5);
    CHECK(std::abs(p.cash() - 500.0) < 1e-9);
    CHECK_THROWS(p.buy("ALFA", 10, 100.0, 0.0), InsufficientFundsError);
    CHECK_THROWS(p.sell("ALFA", 6, 100.0, 0.0), InsufficientSharesError);
    CHECK_THROWS(p.sell("BETA", 1, 10.0, 0.0), InsufficientSharesError);
    CHECK_THROWS(p.buy("ALFA", 0, 100.0, 0.0), InvalidOrderError);
    p.sell("ALFA", 5, 110.0, 0.0);
    CHECK(p.shares("ALFA") == 0);
    CHECK(p.positions().empty());
    CHECK(std::abs(p.cash() - 1050.0) < 1e-9);
}

TEST(portfolio_commission_is_charged) {
    Portfolio p(1000.0);
    p.buy("X", 1, 100.0, 0.01);
    CHECK(std::abs(p.cash() - (1000.0 - 101.0)) < 1e-9);
}

TEST(market_lookup_and_duplicates) {
    Market m;
    m.addStock(std::make_unique<Stock>("AAA", 10.0, std::make_unique<GeometricBrownianMotion>(0.1, 0.2)));
    CHECK(m.hasStock("AAA"));
    CHECK_THROWS(m.stock("ZZZ"), UnknownInstrumentError);
    CHECK_THROWS(m.addStock(std::make_unique<Stock>("AAA", 5.0, std::make_unique<GeometricBrownianMotion>(0.1, 0.2))),
                 ConfigError);
}

TEST(index_starts_at_100) {
    auto sim = buildSimulation(SimulationConfig::defaults(), 1);
    CHECK(std::abs(sim->market().index()->price() - 100.0) < 1e-9);
}

TEST(same_seed_gives_same_simulation) {
    auto a = buildSimulation(SimulationConfig::defaults(), 99);
    auto b = buildSimulation(SimulationConfig::defaults(), 99);
    auto c = buildSimulation(SimulationConfig::defaults(), 100);
    a->run(100); b->run(100); c->run(100);
    CHECK(a->market().stock("ALFA").price() == b->market().stock("ALFA").price());
    CHECK(a->market().stock("ALFA").price() != c->market().stock("ALFA").price());
}

TEST(trader_value_is_never_negative_and_accounting_holds) {
    auto sim = buildSimulation(SimulationConfig::defaults(), 5);
    sim->run(250);
    for (const auto& t : sim->traders()) {
        CHECK(t->portfolio().cash() >= -1e-6);
        CHECK(t->portfolio().totalValue(sim->market()) > 0.0);
    }
}

TEST(observer_is_notified_every_tick) {
    struct Counter : SimulationObserver {
        int ticks = 0; bool finished = false;
        void onTick(const Simulation&) override { ++ticks; }
        void onFinish(const Simulation&) override { finished = true; }
    };
    auto sim = buildSimulation(SimulationConfig::defaults(), 3);
    auto counter = std::make_shared<Counter>();
    sim->attach(counter);
    sim->run(17);
    CHECK(counter->ticks == 17);
    CHECK(counter->finished);
}

TEST(crash_event_lowers_prices) {
    Market m;
    m.addStock(std::make_unique<Stock>("AAA", 100.0, std::make_unique<GeometricBrownianMotion>(0.0, 0.0)));
    CrashEvent crash(1.0);
    Random rng(1);
    crash.apply(m, rng);
    CHECK(m.stock("AAA").price() < 96.0);
    CHECK(m.stock("AAA").history().back() == m.stock("AAA").price());
}

TEST(factory_rejects_unknown_model) {
    StockSpec bad{"X", 10.0, "magic", {}};
    CHECK_THROWS(createModel(bad), ConfigError);
}


int main() {
    for (const auto& t : registry()) {
        const int before = g_failed;
        t.fn();
        std::cout << (g_failed == before ? "[ OK ] " : "[FAIL] ") << t.name << '\n';
    }
    std::cout << "\nSprawdzen: " << g_checks << ", bledow: " << g_failed << '\n';
    return g_failed == 0 ? 0 : 1;
}
