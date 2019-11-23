#include <ctrail/CyclicValuesStorage.h>
#include <cassert>
#include <<algorithm>

namespace ctrail {

CyclicValuesStorage::CyclicValuesStorage(
    const std::vector<std::string>& _counters_names,
    std::size_t _max_time_points)
    : m_CountersNumber{_counters_names.size()}, m_Capacity{_max_time_points},
      m_Head{0}, m_Size{0}, m_CountersNames{_counters_names}
{
    if (_max_time_points == 0)
        throw std::invalid_argument(
            "CyclicValuesStorage: _max_time_points must be positive");
    m_TimePoints.resize(m_Capacity);
    m_Values.resize(m_Capacity * m_CountersNumber);
}

void CyclicValuesStorage::addValues(time_point _time_point,
                                    const std::int64_t* _values,
                                    std::size_t _values_number)
{
    assert(_values_number == m_CountersNumber);
    assert(m_Head < m_Capacity);
    m_TimePoints[m_Head] = _time_point;
    if( _values_number > 0 ) {
        assert(_values != nullptr);
        std::copy(_values, _values + _values_number,
            m_Values.data() + (m_Head * m_CountersNumber));
    }
    m_Head = (m_Head + 1) % m_Capacity;
    m_Size = std::min(m_Size + 1, m_Capacity);
}

std::size_t CyclicValuesStorage::timePointsNumber() const
{
    return m_Size;
}

CyclicValuesStorage::time_point
CyclicValuesStorage::timePoint(std::size_t _index) const
{
    assert(_index < m_Size);

    const auto position = (m_Head + m_Capacity - m_Size + _index) % m_Capacity;
    assert(position < m_Capacity);

    return m_TimePoints[position];
}

void CyclicValuesStorage::copyValuesByTimePoint(
    std::size_t _index, std::int64_t* _buffer,
    [[maybe_unused]] std::size_t _buffer_elements) const
{
    assert(_index < m_Size);
    assert(_buffer != nullptr);
    assert(_buffer_elements == m_CountersNumber);

    const auto position = (m_Head + m_Capacity - m_Size + _index) % m_Capacity;
    assert(position < m_Capacity);

    std::copy(m_Values.data() + position * m_CountersNumber,
              m_Values.data() + position * m_CountersNumber + m_CountersNumber,
              _buffer);
}

std::size_t CyclicValuesStorage::countersNumber() const
{
    return m_CountersNumber;
}

const std::string &CyclicValuesStorage::counterName(std::size_t _index) const
{
    assert(_index < m_CountersNumber);
    return m_CountersNames[_index];
}

void CyclicValuesStorage::copyValuesByCounter(
    std::size_t _index, std::int64_t* _buffer,
    std::size_t _buffer_elements) const
{
    assert(_index < m_CountersNumber);
    assert(_buffer != nullptr);
    assert(_buffer_elements == m_Size);

    const auto position = (m_Head + m_Capacity - m_Size) % m_Capacity;
    assert(position < m_Capacity);
    
    const auto stride = m_CountersNumber;
    const auto values = m_Values.data() + _index;

    for (std::size_t i = 0; i < _buffer_elements; ++i) {
        _buffer[i] = values[ ( (position + i) % m_Size ) * stride ];
    }
}

} // namespace ctrail
