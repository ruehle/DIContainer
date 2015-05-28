#pragma once
#include <memory>

class DIContainer;

template<class... Args>
class Injector
{
public:
    template<class T>
    static std::shared_ptr<T> create(DIContainer &r)
    {
        return std::make_shared<T>(r.resolve<Args...>());
    }

};
