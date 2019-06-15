#pragma once

#include <atomic>
#include <functional>
#include <string>
#include <vector>

namespace ctrail {

struct RegistrySnapshot {
    std::vector<std::string> names;
    std::vector<std::pair<std::size_t, const std::int32_t*>> int32s;
    std::vector<std::pair<std::size_t, const std::uint32_t*>> uint32s;
    std::vector<std::pair<std::size_t, const std::int64_t*>> int64s;
    std::vector<std::pair<std::size_t, const std::uint64_t*>> uint64s;
    std::vector<std::pair<std::size_t, const std::atomic_int32_t*>>
        atomic_int32s;
    std::vector<std::pair<std::size_t, const std::atomic_uint32_t*>>
        atomic_uint32s;
    std::vector<std::pair<std::size_t, const std::atomic_int64_t*>>
        atomic_int64s;
    std::vector<std::pair<std::size_t, const std::atomic_uint64_t*>>
        atomic_uint64s;
    std::vector<std::pair<std::size_t, std::function<std::int64_t()>>> pullers;
};

} // namespace ctrail
