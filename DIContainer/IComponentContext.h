#pragma once

#include <memory>
#include "internal/IService.h"

namespace DIContainer
{
    class IComponentContext
    {
    public:
        virtual ~IComponentContext() = default;

        template<class T>
        std::shared_ptr<T> resolve()
        {
            auto instance = resolveService(TypedService<T>());
            return std::static_pointer_cast<T>(instance);
        }

        template<class T, class KeyType>
        std::shared_ptr<T> resolveKeyed(const KeyType &key)
        {
            auto instance = resolveService(KeyedService<T, KeyType>(key));
            return std::static_pointer_cast<T>(instance);
        }

        template<class T>
        std::shared_ptr<T> resolveNamed(const std::string &name)
        {
            return resolveKeyed<T>(name);
        }

    protected:
        virtual std::shared_ptr< void > resolveService(const IService & service) = 0;
    };
    
}