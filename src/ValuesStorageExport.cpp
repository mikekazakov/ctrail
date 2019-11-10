#include <ctrail/ValuesStorageExport.h>
#include <cassert>

namespace ctrail {

ValuesStorageExport::ValuesStorageExport(const ValuesStorageExport& _rhs)
{
    assert( m_Impl != nullptr );    
    m_Impl = _rhs.m_Impl->clone();
}
    
ValuesStorageExport::ValuesStorageExport(ValuesStorageExport&& _rhs) noexcept = default;
    
std::string ValuesStorageExport::format(const ValuesStorage &_values, Options _options) const
{
    assert( m_Impl != nullptr );
    return m_Impl->format(_values, _options);
}
    
ValuesStorageExport &ValuesStorageExport::operator=(const ValuesStorageExport &_rhs)
{
    assert( _rhs.m_Impl != nullptr );
    m_Impl = _rhs.m_Impl->clone();
    return *this;
}

ValuesStorageExport &ValuesStorageExport::operator=(ValuesStorageExport &&_rhs) noexcept = default;

}
