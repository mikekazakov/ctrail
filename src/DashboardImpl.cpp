#include <ctrail/DashboardImpl.h>
#include <assert.h>

namespace ctrail {

DashboardImpl::DashboardImpl(const RegistrySnapshot& _registry)
    : m_Registry{_registry}
{
}

const std::vector<std::string>& DashboardImpl::names() const
{
    return m_Registry.names;
}

std::vector<std::int64_t> DashboardImpl::values() const
{
    std::vector<std::int64_t> values(m_Registry.names.size());

    const auto memory = values.data();
    gatherInt32s(memory);
    gatherUInt32s(memory);
    gatherInt64s(memory);
    gatherUInt64s(memory);
    gatherAtomicInt32s(memory);
    gatherAtomicUInt32s(memory);
    gatherAtomicInt64s(memory);
    gatherAtomicUInt64s(memory);
    gatherPullers(memory);

    return values;
}

void DashboardImpl::gatherInt32s(std::int64_t* _target) const noexcept
{
    for (const auto& gauge : m_Registry.int32s) {
        assert(gauge.first < m_Registry.names.size());
        _target[gauge.first] = static_cast<std::int64_t>(*gauge.second);
    }
}

void DashboardImpl::gatherUInt32s(std::int64_t* _target) const noexcept
{
    for (const auto& gauge : m_Registry.uint32s) {
        assert(gauge.first < m_Registry.names.size());
        _target[gauge.first] = static_cast<std::int64_t>(*gauge.second);
    }
}

void DashboardImpl::gatherInt64s(std::int64_t* _target) const noexcept
{
    for (const auto& gauge : m_Registry.int64s) {
        assert(gauge.first < m_Registry.names.size());
        _target[gauge.first] = static_cast<std::int64_t>(*gauge.second);
    }
}

void DashboardImpl::gatherUInt64s(std::int64_t* _target) const noexcept
{
    for (const auto& gauge : m_Registry.uint64s) {
        assert(gauge.first < m_Registry.names.size());
        _target[gauge.first] = static_cast<std::int64_t>(*gauge.second);
    }
}

void DashboardImpl::gatherAtomicInt32s(std::int64_t* _target) const noexcept
{
    for (const auto& gauge : m_Registry.atomic_int32s) {
        assert(gauge.first < m_Registry.names.size());
        _target[gauge.first] = static_cast<std::int64_t>(gauge.second->load());
    }
}

void DashboardImpl::gatherAtomicUInt32s(std::int64_t* _target) const noexcept
{
    for (const auto& gauge : m_Registry.atomic_uint32s) {
        assert(gauge.first < m_Registry.names.size());
        _target[gauge.first] = static_cast<std::int64_t>(gauge.second->load());
    }
}

void DashboardImpl::gatherAtomicInt64s(std::int64_t* _target) const noexcept
{
    for (const auto& gauge : m_Registry.atomic_int64s) {
        assert(gauge.first < m_Registry.names.size());
        _target[gauge.first] = static_cast<std::int64_t>(gauge.second->load());
    }
}

void DashboardImpl::gatherAtomicUInt64s(std::int64_t* _target) const noexcept
{
    for (const auto& gauge : m_Registry.atomic_uint64s) {
        assert(gauge.first < m_Registry.names.size());
        _target[gauge.first] = static_cast<std::int64_t>(gauge.second->load());
    }
}

void DashboardImpl::gatherPullers(std::int64_t* _target) const noexcept
{
    for (const auto& gauge : m_Registry.pullers) {
        assert(gauge.first < m_Registry.names.size());
        _target[gauge.first] = static_cast<std::int64_t>(gauge.second());
    }
}

} // namespace ctrail
