#pragma once

#include "Dashboard.h"
#include "ValuesStorageExporter.h"
#include "CSVExporter.h"

#include <memory>
#include <chrono>
#include <functional>

namespace ctrail {

class OneShotMonitor
{
public:
    struct Params {
        const Dashboard *dashboard = nullptr;
        std::chrono::nanoseconds period = std::chrono::milliseconds{100};
        std::chrono::nanoseconds duration = std::chrono::seconds{1};
        ValuesStorageExporter exporter = ValuesStorageExporter{ CSVExporter{} };
        ValuesStorageExporter::Options export_options = ValuesStorageExporter::Options::none;        
        std::function<void(std::string _exported_trail)> save;        
    };
    OneShotMonitor( Params _params );
    OneShotMonitor( const OneShotMonitor & ) = delete;
    OneShotMonitor( OneShotMonitor && ) noexcept = default;
    ~OneShotMonitor();
    OneShotMonitor& operator=(const OneShotMonitor & ) = delete;
    OneShotMonitor& operator=(OneShotMonitor && ) = default;

private:
    class Impl;
    std::unique_ptr<Impl> I; 
};

}
