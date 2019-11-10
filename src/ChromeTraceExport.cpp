#include <ctrail/ChromeTraceExport.h>
#include <numeric>
#include <algorithm>
#include <cassert>

namespace ctrail {

ChromeTraceExport::ChromeTraceExport()
{
}

ChromeTraceExport::ChromeTraceExport( const Formatting &_formatting ):
    m_Formatting(_formatting)
{}

std::string ChromeTraceExport::format(const ValuesStorage &_values, Options _options) const
{
    std::string trace;    
    trace += "{\"traceEvents\":[\n";    
    const std::size_t counters = _values.countersNumber();
    const std::size_t time_points = _values.timePointsNumber();
    const std::unique_ptr<std::int64_t[]> counter_values = std::make_unique<std::int64_t[]>(time_points);
    for( std::size_t counter = 0; counter < counters; ++counter ) {
        auto report = composeCounter(_values, counter, counter_values.get(), time_points, _options);
        if( report.empty() == false ) {
            if( counter > 0 )
                trace += ",\n";        
            trace += report;
        }
    }
    trace += "\n]}";        
    return trace;
}

std::string ChromeTraceExport::composeCounter(const ValuesStorage &_values, std::size_t _counter_index,
                                            std::int64_t *_tmp_buffer, std::size_t _tmp_buffer_size,
                                              Options _options) const
{
    using namespace std::chrono;
    assert(  _counter_index < _values.countersNumber() );
    
    const std::string &counter_name = _values.counterName(_counter_index);
    const std::size_t time_points = _values.timePointsNumber();
    assert( _tmp_buffer_size == time_points );
    _values.copyValuesByCounter(_counter_index, _tmp_buffer, _tmp_buffer_size);
    
    if( (_options & Options::skip_idle_counters) == Options::skip_idle_counters && 
        isIdle(_tmp_buffer, _tmp_buffer_size) )
        return {};
    
    const std::string prefix = "{\"pid\":1,\"tid\":1,\"name\":\"" +
    counter_name + "\",\"ph\":\"C\",\"ts\":";       
    const auto make_report = [prefix](ValuesStorage::time_point::duration _tp,
                                      std::int64_t _value ) -> std::string {
        std::string report = prefix;
        report += std::to_string(_tp.count());
        report += ",\"args\":{\"v\":";
        report += std::to_string(_value);
        report += "}}";
        return report;    
    };
    
    if( (_options & Options::differential) == Options::differential ) {
        std::adjacent_difference(_tmp_buffer, _tmp_buffer + time_points, _tmp_buffer);
    }
    
    const std::size_t start_index = (_options & Options::differential) == Options::differential ?
        1 : 0;
    
    std::string buf;
    if ( time_points > start_index ) {
        const auto start_time = time_point_cast<microseconds>(_values.timePoint(start_index));
        
        for( std::size_t i = start_index; i < time_points; ++i ) {
            const auto now_time = time_point_cast<microseconds>(_values.timePoint(i));
            const auto ts = now_time - start_time;
            const auto value = _tmp_buffer[i];
            
            buf += make_report(ts, value);
            buf += ",\n";
        }
        
        const auto last_tp = _values.timePoint(time_points - 1);
        const auto tp_diff = (time_points > 1) ? 
        ( last_tp - _values.timePoint(time_points - 2) ) :
        duration_cast<ValuesStorage::time_point::duration>(m_Formatting.tombstone_offset); 
        const auto tombstone_tp = last_tp + tp_diff;
        buf += make_report(tombstone_tp - start_time, m_Formatting.tombstone_value);
    }
    return buf;                                                                                            
}

bool ChromeTraceExport::isIdle(const std::int64_t * const _values, const std::size_t _size) noexcept
{
    return std::none_of(_values, _values + _size, [](auto v) { return v != 0; });
}

}
