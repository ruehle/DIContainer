#pragma once
#include "IService.h"

namespace DIContainer {

    template<class T, class KeyType>
    class KeyedService : public IService
    {
    public:
        explicit KeyedService(const KeyType &key)
            : key(key) {}

        virtual const std::type_info &typeInfo() const override { return typeid(T); }

        virtual bool operator==(const IService &info) const override
        {
            auto keyed = dynamic_cast<const KeyedService<T, KeyType>*>(&info);
            if (keyed == nullptr)
                return false;
            return key == keyed->key;
        };

        virtual size_t hash_code() const override
        {
            auto typeHash = typeInfo().hash_code();
            auto keyTypeHash = typeid(KeyType).hash_code();
            // workaround for gcc which does not provide standard hash for enum class
            // auto keyValueHash = std::hash<KeyType>()(key);

            return ((typeHash
                ^ (keyTypeHash << 1)) >> 1)
                ;// ^ (keyValueHash << 1);
        }


    private:

        KeyType key;
    };

}


