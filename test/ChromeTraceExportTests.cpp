#include <catch2/catch.hpp>
#include <array>
#include <ctrail/ChromeTraceExporter.h>
#include <ctrail/MonotonicValuesStorage.h>
#include <ctrail/Ports.h>

#include <iostream>

using namespace ctrail;
using std::chrono::system_clock;
using time_point = system_clock::time_point;
using i64 = std::int64_t;
#define PREFIX "ChromeTraceExport "

static time_t to_local(time_t _time)
{
    struct tm tm = internal::gmtime(_time);
    tm.tm_isdst = -1;
    return mktime(&tm);
}

// TODO: test skip_idle_counters

TEST_CASE(PREFIX"properly composes counters")
{
    const auto none = ValuesStorageExporter::Options::none;
    const auto diff = ValuesStorageExporter::Options::differential;
    
    ChromeTraceExporter exporter;
    
    std::int64_t buf[3];
    ValuesStorage storage{ MonotonicValuesStorage{{"a", "b", "c"}} };
    CHECK( exporter.composeCounter(storage, 0, buf, 0, none) == "" );
    CHECK( exporter.composeCounter(storage, 1, buf, 0, none) == "" );
    CHECK( exporter.composeCounter(storage, 2, buf, 0, none) == "" );
    CHECK( exporter.composeCounter(storage, 0, buf, 0, diff) == "" );
    CHECK( exporter.composeCounter(storage, 1, buf, 0, diff) == "" );
    CHECK( exporter.composeCounter(storage, 2, buf, 0, diff) == "" );
    
    storage.addValues(system_clock::from_time_t(to_local(1)),
                      std::array{i64(10), i64(11), i64(13)}.data(), 3);
    CHECK( exporter.composeCounter(storage, 0, buf, 1, none) == 
        "{\"pid\":1,\"tid\":1,\"name\":\"a\",\"ph\":\"C\",\"ts\":0,\"args\":{\"v\":10}},\n"
        "{\"pid\":1,\"tid\":1,\"name\":\"a\",\"ph\":\"C\",\"ts\":100000,\"args\":{\"v\":0}}");
    CHECK( exporter.composeCounter(storage, 1, buf, 1, none) == 
        "{\"pid\":1,\"tid\":1,\"name\":\"b\",\"ph\":\"C\",\"ts\":0,\"args\":{\"v\":11}},\n"
        "{\"pid\":1,\"tid\":1,\"name\":\"b\",\"ph\":\"C\",\"ts\":100000,\"args\":{\"v\":0}}" );
    CHECK( exporter.composeCounter(storage, 2, buf, 1, none) == 
        "{\"pid\":1,\"tid\":1,\"name\":\"c\",\"ph\":\"C\",\"ts\":0,\"args\":{\"v\":13}},\n"
        "{\"pid\":1,\"tid\":1,\"name\":\"c\",\"ph\":\"C\",\"ts\":100000,\"args\":{\"v\":0}}" );
    CHECK( exporter.composeCounter(storage, 0, buf, 1, diff) == "" );
    CHECK( exporter.composeCounter(storage, 1, buf, 1, diff) == "" );
    CHECK( exporter.composeCounter(storage, 2, buf, 1, diff) == "" );
    
    storage.addValues(system_clock::from_time_t(to_local(2)),
                      std::array{i64(20), i64(22), i64(26)}.data(), 3);
    CHECK( exporter.composeCounter(storage, 0, buf, 2, none) == 
        "{\"pid\":1,\"tid\":1,\"name\":\"a\",\"ph\":\"C\",\"ts\":0,\"args\":{\"v\":10}},\n"
        "{\"pid\":1,\"tid\":1,\"name\":\"a\",\"ph\":\"C\",\"ts\":1000000,\"args\":{\"v\":20}},\n"
        "{\"pid\":1,\"tid\":1,\"name\":\"a\",\"ph\":\"C\",\"ts\":2000000,\"args\":{\"v\":0}}" );
    CHECK( exporter.composeCounter(storage, 1, buf, 2, none) == 
        "{\"pid\":1,\"tid\":1,\"name\":\"b\",\"ph\":\"C\",\"ts\":0,\"args\":{\"v\":11}},\n"
        "{\"pid\":1,\"tid\":1,\"name\":\"b\",\"ph\":\"C\",\"ts\":1000000,\"args\":{\"v\":22}},\n"
        "{\"pid\":1,\"tid\":1,\"name\":\"b\",\"ph\":\"C\",\"ts\":2000000,\"args\":{\"v\":0}}" );
    CHECK( exporter.composeCounter(storage, 2, buf, 2, none) == 
        "{\"pid\":1,\"tid\":1,\"name\":\"c\",\"ph\":\"C\",\"ts\":0,\"args\":{\"v\":13}},\n"
        "{\"pid\":1,\"tid\":1,\"name\":\"c\",\"ph\":\"C\",\"ts\":1000000,\"args\":{\"v\":26}},\n"
        "{\"pid\":1,\"tid\":1,\"name\":\"c\",\"ph\":\"C\",\"ts\":2000000,\"args\":{\"v\":0}}" );
    CHECK( exporter.composeCounter(storage, 0, buf, 2, diff) == 
        "{\"pid\":1,\"tid\":1,\"name\":\"a\",\"ph\":\"C\",\"ts\":0,\"args\":{\"v\":10}},\n"
        "{\"pid\":1,\"tid\":1,\"name\":\"a\",\"ph\":\"C\",\"ts\":1000000,\"args\":{\"v\":0}}" );
    CHECK( exporter.composeCounter(storage, 1, buf, 2, diff) == 
        "{\"pid\":1,\"tid\":1,\"name\":\"b\",\"ph\":\"C\",\"ts\":0,\"args\":{\"v\":11}},\n"
        "{\"pid\":1,\"tid\":1,\"name\":\"b\",\"ph\":\"C\",\"ts\":1000000,\"args\":{\"v\":0}}" );
    CHECK( exporter.composeCounter(storage, 2, buf, 2, diff) == 
        "{\"pid\":1,\"tid\":1,\"name\":\"c\",\"ph\":\"C\",\"ts\":0,\"args\":{\"v\":13}},\n"
        "{\"pid\":1,\"tid\":1,\"name\":\"c\",\"ph\":\"C\",\"ts\":1000000,\"args\":{\"v\":0}}" );
    
    storage.addValues(system_clock::from_time_t(to_local(3)),
                      std::array{i64(30), i64(33), i64(39)}.data(), 3);
    CHECK( exporter.composeCounter(storage, 0, buf, 3, none) == 
        "{\"pid\":1,\"tid\":1,\"name\":\"a\",\"ph\":\"C\",\"ts\":0,\"args\":{\"v\":10}},\n"
        "{\"pid\":1,\"tid\":1,\"name\":\"a\",\"ph\":\"C\",\"ts\":1000000,\"args\":{\"v\":20}},\n"
        "{\"pid\":1,\"tid\":1,\"name\":\"a\",\"ph\":\"C\",\"ts\":2000000,\"args\":{\"v\":30}},\n" 
        "{\"pid\":1,\"tid\":1,\"name\":\"a\",\"ph\":\"C\",\"ts\":3000000,\"args\":{\"v\":0}}" );
    CHECK( exporter.composeCounter(storage, 1, buf, 3, none) ==
        "{\"pid\":1,\"tid\":1,\"name\":\"b\",\"ph\":\"C\",\"ts\":0,\"args\":{\"v\":11}},\n"
        "{\"pid\":1,\"tid\":1,\"name\":\"b\",\"ph\":\"C\",\"ts\":1000000,\"args\":{\"v\":22}},\n"
        "{\"pid\":1,\"tid\":1,\"name\":\"b\",\"ph\":\"C\",\"ts\":2000000,\"args\":{\"v\":33}},\n" 
        "{\"pid\":1,\"tid\":1,\"name\":\"b\",\"ph\":\"C\",\"ts\":3000000,\"args\":{\"v\":0}}" );
    CHECK( exporter.composeCounter(storage, 2, buf, 3, none) == 
        "{\"pid\":1,\"tid\":1,\"name\":\"c\",\"ph\":\"C\",\"ts\":0,\"args\":{\"v\":13}},\n"
        "{\"pid\":1,\"tid\":1,\"name\":\"c\",\"ph\":\"C\",\"ts\":1000000,\"args\":{\"v\":26}},\n"
        "{\"pid\":1,\"tid\":1,\"name\":\"c\",\"ph\":\"C\",\"ts\":2000000,\"args\":{\"v\":39}},\n" 
        "{\"pid\":1,\"tid\":1,\"name\":\"c\",\"ph\":\"C\",\"ts\":3000000,\"args\":{\"v\":0}}" );
    CHECK( exporter.composeCounter(storage, 0, buf, 3, diff) == 
        "{\"pid\":1,\"tid\":1,\"name\":\"a\",\"ph\":\"C\",\"ts\":0,\"args\":{\"v\":10}},\n"
        "{\"pid\":1,\"tid\":1,\"name\":\"a\",\"ph\":\"C\",\"ts\":1000000,\"args\":{\"v\":10}},\n"
        "{\"pid\":1,\"tid\":1,\"name\":\"a\",\"ph\":\"C\",\"ts\":2000000,\"args\":{\"v\":0}}" );
    CHECK( exporter.composeCounter(storage, 1, buf, 3, diff) == 
        "{\"pid\":1,\"tid\":1,\"name\":\"b\",\"ph\":\"C\",\"ts\":0,\"args\":{\"v\":11}},\n"
        "{\"pid\":1,\"tid\":1,\"name\":\"b\",\"ph\":\"C\",\"ts\":1000000,\"args\":{\"v\":11}},\n"
        "{\"pid\":1,\"tid\":1,\"name\":\"b\",\"ph\":\"C\",\"ts\":2000000,\"args\":{\"v\":0}}" );
    CHECK( exporter.composeCounter(storage, 2, buf, 3, diff) == 
        "\{\"pid\":1,\"tid\":1,\"name\":\"c\",\"ph\":\"C\",\"ts\":0,\"args\":{\"v\":13}},\n"
        "{\"pid\":1,\"tid\":1,\"name\":\"c\",\"ph\":\"C\",\"ts\":1000000,\"args\":{\"v\":13}},\n"
        "{\"pid\":1,\"tid\":1,\"name\":\"c\",\"ph\":\"C\",\"ts\":2000000,\"args\":{\"v\":0}}" );
}

