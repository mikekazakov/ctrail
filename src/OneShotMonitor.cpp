#include <ctrail/OneShotMonitor.h>
#include <ctrail/MonotonicValuesStorage.h>
#include <thread>
#include <iostream>

namespace ctrail {

class OneShotMonitor::Impl
{
public:
    Impl(Params _params);
    ~Impl();
    
private:
    void monitor();
    void save( std::string _exported_trail );

    std::thread m_WorkerThread;
    const Dashboard * const m_Dashboard = nullptr;
    const std::chrono::nanoseconds m_Period;
    const std::chrono::nanoseconds m_Duration;
    const ValuesStorageExporter m_Exporter;    
    const ValuesStorageExporter::Options m_ExportOptions;
    ValuesStorage m_Storage;
    const std::function<void(std::string)> m_Save;
};

OneShotMonitor::Impl::Impl(Params _params):
    m_Dashboard{_params.dashboard},
    m_Period{ _params.period },
    m_Duration{ _params.duration },
    m_Exporter{ std::move(_params.exporter) },
    m_ExportOptions{ _params.export_options },
    m_Storage{ MonotonicValuesStorage{ _params.dashboard->names() } }
{
    if( m_Period.count() < 0 )
        throw std::invalid_argument("OneShotMonitor: period must be non-negative");
    if( m_Duration.count() < 0 )
        throw std::invalid_argument("OneShotMonitor: duration must be non-negative");

    m_WorkerThread = std::thread( [this]{ monitor(); } );
}

OneShotMonitor::Impl::~Impl()
{
    if( m_WorkerThread.joinable() )
        m_WorkerThread.join();
}

void OneShotMonitor::Impl::monitor()
{
    const auto start = std::chrono::system_clock::now();
    while( start + m_Duration < std::chrono::system_clock::now() ) {
        const auto values = m_Dashboard->values();
        const auto now = std::chrono::system_clock::now();
        m_Storage.addValues(now, values.data(), values.size());
        std::this_thread::sleep_for(m_Period);            
    }
    save( m_Exporter.format(m_Storage, m_ExportOptions) );
}

void OneShotMonitor::Impl::save( std::string _exported_trail )
{
    if( m_Save ) {
        m_Save( std::move(_exported_trail) );
    }
    else {
        std::cerr << _exported_trail << std::endl;
    }
}

OneShotMonitor::OneShotMonitor( Params _params ):
    I{ std::make_unique<Impl>( std::move(_params) ) }
{
}

OneShotMonitor::~OneShotMonitor() = default;

}
