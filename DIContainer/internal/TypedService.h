#pragma once
#include "IService.h"

namespace DIContainer {

    template<class T>
    class TypedService : public IService
    {
    public:
        virtual const std::type_info &typeInfo() const override { return typeid(T); }

        virtual bool operator==(const IService &key) const override
        {
            return dynamic_cast<const TypedService<T>*>(&key) != nullptr;
        };

        virtual size_t hash_code() const override
        {
            return typeInfo().hash_code();
        }

    };
}

