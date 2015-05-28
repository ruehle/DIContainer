#pragma once
#include <memory>

class DIContainer;

template<class... Args>
class Injector
{
public:
    template<class T>
    std::shared_ptr<T> create(DIContainer &r) const
    {
        return std::make_shared<T>(r.resolve<Args...>());
    }
};
