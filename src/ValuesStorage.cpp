#include <ctrail/ValuesStorage.h>

namespace ctrail {

ValuesStorage::ValuesStorage(ValuesStorage&& _rhs) noexcept = default;

ValuesStorage::ValuesStorage(const ValuesStorage& _rhs)
    : m_Impl{_rhs.m_Impl->clone()}
{
}

ValuesStorage::~ValuesStorage() = default;

ValuesStorage& ValuesStorage::operator=(const ValuesStorage& _rhs)
{
    m_Impl = _rhs.m_Impl->clone();
    return *this;
}

ValuesStorage& ValuesStorage::
operator=(ValuesStorage&& _rhs) noexcept = default;

void ValuesStorage::addValues(time_point _time_point,
                              const std::int64_t* _values,
                              std::size_t _values_number)
{
    m_Impl->addValues(_time_point, _values, _values_number);
}

std::size_t ValuesStorage::timePointsNumber() const
{
    return m_Impl->timePointsNumber();
}

ValuesStorage::time_point ValuesStorage::timePoint(std::size_t _index) const
{
    return m_Impl->timePoint(_index);
}

void ValuesStorage::copyValuesByTimePoint(std::size_t _index,
                                          std::int64_t* _buffer,
                                          std::size_t _buffer_elements) const
{
    return m_Impl->copyValuesByTimePoint(_index, _buffer, _buffer_elements);
}

std::size_t ValuesStorage::countersNumber() const
{
    return m_Impl->countersNumber();
}

const std::string& ValuesStorage::counterName(std::size_t _index) const
{
    return m_Impl->counterName(_index);
}

void ValuesStorage::copyValuesByCounter(std::size_t _index,
                                        std::int64_t* _buffer,
                                        std::size_t _buffer_elements) const
{
    m_Impl->copyValuesByCounter(_index, _buffer, _buffer_elements);
}

void swap(ValuesStorage& _lhs, ValuesStorage& _rhs) noexcept
{
    std::swap(_lhs.m_Impl, _rhs.m_Impl);
}

} // namespace ctrail
