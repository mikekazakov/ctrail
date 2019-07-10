#include <catch2/catch.hpp>
#include <array>
#include <ctrail/CyclicValuesStorage.h>

using namespace ctrail;
using std::chrono::system_clock;
using time_point = std::chrono::system_clock::time_point;

#define PREFIX "CyclicValuesStorage "
TEST_CASE(PREFIX "constructor throws on zero capacity")
{
    CHECK_THROWS(CyclicValuesStorage({}, 0));
}

TEST_CASE(PREFIX "provides the same set of counters names")
{
    ValuesStorage storage{CyclicValuesStorage({"a", "b", "c"}, 5)};

    CHECK(storage.countersNumber() == 3);

    CHECK(storage.counterName(0) == "a");
    CHECK(storage.counterName(1) == "b");
    CHECK(storage.counterName(2) == "c");
}

TEST_CASE(PREFIX "is empty by default")
{
    ValuesStorage storage{CyclicValuesStorage({"a", "b", "c"}, 5)};
    CHECK(storage.timePointsNumber() == 0);
}

TEST_CASE(PREFIX "works with no counters")
{
    ValuesStorage storage{CyclicValuesStorage({}, 5)};
    CHECK(storage.countersNumber() == 0);
    CHECK(storage.timePointsNumber() == 0);
    std::int64_t v = 0;
    auto tp1 = time_point{system_clock::duration{1}};
    storage.addValues(tp1, &v, 0);
    REQUIRE(storage.timePointsNumber() == 1);
    REQUIRE(storage.timePoint(0) == tp1);
    std::int64_t s = -1;
    storage.copyValuesByTimePoint(0, &s, 0);
    CHECK(s == -1);
}

TEST_CASE(PREFIX "stores values for a single counter")
{
    using i64 = std::int64_t;
    SECTION("Capacity = 1")
    {
        ValuesStorage storage{CyclicValuesStorage({"a"}, 1)};
        REQUIRE(storage.timePointsNumber() == 0);
        auto sink = std::array{i64(0)};

        auto v1 = std::array{i64(10)};
        auto tp1 = time_point{system_clock::duration{1}};
        storage.addValues(tp1, v1.data(), 1);
        REQUIRE(storage.timePointsNumber() == 1);
        REQUIRE(storage.timePoint(0) == tp1);
        storage.copyValuesByTimePoint(0, sink.data(), 1);
        REQUIRE(sink == v1);
        sink.fill(0);
        storage.copyValuesByCounter(0, sink.data(), 1);
        REQUIRE(sink == v1);

        auto v2 = std::array{i64(20)};
        auto tp2 = time_point{system_clock::duration{2}};
        storage.addValues(tp2, v2.data(), 1);
        REQUIRE(storage.timePointsNumber() == 1);
        REQUIRE(storage.timePoint(0) == tp2);
        storage.copyValuesByTimePoint(0, sink.data(), 1);
        REQUIRE(sink == v2);
        sink.fill(0);
        storage.copyValuesByCounter(0, sink.data(), 1);
        REQUIRE(sink == v2);

        auto v3 = std::array{i64(30)};
        auto tp3 = time_point{system_clock::duration{3}};
        storage.addValues(tp3, v3.data(), 1);
        REQUIRE(storage.timePointsNumber() == 1);
        REQUIRE(storage.timePoint(0) == tp3);
        storage.copyValuesByTimePoint(0, sink.data(), 1);
        REQUIRE(sink == v3);
        sink.fill(0);
        storage.copyValuesByCounter(0, sink.data(), 1);
        REQUIRE(sink == v3);
    }
    SECTION("Capacity = 2")
    {
        ValuesStorage storage{CyclicValuesStorage({"a"}, 2)};
        REQUIRE(storage.timePointsNumber() == 0);
        auto sink_1 = std::array{i64(0)};
        auto sink_2 = std::array{i64(0), i64(0)};

        auto v1 = std::array{i64(10)};
        auto tp1 = time_point{system_clock::duration{1}};
        storage.addValues(tp1, v1.data(), 1);
        REQUIRE(storage.timePointsNumber() == 1);
        REQUIRE(storage.timePoint(0) == tp1);
        storage.copyValuesByTimePoint(0, sink_1.data(), 1);
        REQUIRE(sink_1 == v1);
        sink_1.fill(0);
        storage.copyValuesByCounter(0, sink_1.data(), 1);
        REQUIRE(sink_1 == std::array{i64(10)});

        auto v2 = std::array{i64(20)};
        auto tp2 = time_point{system_clock::duration{2}};
        storage.addValues(tp2, v2.data(), 1);
        REQUIRE(storage.timePointsNumber() == 2);
        REQUIRE(storage.timePoint(0) == tp1);
        REQUIRE(storage.timePoint(1) == tp2);
        storage.copyValuesByTimePoint(0, sink_1.data(), 1);
        REQUIRE(sink_1 == v1);
        storage.copyValuesByTimePoint(1, sink_1.data(), 1);
        REQUIRE(sink_1 == v2);
        storage.copyValuesByCounter(0, sink_2.data(), 2);
        REQUIRE(sink_2 == std::array{i64(10), i64(20)});

        auto v3 = std::array{i64(30)};
        auto tp3 = time_point{system_clock::duration{3}};
        storage.addValues(tp3, v3.data(), 1);
        REQUIRE(storage.timePointsNumber() == 2);
        REQUIRE(storage.timePoint(0) == tp2);
        REQUIRE(storage.timePoint(1) == tp3);
        storage.copyValuesByTimePoint(0, sink_1.data(), 1);
        REQUIRE(sink_1 == v2);
        storage.copyValuesByTimePoint(1, sink_1.data(), 1);
        REQUIRE(sink_1 == v3);
        storage.copyValuesByCounter(0, sink_2.data(), 2);
        REQUIRE(sink_2 == std::array{i64(20), i64(30)});

        auto v4 = std::array{i64(40)};
        auto tp4 = time_point{system_clock::duration{4}};
        storage.addValues(tp4, v4.data(), 1);
        REQUIRE(storage.timePointsNumber() == 2);
        REQUIRE(storage.timePoint(0) == tp3);
        REQUIRE(storage.timePoint(1) == tp4);
        storage.copyValuesByTimePoint(0, sink_1.data(), 1);
        REQUIRE(sink_1 == v3);
        storage.copyValuesByTimePoint(1, sink_1.data(), 1);
        REQUIRE(sink_1 == v4);
        storage.copyValuesByCounter(0, sink_2.data(), 2);
        REQUIRE(sink_2 == std::array{i64(30), i64(40)});
    }
}

