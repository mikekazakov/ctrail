#include <ctrail/MonotonicValuesStorage.h>
#include <stdexcept>
#include <cassert>

namespace ctrail {

MonotonicValuesStorage::MonotonicValuesStorage(
    const std::vector<std::string>& _counters_names)
    : m_CountersNumber(_counters_names.size()), m_CountersNames(_counters_names)
{
}

MonotonicValuesStorage::~MonotonicValuesStorage() = default;

void MonotonicValuesStorage::addValues(const time_point _time_point,
                                       const std::int64_t* const _values,
                                       const std::size_t _values_number)
{
    assert(_values_number == m_CountersNumber);
    assert(_values != nullptr);
    m_TimePoints.push_back(_time_point);
    m_Values.insert(m_Values.end(), _values, _values + _values_number);
}

std::size_t MonotonicValuesStorage::timePointsNumber() const
{
    return m_TimePoints.size();
}

ValuesStorage::time_point
MonotonicValuesStorage::timePoint(std::size_t _index) const
{
    assert(_index < m_TimePoints.size());
    return m_TimePoints[_index];
}

void MonotonicValuesStorage::copyValuesByTimePoint(
    const std::size_t _index, std::int64_t* const _buffer,
    const std::size_t _buffer_elements) const
{
    assert(_index < m_TimePoints.size());
    assert(_buffer != nullptr);
    assert(_buffer_elements == m_CountersNumber);

    const std::int64_t* source = m_Values.data() + _index * m_CountersNumber;
    std::copy(source, source + _buffer_elements, _buffer);
}

std::size_t MonotonicValuesStorage::countersNumber() const
{
    return m_CountersNumber;
}

std::string MonotonicValuesStorage::counterName(std::size_t _index) const
{
    assert(_index < m_CountersNumber);
    return m_CountersNames[_index];
}

void MonotonicValuesStorage::copyValuesByCounter(
    const std::size_t _index, std::int64_t* const _buffer,
    const std::size_t _buffer_elements) const
{
    assert(_index < m_CountersNumber);
    assert(_buffer != nullptr);
    assert(_buffer_elements == m_TimePoints.size());

    const std::int64_t* source = m_Values.data() + _index;
    const std::size_t stride = m_CountersNumber;
    for (std::size_t i = 0; i < _buffer_elements; ++i) {
        _buffer[i] = source[i * stride];
    }
}

} // namespace ctrail
