#include <catch2/catch.hpp>
#include <ctrail/OneShotMonitor.h>
#include <ctrail/RegistryImpl.h>
#include <ctrail/DashboardImpl.h>
#include <optional>

using namespace ctrail;

#define PREFIX "ctrail::OneShotMonitor "

namespace {

class Exporter
{
public:
    using Options = ValuesStorageExporter::Options;
    std::string format(const ValuesStorage &_values, Options _options) const;    
};

struct MonitorWithStubs : OneShotMonitor
{
    using OneShotMonitor::OneShotMonitor;
    std::chrono::system_clock::time_point nowOnSystemClock() const noexcept override;
    void fire( std::chrono::nanoseconds _period, std::chrono::nanoseconds _duration, std::function<void()> _job ) override;
    void join();

    mutable std::chrono::system_clock::time_point now{ std::chrono::seconds{1} };
};

}

std::string Exporter::format(const ValuesStorage &_values, Options _options) const
{
    const auto counters_number = _values.countersNumber();
    const auto times_points_number = _values.timePointsNumber();
    std::unique_ptr<int64_t[]> counters_values = std::make_unique<int64_t[]>(times_points_number);
    std::string result; 
    for( size_t counter = 0; counter < counters_number; ++counter ) {
        _values.copyValuesByCounter(counter, counters_values.get(), times_points_number);
        for( size_t time_point = 0; time_point < times_points_number; ++time_point )
            result += std::to_string(counters_values[time_point]);
    }
    return result;
}

std::chrono::system_clock::time_point MonitorWithStubs::nowOnSystemClock() const noexcept
{
    return now;
}

void MonitorWithStubs::fire( std::chrono::nanoseconds _period, std::chrono::nanoseconds _duration, std::function<void()> _job )
{
    const auto fires = _duration.count() / _period.count();
    for( int i = 0; i < fires; ++i ) {
        _job();
        now = now + std::chrono::seconds{1};
    }
}

void MonitorWithStubs::join()
{
    OneShotMonitor::join();
}

TEST_CASE(PREFIX"Gathers values ten times and exports it")
{
    int64_t value_a = 1;
    int64_t value_b = 2;
    RegistryImpl registry;
    registry.add("a", value_a);
    registry.add("b", value_b);
    DashboardImpl dashboard{ registry.bake() };
    
    std::string result;
    OneShotMonitor::Params params;
    params.dashboard = &dashboard;
    params.period = std::chrono::milliseconds{10};
    params.duration = std::chrono::milliseconds{100};
    params.exporter = ValuesStorageExporter{ Exporter{} };
    params.save = [&](std::string _exported){ result = _exported; };
    MonitorWithStubs monitor{ std::move(params) };
    monitor.join();
    
    CHECK( result == "11111111112222222222" );
}
