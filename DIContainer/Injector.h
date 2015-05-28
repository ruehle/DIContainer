#pragma once
#include <memory>

namespace DIContainer
{
    class Container;

    template<class... Args>
    class Injector
    {
    public:
        template<class T>
        std::shared_ptr<T> create(Container &r) const
        {
            return std::make_shared<T>(r.resolve<Args...>());
        }
    };
}
