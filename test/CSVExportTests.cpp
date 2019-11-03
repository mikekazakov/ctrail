#include <catch2/catch.hpp>
#include <array>
#include <ctrail/CSVExport.h>
#include <ctrail/MonotonicValuesStorage.h>

using namespace ctrail;
using std::chrono::system_clock;
using time_point = system_clock::time_point;
using i64 = std::int64_t;
#define PREFIX "CSVExport "

static time_t to_local(time_t _time)
{
    struct tm tm;
    gmtime_r(&_time, &tm);
    tm.tm_isdst = -1;
    return mktime(&tm);
}

// TODO: test skip_idle_counters

TEST_CASE(PREFIX"properly composes headers")
{
    CSVExport exporter;
    
    ValuesStorage storage{ MonotonicValuesStorage{{"a", "b", "c"}} };
    { // default options
        auto headers = exporter.composeHeaders(storage, ValuesStorageExport::Options::none);
        CHECK( headers == "counter\n" );
    }
    { // "differential"
        auto headers = exporter.composeHeaders(storage, ValuesStorageExport::Options::differential);
        CHECK( headers == "counter\n" );
    }
    
    storage.addValues(system_clock::from_time_t(to_local(1)),
                      std::array{i64(10), i64(11), i64(13)}.data(), 3);
    { // default options
        auto headers = exporter.composeHeaders(storage, ValuesStorageExport::Options::none);
        CHECK( headers == "counter,1970-01-01T00:00:01.000\n" );
    }
    { // "differential"
        auto headers = exporter.composeHeaders(storage, ValuesStorageExport::Options::differential);
        CHECK( headers == "counter\n" );
    }
    
    storage.addValues(system_clock::from_time_t(to_local(2)),
                      std::array{i64(20), i64(22), i64(26)}.data(), 3);
    { // default options
        auto headers = exporter.composeHeaders(storage, ValuesStorageExport::Options::none);
        CHECK( headers == "counter,1970-01-01T00:00:01.000,1970-01-01T00:00:02.000\n" );
    }
    { // "differential"
        auto headers = exporter.composeHeaders(storage, ValuesStorageExport::Options::differential);
        CHECK( headers == "counter,1970-01-01T00:00:02.000\n" );
    }
    
    storage.addValues(system_clock::from_time_t(to_local(3)),
                      std::array{i64(30), i64(33), i64(39)}.data(), 3);
    { // "default options"
        auto headers = exporter.composeHeaders(storage, ValuesStorageExport::Options::none);
        CHECK( headers == "counter,1970-01-01T00:00:01.000,1970-01-01T00:00:02.000,1970-01-01T00:00:03.000\n" );
    }
    { // "differential"
        auto headers = exporter.composeHeaders(storage, ValuesStorageExport::Options::differential);
        CHECK( headers == "counter,1970-01-01T00:00:02.000,1970-01-01T00:00:03.000\n" );
    }
}

TEST_CASE(PREFIX"properly composes rows")
{
    const auto none = ValuesStorageExport::Options::none;
    const auto diff = ValuesStorageExport::Options::differential;
    const auto skip = ValuesStorageExport::Options::skip_idle_counters;
    
    CSVExport exporter;
    
    std::int64_t buf[3];
    ValuesStorage storage{ MonotonicValuesStorage{{"a", "b", "c"}} };
    CHECK( exporter.composeRow(storage, 0, buf, 0, none) == "a\n" );
    CHECK( exporter.composeRow(storage, 1, buf, 0, none) == "b\n" );
    CHECK( exporter.composeRow(storage, 2, buf, 0, none) == "c\n" );
    CHECK( exporter.composeRow(storage, 0, buf, 0, diff) == "a\n" );
    CHECK( exporter.composeRow(storage, 1, buf, 0, diff) == "b\n" );
    CHECK( exporter.composeRow(storage, 2, buf, 0, diff) == "c\n" );
    CHECK( exporter.composeRow(storage, 0, buf, 0, skip) == "" );
    CHECK( exporter.composeRow(storage, 1, buf, 0, skip) == "" );
    CHECK( exporter.composeRow(storage, 2, buf, 0, skip) == "" );

    
    storage.addValues(system_clock::from_time_t(to_local(1)),
                      std::array{i64(10), i64(11), i64(13)}.data(), 3);
    CHECK( exporter.composeRow(storage, 0, buf, 1, none) == "a,10\n" );
    CHECK( exporter.composeRow(storage, 1, buf, 1, none) == "b,11\n" );
    CHECK( exporter.composeRow(storage, 2, buf, 1, none) == "c,13\n" );
    CHECK( exporter.composeRow(storage, 0, buf, 1, diff) == "a\n" );
    CHECK( exporter.composeRow(storage, 1, buf, 1, diff) == "b\n" );
    CHECK( exporter.composeRow(storage, 2, buf, 1, diff) == "c\n" );
    CHECK( exporter.composeRow(storage, 0, buf, 1, skip) == "a,10\n" );
    CHECK( exporter.composeRow(storage, 1, buf, 1, skip) == "b,11\n" );
    CHECK( exporter.composeRow(storage, 2, buf, 1, skip) == "c,13\n" );    
    
    storage.addValues(system_clock::from_time_t(to_local(2)),
                      std::array{i64(20), i64(22), i64(26)}.data(), 3);
    CHECK( exporter.composeRow(storage, 0, buf, 2, none) == "a,10,20\n" );
    CHECK( exporter.composeRow(storage, 1, buf, 2, none) == "b,11,22\n" );
    CHECK( exporter.composeRow(storage, 2, buf, 2, none) == "c,13,26\n" );
    CHECK( exporter.composeRow(storage, 0, buf, 2, diff) == "a,10\n" );
    CHECK( exporter.composeRow(storage, 1, buf, 2, diff) == "b,11\n" );
    CHECK( exporter.composeRow(storage, 2, buf, 2, diff) == "c,13\n" );
    CHECK( exporter.composeRow(storage, 0, buf, 2, skip) == "a,10,20\n" );
    CHECK( exporter.composeRow(storage, 1, buf, 2, skip) == "b,11,22\n" );
    CHECK( exporter.composeRow(storage, 2, buf, 2, skip) == "c,13,26\n" );
    
    storage.addValues(system_clock::from_time_t(to_local(3)),
                      std::array{i64(30), i64(33), i64(39)}.data(), 3);
    CHECK( exporter.composeRow(storage, 0, buf, 3, none) == "a,10,20,30\n" );
    CHECK( exporter.composeRow(storage, 1, buf, 3, none) == "b,11,22,33\n" );
    CHECK( exporter.composeRow(storage, 2, buf, 3, none) == "c,13,26,39\n" );
    CHECK( exporter.composeRow(storage, 0, buf, 3, diff) == "a,10,10\n" );
    CHECK( exporter.composeRow(storage, 1, buf, 3, diff) == "b,11,11\n" );
    CHECK( exporter.composeRow(storage, 2, buf, 3, diff) == "c,13,13\n" );
    CHECK( exporter.composeRow(storage, 0, buf, 3, skip) == "a,10,20,30\n" );
    CHECK( exporter.composeRow(storage, 1, buf, 3, skip) == "b,11,22,33\n" );
    CHECK( exporter.composeRow(storage, 2, buf, 3, skip) == "c,13,26,39\n" );    
}

