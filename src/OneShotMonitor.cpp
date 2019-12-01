#include <ctrail/OneShotMonitor.h>
#include <ctrail/MonotonicValuesStorage.h>
#include <thread>
#include <iostream>
#include <cassert>

namespace ctrail {
 
OneShotMonitor::OneShotMonitor( Params _params ):
    m_Dashboard{_params.dashboard},
    m_Period{ _params.period },
    m_Duration{ _params.duration },
    m_Exporter{ std::move(_params.exporter) },
    m_ExportOptions{ _params.export_options },
    m_Storage{ MonotonicValuesStorage{ throwIfNull(_params.dashboard)->names() } },
    m_Save{ std::move(_params.save) }
{
    if( m_Period.count() < 0 )
        throw std::invalid_argument("OneShotMonitor: period must be non-negative");
    if( m_Duration.count() < 0 )
        throw std::invalid_argument("OneShotMonitor: duration must be non-negative");

    m_WorkerThread = std::thread( [this]{ monitor(); } );
}

OneShotMonitor::~OneShotMonitor()
{
    if( m_WorkerThread.joinable() )
        m_WorkerThread.join();
}

void OneShotMonitor::join()
{
    if( m_WorkerThread.joinable() )
        m_WorkerThread.join();
}

void OneShotMonitor::monitor()
{
    fire( m_Period, m_Duration, [this]{
        const auto values = m_Dashboard->values();
        const auto now = nowOnSystemClock();
        m_Storage.addValues(now, values.data(), values.size());
    });
    save( m_Exporter.format(m_Storage, m_ExportOptions) );
}

void OneShotMonitor::save( std::string _exported_trail )
{
    if( m_Save ) {
        m_Save( std::move(_exported_trail) );
    }
    else {
        std::cerr << _exported_trail << std::endl;
    }
}

std::chrono::system_clock::time_point OneShotMonitor::nowOnSystemClock() const noexcept
{
    return std::chrono::system_clock::now(); 
}

void OneShotMonitor::fire( std::chrono::nanoseconds _period, std::chrono::nanoseconds _duration, std::function<void()> _job )
{
    assert( _job );
    assert( _period.count() > 0 );
    assert( _duration.count() > 0 );
    const auto start = std::chrono::steady_clock::now();
    auto to_sleep = _period;
    while( std::chrono::steady_clock::now() < start + _duration ) {
        _job();
        
        const auto tp_before_sleep = std::chrono::steady_clock::now(); 
        std::this_thread::sleep_for(to_sleep);
        const auto tp_after_sleep = std::chrono::steady_clock::now();
        to_sleep = 2 * _period - (tp_after_sleep - tp_before_sleep); 
    }         
}

const Dashboard *OneShotMonitor::throwIfNull(const Dashboard * const dashboard)
{
    if( dashboard == nullptr ) 
        throw std::invalid_argument("OneShotMonitor: dashboard cant be nullptr");
    return dashboard;
}

}
