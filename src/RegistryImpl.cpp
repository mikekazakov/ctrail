#include <ctrail/RegistryImpl.h>
#include <ctrail/RegistrySnapshot.h>
#include <exception>
#include <algorithm>

namespace ctrail {

RegistryImpl::RegistryImpl() = default;

RegistryImpl::~RegistryImpl() = default;

void RegistryImpl::add(std::string_view _name, const std::int32_t& _counter)
{
    noteNewName(_name);
    m_Int32s.emplace_back(_name, &_counter);
}

void RegistryImpl::add(std::string_view _name, const std::uint32_t& _counter)
{
    noteNewName(_name);
    m_UInt32s.emplace_back(_name, &_counter);
}

void RegistryImpl::add(std::string_view _name, const std::int64_t& _counter)
{
    noteNewName(_name);
    m_Int64s.emplace_back(_name, &_counter);
}

void RegistryImpl::add(std::string_view _name, const std::uint64_t& _counter)
{
    noteNewName(_name);
    m_UInt64s.emplace_back(_name, &_counter);
}

void RegistryImpl::add(std::string_view _name,
                       const std::atomic_int32_t& _counter)
{
    noteNewName(_name);
    m_AtomicInt32s.emplace_back(_name, &_counter);
}

void RegistryImpl::add(std::string_view _name,
                       const std::atomic_uint32_t& _counter)
{
    noteNewName(_name);
    m_AtomicUInt32s.emplace_back(_name, &_counter);
}

void RegistryImpl::add(std::string_view _name,
                       const std::atomic_int64_t& _counter)
{
    noteNewName(_name);
    m_AtomicInt64s.emplace_back(_name, &_counter);
}

void RegistryImpl::add(std::string_view _name,
                       const std::atomic_uint64_t& _counter)
{
    noteNewName(_name);
    m_AtomicUInt64s.emplace_back(_name, &_counter);
}

void RegistryImpl::add(std::string_view _name,
                       std::function<std::int64_t()> _puller)
{
    if (!_puller)
        throw std::invalid_argument(
            "RegistryImpl::add(): puller function cannot be empty.");
    noteNewName(_name);
    m_Pullers.emplace_back(_name, std::move(_puller));
}

void RegistryImpl::noteNewName(std::string_view _name)
{
    const auto name = std::string{_name};
    if (name.empty()) {
        throw std::invalid_argument(std::string(
            "ctrail::Registry::add() was invoked with an empty counter name"));
    }
    if (m_RegisteredNames.count(name) != 0) {
        throw std::invalid_argument(
            std::string("ctrail::Registry::add() was invoked with a counter "
                        "name which is aready used: ") +
            name);
    }
    m_RegisteredNames.emplace(name);
}

static size_t index_of_sorted(const std::vector<std::string>& _names,
                              const std::string& _name)
{
    auto it = std::lower_bound(_names.begin(), _names.end(), _name);
    if (it == _names.end() || *it != _name)
        throw std::logic_error("index_of_sorted expected an element to exist");
    return std::distance(_names.begin(), it);
}

RegistrySnapshot RegistryImpl::bake() const
{
    RegistrySnapshot snapshot;
    snapshot.names.assign(m_RegisteredNames.begin(), m_RegisteredNames.end());
    std::sort(snapshot.names.begin(), snapshot.names.end());
    const auto find_index = [&](const std::string& _name) {
        return index_of_sorted(snapshot.names, _name);
    };

    for (const auto& cnt : m_Int32s)
        snapshot.int32s.emplace_back(find_index(cnt.first), cnt.second);

    for (const auto& cnt : m_UInt32s)
        snapshot.uint32s.emplace_back(find_index(cnt.first), cnt.second);

    for (const auto& cnt : m_Int64s)
        snapshot.int64s.emplace_back(find_index(cnt.first), cnt.second);

    for (const auto& cnt : m_UInt64s)
        snapshot.uint64s.emplace_back(find_index(cnt.first), cnt.second);

    for (const auto& cnt : m_AtomicInt32s)
        snapshot.atomic_int32s.emplace_back(find_index(cnt.first), cnt.second);

    for (const auto& cnt : m_AtomicUInt32s)
        snapshot.atomic_uint32s.emplace_back(find_index(cnt.first), cnt.second);

    for (const auto& cnt : m_AtomicInt64s)
        snapshot.atomic_int64s.emplace_back(find_index(cnt.first), cnt.second);

    for (const auto& cnt : m_AtomicUInt64s)
        snapshot.atomic_uint64s.emplace_back(find_index(cnt.first), cnt.second);

    for (const auto& cnt : m_Pullers)
        snapshot.pullers.emplace_back(find_index(cnt.first), cnt.second);

    return snapshot;
}

} // namespace ctrail
