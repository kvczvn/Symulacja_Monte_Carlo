#pragma once
#include <stdexcept>
#include <string>

namespace mcx {

// Wspólna baza wszystkich błędów symulatora.
class SimulationError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class ConfigError : public SimulationError {
public:
    using SimulationError::SimulationError;
};

class UnknownInstrumentError : public SimulationError {
public:
    using SimulationError::SimulationError;
};

class InvalidOrderError : public SimulationError {
public:
    using SimulationError::SimulationError;
};

class InsufficientFundsError : public SimulationError {
public:
    using SimulationError::SimulationError;
};

class InsufficientSharesError : public SimulationError {
public:
    using SimulationError::SimulationError;
};

}  // namespace mcx