TEST_CASE(PREFIX "stores values for two counters")
{
    using i64 = std::int64_t;
    SECTION("Capacity = 1")
    {
        ValuesStorage storage{CyclicValuesStorage({"a", "b"}, 1)};
        REQUIRE(storage.timePointsNumber() == 0);
        auto sink_1 = std::array{i64(0)};
        auto sink_2 = std::array{i64(0), i64(0)};

        auto tp1 = time_point{system_clock::duration{1}};
        storage.addValues(tp1, std::array{i64(10), i64(11)}.data(), 2);
        REQUIRE(storage.timePointsNumber() == 1);
        REQUIRE(storage.timePoint(0) == tp1);
        storage.copyValuesByTimePoint(0, sink_2.data(), 2);
        REQUIRE(sink_2 == std::array{i64(10), i64(11)});
        storage.copyValuesByCounter(0, sink_1.data(), 1);
        REQUIRE(sink_1 == std::array{i64(10)});
        storage.copyValuesByCounter(1, sink_1.data(), 1);
        REQUIRE(sink_1 == std::array{i64(11)});

        auto tp2 = time_point{system_clock::duration{2}};
        storage.addValues(tp2, std::array{i64(20), i64(21)}.data(), 2);
        REQUIRE(storage.timePointsNumber() == 1);
        REQUIRE(storage.timePoint(0) == tp2);
        storage.copyValuesByTimePoint(0, sink_2.data(), 2);
        REQUIRE(sink_2 == std::array{i64(20), i64(21)});
        storage.copyValuesByCounter(0, sink_1.data(), 1);
        REQUIRE(sink_1 == std::array{i64(20)});
        storage.copyValuesByCounter(1, sink_1.data(), 1);
        REQUIRE(sink_1 == std::array{i64(21)});
    }
    SECTION("Capacity = 2")
    {
        ValuesStorage storage{CyclicValuesStorage({"a", "b"}, 2)};
        REQUIRE(storage.timePointsNumber() == 0);
        auto sink_1 = std::array{i64(0)};
        auto sink_2 = std::array{i64(0), i64(0)};

        auto tp1 = time_point{system_clock::duration{1}};
        storage.addValues(tp1, std::array{i64(10), i64(11)}.data(), 2);
        REQUIRE(storage.timePointsNumber() == 1);
        REQUIRE(storage.timePoint(0) == tp1);
        storage.copyValuesByTimePoint(0, sink_2.data(), 2);
        REQUIRE(sink_2 == std::array{i64(10), i64(11)});
        storage.copyValuesByCounter(0, sink_1.data(), 1);
        REQUIRE(sink_1 == std::array{i64(10)});
        storage.copyValuesByCounter(1, sink_1.data(), 1);
        REQUIRE(sink_1 == std::array{i64(11)});

        auto tp2 = time_point{system_clock::duration{2}};
        storage.addValues(tp2, std::array{i64(20), i64(21)}.data(), 2);
        REQUIRE(storage.timePointsNumber() == 2);
        REQUIRE(storage.timePoint(0) == tp1);
        REQUIRE(storage.timePoint(1) == tp2);
        storage.copyValuesByTimePoint(0, sink_2.data(), 2);
        REQUIRE(sink_2 == std::array{i64(10), i64(11)});
        storage.copyValuesByTimePoint(1, sink_2.data(), 2);
        REQUIRE(sink_2 == std::array{i64(20), i64(21)});
        storage.copyValuesByCounter(0, sink_2.data(), 2);
        REQUIRE(sink_2 == std::array{i64(10), i64(20)});
        storage.copyValuesByCounter(1, sink_2.data(), 2);
        REQUIRE(sink_2 == std::array{i64(11), i64(21)});

        auto tp3 = time_point{system_clock::duration{3}};
        storage.addValues(tp3, std::array{i64(30), i64(31)}.data(), 2);
        REQUIRE(storage.timePointsNumber() == 2);
        REQUIRE(storage.timePoint(0) == tp2);
        REQUIRE(storage.timePoint(1) == tp3);
        storage.copyValuesByTimePoint(0, sink_2.data(), 2);
        REQUIRE(sink_2 == std::array{i64(20), i64(21)});
        storage.copyValuesByTimePoint(1, sink_2.data(), 2);
        REQUIRE(sink_2 == std::array{i64(30), i64(31)});
        storage.copyValuesByCounter(0, sink_2.data(), 2);
        REQUIRE(sink_2 == std::array{i64(20), i64(30)});
        storage.copyValuesByCounter(1, sink_2.data(), 2);
        REQUIRE(sink_2 == std::array{i64(21), i64(31)});

        auto tp4 = time_point{system_clock::duration{4}};
        storage.addValues(tp4, std::array{i64(40), i64(41)}.data(), 2);
        REQUIRE(storage.timePointsNumber() == 2);
        REQUIRE(storage.timePoint(0) == tp3);
        REQUIRE(storage.timePoint(1) == tp4);
        storage.copyValuesByTimePoint(0, sink_2.data(), 2);
        REQUIRE(sink_2 == std::array{i64(30), i64(31)});
        storage.copyValuesByTimePoint(1, sink_2.data(), 2);
        REQUIRE(sink_2 == std::array{i64(40), i64(41)});
        storage.copyValuesByCounter(0, sink_2.data(), 2);
        REQUIRE(sink_2 == std::array{i64(30), i64(40)});
        storage.copyValuesByCounter(1, sink_2.data(), 2);
        REQUIRE(sink_2 == std::array{i64(31), i64(41)});
    }
}

TEST_CASE(PREFIX "swap sworks")
{
    ValuesStorage storage1{CyclicValuesStorage({"a", "b", "c"}, 5)};
    ValuesStorage storage2{CyclicValuesStorage({"e", "f", "g"}, 5)};
    std::swap(storage1, storage2);

    CHECK(storage1.counterName(0) == "e");
    CHECK(storage2.counterName(0) == "a");
}
