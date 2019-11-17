#include <ctrail/ValuesStorageExporter.h>
#include <cassert>

namespace ctrail {

ValuesStorageExporter::ValuesStorageExporter(const ValuesStorageExporter& _rhs)
{
    assert( m_Impl != nullptr );    
    m_Impl = _rhs.m_Impl->clone();
}
    
ValuesStorageExporter::ValuesStorageExporter(ValuesStorageExporter&& _rhs) noexcept = default;
    
std::string ValuesStorageExporter::format(const ValuesStorage &_values, Options _options) const
{
    assert( m_Impl != nullptr );
    return m_Impl->format(_values, _options);
}
    
ValuesStorageExporter &ValuesStorageExporter::operator=(const ValuesStorageExporter &_rhs)
{
    assert( _rhs.m_Impl != nullptr );
    m_Impl = _rhs.m_Impl->clone();
    return *this;
}

ValuesStorageExporter &ValuesStorageExporter::operator=(ValuesStorageExporter &&_rhs) noexcept = default;

}
