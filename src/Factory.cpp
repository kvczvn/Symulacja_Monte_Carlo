#include "mcx/Factory.hpp"
#include "mcx/Exceptions.hpp"

namespace mcx {

SimulationConfig SimulationConfig::defaults() {
    SimulationConfig c;
    c.stocks = {
        {"ALFA", 100.0, "gbm",     {{"mu", 0.08}, {"sigma", 0.20}}},
        {"BETA", 50.0,  "gbm",     {{"mu", 0.12}, {"sigma", 0.35}}},
        {"GAMA", 200.0, "jump",    {{"mu", 0.10}, {"sigma", 0.25}, {"lambda", 6.0}, {"jumpMean", -0.02}, {"jumpSigma", 0.08}}},
        {"DELT", 75.0,  "meanrev", {{"kappa", 4.0}, {"theta", 75.0}, {"sigma", 0.30}}},
        {"OMEG", 30.0,  "gbm",     {{"mu", 0.15}, {"sigma", 0.50}}},
    };
    return c;
}

std::unique_ptr<PriceModel> createModel(const StockSpec& s) {
    auto get = [&](const std::string& key, double fallback) {
        auto it = s.params.find(key);
        return it == s.params.end() ? fallback : it->second;
    };
    if (s.model == "gbm") return std::make_unique<GeometricBrownianMotion>(get("mu", 0.08), get("sigma", 0.2));
    if (s.model == "jump")
        return std::make_unique<JumpDiffusionModel>(get("mu", 0.08), get("sigma", 0.2), get("lambda", 3.0),
                                                    get("jumpMean", -0.02), get("jumpSigma", 0.08));
    if (s.model == "meanrev")
        return std::make_unique<MeanRevertingModel>(get("kappa", 3.0), get("theta", s.initialPrice),
                                                    get("sigma", 0.3));
    throw ConfigError("Nieznany model ceny: " + s.model);
}

std::unique_ptr<Simulation> buildSimulation(const SimulationConfig& cfg, std::uint64_t seed) {
    if (cfg.stocks.empty()) throw ConfigError("Konfiguracja bez spolek");

    Market market;
    for (const StockSpec& spec : cfg.stocks)
        market.addStock(std::make_unique<Stock>(spec.ticker, spec.initialPrice, createModel(spec), cfg.historySize));
    market.buildIndex("MCX5");
    if (cfg.enableEvents) {
        market.addEvent(std::make_unique<CrashEvent>());
        market.addEvent(std::make_unique<RallyEvent>());
    }

    auto sim = std::make_unique<Simulation>(std::move(market), cfg.dt, seed, cfg.commission);
    sim->addTrader(std::make_unique<BuyAndHoldTrader>("Ala [BuyAndHold]", cfg.initialCash));
    sim->addTrader(std::make_unique<RandomTrader>("Bartek [Random]", cfg.initialCash));
    sim->addTrader(std::make_unique<MomentumTrader>("Celina [Momentum]", cfg.initialCash));
    sim->addTrader(std::make_unique<MeanReversionTrader>("Darek [MeanRev]", cfg.initialCash));
    return sim;
}

}  // namespace mcx
