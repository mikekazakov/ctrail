#include <ctrail/ValuesStorageExporter.h>
#include <ctrail/CSVExporter.h>
#include <ctrail/ChromeTraceExporter.h>
#include <ctrail/MonotonicValuesStorage.h>
#include <ctrail/RegistryImpl.h>
#include <ctrail/DashboardImpl.h>
#include <ctrail/ValuesStorage.h>

#include <iostream>
#include <atomic>
#include <thread>
#include <chrono>
#include <random>
#include <fstream>

static std::atomic_bool g_Work{true};
static std::atomic_int g_FooCnt{0};
static std::atomic_int g_BarCnt{0};
static std::atomic_int g_BazCnt{0};
static const std::chrono::seconds g_TotalTime{2};
static const std::chrono::milliseconds g_ReportPeriod{100};

static void RegisterCounters(ctrail::Registry& registry)
{
    registry.add("Foo", g_FooCnt);
    registry.add("Bar", g_BarCnt);
    registry.add("Baz", g_BazCnt);
}

static void Watchdog( ctrail::Dashboard &dashboard,
              ctrail::ValuesStorage &storage,
              std::chrono::milliseconds period )
{
    while( g_Work ) {
        const auto values = dashboard.values();
        const auto now = std::chrono::system_clock::now();
        storage.addValues(now, values.data(), values.size());
        std::this_thread::sleep_for(period);
    }
}

int main()
{
    using namespace std::this_thread;
    using std::chrono::milliseconds;

    // Set up ctrail machinery
    ctrail::RegistryImpl registry;
    RegisterCounters(registry);
    ctrail::DashboardImpl dashboard{ registry.bake() };
    ctrail::ValuesStorage storage{ ctrail::MonotonicValuesStorage{dashboard.names()} };
    std::thread watchdog([&]{ Watchdog( dashboard, storage, g_ReportPeriod); });

    // Set up 'subsystems'
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> dist(100, 500); 
    std::thread foo{[&]{ while( g_Work ) ++g_FooCnt, sleep_for( milliseconds{dist(gen)} ); }};
    std::thread bar{[&]{ while( g_Work ) ++g_BarCnt, sleep_for( milliseconds{dist(gen)} ); }};
    std::thread baz{[&]{ while( g_Work ) ++g_BazCnt, sleep_for( milliseconds{dist(gen)} ); }};    
    
    // Do some work
    sleep_for(g_TotalTime);
    g_Work = false;
    foo.join();
    bar.join();
    baz.join();
    watchdog.join();
    
    // Export the gathered counters
    const auto export_options = ctrail::ValuesStorageExporter::Options::differential;
    const ctrail::ValuesStorageExporter csv_exporter{ ctrail::CSVExporter{} };
    const ctrail::ValuesStorageExporter trace_exporter{ ctrail::ChromeTraceExporter{} };    
    std::ofstream("async.csv") << csv_exporter.format(storage, export_options);
    std::ofstream("async.json") << trace_exporter.format(storage, export_options);
}
