#include <catch2/catch.hpp>
#include <ctrail/MonotonicValuesStorage.h>

using namespace ctrail;
using std::chrono::system_clock;

#define PREFIX "MonotonicValuesStorage "
TEST_CASE(PREFIX "provides the same set of counters names")
{
    MonotonicValuesStorage storage({"a", "b", "c"});

    CHECK(storage.countersNumber() == 3);

    CHECK(storage.counterName(0) == "a");
    CHECK(storage.counterName(1) == "b");
    CHECK(storage.counterName(2) == "c");
}

TEST_CASE(PREFIX "provides correct views")
{
    MonotonicValuesStorage storage({"a", "b", "c"});

    std::vector<std::int64_t> v1{10, 11, 12};
    auto tp1 = system_clock::time_point{system_clock::duration{1}};
    storage.addValues(tp1, v1.data(), v1.size());

    std::vector<std::int64_t> v2{20, 21, 22};
    auto tp2 = system_clock::time_point{system_clock::duration{2}};
    storage.addValues(tp2, v2.data(), v2.size());

    std::vector<std::int64_t> v3{30, 31, 32};
    auto tp3 = system_clock::time_point{system_clock::duration{3}};
    storage.addValues(tp3, v3.data(), v3.size());

    std::vector<std::int64_t> v4{40, 41, 42};
    auto tp4 = system_clock::time_point{system_clock::duration{4}};
    storage.addValues(tp4, v4.data(), v4.size());

    CHECK(storage.countersNumber() == 3);
    CHECK(storage.timePointsNumber() == 4);

    std::vector<std::int64_t> by_counter(4);
    storage.copyValuesByCounter(0, by_counter.data(), by_counter.size());
    CHECK(by_counter == std::vector<std::int64_t>{10, 20, 30, 40});

    storage.copyValuesByCounter(1, by_counter.data(), by_counter.size());
    CHECK(by_counter == std::vector<std::int64_t>{11, 21, 31, 41});

    storage.copyValuesByCounter(2, by_counter.data(), by_counter.size());
    CHECK(by_counter == std::vector<std::int64_t>{12, 22, 32, 42});

    std::vector<std::int64_t> by_tp(3);
    storage.copyValuesByTimePoint(0, by_tp.data(), by_tp.size());
    CHECK(by_tp == std::vector<std::int64_t>{10, 11, 12});

    storage.copyValuesByTimePoint(1, by_tp.data(), by_tp.size());
    CHECK(by_tp == std::vector<std::int64_t>{20, 21, 22});

    storage.copyValuesByTimePoint(2, by_tp.data(), by_tp.size());
    CHECK(by_tp == std::vector<std::int64_t>{30, 31, 32});

    storage.copyValuesByTimePoint(3, by_tp.data(), by_tp.size());
    CHECK(by_tp == std::vector<std::int64_t>{40, 41, 42});
}

TEST_CASE(PREFIX "supports the edge case of no counters")
{
    MonotonicValuesStorage storage({});
    CHECK(storage.countersNumber() == 0);

    std::int64_t v = 0xDEADBEEF;
    auto tp = system_clock::time_point{system_clock::duration{42}};
    storage.addValues(tp, &v, 0);
    CHECK(storage.timePointsNumber() == 1);
    CHECK(storage.timePoint(0) == tp);

    storage.copyValuesByTimePoint(0, &v, 0);
    CHECK(v == 0xDEADBEEF);
}

TEST_CASE(PREFIX "is virtually unlimited")
{
    MonotonicValuesStorage storage({"a"});
    const size_t number = 1000000;
    const std::int64_t v = std::numeric_limits<std::int64_t>::max();
    const auto tp = system_clock::time_point{system_clock::duration{42}};
    for (auto i = 0; i < number; ++i)
        storage.addValues(tp, &v, 1);

    CHECK(storage.timePointsNumber() == number);

    std::vector<std::int64_t> values(number);
    storage.copyValuesByCounter(0, values.data(), values.size());
    CHECK(std::count(values.begin(), values.end(), v) == number);

    std::fill(values.begin(), values.end(), 0);
    for (auto i = 0; i < number; ++i)
        storage.copyValuesByTimePoint(i, &values[i], 1);
    CHECK(std::count(values.begin(), values.end(), v) == number);
}
