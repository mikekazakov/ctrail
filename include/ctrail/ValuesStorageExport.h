#pragma once

#include <ctrail/ValuesStorage.h>
#include <string>
#include <memory>

namespace ctrail {

class ValuesStorageExport
{
public:
    enum class Options {
        none = 0,
        differential = 1,
        skip_idle_counters = 2
    };
    template <class T>
    ValuesStorageExport(T _impl);
    ValuesStorageExport(const ValuesStorageExport& _rhs);
    ValuesStorageExport(ValuesStorageExport&& _rhs) noexcept;
    
    std::string format(const ValuesStorage &_values, Options _options = Options::none) const;
    
    ValuesStorageExport &operator=(const ValuesStorageExport &_rhs);
    ValuesStorageExport &operator=(ValuesStorageExport &&_rhs) noexcept;
    
private:
    class Concept;
    template <class T> class Model;
    std::unique_ptr<Concept> m_Impl;
};
    
class ValuesStorageExport::Concept {
public:
    virtual ~Concept() = default;
    virtual std::unique_ptr<Concept> clone() const = 0;
    virtual std::string format(const ValuesStorage &_values, Options _options) const = 0;
};
    
template <class T> class ValuesStorageExport::Model : public ValuesStorageExport::Concept {
public:
    Model(T _obj) noexcept;
    std::unique_ptr<Concept> clone() const override;
    std::string format(const ValuesStorage &_values, Options _options) const override;
private:
    T m_Obj;
};

inline ValuesStorageExport::Options operator|(ValuesStorageExport::Options _lhs,
                                              ValuesStorageExport::Options _rhs) noexcept
{
    return static_cast<ValuesStorageExport::Options>(static_cast<int>(_lhs) |
                                                     static_cast<int>(_rhs) );
}

inline ValuesStorageExport::Options operator&(ValuesStorageExport::Options _lhs,
                                              ValuesStorageExport::Options _rhs) noexcept
{
    return static_cast<ValuesStorageExport::Options>(static_cast<int>(_lhs) &
                                                     static_cast<int>(_rhs) );
}

inline ValuesStorageExport::Options operator~(ValuesStorageExport::Options _val) noexcept
{
    return static_cast<ValuesStorageExport::Options>(~(static_cast<int>(_val)));
}

template <class T>
ValuesStorageExport::ValuesStorageExport(T _impl):
    m_Impl(std::make_unique<Model<T>>(std::move(_impl)))
{
}

template <class T>
ValuesStorageExport::Model<T>::Model(T _obj) noexcept: m_Obj{std::move(_obj)}
{}

template <class T>
std::unique_ptr<ValuesStorageExport::Concept>
    ValuesStorageExport::Model<T>::clone() const
{
    return std::make_unique<Model>(m_Obj);
}
    
template <class T>
std::string ValuesStorageExport::Model<T>::format(const ValuesStorage &_values,
                                                  Options _options) const
{
    return m_Obj.format(_values, _options);
}

}