TEST_CASE(PREFIX"skips rows when asked to")
{
    const auto none = ValuesStorageExport::Options::none;
    const auto diff = ValuesStorageExport::Options::differential;
    const auto skip = ValuesStorageExport::Options::skip_idle_counters;
        
    ValuesStorage storage{ MonotonicValuesStorage{{"a", "b", "c"}} };
    storage.addValues(system_clock::from_time_t(to_local(1)),
                      std::array{i64(10), i64(0), i64(13)}.data(), 3);
    storage.addValues(system_clock::from_time_t(to_local(2)),
                      std::array{i64(20), i64(0), i64(26)}.data(), 3);    

    std::int64_t buf[3];
    CSVExport exporter;        
    CHECK( exporter.composeRow(storage, 0, buf, 2, none) == "a,10,20\n" );
    CHECK( exporter.composeRow(storage, 1, buf, 2, none) == "b,0,0\n" );
    CHECK( exporter.composeRow(storage, 2, buf, 2, none) == "c,13,26\n" );
    CHECK( exporter.composeRow(storage, 0, buf, 2, diff) == "a,10\n" );
    CHECK( exporter.composeRow(storage, 1, buf, 2, diff) == "b,0\n" );
    CHECK( exporter.composeRow(storage, 2, buf, 2, diff) == "c,13\n" );
    CHECK( exporter.composeRow(storage, 0, buf, 2, skip) == "a,10,20\n" );
    CHECK( exporter.composeRow(storage, 1, buf, 2, skip) == "" );
    CHECK( exporter.composeRow(storage, 2, buf, 2, skip) == "c,13,26\n" );
    CHECK( exporter.composeRow(storage, 0, buf, 2, skip|diff) == "a,10\n" );
    CHECK( exporter.composeRow(storage, 1, buf, 2, skip|diff) == "" );
    CHECK( exporter.composeRow(storage, 2, buf, 2, skip|diff) == "c,13\n" );        
}

TEST_CASE(PREFIX"properly composes a CSV")
{
    CSVExport exporter;
    
    ValuesStorage storage{ MonotonicValuesStorage{{"a", "b", "c"}} };
    storage.addValues(system_clock::from_time_t(to_local(1)),
                      std::array{i64(10), i64(11), i64(13)}.data(), 3);
    storage.addValues(system_clock::from_time_t(to_local(2)),
                      std::array{i64(20), i64(22), i64(26)}.data(), 3);
    storage.addValues(system_clock::from_time_t(to_local(3)),
                      std::array{i64(30), i64(33), i64(39)}.data(), 3);
    
    const std::string csv = exporter.format(storage, ValuesStorageExport::Options::none);
    CHECK( csv ==
          "counter,1970-01-01T00:00:01.000,1970-01-01T00:00:02.000,1970-01-01T00:00:03.000\n"
          "a,10,20,30\n"
          "b,11,22,33\n"
          "c,13,26,39\n");
    
    const std::string csv_diff = exporter.format(storage,
                                                 ValuesStorageExport::Options::differential);
    CHECK( csv_diff ==
          "counter,1970-01-01T00:00:02.000,1970-01-01T00:00:03.000\n"
          "a,10,10\n"
          "b,11,11\n"
          "c,13,13\n");
}
