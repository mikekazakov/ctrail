#pragma once

#include <chrono>
#include <cstddef>
#include <string>

namespace ctrail {

class ValuesStorage {
public:
    using time_point = std::chrono::system_clock::time_point;

    virtual ~ValuesStorage() = 0;

    virtual void addValues(time_point _time_point, const std::int64_t* _values,
                           std::size_t _values_number) = 0;

    virtual std::size_t timePointsNumber() const = 0;
    virtual time_point timePoint(std::size_t _index) const = 0;
    virtual void copyValuesByTimePoint(std::size_t _index,
                                       std::int64_t* _buffer,
                                       std::size_t _buffer_elements) const = 0;

    virtual std::size_t countersNumber() const = 0;
    virtual std::string counterName(std::size_t _index) const = 0;
    virtual void copyValuesByCounter(std::size_t _index, std::int64_t* _buffer,
                                     std::size_t _buffer_elements) const = 0;
};

} // namespace ctrail
