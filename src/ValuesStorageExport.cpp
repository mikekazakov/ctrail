#include <ctrail/ValuesStorageExport.h>

namespace ctrail {

ValuesStorageExport::ValuesStorageExport(const ValuesStorageExport& _rhs):
    m_Impl(_rhs.m_Impl->clone())
{
}
    
ValuesStorageExport::ValuesStorageExport(ValuesStorageExport&& _rhs) noexcept = default;
    
std::string ValuesStorageExport::format(const ValuesStorage &_values, Options _options) const
{
    return m_Impl->format(_values, _options);
}
    
ValuesStorageExport &ValuesStorageExport::operator=(const ValuesStorageExport &_rhs)
{
    m_Impl = _rhs.m_Impl->clone();
    return *this;
}

ValuesStorageExport &ValuesStorageExport::operator=(ValuesStorageExport &&_rhs) noexcept = default;

}
