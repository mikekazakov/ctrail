#pragma once

#include <ctrail/ValuesStorageExport.h>
#include <string>

namespace ctrail {

class CSVExport /* conforms to the ValuesStorageExport 'concept' */
{
public:
    struct Formatting {
        std::string counters_column_title = "counter";
        std::string values_delimiter = ",";
        std::string newline_delimiter = "\n";
    };
    using Options = ValuesStorageExport::Options;
    
    CSVExport();
    CSVExport( const Formatting &_formatting );
    
    std::string format(const ValuesStorage &_values, Options _options) const;

    std::string composeHeaders(const ValuesStorage &_values, Options _options) const;
    std::string composeRow(const ValuesStorage &_values, std::size_t _counter_index,
                           std::int64_t *_tmp_buffer, std::size_t _tmp_buffer_size,
                           Options _options) const;
private:
    Formatting m_Formatting;
};
    
}
