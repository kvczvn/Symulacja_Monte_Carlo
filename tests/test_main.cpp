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

int main() {
    for (const auto& t : registry()) {
        const int before = g_failed;
        t.fn();
        std::cout << (g_failed == before ? "[ OK ] " : "[FAIL] ") << t.name << '\n';
    }
    std::cout << "\nSprawdzen: " << g_checks << ", bledow: " << g_failed << '\n';
    return g_failed == 0 ? 0 : 1;
}
