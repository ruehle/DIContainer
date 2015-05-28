#pragma once
#include <memory>

class DIContainer;

template<class T, class... Args>
class AutoConstruct
{
public:
    std::shared_ptr<T> operator()(DIContainer &r)
    {
        return std::make_shared<T>(r.resolve<Args...>());
    }

};
