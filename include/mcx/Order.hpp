#pragma once
#include <string>

namespace mcx {

enum class Side { Buy, Sell };

struct Order {
    std::string ticker;
    Side side;
    int quantity;
};

}  // namespace mcx