TEST_CASE(PREFIX"properly composes a trace")
{
    const auto none = ValuesStorageExporter::Options::none;
    const auto diff = ValuesStorageExporter::Options::differential;
    
    ValuesStorageExporter  exporter{ ChromeTraceExporter{} };
    ValuesStorage storage{ MonotonicValuesStorage{{"a", "b", "c"}} };
    
    SECTION("Empty") {
        SECTION("default export options") {
            const std::string json = exporter.format(storage, none);
            const auto reference = "{\"traceEvents\":[\n\n]}"; 
            CHECK(json == reference );
        }
        SECTION("differential") {
            const std::string json = exporter.format(storage, none);
            const auto reference = "{\"traceEvents\":[\n\n]}"; 
            CHECK(json == reference );
        }
    }
    SECTION("Single report") {
        storage.addValues(system_clock::from_time_t(to_local(1)),
                          std::array{i64(10), i64(11), i64(13)}.data(), 3);
        SECTION("default export options") {
            const std::string json = exporter.format(storage, none);
            const auto reference =
            "{\"traceEvents\":[\n"
            "{\"pid\":1,\"tid\":1,\"name\":\"a\",\"ph\":\"C\",\"ts\":0,\"args\":{\"v\":10}},\n"
            "{\"pid\":1,\"tid\":1,\"name\":\"a\",\"ph\":\"C\",\"ts\":100000,\"args\":{\"v\":0}},\n"
            "{\"pid\":1,\"tid\":1,\"name\":\"b\",\"ph\":\"C\",\"ts\":0,\"args\":{\"v\":11}},\n"
            "{\"pid\":1,\"tid\":1,\"name\":\"b\",\"ph\":\"C\",\"ts\":100000,\"args\":{\"v\":0}},\n" 
            "{\"pid\":1,\"tid\":1,\"name\":\"c\",\"ph\":\"C\",\"ts\":0,\"args\":{\"v\":13}},\n"
            "{\"pid\":1,\"tid\":1,\"name\":\"c\",\"ph\":\"C\",\"ts\":100000,\"args\":{\"v\":0}}\n"
            "]}" ;
            CHECK(json == reference );
        }
        SECTION("differential") {
            const std::string json = exporter.format(storage, diff);
            const auto reference = "{\"traceEvents\":[\n\n]}";         
            CHECK(json == reference );
        }
    }
    SECTION("Two reports") {
        storage.addValues(system_clock::from_time_t(to_local(1)),
                          std::array{i64(10), i64(11), i64(13)}.data(), 3);    
        storage.addValues(system_clock::from_time_t(to_local(2)),
                          std::array{i64(20), i64(22), i64(26)}.data(), 3);    
        SECTION("default export options") {
            const std::string json = exporter.format(storage, none);
            const auto reference =             
            "{\"traceEvents\":[\n"
            "{\"pid\":1,\"tid\":1,\"name\":\"a\",\"ph\":\"C\",\"ts\":0,\"args\":{\"v\":10}},\n"
            "{\"pid\":1,\"tid\":1,\"name\":\"a\",\"ph\":\"C\",\"ts\":1000000,\"args\":{\"v\":20}},\n"
            "{\"pid\":1,\"tid\":1,\"name\":\"a\",\"ph\":\"C\",\"ts\":2000000,\"args\":{\"v\":0}},\n"                  
            "{\"pid\":1,\"tid\":1,\"name\":\"b\",\"ph\":\"C\",\"ts\":0,\"args\":{\"v\":11}},\n"
            "{\"pid\":1,\"tid\":1,\"name\":\"b\",\"ph\":\"C\",\"ts\":1000000,\"args\":{\"v\":22}},\n"
            "{\"pid\":1,\"tid\":1,\"name\":\"b\",\"ph\":\"C\",\"ts\":2000000,\"args\":{\"v\":0}},\n"
            "{\"pid\":1,\"tid\":1,\"name\":\"c\",\"ph\":\"C\",\"ts\":0,\"args\":{\"v\":13}},\n"
            "{\"pid\":1,\"tid\":1,\"name\":\"c\",\"ph\":\"C\",\"ts\":1000000,\"args\":{\"v\":26}},\n"
            "{\"pid\":1,\"tid\":1,\"name\":\"c\",\"ph\":\"C\",\"ts\":2000000,\"args\":{\"v\":0}}\n"
            "]}" ;
            CHECK(json == reference );        
        }
        SECTION("differential") {
            const std::string json = exporter.format(storage, diff);
            const auto reference =             
            "{\"traceEvents\":[\n"
            "{\"pid\":1,\"tid\":1,\"name\":\"a\",\"ph\":\"C\",\"ts\":0,\"args\":{\"v\":10}},\n"
            "{\"pid\":1,\"tid\":1,\"name\":\"a\",\"ph\":\"C\",\"ts\":1000000,\"args\":{\"v\":0}},\n"                 
            "{\"pid\":1,\"tid\":1,\"name\":\"b\",\"ph\":\"C\",\"ts\":0,\"args\":{\"v\":11}},\n"
            "{\"pid\":1,\"tid\":1,\"name\":\"b\",\"ph\":\"C\",\"ts\":1000000,\"args\":{\"v\":0}},\n"
            "{\"pid\":1,\"tid\":1,\"name\":\"c\",\"ph\":\"C\",\"ts\":0,\"args\":{\"v\":13}},\n"
            "{\"pid\":1,\"tid\":1,\"name\":\"c\",\"ph\":\"C\",\"ts\":1000000,\"args\":{\"v\":0}}\n"
            "]}" ;
            CHECK(json == reference );           
        }
    }
}
