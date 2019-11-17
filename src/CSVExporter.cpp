#include <ctrail/CSVExporter.h>
#include <sstream>
#include <time.h>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <cassert>

namespace ctrail {

CSVExporter::CSVExporter() = default;
    
CSVExporter::CSVExporter( const Formatting &_formatting ):
    m_Formatting(_formatting)
{
}

std::string CSVExporter::format(const ValuesStorage &_values, Options _options) const
{
    std::string csv;
    csv += composeHeaders(_values, _options);
    
    const std::size_t counters = _values.countersNumber();
    const std::size_t time_points = _values.timePointsNumber();
    const std::unique_ptr<std::int64_t[]> counter_values = std::make_unique<std::int64_t[]>(time_points);
    for( std::size_t counter = 0; counter < counters; ++counter )
        csv += composeRow(_values, counter, counter_values.get(), time_points, _options);
    
    return csv;
}
    
std::string CSVExporter::composeHeaders(const ValuesStorage &_values, Options _options) const
{
    std::string buf;
    buf += m_Formatting.counters_column_title;

    const std::size_t time_points = _values.timePointsNumber();
    
    const std::size_t start_index = (_options & Options::differential) == Options::differential ?
        1 : 0;
    
    if ( time_points > start_index ) {
        buf += m_Formatting.values_delimiter;
        for( std::size_t i = start_index; i < time_points; ++i ) {
            buf += fmtTime(_values.timePoint(i));
            if( i + 1 < time_points ) {
                buf += m_Formatting.values_delimiter;
            }
        }
    }
    buf += m_Formatting.newline_delimiter;
    return buf;
}

bool CSVExporter::isIdle(const std::int64_t * const _values, const std::size_t _size) noexcept
{
    return std::none_of(_values, _values + _size, [](auto v) { return v != 0; });
}

std::string CSVExporter::composeRow(const ValuesStorage &_values, std::size_t _counter_index,
                                  std::int64_t *_tmp_buffer, std::size_t _tmp_buffer_size,
                                  Options _options) const
{
    assert(  _counter_index < _values.countersNumber() );
    
    const std::string &counter_name = _values.counterName(_counter_index);
    const std::size_t time_points = _values.timePointsNumber();
    assert( _tmp_buffer_size == time_points );
    _values.copyValuesByCounter(_counter_index, _tmp_buffer, _tmp_buffer_size);
 
    if( (_options & Options::skip_idle_counters) == Options::skip_idle_counters && 
        isIdle(_tmp_buffer, _tmp_buffer_size) )
        return {};
    
    std::string buf;
    buf += counter_name;

    if( (_options & Options::differential) == Options::differential ) {
        std::adjacent_difference(_tmp_buffer, _tmp_buffer + time_points, _tmp_buffer);
    }
    
    const std::size_t start_index = (_options & Options::differential) == Options::differential ?
        1 : 0;
    
    if ( time_points > start_index ) {
        buf += m_Formatting.values_delimiter;
        for( std::size_t i = start_index; i < time_points; ++i ) {
            buf += std::to_string(_tmp_buffer[i]);
            if( i + 1 < time_points ) {
                buf += m_Formatting.values_delimiter;
            }
        }
    }
    
    buf += m_Formatting.newline_delimiter;
    return buf;
}

std::string CSVExporter::fmtTime(std::chrono::system_clock::time_point _tp)
{
    using namespace std::chrono;
    const auto time = system_clock::to_time_t(_tp);

    struct tm tm;
    localtime_r(&time, &tm);
    
    auto ms = duration_cast<milliseconds>(_tp.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time( &tm, "%FT%T" ) << "." << std::setfill('0') << std::setw(3) << ms.count();
    
    return ss.str();
}
    
}
